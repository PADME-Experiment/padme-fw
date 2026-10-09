#include "TCanvas.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TKey.h"
#include "TMath.h"
#include "TMatrixDSym.h"
#include "TROOT.h"
#include "TString.h"
#include "TSystem.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

#define MM_N_LAYERS 8

namespace {

  constexpr double SATURATION_THRESHOLD = 1600.;
  constexpr double MM_PITCH = 1.2;            // mm
  constexpr int DEFAULT_HALF_WINDOW_STRIPS = 15;
  constexpr int L0_RIGHT_ALLOWANCE_STRIPS = 1;
  constexpr double BEAM_EXPECTED_POSITION = -2.0;  // mm
  constexpr int BEAM_SEARCH_HALF_WINDOW_STRIPS = 20;
  constexpr int PROJECTION_FIT_HALF_WINDOW_STRIPS = 4;

  const TString layerTag[MM_N_LAYERS] = {"L0", "L1", "L2", "L3", "L4", "L5", "L6", "L7"};

  enum FitModel {
    kSingleGaussian = 0,
    kDoubleGaussian = 1,
    kVoigt = 2,
    N_FIT_MODELS = 3
  };

  const TString fitModelTag[N_FIT_MODELS] = {
    "SingleGaussian",
    "DoubleGaussian",
    "Voigt"
  };

  struct SliceFitResult {
    TH1D *amp   = nullptr;
    TH1D *mean  = nullptr;
    TH1D *sigma = nullptr;
    TH1D *chi2  = nullptr;
  };

  struct ChargeResult {
    double chargeADC = -99.;
    double chargeErrADC = -99.;
    double nEvents = 0.;
  };

  struct ShapeResult {
    double seed = -99.;

    double position = -99.;
    double positionErr = -99.;
    double sigma = -99.;
    double sigmaErr = -99.;

    int fitStatus = -99;
    int covStatus = -99;
    bool accepted = false;
  };

  int TGraphAttribute(TGraphErrors *g, TString name, TString xtitle, TString ytitle, int markerstyle, int color){
    if (!g) return 1;
    g->SetName(name);
    g->SetTitle(name);
    g->GetXaxis()->SetTitle(xtitle);
    g->GetYaxis()->SetTitle(ytitle);
    g->SetMarkerStyle(markerstyle);
    g->SetMarkerColor(color);
    return 0;
  }

  TH2F *GetH2F(TDirectory *dir, const TString &name){
    if (!dir) return nullptr;

    TObject *obj = dir->Get(name);
    if (!obj) return nullptr;

    TH2F *h = dynamic_cast<TH2F *>(obj);
    if (!h) {
      cerr << "WARNING: " << name << " exists but is not TH2F" << endl;
      return nullptr;
    }

    return h;
  }

  vector<int> FindBlockNumbers(TDirectory *dir){
    set<int> blocks;

    if (!dir) return vector<int>();

    TIter next(dir->GetListOfKeys());
    TKey *key = nullptr;

    while ((key = (TKey *)next())) {
      TString name = key->GetName();

      if (!name.BeginsWith("block_")) continue;

      int iblk = -1;
      sscanf(name.Data(), "block_%d", &iblk);

      if (iblk >= 0) blocks.insert(iblk);
    }

    return vector<int>(blocks.begin(), blocks.end());
  }

  bool IsFitAccepted(TFitResultPtr fitResult, int maxFitStatusAccepted = 1, int minCovMatrixStatusAccepted = 2){
    if (!fitResult.Get()) return false;
    if (!fitResult->IsValid()) return false;

    const int status = (int)fitResult;
    const int covStatus = fitResult->CovMatrixStatus();

    if (status > maxFitStatusAccepted) return false;
    if (covStatus < minCovMatrixStatusAccepted) return false;

    return true;
  }

  double ClampToLimits(double x, double low, double up){
    if (x < low) return low;
    if (x > up) return up;
    return x;
  }

  ChargeResult ComputeGlobalCharge(TH2F *h2, double nEvents){
    ChargeResult out;

    if (!h2 || nEvents <= 0.) return out;

    Double_t stats[7] = {0.};
    h2->GetStats(stats);

    // For TH2:
    // stats[4] = Sum(w*y), stats[5] = Sum(w*y*y).
    // Since RecoMM fills h2 with weight=1 and y=qmax, this is the sum of qmax.
    out.chargeADC = stats[4] / nEvents;
    out.chargeErrADC = sqrt(max(0.0, stats[5])) / nEvents;
    out.nEvents = nEvents;

    return out;
  }

  TH1D *MakeNoSatProjectionX(TH2F *h2, TString name){
    if (!h2) return nullptr;

    const int firstYBin = 1;
    int lastYBin = h2->GetYaxis()->FindBin(SATURATION_THRESHOLD - 1.e-6);

    lastYBin = min(lastYBin, h2->GetNbinsY());

    if (lastYBin < firstYBin) return nullptr;

    TH1D *h = h2->ProjectionX(name, firstYBin, lastYBin, "e");
    if (!h) return nullptr;

    h->SetDirectory(nullptr);
    h->SetTitle(Form("%s;position [mm];entries", name.Data()));

    return h;
  }

  double GetProjectionSeed(TH1D *h, double expectedPosition, int searchHalfWindowStrips){

    if (!h) return -999.;

    const double searchHalfWidth = searchHalfWindowStrips * MM_PITCH;
    const double searchMin = expectedPosition - searchHalfWidth;
    const double searchMax = expectedPosition + searchHalfWidth;

    int firstBin = h->GetXaxis()->FindBin(searchMin);
    int lastBin  = h->GetXaxis()->FindBin(searchMax);

    firstBin = max(1, firstBin);
    lastBin  = min(h->GetNbinsX(), lastBin);

    if (lastBin < firstBin) return -999.;

    int maxBin = -1;
    double maxContent = -1.;

    for (int ibin = firstBin; ibin <= lastBin; ++ibin) {

      const double content = h->GetBinContent(ibin);
      if (content > maxContent) {
        maxContent = content;
        maxBin = ibin;
      }
    }

    if (maxBin < 0 || maxContent <= 0.) return -999.;

    return h->GetXaxis()->GetBinCenter(maxBin);
  }

  void ClipWindowToHistogram(TH1 *h, double &xmin, double &xmax){
    if (!h) return;

    xmin = max(xmin, h->GetXaxis()->GetXmin());
    xmax = min(xmax, h->GetXaxis()->GetXmax());
  }

  SliceFitResult RunFitSlicesY(TH2F *H, TString tag, double xmin, double xmax){
    SliceFitResult out;

    if (!H || xmax <= xmin) return out;

    const int firstBin = H->GetXaxis()->FindBin(xmin);
    const int lastBin  = H->GetXaxis()->FindBin(xmax);

    TH2F *work = (TH2F *)H->Clone(Form("%s_%s_work", H->GetName(), tag.Data()));
    if (!work) return out;

    work->SetDirectory(nullptr);

    // We deliberately fit only the unsaturated charge region even when the input histogram is the full RAW histogram.
    TF1 *sliceGaus = new TF1(Form("sliceGaus_%s", tag.Data()), "gaus", 0., SATURATION_THRESHOLD);

    const int minEntriesPerSlice = 10;

    TDirectory *oldDir = gDirectory;
    gROOT->cd();

    work->FitSlicesY(sliceGaus, firstBin, lastBin, minEntriesPerSlice, "QNR");

    TH1D *h0 = (TH1D *)gDirectory->Get(Form("%s_0", work->GetName()));
    TH1D *h1 = (TH1D *)gDirectory->Get(Form("%s_1", work->GetName()));
    TH1D *h2 = (TH1D *)gDirectory->Get(Form("%s_2", work->GetName()));
    TH1D *h3 = (TH1D *)gDirectory->Get(Form("%s_chi2", work->GetName()));

    if (h0) {
      out.amp = (TH1D *)h0->Clone(Form("hAmpSlice_%s", tag.Data()));
      out.amp->SetDirectory(nullptr);
    }

    if (h1) {
      out.mean = (TH1D *)h1->Clone(Form("hMeanSlice_%s", tag.Data()));
      out.mean->SetDirectory(nullptr);
    }

    if (h2) {
      out.sigma = (TH1D *)h2->Clone(Form("hSigmaSlice_%s", tag.Data()));
      out.sigma->SetDirectory(nullptr);
    }

    if (h3) {
      out.chi2 = (TH1D *)h3->Clone(Form("hChi2Slice_%s", tag.Data()));
      out.chi2->SetDirectory(nullptr);
    }

    gDirectory->Delete(Form("%s_0;*", work->GetName()));
    gDirectory->Delete(Form("%s_1;*", work->GetName()));
    gDirectory->Delete(Form("%s_2;*", work->GetName()));
    gDirectory->Delete(Form("%s_chi2;*", work->GetName()));

    if (oldDir) oldDir->cd();

    delete sliceGaus;
    delete work;

    return out;
  }

  TF1 *FitSingleGaussian(TH1D *h, TString name, double xmin, double xmax, TFitResultPtr &fitResult){
    fitResult = TFitResultPtr();

    if (!h || xmax <= xmin) return nullptr;
    if (h->GetMaximum() <= 0.) return nullptr;

    const int firstBin = h->GetXaxis()->FindBin(xmin);
    const int lastBin = h->GetXaxis()->FindBin(xmax);

    int maxBin = -1;
    double maxContent = -1.;

    for (int ibin = firstBin; ibin <= lastBin; ++ibin) {
      const double content = h->GetBinContent(ibin);
      if (content > maxContent) {
        maxContent = content;
        maxBin = ibin;
      }
    }
    if (maxBin < 0 || maxContent <= 0.) return nullptr;
    const double mu0 = h->GetXaxis()->GetBinCenter(maxBin);
    double sigma0 = h->GetRMS();

    if (!TMath::Finite(sigma0) || sigma0 <= 0.) sigma0 = 0.1 * (xmax - xmin);

    const double area0 = max(1.0, maxContent * sqrt(2. * TMath::Pi()) * sigma0);

    TF1 *fit = new TF1(Form("fSingleGaussian_%s", name.Data()), "[0]/(sqrt(2*TMath::Pi())*[2])*exp(-0.5*((x-[1])/[2])^2)", xmin, xmax);

    fit->SetParNames("I", "mean", "sigma");

    const double xrange = xmax - xmin;

    const double I_low = 0.;
    const double I_up = max(1.0, 10. * area0);

    const double sigma_low = 0.1;
    const double sigma_up = max(2. * sigma_low, xrange);

    const double mu_seed = ClampToLimits(mu0, xmin, xmax);

    const double sigma_seed = ClampToLimits(sigma0, sigma_low, sigma_up);

    fit->SetParameters(area0, mu_seed, sigma_seed);

    fit->SetParLimits(0, I_low, I_up);
    fit->SetParLimits(1, xmin, xmax);
    fit->SetParLimits(2, sigma_low, sigma_up);

    fitResult = h->Fit(fit, "RQS");

    if (!fitResult.Get()) {
      delete fit;
      return nullptr;
    }

    return fit;
  }

  TF1 *FitDoubleGaussian(TH1D *h, TString name, double xmin, double xmax, TFitResultPtr &fitResult){
    fitResult = TFitResultPtr();

    if (!h || xmax <= xmin) return nullptr;
    if (h->GetMaximum() <= 0.) return nullptr;

    TF1 *prefit = new TF1(Form("prefitDouble_%s", name.Data()), "gaus", xmin, xmax);
    TFitResultPtr prefitResult = h->Fit(prefit, "RQS0");

    double amp0 = prefit->GetParameter(0);
    double mu0  = prefit->GetParameter(1);
    double s0   = fabs(prefit->GetParameter(2));

    if (!prefitResult.Get() || (int)prefitResult != 0 || !TMath::Finite(amp0) || !TMath::Finite(mu0) || !TMath::Finite(s0)) {
      const int maxBin = h->GetMaximumBin();
      amp0 = h->GetBinContent(maxBin);
      mu0 = h->GetBinCenter(maxBin);
      s0 = h->GetRMS();
    }

    if (s0 <= 0.) s0 = 0.1 * (xmax - xmin);

    const double xrange = xmax - xmin;
    const double area0 = max(1.0, amp0 * sqrt(2. * TMath::Pi()) * s0);

    TF1 *fit = new TF1(
      Form("fDoubleGaussian_%s", name.Data()),
      "[0]/(sqrt(2*TMath::Pi())*[2])*exp(-0.5*((x-[1])/[2])^2)"
      "+[3]/(sqrt(2*TMath::Pi())*[4])*exp(-0.5*((x-[1])/[4])^2)",
      xmin,
      xmax
    );

    fit->SetParNames("I_1", "mean", "sigma_1", "I_2", "sigma_2");

    const double Iup = max(1.0, 10. * area0);

    fit->SetParameters(
      ClampToLimits(0.7 * area0, 0., Iup),
      ClampToLimits(mu0, xmin, xmax),
      ClampToLimits(0.7 * s0, 0.1, xrange),
      ClampToLimits(0.3 * area0, 0., Iup),
      ClampToLimits(2.0 * s0, 0.1, 5. * xrange)
    );

    fit->SetParLimits(0, 0., Iup);
    fit->SetParLimits(1, xmin, xmax);
    fit->SetParLimits(2, 0.1, max(0.2, xrange));
    fit->SetParLimits(3, 0., Iup);
    fit->SetParLimits(4, 0.1, max(0.2, 5. * xrange));

    fitResult = h->Fit(fit, "RQS");

    delete prefit;

    if (!fitResult.Get()) {
      delete fit;
      return nullptr;
    }

    return fit;
  }

  double VoigtIntegralPDF(double *x, double *par){
    const double I     = par[0];
    const double mu    = par[1];
    const double sigma = fabs(par[2]);
    const double gamma = fabs(par[3]);

    if (sigma <= 0. || gamma <= 0.) return 0.;

    return I * TMath::Voigt(x[0] - mu, sigma, gamma);
  }

  TF1 *FitVoigt(TH1D *h, TString name, double xmin, double xmax, TFitResultPtr &fitResult){
    fitResult = TFitResultPtr();

    if (!h || xmax <= xmin) return nullptr;
    if (h->GetMaximum() <= 0.) return nullptr;

    TF1 *prefit = new TF1(Form("prefitVoigt_%s", name.Data()), "gaus", xmin, xmax);
    TFitResultPtr prefitResult = h->Fit(prefit, "RQS0");

    double amp0 = prefit->GetParameter(0);
    double mu0  = prefit->GetParameter(1);
    double s0   = fabs(prefit->GetParameter(2));

    if (!prefitResult.Get() || (int)prefitResult != 0 || !TMath::Finite(amp0) || !TMath::Finite(mu0) || !TMath::Finite(s0)) {
      const int maxBin = h->GetMaximumBin();
      amp0 = h->GetBinContent(maxBin);
      mu0 = h->GetBinCenter(maxBin);
      s0 = h->GetRMS();
    }

    if (s0 <= 0.) s0 = 0.1 * (xmax - xmin);

    const double area0 = max(1.0, amp0 * sqrt(2. * TMath::Pi()) * s0);
    const double xrange = xmax - xmin;

    TF1 *fit = new TF1(Form("fVoigt_%s", name.Data()), VoigtIntegralPDF, xmin, xmax, 4);

    fit->SetParNames("I", "mean", "sigmaG", "gammaL");

    const double Iup = max(1.0, 10. * area0);

    fit->SetParameters(
      ClampToLimits(area0, 0., Iup),
      ClampToLimits(mu0, xmin, xmax),
      ClampToLimits(s0, 0.1, xrange),
      ClampToLimits(0.5 * s0, 0.001, xrange)
    );

    fit->SetParLimits(0, 0., Iup);
    fit->SetParLimits(1, xmin, xmax);
    fit->SetParLimits(2, 0.1, max(0.2, xrange));
    fit->SetParLimits(3, 0.001, max(0.002, xrange));

    fitResult = h->Fit(fit, "RQS");

    delete prefit;

    if (!fitResult.Get()) {
      delete fit;
      return nullptr;
    }

    return fit;
  }

  ShapeResult ExtractShapeResult(TF1 *fit, TFitResultPtr fitResult, FitModel model){
    ShapeResult out;

    if (!fit || !fitResult.Get()) return out;

    out.fitStatus = (int)fitResult;
    out.covStatus = fitResult->CovMatrixStatus();
    out.accepted = IsFitAccepted(fitResult);

    if (!out.accepted) return out;

    if (model == kSingleGaussian) {
      out.position = fit->GetParameter(1);
      out.positionErr = fit->GetParError(1);

      out.sigma = fabs(fit->GetParameter(2));
      out.sigmaErr = fit->GetParError(2);

      return out;
    }

    if (model == kDoubleGaussian) {
      const double I1 = fit->GetParameter(0);
      const double mu = fit->GetParameter(1);
      const double s1 = fabs(fit->GetParameter(2));
      const double I2 = fit->GetParameter(3);
      const double s2 = fabs(fit->GetParameter(4));

      out.position = mu;
      out.positionErr = fit->GetParError(1);

      const double den = I1 + I2;

      if (den > 0.) {
        out.sigma = sqrt(max(0.0, (I1 * s1 * s1 + I2 * s2 * s2) / den));
      }

      // A full covariance propagation can be added later.
      out.sigmaErr = -1.;

      return out;
    }

    if (model == kVoigt) {
      const double mu = fit->GetParameter(1);
      const double sigmaG = fabs(fit->GetParameter(2));
      const double gammaL = fabs(fit->GetParameter(3));

      out.position = mu;
      out.positionErr = fit->GetParError(1);

      const double fG = 2.354820045 * sigmaG;
      const double fL = 2.0 * gammaL;
      const double fV = 0.5346 * fL + sqrt(0.2166 * fL * fL + fG * fG);

      out.sigma = fV / 2.354820045;
      out.sigmaErr = -1.;

      return out;
    }

    return out;
  }

  ShapeResult FitProjectionGaussian(TH2F *h2, TString tag, int halfWindowStrips, TDirectory *outDir){
    ShapeResult out;

    TH1D *proj = MakeNoSatProjectionX(h2, Form("hProjectionX_%s", tag.Data()));
    if (!proj) return out;

    const double seed = GetProjectionSeed(proj, BEAM_EXPECTED_POSITION, BEAM_SEARCH_HALF_WINDOW_STRIPS );
    cout << "Beam seed " << tag << " = " << seed << " mm" << endl;

    if (seed <= -900.) {
      delete proj;
      return out;
    }

    double xmin = seed - PROJECTION_FIT_HALF_WINDOW_STRIPS * MM_PITCH;
    double xmax = seed + PROJECTION_FIT_HALF_WINDOW_STRIPS * MM_PITCH;

    ClipWindowToHistogram(proj, xmin, xmax);

    TFitResultPtr fitResult;
    TF1 *fit = FitSingleGaussian(proj, tag, xmin, xmax, fitResult);

    out = ExtractShapeResult(fit, fitResult, kSingleGaussian);

    out.seed = seed;

    if (outDir) {
      TDirectory *oldDir = gDirectory;
      outDir->cd();

      proj->Write();
      if (fit) fit->Write();

      if (oldDir) oldDir->cd();
    }

    delete fit;
    delete proj;

    return out;
  }

  void FitSliceModels(TH2F *h2, TString tag, int layer, int halfWindowStrips, ShapeResult out[N_FIT_MODELS], TDirectory *outDir){
    TH1D *proj = MakeNoSatProjectionX(h2, Form("hSeedProjection_%s", tag.Data()));
    if (!proj) return;

    const double seed = GetProjectionSeed(proj, BEAM_EXPECTED_POSITION, BEAM_SEARCH_HALF_WINDOW_STRIPS );
    cout << "Beam seed " << tag << " = " << seed << " mm" << endl;

    if (seed <= -900.) {
      delete proj;
      return;
    }

    double xmin = seed - halfWindowStrips * MM_PITCH;
    double xmax = seed + halfWindowStrips * MM_PITCH;

    // L0: keep the peak bin and the left side only.
    // This is deliberately different from L3 because the right-hand side of L0
    // is affected by the inefficiency region.
    if (layer == 0) {
      xmax = seed + L0_RIGHT_ALLOWANCE_STRIPS * MM_PITCH;
    }

    ClipWindowToHistogram(proj, xmin, xmax);

    SliceFitResult slices = RunFitSlicesY(h2, tag, xmin, xmax);

    if (!slices.mean) {
      delete proj;
      delete slices.amp;
      delete slices.sigma;
      delete slices.chi2;
      return;
    }

    for (int im = 0; im < N_FIT_MODELS; ++im) {
      TFitResultPtr fitResult;
      TF1 *fit = nullptr;

      TString modelTag = Form("%s_%s", tag.Data(), fitModelTag[im].Data());

      if (im == kSingleGaussian) {
        fit = FitSingleGaussian(slices.mean, modelTag, xmin, xmax, fitResult);
      } else if (im == kDoubleGaussian) {
        fit = FitDoubleGaussian(slices.mean, modelTag, xmin, xmax, fitResult);
      } else if (im == kVoigt) {
        fit = FitVoigt(slices.mean, modelTag, xmin, xmax, fitResult);
      }

      out[im] = ExtractShapeResult(fit, fitResult, (FitModel)im);

      if (outDir && fit) {
        TDirectory *oldDir = gDirectory;
        outDir->cd();
        fit->Write();
        if (oldDir) oldDir->cd();
      }

      delete fit;
    }

    if (outDir) {
      TDirectory *oldDir = gDirectory;
      outDir->cd();

      proj->Write();
      if (slices.amp) slices.amp->Write();
      if (slices.mean) slices.mean->Write();
      if (slices.sigma) slices.sigma->Write();
      if (slices.chi2) slices.chi2->Write();

      if (oldDir) oldDir->cd();
    }

    delete proj;
    delete slices.amp;
    delete slices.mean;
    delete slices.sigma;
    delete slices.chi2;
  }

  void AddPoint(TGraphErrors *g, double x, double ex, double y, double ey){
    if (!g) return;

    const int p = g->GetN();
    g->SetPoint(p, x, y);
    g->SetPointError(p, ex, ey);
  }

  bool LayerHasShapeAnalysis(int layer){
    return layer == 0 || layer == 3 || layer == 4 || layer == 7;
  }

  bool LayerUsesFitSlices(int layer){
    return layer == 0 || layer == 3;
  }

  bool LayerUsesProjection(int layer){
    return layer == 0 || layer == 3 || layer == 4 || layer == 7;
  }
} // namespace


void AnalysisMM(const char *InputFileName, int NevtBlock = 1000, int halfWindowStrips = DEFAULT_HALF_WINDOW_STRIPS){

  TFile *fin = TFile::Open(InputFileName, "READ");

  if (!fin || fin->IsZombie()) {
    cerr << "ERROR: cannot open input file " << InputFileName << endl;
    return;
  }

  TString inName = gSystem->BaseName(InputFileName);
  inName.ReplaceAll(".root", "");

  TString outName = Form("AnalysisMM_%s.root", inName.Data());

  TFile *fout = TFile::Open(outName, "RECREATE");

  if (!fout || fout->IsZombie()) {
    cerr << "ERROR: cannot create output file " << outName << endl;
    fin->Close();
    return;
  }

  TDirectory *dOverallIn = (TDirectory *)fin->Get("Overall");
  TDirectory *dOverallRawIn = dOverallIn ? (TDirectory *)dOverallIn->Get("Raw") : nullptr;
  TDirectory *dOverallTimeIn = dOverallIn ? (TDirectory *)dOverallIn->Get("Time") : nullptr;

  TDirectory *dBlocksIn = (TDirectory *)fin->Get("Blocks");
  TDirectory *dMonitoringIn = dBlocksIn ? (TDirectory *)dBlocksIn->Get("Monitoring") : nullptr;

  if (!dOverallRawIn || !dOverallTimeIn || !dBlocksIn || !dMonitoringIn) {
    cerr << "ERROR: missing RecoMM input directories" << endl;
    fout->Close();
    fin->Close();
    return;
  }

  // ---------------------------------------------------------------------------
  // Rebuild the actual event count per block from g_evt_vs_iev.
  // This avoids the fixed-1000 normalization problem for the final partial block.
  // ---------------------------------------------------------------------------

  map<int, int> blockEventCount;
  double totalEvents = 0.;

  TGraphErrors *gEvtVsIev = (TGraphErrors *)dOverallTimeIn->Get("g_evt_vs_iev");

  if (gEvtVsIev && gEvtVsIev->GetN() > 0) {
    double dummyX = 0.;
    double firstEvt = 0.;

    gEvtVsIev->GetPoint(0, dummyX, firstEvt);

    for (int ip = 0; ip < gEvtVsIev->GetN(); ++ip) {
      double iev = 0.;
      double evt = 0.;

      gEvtVsIev->GetPoint(ip, iev, evt);

      const int iblock = (int)((evt - firstEvt) / (double)NevtBlock);

      if (iblock >= 0) blockEventCount[iblock]++;
    }

    totalEvents = gEvtVsIev->GetN();
  }

  if (totalEvents <= 0.) {
    cerr << "WARNING: cannot determine exact event counts from Overall/Time/g_evt_vs_iev" << endl;
  }

  // ---------------------------------------------------------------------------
  // Block -> DAQ time association
  // ---------------------------------------------------------------------------

  TGraphErrors *gBlockMeanDaqTime = (TGraphErrors *)dMonitoringIn->Get("g_BlockMeanDaqTime");

  if (!gBlockMeanDaqTime) {
    cerr << "ERROR: cannot find Blocks/Monitoring/g_BlockMeanDaqTime" << endl;
    fout->Close();
    fin->Close();
    return;
  }

  map<int, double> blockMeanDaqTime;
  map<int, double> blockDaqTimeHalfWidth;

  for (int ip = 0; ip < gBlockMeanDaqTime->GetN(); ++ip) {
    double blockId = 0.;
    double meanTime = 0.;

    gBlockMeanDaqTime->GetPoint(ip, blockId, meanTime);

    const int iblock = (int)llround(blockId);

    blockMeanDaqTime[iblock] = meanTime;
    blockDaqTimeHalfWidth[iblock] = gBlockMeanDaqTime->GetErrorY(ip);
  }

  // ---------------------------------------------------------------------------
  // Output directories
  // ---------------------------------------------------------------------------

  TDirectory *dOverallOut = fout->mkdir("Overall");

  TDirectory *dGraphsOut = fout->mkdir("Graphs");
  TDirectory *dChargeGraphsOut = dGraphsOut->mkdir("Charge");
  TDirectory *dPositionGraphsOut = dGraphsOut->mkdir("Position");
  TDirectory *dSigmaGraphsOut = dGraphsOut->mkdir("Sigma");
  TDirectory *dStatusGraphsOut = dGraphsOut->mkdir("FitStatus");

  TDirectory *dBlocksOut = fout->mkdir("Blocks");

  // ---------------------------------------------------------------------------
  // Graphs
  // ---------------------------------------------------------------------------

  TGraphErrors *gCharge[MM_N_LAYERS] = {nullptr};

  TGraphErrors *gSeedPosition[MM_N_LAYERS] = {nullptr};

  // Graph derived from FitSlicesY and after fitted with N_FIT_MODELS
  TGraphErrors *gPosition[MM_N_LAYERS][N_FIT_MODELS] = {{nullptr}};
  TGraphErrors *gSigma[MM_N_LAYERS][N_FIT_MODELS] = {{nullptr}};

  TGraph *gFitStatus[MM_N_LAYERS][N_FIT_MODELS] = {{nullptr}};
  TGraph *gCovStatus[MM_N_LAYERS][N_FIT_MODELS] = {{nullptr}};

  // Graph derived from ProjecitionX and after fitted single gaussian only
  TGraphErrors *gPositionProjection[MM_N_LAYERS] = {nullptr};
  TGraphErrors *gSigmaProjection[MM_N_LAYERS] = {nullptr};

  TGraph *gFitStatusProjection[MM_N_LAYERS] = {nullptr};
  TGraph *gCovStatusProjection[MM_N_LAYERS] = {nullptr};

  TGraphErrors *gOverallCharge = new TGraphErrors();
  TGraphAttribute(gOverallCharge, "gOverallCharge", "MM layer", "global collected charge [ADC/event]", 20, kBlack);

  for (int l = 0; l < MM_N_LAYERS; ++l) {
    gCharge[l] = new TGraphErrors();
    TGraphAttribute(gCharge[l], Form("gCharge_%s", layerTag[l].Data()), "Time [s]", Form("global collected charge %s [ADC/event]", layerTag[l].Data()), 20, kBlack);

    if (LayerHasShapeAnalysis(l)) {
      gSeedPosition[l] = new TGraphErrors();
      TGraphAttribute(gSeedPosition[l], Form("gSeedPosition_%s", layerTag[l].Data()), "Time [s]", Form("%s ProjectionX seed [mm]", layerTag[l].Data()), 25, kBlack);
    } 
    if (!LayerHasShapeAnalysis(l)) continue;

    if (LayerUsesFitSlices(l)) {
      for (int im = 0; im < N_FIT_MODELS; ++im) {
        gPosition[l][im] = new TGraphErrors();
        gSigma[l][im] = new TGraphErrors();

        TGraphAttribute(gPosition[l][im], Form("gPosition_%s_FitSlices_%s", layerTag[l].Data(), fitModelTag[im].Data()), "Time [s]", Form("%s position [mm]", layerTag[l].Data()), 20 + im, kBlue + im);
        TGraphAttribute(gSigma[l][im], Form("gSigma_%s_FitSlices_%s", layerTag[l].Data(), fitModelTag[im].Data()), "Time [s]", Form("#sigma_{%s} [mm]", layerTag[l].Data()), 20 + im, kRed + im);

        gFitStatus[l][im] = new TGraph();
        gCovStatus[l][im] = new TGraph();

        gFitStatus[l][im]->SetName(Form("gFitStatus_%s_FitSlices_%s", layerTag[l].Data(), fitModelTag[im].Data()));
        gCovStatus[l][im]->SetName(Form("gCovStatus_%s_FitSlices_%s", layerTag[l].Data(), fitModelTag[im].Data()));
      }
    }


    if (LayerUsesProjection(l)) {

      gPositionProjection[l] = new TGraphErrors();
      gSigmaProjection[l] = new TGraphErrors();

      TGraphAttribute(gPositionProjection[l], Form("gPosition_%s_ProjectionGaussian", layerTag[l].Data()), "Time [s]", Form("%s position [mm]", layerTag[l].Data()), 24, kBlue + l);
      TGraphAttribute(gSigmaProjection[l], Form("gSigma_%s_ProjectionGaussian", layerTag[l].Data()), "Time [s]", Form("#sigma_{%s} [mm]", layerTag[l].Data()), 24, kRed + l);

      gFitStatusProjection[l] = new TGraph();
      gCovStatusProjection[l] = new TGraph();

      gFitStatusProjection[l]->SetName(Form("gFitStatus_%s_ProjectionGaussian", layerTag[l].Data()));
      gCovStatusProjection[l]->SetName(Form("gCovStatus_%s_ProjectionGaussian", layerTag[l].Data()));
    }
  }

  // ---------------------------------------------------------------------------
  // Overall analysis
  // ---------------------------------------------------------------------------

  cout << endl;
  cout << "===================================================" << endl;
  cout << "=============== MM ANALYSIS OVERALL ===============" << endl;
  cout << "===================================================" << endl;
  cout << endl;

  for (int l = 0; l < MM_N_LAYERS; ++l) {
    TH2F *h2Overall = GetH2F(dOverallRawIn, Form("hqmaxstripFull%s", layerTag[l].Data()));

    ChargeResult qOverall = ComputeGlobalCharge(h2Overall, totalEvents);

    gOverallCharge->SetPoint(l, l, qOverall.chargeADC);
    gOverallCharge->SetPointError(l, 0., qOverall.chargeErrADC);

    cout << layerTag[l] << " global charge = " << qOverall.chargeADC << " +/- " << qOverall.chargeErrADC << " ADC/event" << endl;

    if (!LayerHasShapeAnalysis(l) || !h2Overall) continue;

    if (LayerUsesFitSlices(l)) {
      ShapeResult shape[N_FIT_MODELS];

      // cout << "DEBUG: halfWindowStrips = " << halfWindowStrips << endl;
      FitSliceModels(h2Overall, Form("Overall_%s", layerTag[l].Data()), l, halfWindowStrips, shape, dOverallOut); // halfwindowstrips che numero è??

      for (int im = 0; im < N_FIT_MODELS; ++im) {
        cout << "  " << fitModelTag[im] << ": x = " << shape[im].position << " +/- " << shape[im].positionErr << " mm, sigma = " << shape[im].sigma << " +/- " << shape[im].sigmaErr << " mm, status = " << shape[im].fitStatus << ", cov = " << shape[im].covStatus << endl;
      }
    }

    if (LayerUsesProjection(l)) {
      ShapeResult shape = FitProjectionGaussian(h2Overall, Form("Overall_%s", layerTag[l].Data()), PROJECTION_FIT_HALF_WINDOW_STRIPS, dOverallOut);
      cout << "  Projection Gaussian: x = " << shape.position << " +/- " << shape.positionErr << " mm, sigma = " << shape.sigma << " +/- " << shape.sigmaErr << " mm" << endl;
    }
  }

  // ---------------------------------------------------------------------------
  // Block analysis
  // ---------------------------------------------------------------------------

  vector<int> blocks = FindBlockNumbers(dBlocksIn);

  cout << endl;
  cout << "N blocks to analyse = " << blocks.size() << endl;

  for (size_t i = 0; i < blocks.size(); ++i) {
    const int iblock = blocks[i];

    auto itTime = blockMeanDaqTime.find(iblock);

    if (itTime == blockMeanDaqTime.end()) {
      cerr << "WARNING: missing DAQ time for block " << iblock << endl;
      continue;
    }

    const double meanDaqTime = itTime->second;
    const double daqTimeHalfWidth = blockDaqTimeHalfWidth[iblock];

    // cout << "DEBUG: meanDaqTime = " << meanDaqTime << endl;

    TDirectory *dBlockIn = (TDirectory *)dBlocksIn->Get(Form("block_%04d", iblock));

    // cout << "DEBUG: dBlockIn = " << dBlockIn << endl;

    if (!dBlockIn) {
      cerr << "WARNING: missing input block " << iblock << endl;
      continue;
    }

    TDirectory *dRawIn = (TDirectory *)dBlockIn->Get("Raw");
    // cout << "DEBUG: dRawIn = " << dRawIn << endl;

    if (!dRawIn) {
      cerr << "WARNING: missing Raw directory in block " << iblock << endl;
      continue;
    }

    TDirectory *dBlockOut = dBlocksOut->mkdir(Form("block_%04d", iblock));
    const double nEventsBlock = blockEventCount.count(iblock) ? (double)blockEventCount[iblock] : (double)NevtBlock;
    // cout << "DEBUG: dBlockOut = " << dBlockOut << " - nEventsBlock = " << nEventsBlock << endl;

    // riparti da qui!!!
    for (int l = 0; l < MM_N_LAYERS; ++l) {
      TH2F *h2 = GetH2F(dRawIn, Form("hBlockqmaxstripFull%s_block%04d", layerTag[l].Data(), iblock));

      if (!h2) continue;
      // Charge: all 8 layers.
      ChargeResult q = ComputeGlobalCharge(h2, nEventsBlock);
      AddPoint(gCharge[l], meanDaqTime, daqTimeHalfWidth, q.chargeADC, q.chargeErrADC);
      // cout << "DEBUG: q.adc = " << q.chargeADC << " - mm layer = " << l << endl;

      // L1, L2, L5, L6 stop here.
      if (!LayerHasShapeAnalysis(l)) continue;

      TDirectory *dLayerOut = dBlockOut->mkdir(layerTag[l]);
      // cout << "DEBUG: dLayerOut = " << dLayerOut << " - mm layer = " << l << endl;

      if (LayerUsesFitSlices(l)) {
        ShapeResult shape[N_FIT_MODELS];
        // cout << "DEBUG: LayerUsesFitSlices passed = " << LayerUsesFitSlices(l) << endl;

        FitSliceModels(h2, Form("%s_block%04d", layerTag[l].Data(), iblock), l, halfWindowStrips, shape, dLayerOut);

        // cout << "DEBUG: FitSliceModels passed" << endl;
        
        for (int im = 0; im < N_FIT_MODELS; ++im) {
          AddPoint(gPosition[l][im], meanDaqTime, daqTimeHalfWidth, shape[im].position, shape[im].positionErr);
          AddPoint(gSigma[l][im], meanDaqTime, daqTimeHalfWidth, shape[im].sigma, shape[im].sigmaErr);

          gFitStatus[l][im]->SetPoint(gFitStatus[l][im]->GetN(), meanDaqTime, shape[im].fitStatus);
          gCovStatus[l][im]->SetPoint(gCovStatus[l][im]->GetN(), meanDaqTime, shape[im].covStatus );
        }
        // cout << "DEBUG: TGraph fitstatus and covstatus filled after fit slices" << endl;
      }

      if (LayerUsesProjection(l)) {
        ShapeResult shape = FitProjectionGaussian(h2, Form("%s_block%04d", layerTag[l].Data(), iblock), PROJECTION_FIT_HALF_WINDOW_STRIPS, dLayerOut);

        // cout << "DEBUG: FitProjectionGaussian passed" << endl;
        AddPoint(gPositionProjection[l],meanDaqTime,daqTimeHalfWidth,shape.position,shape.positionErr);
        AddPoint(gSigmaProjection[l], meanDaqTime, daqTimeHalfWidth, shape.sigma, shape.sigmaErr);

        AddPoint(gPositionProjection[l], meanDaqTime, daqTimeHalfWidth, shape.position, shape.positionErr);
        AddPoint(gSigmaProjection[l], meanDaqTime, daqTimeHalfWidth, shape.sigma, shape.sigmaErr);

        gFitStatusProjection[l]->SetPoint(gFitStatusProjection[l]->GetN(), meanDaqTime, shape.fitStatus);
        gCovStatusProjection[l]->SetPoint(gCovStatusProjection[l]->GetN(), meanDaqTime, shape.covStatus);
      }
      // cout << "DEBUG: TGraph fitstatus and covstatus filled after projection" << endl;

    }
  }

  // ---------------------------------------------------------------------------
  // Write graphs
  // ---------------------------------------------------------------------------

  dOverallOut->cd();
  gOverallCharge->Write();

  dChargeGraphsOut->cd();

  for (int l = 0; l < MM_N_LAYERS; ++l) {
    if (gCharge[l]) gCharge[l]->Write();
  }

  dPositionGraphsOut->cd();

  for (int l = 0; l < MM_N_LAYERS; ++l) {
    for (int im = 0; im < N_FIT_MODELS; ++im) {
      if (gPosition[l][im]) gPosition[l][im]->Write();
      if (gSeedPosition[l]) gSeedPosition[l]->Write();
    }
    if (gPositionProjection[l]) gPositionProjection[l]->Write();
  }

  dSigmaGraphsOut->cd();

  for (int l = 0; l < MM_N_LAYERS; ++l) {
    for (int im = 0; im < N_FIT_MODELS; ++im) {
      if (gSigma[l][im]) gSigma[l][im]->Write();
    }
    if (gSigmaProjection[l]) gSigmaProjection[l]->Write();
  }

  dStatusGraphsOut->cd();

  for (int l = 0; l < MM_N_LAYERS; ++l) {
    for (int im = 0; im < N_FIT_MODELS; ++im) {
      if (gFitStatus[l][im]) gFitStatus[l][im]->Write();
      if (gCovStatus[l][im]) gCovStatus[l][im]->Write();
    }
    if (gFitStatusProjection[l]) gFitStatusProjection[l]->Write();
    if (gCovStatusProjection[l]) gCovStatusProjection[l]->Write();
  }

  fout->Close();
  fin->Close();

  cout << endl;
  cout << "AnalysisMM output written to " << outName << endl;
}
