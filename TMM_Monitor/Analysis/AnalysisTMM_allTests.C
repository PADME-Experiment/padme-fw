#include "TRandom.h"
#include "TRandom3.h"
#include "TMarker.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TTreeIndex.h"
#include "TBranch.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2.h"
#include "TProfile.h"
#include "TMath.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TSpline.h"
#include "TString.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TPaveText.h"
#include "TObject.h"
#include "TF1.h"
#include "TButton.h"
#include "TTimeStamp.h"
#include "Riostream.h"
#include "TMinuit.h"
#include "TKey.h"

#include <set>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <cmath>
#include <sys/stat.h>
#include <vector>

using namespace std;

#define TMMCH_N_Readout 2
#define NEvtBLOCKS 10000

struct SliceFitResult {
  TH1D *amp = nullptr;
  TH1D *mean = nullptr;
  TH1D *sigma = nullptr;
  TH1D *chi2 = nullptr;
};

struct ObservableResult {
  double entries = 0.;
  double charge = 0.;
  double chargeErr = 0.;
  double meanCharge = 0.;
  double meanChargeErr = 0.;
  double spot = 0.;
  double spotErr = 0.;
  double spotErrCons = 0.;
  double width = 0.;
  double widthErr = 0.;
  double widthCore = 0.;
  double widthCoreErr = 0.;
  double widthTail = 0.;
  double widthTailErr = 0.;
  double widthSingleCore = 0.;
  double widthSingleCoreErr = 0.;
};

int TGraphAttribute(TGraphErrors *Graph, TString title, TString xlabel, TString ylabel, int markerstyle, int color){
  Graph->SetTitle(title);
  Graph->SetName(title);
  Graph->GetXaxis()->SetTitle(xlabel);
  Graph->GetYaxis()->SetTitle(ylabel);
  Graph->SetMarkerStyle(markerstyle);
  Graph->SetMarkerColor(color);
  return 0;
}

TH2F* GetH2F(TDirectory *dir, const TString &name)
{
  if (!dir) return nullptr;
  TObject *obj = dir->Get(name);
  if (!obj) return nullptr;

  TH2F *h = dynamic_cast<TH2F*>(obj);
  if (!h) {
    cerr << "WARNING: " << name << " exists but is not TH2F" << endl;
    return nullptr;
  }

  return h;
}

vector<int> FindBlockNumbers(TDirectory *dir)
{
  set<int> blocks;

  if (!dir) return vector<int>();

  TIter next(dir->GetListOfKeys());
  TKey *key = nullptr;

  while ((key = (TKey*)next())) {
    TString name = key->GetName();

    if (!name.BeginsWith("block_")) continue;

    int iblk = -1;
    sscanf(name.Data(), "block_%d", &iblk);

    if (iblk >= 0) blocks.insert(iblk);
  }

  return vector<int>(blocks.begin(), blocks.end());
}

SliceFitResult RunFitSlicesY(TH2F *h2, TString tag){
  SliceFitResult out;

  if (!h2) return out;

  TString oldName = h2->GetName();
  TString uniqueName = Form("%s_%s", oldName.Data(), tag.Data());

  h2->SetName(uniqueName);

  h2->FitSlicesY();

  TH1D *h0  = (TH1D*)gDirectory->Get(Form("%s_0",    h2->GetName()));
  TH1D *h1  = (TH1D*)gDirectory->Get(Form("%s_1",    h2->GetName()));
  TH1D *h2s = (TH1D*)gDirectory->Get(Form("%s_2",    h2->GetName()));
  TH1D *hC2 = (TH1D*)gDirectory->Get(Form("%s_chi2", h2->GetName()));

  if (!h0 || !h1 || !h2s || !hC2) {
    cerr << "ERROR: FitSlicesY failed for " << h2->GetName() << endl;
    h2->SetName(oldName);
    return out;
  }

  out.amp   = (TH1D*)h0->Clone(Form("hAmpSlice_%s", tag.Data()));
  out.mean  = (TH1D*)h1->Clone(Form("hMeanSlice_%s", tag.Data()));
  out.sigma = (TH1D*)h2s->Clone(Form("hSigmaSlice_%s", tag.Data()));
  out.chi2  = (TH1D*)hC2->Clone(Form("hChi2Slice_%s", tag.Data()));

  out.amp->SetDirectory(0);
  out.mean->SetDirectory(0);
  out.sigma->SetDirectory(0);
  out.chi2->SetDirectory(0);

  gDirectory->Delete(Form("%s_0;*",    h2->GetName()));
  gDirectory->Delete(Form("%s_1;*",    h2->GetName()));
  gDirectory->Delete(Form("%s_2;*",    h2->GetName()));
  gDirectory->Delete(Form("%s_chi2;*", h2->GetName()));

  h2->SetName(oldName);

  return out;
}

TF1* FitDoubleGaussian(TH1D *h, TString name, double xmin, double xmax){
  
  if (!h) return nullptr;

  TF1 *prefit = new TF1(Form("prefit_%s", name.Data()), "gaus", xmin, xmax);
  h->Fit(prefit, "RQ0");

  double A0  = prefit->GetParameter(0);
  double mu0 = prefit->GetParameter(1);
  double s0  = fabs(prefit->GetParameter(2));
  if (s0 <= 0) s0 = 10.;

  TF1 *fit = new TF1(Form("f_%s", name.Data()), "[0]*exp(-0.5*((x-[1])/[2])^2)+[3]*exp(-0.5*((x-[1])/[4])^2)", xmin, xmax );

  fit->SetParNames("A_1", "mean", "sigma_1", "A_2", "sigma_2");
  fit->SetParameters(0.7*A0, mu0, 0.7*s0, 0.3*A0, 2.0*s0);

  fit->SetParLimits(0, 0., 10. * A0);
  fit->SetParLimits(1, xmin, xmax);
  fit->SetParLimits(2, 0.1, xmax - xmin);
  fit->SetParLimits(3, 0., 10. * A0);
  fit->SetParLimits(4, 0.1, 5. * (xmax - xmin));

  h->Fit(fit, "RQ");

  delete prefit;
  return fit;
}

void ExtractDoubleGaussianObservables(TF1 *f, TH1D *h, ObservableResult &obs)
{
  if (!f || !h) return;

  const double xmin = h->GetXaxis()->GetXmin();
  const double xmax = h->GetXaxis()->GetXmax();
  const double binWidth = h->GetXaxis()->GetBinWidth(1);

  const double A1 = f->GetParameter(0);
  const double mu = f->GetParameter(1);
  const double s1 = fabs(f->GetParameter(2));
  const double A2 = f->GetParameter(3);
  const double s2 = fabs(f->GetParameter(4));

  const double eMu = f->GetParError(1);
  const double eS1 = f->GetParError(2);
  const double eS2 = f->GetParError(4);

  obs.spot = mu;
  obs.spotErr = eMu;

  if (s1 <= s2) {
    obs.widthCore = s1;
    obs.widthCoreErr = eS1;
    obs.widthTail = s2;
    obs.widthTailErr = eS2;
  } else {
    obs.widthCore = s2;
    obs.widthCoreErr = eS2;
    obs.widthTail = s1;
    obs.widthTailErr = eS1;
  }

  obs.charge = NEvtBLOCKS * f->Integral(xmin, xmax) / binWidth;
  obs.chargeErr = sqrt(NEvtBLOCKS * max(0.0, obs.charge));
  obs.meanCharge = f->Integral(xmin, xmax) / binWidth;
  obs.meanChargeErr = sqrt(max(0.0, obs.meanCharge));

  const double area1 = A1 * s1;
  const double area2 = A2 * s2;

  if (A1 * s1 + A2 * s2 > 0.) {
    // weighted sum of the single variance
    obs.width = sqrt((A1 * s1 * s1 * s1 + A2 * s2 * s2 * s2) / (A1 * s1 + A2 * s2));
    obs.widthErr = 0.5 / obs.width * sqrt(
        pow(((3.*A1*s1*s1*(A1*s1 + A2*s2) - A1*(A1*s1*s1*s1 + A2*s2*s2*s2)) / pow(A1*s1 + A2*s2, 2)) * eS1, 2) +
        pow(((3.*A2*s2*s2*(A1*s1 + A2*s2) - A2*(A1*s1*s1*s1 + A2*s2*s2*s2)) / pow(A1*s1 + A2*s2, 2)) * eS2, 2)
    );  
  }
}

void AddGraphPoint(TGraphErrors *g, int ip, double x, double y, double ey)
{
  if (!g) return;
  g->SetPoint(ip, x, y);
  g->SetPointError(ip, 0., ey);
}

void WriteSliceFit(TH2F *h2, TString tag, TDirectory *outdir)
{
  if (!h2 || !outdir) return;

  outdir->cd();

  SliceFitResult s = RunFitSlicesY(h2, tag);

  if (s.amp)   s.amp->Write();
  if (s.mean)  s.mean->Write();
  if (s.sigma) s.sigma->Write();
  if (s.chi2)  s.chi2->Write();
}

ObservableResult ComputeObservables(TH2F *h2, TString tag)
{
  ObservableResult obs;
  if (!h2) return obs;

  double entries = 0.;
  double charge = 0.;
  double err_charge = 0.;
  double charge2 = 0.;
  double mean_charge = 0;
  double err_mean_charge = 0;

  for (int ix = 1; ix <= h2->GetNbinsX(); ++ix) {
    for (int iy = 1; iy <= h2->GetNbinsY(); ++iy) {
      const double n = h2->GetBinContent(ix, iy);
      const double q = h2->GetYaxis()->GetBinCenter(iy);

      entries += n;
      charge  += n * q;
      charge2 += n * q * q;
    }
  }

  err_charge = sqrt(max(0.0, charge2));

  if (entries > 0.) {
    mean_charge = charge / NEvtBLOCKS;
    const double variance = charge2 / NEvtBLOCKS - mean_charge * mean_charge;    
    err_mean_charge = sqrt(max(0.0, variance) / NEvtBLOCKS);
  }

  obs.entries = entries;
  obs.charge = charge;
  obs.chargeErr = err_charge;
  obs.meanCharge = mean_charge; 
  obs.meanChargeErr = err_mean_charge;
  obs.spot = h2->GetMean();
  obs.spotErr = h2->GetMeanError();
  obs.spotErrCons = 0; // to be determined
  obs.width = h2->GetRMS();
  obs.widthErr = h2->GetRMSError();
  return obs;
}

ObservableResult ComputeObservablesFitSlice(TH2F *h2, TString tag)
{
  ObservableResult obs;
  if (!h2) return obs;

  SliceFitResult s = RunFitSlicesY(h2, tag);

  if (!s.mean) return obs;

  const double xmin = s.mean->GetXaxis()->GetXmin();
  const double xmax = s.mean->GetXaxis()->GetXmax();

  TF1 *f = FitDoubleGaussian(s.mean, Form("DoubleGaussian_FitSlices_%s", tag.Data()), xmin, xmax);

  if (f) {
    ExtractDoubleGaussianObservables(f, s.mean, obs);
    delete f;
  }

  TF1 *fg = new TF1("fg", "gaus", s.mean->GetBinCenter(123), s.mean->GetBinCenter(231));  
  s.mean->Fit(fg, "RQ");
  obs.widthSingleCore = fabs(fg->GetParameter(2));
  obs.widthSingleCoreErr = fg->GetParError(2);

  delete fg;

  delete s.amp;
  delete s.mean;
  delete s.sigma;
  delete s.chi2;  

  return obs;
}

void InitGraphPair(TGraphErrors *g[2])
{
  g[0] = new TGraphErrors();
  g[1] = new TGraphErrors();
}

void TGraphAttributePair(TGraphErrors *g[2], TString baseName, TString view, TString ytitle, int markerstyle, int color)
{
  TGraphAttribute(g[0], Form("g_Block%s_%s", baseName.Data(), view.Data()), "block", ytitle, markerstyle, color);
  TGraphAttribute(g[1], Form("g_Block%sFitSlices_%s", baseName.Data(), view.Data()), "block", ytitle, markerstyle, color);
}

void FillGraphPair(TGraphErrors *gNentries[2], TGraphErrors *gSpot[2], TGraphErrors *gWidth[2], TGraphErrors *gWidthCore[2], TGraphErrors *gWidthTail[2], TGraphErrors *gSingleWidthCore[2], TGraphErrors *gCharge[2], TGraphErrors *gMeanQ[2], int ip, int iblk, const vector<ObservableResult> obs[2])
{
  for (int im = 0; im < 2; ++im) {
    if (obs[im].empty()) continue;

    const ObservableResult &o = obs[im].back();

    AddGraphPoint(gNentries[im],        ip, iblk, o.entries,         0);
    AddGraphPoint(gSpot[im],            ip, iblk, o.spot,            o.spotErr);
    AddGraphPoint(gWidth[im],           ip, iblk, o.width,           o.widthErr);
    AddGraphPoint(gWidthCore[im],       ip, iblk, o.widthCore,       o.widthCoreErr);
    AddGraphPoint(gWidthTail[im],       ip, iblk, o.widthTail,       o.widthTailErr);
    AddGraphPoint(gSingleWidthCore[im], ip, iblk, o.widthSingleCore, o.widthSingleCoreErr);
    AddGraphPoint(gCharge[im],          ip, iblk, o.charge,          o.chargeErr);
    AddGraphPoint(gMeanQ[im],           ip, iblk, o.meanCharge,      o.meanChargeErr);
  }
}

void WriteGraphPair(TGraphErrors *g[2])
{
  if (g[0]) g[0]->Write();
  if (g[1]) g[1]->Write();
}

void WriteMeasurementGraphs(TDirectory *dir, TGraphErrors *gNentries[TMMCH_N_Readout][2], TGraphErrors *gSpot[TMMCH_N_Readout][2], TGraphErrors *gWidth[TMMCH_N_Readout][2], TGraphErrors *gWidthCore[TMMCH_N_Readout][2], TGraphErrors *gWidthTail[TMMCH_N_Readout][2], TGraphErrors *gSingleWidthCore[TMMCH_N_Readout][2], TGraphErrors *gCharge[TMMCH_N_Readout][2], TGraphErrors *gMeanQ[TMMCH_N_Readout][2])
{
  if (!dir) return;

  dir->cd();

  for (int iv = 0; iv < TMMCH_N_Readout; ++iv) {
    gNentries[iv][0]->Write();
    WriteGraphPair(gSpot[iv]);
    WriteGraphPair(gWidth[iv]);
    gWidthCore[iv][1]->Write();
    gWidthTail[iv][1]->Write();
    gSingleWidthCore[iv][1]->Write();
    WriteGraphPair(gCharge[iv]);
    WriteGraphPair(gMeanQ[iv]);
  }
}

ObservableResult DoObsRatio(ObservableResult obs_tot, vector<ObservableResult> obs_block, int iblk){
  ObservableResult obsratio;
      
  obsratio.entries = 0.;
  obsratio.charge = fabs(obs_tot.charge)>0 ? obs_block.at(iblk).charge/obs_tot.charge : -99;
  obsratio.chargeErr = 0.;
  obsratio.meanCharge = fabs(obs_tot.meanCharge)>0 ? obs_block.at(iblk).meanCharge/obs_tot.meanCharge : -99;
  obsratio.meanChargeErr = 0.;
  obsratio.spot = fabs(obs_tot.spot)>0 ? obs_block.at(iblk).spot/obs_tot.spot : -99;
  obsratio.spotErr = 0.;
  obsratio.spotErrCons = 0.;
  obsratio.width = fabs(obs_tot.width)>0 ? obs_block.at(iblk).width/obs_tot.width : -99;
  obsratio.widthErr = 0.;
  obsratio.widthCore = fabs(obs_tot.widthCore)>0 ? obs_block.at(iblk).widthCore/obs_tot.widthCore : -99;
  obsratio.widthCoreErr = 0.;
  obsratio.widthTail = fabs(obs_tot.widthTail)>0 ? obs_block.at(iblk).widthTail/obs_tot.widthTail : -99;
  obsratio.widthTailErr = 0.;
  obsratio.widthSingleCore = fabs(obs_tot.widthSingleCore)>0 ? obs_block.at(iblk).widthSingleCore/obs_tot.widthSingleCore : -99;
  obsratio.widthSingleCoreErr = 0.;

  return obsratio;
}

void PrintObservableResult(const TString &label, const ObservableResult obs[2])
{
  cout << "---------------------------------------------------" << endl;
  cout << label << endl;
  cout << "---------------------------------------------------" << endl;

  for (int im = 0; im < 2; ++im) {

    TString method = (im == 0) ? "Direct" : "FitSlicesY";

    cout << "  Method: " << method << endl;
    cout << "    nentries          = " << obs[im].entries  << endl;
    cout << "    spot              = " << obs[im].spot << " +/- " << obs[im].spotErr << endl;
    cout << "    width             = " << obs[im].width << " +/- " << obs[im].widthErr << endl;
    cout << "    widthCore         = " << obs[im].widthCore << " +/- " << obs[im].widthCoreErr << endl;
    cout << "    widthTail         = " << obs[im].widthTail << " +/- " << obs[im].widthTailErr << endl;
    cout << "    widthSingleCore   = " << obs[im].widthSingleCore << " +/- " << obs[im].widthSingleCoreErr << endl;
    cout << "    charge            = " << obs[im].charge << " +/- " << obs[im].chargeErr << endl;
    cout << "    meanCharge        = " << obs[im].meanCharge << " +/- " << obs[im].meanChargeErr << endl;
    cout << endl;
  }
}

void AnalysisTMM(const char *InputFileName)
{
  TFile *fin = TFile::Open(InputFileName, "READ");

  if (!fin || fin->IsZombie()) {
    cerr << "ERROR: cannot open input file " << InputFileName << endl;
    return;
  }

  TString inName = gSystem->BaseName(InputFileName);
  inName.ReplaceAll(".root", "");

  TString outName = Form("AnalysisTMM_%s.root", inName.Data());

  TFile *fout = TFile::Open(outName, "RECREATE");
  TDirectory *dGraphsOut = fout->mkdir("Graphs");

  if (!dGraphsOut) {
    cerr << "ERROR: cannot create/get Graphs directory" << endl;
    fout->Close();
    fin->Close();
    return;
  }

  TDirectory *dGraphsRawOut             = dGraphsOut->mkdir("Raw");
  TDirectory *dGraphsCalibOut           = dGraphsOut->mkdir("Calib");
  TDirectory *dGraphsCalib3sOut         = dGraphsOut->mkdir("Calib3s");
  TDirectory *dGraphsCalibOverallOut    = dGraphsOut->mkdir("CalibOverall");
  TDirectory *dGraphsCalib3sOverallOut  = dGraphsOut->mkdir("Calib3sOverall");
  TDirectory *dRatioOut                 = dGraphsOut->mkdir("Ratio");
  TDirectory *dRatioRawOut              = dRatioOut->mkdir("Raw");
  TDirectory *dRatioCalibOut            = dRatioOut->mkdir("Calib");
  TDirectory *dRatioCalib3sOut          = dRatioOut->mkdir("Calib3s");
  TDirectory *dRatioCalibOverallOut     = dRatioOut->mkdir("CalibOverall");
  TDirectory *dRatioCalib3sOverallOut   = dRatioOut->mkdir("Calib3sOverall");

  if (!dGraphsRawOut || !dGraphsCalibOut || !dGraphsCalib3sOut ||
      !dGraphsCalibOverallOut || !dGraphsCalib3sOverallOut) {
    cerr << "ERROR: cannot create/get Graphs subdirectories" << endl;
    fout->Close();
    fin->Close();
    return;
  }

  if (!fout || fout->IsZombie()) {
    cerr << "ERROR: cannot create output file " << outName << endl;
    fin->Close();
    return;
  }

  TDirectory *dOverallIn       = (TDirectory*)fin->Get("Overall");
  TDirectory *dOverlallInRaw   = (TDirectory*)dOverallIn->Get("Raw");
  TDirectory *dOverlallInCalib = (TDirectory*)dOverallIn->Get("Calib");
  
  TDirectory *dBlocksIn        = (TDirectory*)fin->Get("Blocks");

  TString views[TMMCH_N_Readout] = {"X", "Y"};

  // ============================================================
  // Overall analysis
  // ============================================================

  // prepare the overall structs for the results
  ObservableResult RawObs[TMMCH_N_Readout][2], CalibObs[TMMCH_N_Readout][2], Calib3sObs[TMMCH_N_Readout][2];
  
  for (int iv = 0; iv < TMMCH_N_Readout; ++iv) {
    TString v = views[iv];

    if (dOverlallInRaw) {
      TH2F *h2RawOverall = GetH2F(dOverlallInRaw, Form("hqmaxstripFull%s", v.Data()));
      RawObs[iv][0] = ComputeObservables(h2RawOverall, v);
      RawObs[iv][1] = ComputeObservablesFitSlice(h2RawOverall, v);    
    }

    if (dOverlallInCalib) {
      TH2F *h2CalibOverall = GetH2F(dOverlallInCalib, Form("hqmaxstripFull_cal%s", v.Data()));
      CalibObs[iv][0] = ComputeObservables(h2CalibOverall, v);
      CalibObs[iv][1] = ComputeObservablesFitSlice(h2CalibOverall, v);    

      TH2F *h2Calib3sOverall = GetH2F(dOverlallInCalib, Form("hqmaxstripFull_cal3s%s", v.Data()));
      Calib3sObs[iv][0] = ComputeObservables(h2Calib3sOverall, v);
      Calib3sObs[iv][1] = ComputeObservablesFitSlice(h2Calib3sOverall, v);    
    }

    cout << "===================================================" << endl;
    cout << "======== Overall analysis results - view " << v << " ========" << endl;
    cout << "===================================================" << endl;

    PrintObservableResult("Raw observables", RawObs[iv]);
    PrintObservableResult("Calib observables", CalibObs[iv]);
    PrintObservableResult("Calib3s observables", Calib3sObs[iv]);

    cout << "===================================================" << endl;
    cout << endl;

  }
  
  // write out the overall results and keep them as reference for the other coparison block by block
  // maybe write them out to a txt file

  // ============================================================
  // Blocks
  // ============================================================

  //prepare TGraphs and struct for the block analysis (runs over possible analysis methods)
  TGraphErrors *gNentriesRaw[TMMCH_N_Readout][2];
  TGraphErrors *gSpotRaw[TMMCH_N_Readout][2];
  TGraphErrors *gWidthRaw[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCoreRaw[TMMCH_N_Readout][2];
  TGraphErrors *gWidthTailRaw[TMMCH_N_Readout][2];
  TGraphErrors *gSingleWidthCoreRaw[TMMCH_N_Readout][2];
  TGraphErrors *gChargeRaw[TMMCH_N_Readout][2];
  TGraphErrors *gMeanQRaw[TMMCH_N_Readout][2];

  TGraphErrors *gNentriesCalib[TMMCH_N_Readout][2];
  TGraphErrors *gSpotCalib[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCalib[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCoreCalib[TMMCH_N_Readout][2];
  TGraphErrors *gWidthTailCalib[TMMCH_N_Readout][2];
  TGraphErrors *gSingleWidthCoreCalib[TMMCH_N_Readout][2];
  TGraphErrors *gChargeCalib[TMMCH_N_Readout][2];
  TGraphErrors *gMeanQCalib[TMMCH_N_Readout][2];

  TGraphErrors *gNentriesCalib3s[TMMCH_N_Readout][2];
  TGraphErrors *gSpotCalib3s[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCalib3s[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCoreCalib3s[TMMCH_N_Readout][2];
  TGraphErrors *gWidthTailCalib3s[TMMCH_N_Readout][2];
  TGraphErrors *gSingleWidthCoreCalib3s[TMMCH_N_Readout][2];
  TGraphErrors *gChargeCalib3s[TMMCH_N_Readout][2];
  TGraphErrors *gMeanQCalib3s[TMMCH_N_Readout][2];

  TGraphErrors *gNentriesCalibOverall[TMMCH_N_Readout][2];
  TGraphErrors *gSpotCalibOverall[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCalibOverall[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCoreCalibOverall[TMMCH_N_Readout][2];
  TGraphErrors *gWidthTailCalibOverall[TMMCH_N_Readout][2];
  TGraphErrors *gSingleWidthCoreCalibOverall[TMMCH_N_Readout][2];
  TGraphErrors *gChargeCalibOverall[TMMCH_N_Readout][2];
  TGraphErrors *gMeanQCalibOverall[TMMCH_N_Readout][2];

  TGraphErrors *gNentriesCalib3sOverall[TMMCH_N_Readout][2];
  TGraphErrors *gSpotCalib3sOverall[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCalib3sOverall[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCoreCalib3sOverall[TMMCH_N_Readout][2];
  TGraphErrors *gWidthTailCalib3sOverall[TMMCH_N_Readout][2];
  TGraphErrors *gSingleWidthCoreCalib3sOverall[TMMCH_N_Readout][2];
  TGraphErrors *gChargeCalib3sOverall[TMMCH_N_Readout][2];
  TGraphErrors *gMeanQCalib3sOverall[TMMCH_N_Readout][2];

  vector <ObservableResult> BlockRawObs[TMMCH_N_Readout][2];
  vector <ObservableResult> BlockCalibObs[TMMCH_N_Readout][2], BlockCalib3sObs[TMMCH_N_Readout][2];
  vector <ObservableResult> BlockCalibOverallObs[TMMCH_N_Readout][2], BlockCalib3sOverallObs[TMMCH_N_Readout][2];

  vector<int> blocks = FindBlockNumbers(dBlocksIn);
  cout << "DEBUG: found " << blocks.size() << " blocks" << endl;
  
  for (int iv = 0; iv < TMMCH_N_Readout; ++iv) {
    TString v = views[iv];

    InitGraphPair(gNentriesRaw[iv]);
    InitGraphPair(gSpotRaw[iv]);
    InitGraphPair(gWidthRaw[iv]);
    InitGraphPair(gWidthCoreRaw[iv]);
    InitGraphPair(gWidthTailRaw[iv]);
    InitGraphPair(gSingleWidthCoreRaw[iv]);
    InitGraphPair(gChargeRaw[iv]);
    InitGraphPair(gMeanQRaw[iv]);

    InitGraphPair(gNentriesCalib[iv]);
    InitGraphPair(gSpotCalib[iv]);
    InitGraphPair(gWidthCalib[iv]);
    InitGraphPair(gWidthCoreCalib[iv]);
    InitGraphPair(gWidthTailCalib[iv]);
    InitGraphPair(gSingleWidthCoreCalib[iv]);
    InitGraphPair(gChargeCalib[iv]);
    InitGraphPair(gMeanQCalib[iv]);

    InitGraphPair(gNentriesCalib3s[iv]);
    InitGraphPair(gSpotCalib3s[iv]);
    InitGraphPair(gWidthCalib3s[iv]);
    InitGraphPair(gWidthCoreCalib3s[iv]);
    InitGraphPair(gWidthTailCalib3s[iv]);
    InitGraphPair(gSingleWidthCoreCalib3s[iv]);
    InitGraphPair(gChargeCalib3s[iv]);
    InitGraphPair(gMeanQCalib3s[iv]);

    InitGraphPair(gNentriesCalibOverall[iv]);
    InitGraphPair(gSpotCalibOverall[iv]);
    InitGraphPair(gWidthCalibOverall[iv]);
    InitGraphPair(gWidthCoreCalibOverall[iv]);
    InitGraphPair(gWidthTailCalibOverall[iv]);
    InitGraphPair(gSingleWidthCoreCalibOverall[iv]);
    InitGraphPair(gChargeCalibOverall[iv]);
    InitGraphPair(gMeanQCalibOverall[iv]);

    InitGraphPair(gNentriesCalib3sOverall[iv]);
    InitGraphPair(gSpotCalib3sOverall[iv]);
    InitGraphPair(gWidthCalib3sOverall[iv]);
    InitGraphPair(gWidthCoreCalib3sOverall[iv]);
    InitGraphPair(gWidthTailCalib3sOverall[iv]);
    InitGraphPair(gSingleWidthCoreCalib3sOverall[iv]);
    InitGraphPair(gChargeCalib3sOverall[iv]);
    InitGraphPair(gMeanQCalib3sOverall[iv]);

    TGraphAttributePair(gNentriesRaw[iv],        "NentriesRaw",        v, "number of entries",       20, iv + 1);
    TGraphAttributePair(gSpotRaw[iv],            "BeamSpotRaw",        v, "beam spot [strip]",       20, iv + 1);
    TGraphAttributePair(gWidthRaw[iv],           "BeamSpreadRaw",      v, "beam width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthCoreRaw[iv],       "BeamCoreWidthRaw",   v, "core width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthTailRaw[iv],       "BeamTailWidthRaw",   v, "tail width [strip]",      20, iv + 1);
    TGraphAttributePair(gSingleWidthCoreRaw[iv], "SingleCoreWidthRaw", v, "single-core width [strip]", 20, iv + 1);
    TGraphAttributePair(gChargeRaw[iv],          "BeamChargeRaw",      v, "integrated charge",       20, iv + 1);
    TGraphAttributePair(gMeanQRaw[iv],           "MeanChargeRaw",      v, "mean charge/event",       20, iv + 1);

    TGraphAttributePair(gNentriesCalib[iv],        "NentriesCalib",        v, "number of entries",       20, iv + 1);
    TGraphAttributePair(gSpotCalib[iv],            "BeamSpotCalib",        v, "beam spot [strip]",       20, iv + 1);
    TGraphAttributePair(gWidthCalib[iv],           "BeamSpreadCalib",      v, "beam width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthCoreCalib[iv],       "BeamCoreWidthCalib",   v, "core width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthTailCalib[iv],       "BeamTailWidthCalib",   v, "tail width [strip]",      20, iv + 1);
    TGraphAttributePair(gSingleWidthCoreCalib[iv], "SingleCoreWidthCalib", v, "single-core width [strip]", 20, iv + 1);
    TGraphAttributePair(gChargeCalib[iv],          "BeamChargeCalib",      v, "integrated charge",       20, iv + 1);
    TGraphAttributePair(gMeanQCalib[iv],           "MeanChargeCalib",      v, "mean charge/event",       20, iv + 1);

    TGraphAttributePair(gNentriesCalib3s[iv],        "NentriesCalib3s",        v, "number of entries",       20, iv + 1);
    TGraphAttributePair(gSpotCalib3s[iv],            "BeamSpotCalib3s",        v, "beam spot [strip]",       20, iv + 1);
    TGraphAttributePair(gWidthCalib3s[iv],           "BeamSpreadCalib3s",      v, "beam width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthCoreCalib3s[iv],       "BeamCoreWidthCalib3s",   v, "core width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthTailCalib3s[iv],       "BeamTailWidthCalib3s",   v, "tail width [strip]",      20, iv + 1);
    TGraphAttributePair(gSingleWidthCoreCalib3s[iv], "SingleCoreWidthCalib3s", v, "single-core width [strip]", 20, iv + 1);
    TGraphAttributePair(gChargeCalib3s[iv],          "BeamChargeCalib3s",      v, "integrated charge",       20, iv + 1);
    TGraphAttributePair(gMeanQCalib3s[iv],           "MeanChargeCalib3s",      v, "mean charge/event",       20, iv + 1);

    TGraphAttributePair(gNentriesCalibOverall[iv],        "NentriesCalibOverall",        v, "number of entries",       20, iv + 1);
    TGraphAttributePair(gSpotCalibOverall[iv],            "BeamSpotCalibOverall",        v, "beam spot [strip]",       20, iv + 1);
    TGraphAttributePair(gWidthCalibOverall[iv],           "BeamSpreadCalibOverall",      v, "beam width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthCoreCalibOverall[iv],       "BeamCoreWidthCalibOverall",   v, "core width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthTailCalibOverall[iv],       "BeamTailWidthCalibOverall",   v, "tail width [strip]",      20, iv + 1);
    TGraphAttributePair(gSingleWidthCoreCalibOverall[iv], "SingleCoreWidthCalibOverall", v, "single-core width [strip]", 20, iv + 1);
    TGraphAttributePair(gChargeCalibOverall[iv],          "BeamChargeCalibOverall",      v, "integrated charge",       20, iv + 1);
    TGraphAttributePair(gMeanQCalibOverall[iv],           "MeanChargeCalibOverall",      v, "mean charge/event",       20, iv + 1);

    TGraphAttributePair(gNentriesCalib3sOverall[iv],        "NentriesCalib3sOverall",        v, "number of entries",       20, iv + 1);
    TGraphAttributePair(gSpotCalib3sOverall[iv],            "BeamSpotCalib3sOverall",        v, "beam spot [strip]",       20, iv + 1);
    TGraphAttributePair(gWidthCalib3sOverall[iv],           "BeamSpreadCalib3sOverall",      v, "beam width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthCoreCalib3sOverall[iv],       "BeamCoreWidthCalib3sOverall",   v, "core width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthTailCalib3sOverall[iv],       "BeamTailWidthCalib3sOverall",   v, "tail width [strip]",      20, iv + 1);
    TGraphAttributePair(gSingleWidthCoreCalib3sOverall[iv], "SingleCoreWidthCalib3sOverall", v, "single-core width [strip]", 20, iv + 1);
    TGraphAttributePair(gChargeCalib3sOverall[iv],          "BeamChargeCalib3sOverall",      v, "integrated charge",       20, iv + 1);
    TGraphAttributePair(gMeanQCalib3sOverall[iv],           "MeanChargeCalib3sOverall",      v, "mean charge/event",       20, iv + 1);
  }

  for (size_t i = 0; i < blocks.size(); ++i) {

    const int iblk = blocks[i];
    cout << "DEBUG: iblk = " << iblk << endl;
    TDirectory *dBIn = (TDirectory*)dBlocksIn->Get(Form("block_%04d", iblk));
    if (!dBIn) {
      cerr << "WARNING: missing input block directory block_" << setw(4) << setfill('0') << iblk << setfill(' ') << endl;
      continue;
    }

    TDirectory *dBRawIn   = (TDirectory*)dBIn->Get("Raw");
    TDirectory *dBCalIn   = (TDirectory*)dBIn->Get("Calib");
    TDirectory *dBCalOvIn = (TDirectory*)dBIn->Get("Calib_overall");
    if (!dBRawIn || !dBCalIn || !dBCalOvIn) {
      cerr << "WARNING: bad input subdirectories for block " << iblk << endl;
      continue;
    }

    TDirectory *dBlocksOut = fout->mkdir(Form("block%04d", iblk));
    if (!dBlocksOut) {
      cerr << "ERROR: cannot create/get output block directory " << iblk << endl;
      continue;
    }
    dBlocksOut->cd();

    TDirectory *dBlocksRawOut        = dBlocksOut->mkdir("Raw");
    TDirectory *dBlocksCalOut        = dBlocksOut->mkdir("Calib");
    TDirectory *dBlocksCalOverallOut = dBlocksOut->mkdir("Calib_overall");
    if (!dBlocksRawOut || !dBlocksCalOut || !dBlocksCalOverallOut) {
      cerr << "ERROR: cannot create/get output subdirectories for block " << iblk << endl;
      continue;
    }

    for (int iv = 0; iv < TMMCH_N_Readout; ++iv) {

      TString v = views[iv];

      TH2F *h2RawBlock = GetH2F(dBRawIn, Form("hBlockqmaxstripFull%s_block%04d", v.Data(), iblk));
      TH2F *h2CalBlock = GetH2F(dBCalIn,Form("hBlockqmaxstripFull_cal%s_block%04d", v.Data(), iblk));
      TH2F *h2Cal3sBlock = GetH2F(dBCalIn,Form("hBlockqmaxstripFull_cal3s%s_block%04d", v.Data(), iblk));
      TH2F *h2CalOverallBlock = GetH2F(dBCalOvIn,Form("hBlockqmaxstripFull_Overallcal%s_block%04d", v.Data(), iblk));
      TH2F *h2Cal3sOverallBlock = GetH2F(dBCalOvIn,Form("hBlockqmaxstripFull_Overallcal3s%s_block%04d", v.Data(), iblk));

      BlockRawObs[iv][0].push_back(ComputeObservables(h2RawBlock, v));
      BlockRawObs[iv][1].push_back(ComputeObservablesFitSlice(h2RawBlock, v));

      BlockCalibObs[iv][0].push_back(ComputeObservables(h2CalBlock, v));
      BlockCalibObs[iv][1].push_back(ComputeObservablesFitSlice(h2CalBlock, v));

      BlockCalib3sObs[iv][0].push_back(ComputeObservables(h2Cal3sBlock, v));
      BlockCalib3sObs[iv][1].push_back(ComputeObservablesFitSlice(h2Cal3sBlock, v));

      BlockCalibOverallObs[iv][0].push_back(ComputeObservables(h2CalOverallBlock, v));
      BlockCalibOverallObs[iv][1].push_back(ComputeObservablesFitSlice(h2CalOverallBlock, v));

      BlockCalib3sOverallObs[iv][0].push_back(ComputeObservables(h2Cal3sOverallBlock, v));
      BlockCalib3sOverallObs[iv][1].push_back(ComputeObservablesFitSlice(h2Cal3sOverallBlock, v));

      dBlocksOut->cd();
      WriteSliceFit(h2RawBlock, Form("Raw_%s_block%04d", v.Data(), iblk), dBlocksRawOut);
      WriteSliceFit(h2CalBlock, Form("Calib_%s_block%04d", v.Data(), iblk), dBlocksCalOut);
      WriteSliceFit(h2Cal3sBlock, Form("Calib3s_%s_block%04d", v.Data(), iblk), dBlocksCalOut);
      WriteSliceFit(h2CalOverallBlock, Form("CalibOverall_%s_block%04d", v.Data(), iblk), dBlocksCalOverallOut);
      WriteSliceFit(h2Cal3sOverallBlock, Form("Calib3sOverall_%s_block%04d", v.Data(), iblk), dBlocksCalOverallOut);

      FillGraphPair(gNentriesRaw[iv],
          gSpotRaw[iv],
          gWidthRaw[iv], gWidthCoreRaw[iv], gWidthTailRaw[iv], gSingleWidthCoreRaw[iv],
          gChargeRaw[iv], gMeanQRaw[iv],
          i, iblk,
          BlockRawObs[iv]
      );

      FillGraphPair(gNentriesCalib[iv],
          gSpotCalib[iv],
          gWidthCalib[iv], gWidthCoreCalib[iv], gWidthTailCalib[iv], gSingleWidthCoreCalib[iv],
          gChargeCalib[iv], gMeanQCalib[iv],
          i, iblk,
          BlockCalibObs[iv]
      );

      FillGraphPair(gNentriesCalib3s[iv],
          gSpotCalib3s[iv],
          gWidthCalib3s[iv], gWidthCoreCalib3s[iv], gWidthTailCalib3s[iv], gSingleWidthCoreCalib3s[iv],
          gChargeCalib3s[iv], gMeanQCalib3s[iv],
          i, iblk,
          BlockCalib3sObs[iv]
      );

      FillGraphPair(gNentriesCalibOverall[iv],
          gSpotCalibOverall[iv],
          gWidthCalibOverall[iv], gWidthCoreCalibOverall[iv], gWidthTailCalibOverall[iv], gSingleWidthCoreCalibOverall[iv],
          gChargeCalibOverall[iv], gMeanQCalibOverall[iv],
          i, iblk,
          BlockCalibOverallObs[iv]
      );

      FillGraphPair(gNentriesCalib3sOverall[iv],
          gSpotCalib3sOverall[iv],
          gWidthCalib3sOverall[iv], gWidthCoreCalib3sOverall[iv], gWidthTailCalib3sOverall[iv], gSingleWidthCoreCalib3sOverall[iv],
          gChargeCalib3sOverall[iv], gMeanQCalib3sOverall[iv],
          i, iblk,
          BlockCalib3sOverallObs[iv]
      );
    }
  }

  // do the comparison analysis to get the final plots
  // ratio wrt to the overall measurements (overall analysis) to verify the stability of the calibration procedure in time
  // TMultiGraph to se by eye the effect of the calibration
  // afterwards --> systematics evaluation

  TGraphErrors *gNentriesRawRatio[TMMCH_N_Readout][2];
  TGraphErrors *gSpotRawRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthRawRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCoreRawRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthTailRawRatio[TMMCH_N_Readout][2];
  TGraphErrors *gSingleWidthCoreRawRatio[TMMCH_N_Readout][2];
  TGraphErrors *gChargeRawRatio[TMMCH_N_Readout][2];
  TGraphErrors *gMeanQRawRatio[TMMCH_N_Readout][2];

  TGraphErrors *gNentriesCalibRatio[TMMCH_N_Readout][2];
  TGraphErrors *gSpotCalibRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCalibRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCoreCalibRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthTailCalibRatio[TMMCH_N_Readout][2];
  TGraphErrors *gSingleWidthCoreCalibRatio[TMMCH_N_Readout][2];
  TGraphErrors *gChargeCalibRatio[TMMCH_N_Readout][2];
  TGraphErrors *gMeanQCalibRatio[TMMCH_N_Readout][2];

  TGraphErrors *gNentriesCalib3sRatio[TMMCH_N_Readout][2];
  TGraphErrors *gSpotCalib3sRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCalib3sRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCoreCalib3sRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthTailCalib3sRatio[TMMCH_N_Readout][2];
  TGraphErrors *gSingleWidthCoreCalib3sRatio[TMMCH_N_Readout][2];
  TGraphErrors *gChargeCalib3sRatio[TMMCH_N_Readout][2];
  TGraphErrors *gMeanQCalib3sRatio[TMMCH_N_Readout][2];

  TGraphErrors *gNentriesCalibOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gSpotCalibOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCalibOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCoreCalibOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthTailCalibOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gSingleWidthCoreCalibOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gChargeCalibOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gMeanQCalibOverallRatio[TMMCH_N_Readout][2];

  TGraphErrors *gNentriesCalib3sOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gSpotCalib3sOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCalib3sOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCoreCalib3sOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthTailCalib3sOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gSingleWidthCoreCalib3sOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gChargeCalib3sOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gMeanQCalib3sOverallRatio[TMMCH_N_Readout][2];
  
  vector<ObservableResult> ObsRawRatio[TMMCH_N_Readout][2];
  vector<ObservableResult> ObsCalibRatio[TMMCH_N_Readout][2];
  vector<ObservableResult> ObsCalib3sRatio[TMMCH_N_Readout][2];
  vector<ObservableResult> ObsCalibOverallRatio[TMMCH_N_Readout][2];
  vector<ObservableResult> ObsCalib3sOverallRatio[TMMCH_N_Readout][2];
  for (int iv = 0; iv < TMMCH_N_Readout; ++iv) {
    TString v = views[iv];

    InitGraphPair(gNentriesRawRatio[iv]);
    InitGraphPair(gSpotRawRatio[iv]);
    InitGraphPair(gWidthRawRatio[iv]);
    InitGraphPair(gWidthCoreRawRatio[iv]);
    InitGraphPair(gWidthTailRawRatio[iv]);
    InitGraphPair(gSingleWidthCoreRawRatio[iv]);
    InitGraphPair(gChargeRawRatio[iv]);
    InitGraphPair(gMeanQRawRatio[iv]);

    InitGraphPair(gNentriesCalibRatio[iv]);
    InitGraphPair(gSpotCalibRatio[iv]);
    InitGraphPair(gWidthCalibRatio[iv]);
    InitGraphPair(gWidthCoreCalibRatio[iv]);
    InitGraphPair(gWidthTailCalibRatio[iv]);
    InitGraphPair(gSingleWidthCoreCalibRatio[iv]);
    InitGraphPair(gChargeCalibRatio[iv]);
    InitGraphPair(gMeanQCalibRatio[iv]);

    InitGraphPair(gNentriesCalib3sRatio[iv]);
    InitGraphPair(gSpotCalib3sRatio[iv]);
    InitGraphPair(gWidthCalib3sRatio[iv]);
    InitGraphPair(gWidthCoreCalib3sRatio[iv]);
    InitGraphPair(gWidthTailCalib3sRatio[iv]);
    InitGraphPair(gSingleWidthCoreCalib3sRatio[iv]);
    InitGraphPair(gChargeCalib3sRatio[iv]);
    InitGraphPair(gMeanQCalib3sRatio[iv]);

    InitGraphPair(gNentriesCalibOverallRatio[iv]);
    InitGraphPair(gSpotCalibOverallRatio[iv]);
    InitGraphPair(gWidthCalibOverallRatio[iv]);
    InitGraphPair(gWidthCoreCalibOverallRatio[iv]);
    InitGraphPair(gWidthTailCalibOverallRatio[iv]);
    InitGraphPair(gSingleWidthCoreCalibOverallRatio[iv]);
    InitGraphPair(gChargeCalibOverallRatio[iv]);
    InitGraphPair(gMeanQCalibOverallRatio[iv]);

    InitGraphPair(gNentriesCalib3sOverallRatio[iv]);
    InitGraphPair(gSpotCalib3sOverallRatio[iv]);
    InitGraphPair(gWidthCalib3sOverallRatio[iv]);
    InitGraphPair(gWidthCoreCalib3sOverallRatio[iv]);
    InitGraphPair(gWidthTailCalib3sOverallRatio[iv]);
    InitGraphPair(gSingleWidthCoreCalib3sOverallRatio[iv]);
    InitGraphPair(gChargeCalib3sOverallRatio[iv]);
    InitGraphPair(gMeanQCalib3sOverallRatio[iv]);

    TGraphAttributePair(gNentriesRawRatio[iv],        "NentriesRawRatio",        v, "number of entries",       20, iv + 1);
    TGraphAttributePair(gSpotRawRatio[iv],            "BeamSpotRawRatio",        v, "beam spot [strip]",       20, iv + 1);
    TGraphAttributePair(gWidthRawRatio[iv],           "BeamSpreadRawRatio",      v, "beam width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthCoreRawRatio[iv],       "BeamCoreWidthRawRatio",   v, "core width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthTailRawRatio[iv],       "BeamTailWidthRawRatio",   v, "tail width [strip]",      20, iv + 1);
    TGraphAttributePair(gSingleWidthCoreRawRatio[iv], "SingleCoreWidthRawRatio", v, "single-core width [strip]", 20, iv + 1);
    TGraphAttributePair(gChargeRawRatio[iv],          "BeamChargeRawRatio",      v, "integrated charge",       20, iv + 1);
    TGraphAttributePair(gMeanQRawRatio[iv],           "MeanChargeRawRatio",      v, "mean charge/event",       20, iv + 1);

    TGraphAttributePair(gNentriesCalibRatio[iv],        "NentriesCalibRatio",        v, "number of entries",       20, iv + 1);
    TGraphAttributePair(gSpotCalibRatio[iv],            "BeamSpotCalibRatio",        v, "beam spot [strip]",       20, iv + 1);
    TGraphAttributePair(gWidthCalibRatio[iv],           "BeamSpreadCalibRatio",      v, "beam width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthCoreCalibRatio[iv],       "BeamCoreWidthCalibRatio",   v, "core width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthTailCalibRatio[iv],       "BeamTailWidthCalibRatio",   v, "tail width [strip]",      20, iv + 1);
    TGraphAttributePair(gSingleWidthCoreCalibRatio[iv], "SingleCoreWidthCalibRatio", v, "single-core width [strip]", 20, iv + 1);
    TGraphAttributePair(gChargeCalibRatio[iv],          "BeamChargeCalibRatio",      v, "integrated charge",       20, iv + 1);
    TGraphAttributePair(gMeanQCalibRatio[iv],           "MeanChargeCalibRatio",      v, "mean charge/event",       20, iv + 1);

    TGraphAttributePair(gNentriesCalib3sRatio[iv],        "NentriesCalib3sRatio",        v, "number of entries",       20, iv + 1);
    TGraphAttributePair(gSpotCalib3sRatio[iv],            "BeamSpotCalib3sRatio",        v, "beam spot [strip]",       20, iv + 1);
    TGraphAttributePair(gWidthCalib3sRatio[iv],           "BeamSpreadCalib3sRatio",      v, "beam width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthCoreCalib3sRatio[iv],       "BeamCoreWidthCalib3sRatio",   v, "core width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthTailCalib3sRatio[iv],       "BeamTailWidthCalib3sRatio",   v, "tail width [strip]",      20, iv + 1);
    TGraphAttributePair(gSingleWidthCoreCalib3sRatio[iv], "SingleCoreWidthCalib3sRatio", v, "single-core width [strip]", 20, iv + 1);
    TGraphAttributePair(gChargeCalib3sRatio[iv],          "BeamChargeCalib3sRatio",      v, "integrated charge",       20, iv + 1);
    TGraphAttributePair(gMeanQCalib3sRatio[iv],           "MeanChargeCalib3sRatio",      v, "mean charge/event",       20, iv + 1);

    TGraphAttributePair(gNentriesCalibOverallRatio[iv],        "NentriesCalibOverallRatio",        v, "number of entries",       20, iv + 1);
    TGraphAttributePair(gSpotCalibOverallRatio[iv],            "BeamSpotCalibOverallRatio",        v, "beam spot [strip]",       20, iv + 1);
    TGraphAttributePair(gWidthCalibOverallRatio[iv],           "BeamSpreadCalibOverallRatio",      v, "beam width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthCoreCalibOverallRatio[iv],       "BeamCoreWidthCalibOverallRatio",   v, "core width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthTailCalibOverallRatio[iv],       "BeamTailWidthCalibOverallRatio",   v, "tail width [strip]",      20, iv + 1);
    TGraphAttributePair(gSingleWidthCoreCalibOverallRatio[iv], "SingleCoreWidthCalibOverallRatio", v, "single-core width [strip]", 20, iv + 1);
    TGraphAttributePair(gChargeCalibOverallRatio[iv],          "BeamChargeCalibOverallRatio",      v, "integrated charge",       20, iv + 1);
    TGraphAttributePair(gMeanQCalibOverallRatio[iv],           "MeanChargeCalibOverallRatio",      v, "mean charge/event",       20, iv + 1);

    TGraphAttributePair(gNentriesCalib3sOverallRatio[iv],        "NentriesCalib3sOverallRatio",        v, "number of entries",       20, iv + 1);
    TGraphAttributePair(gSpotCalib3sOverallRatio[iv],            "BeamSpotCalib3sOverallRatio",        v, "beam spot [strip]",       20, iv + 1);
    TGraphAttributePair(gWidthCalib3sOverallRatio[iv],           "BeamSpreadCalib3sOverallRatio",      v, "beam width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthCoreCalib3sOverallRatio[iv],       "BeamCoreWidthCalib3sOverallRatio",   v, "core width [strip]",      20, iv + 1);
    TGraphAttributePair(gWidthTailCalib3sOverallRatio[iv],       "BeamTailWidthCalib3sOverallRatio",   v, "tail width [strip]",      20, iv + 1);
    TGraphAttributePair(gSingleWidthCoreCalib3sOverallRatio[iv], "SingleCoreWidthCalib3sOverallRatio", v, "single-core width [strip]", 20, iv + 1);
    TGraphAttributePair(gChargeCalib3sOverallRatio[iv],          "BeamChargeCalib3sOverallRatio",      v, "integrated charge",       20, iv + 1);
    TGraphAttributePair(gMeanQCalib3sOverallRatio[iv],           "MeanChargeCalib3sOverallRatio",      v, "mean charge/event",       20, iv + 1);

  }

  for (size_t i = 0; i < blocks.size(); ++i) {
    const int iblk = blocks[i];   

    for (int iv = 0; iv < TMMCH_N_Readout; ++iv) {
      TString v = views[iv];

      ObsRawRatio[iv][0].push_back(DoObsRatio(RawObs[iv][0], BlockRawObs[iv][0], iblk));
      ObsRawRatio[iv][1].push_back(DoObsRatio(RawObs[iv][1], BlockRawObs[iv][1], iblk));
      ObsCalibRatio[iv][0].push_back(DoObsRatio(CalibObs[iv][0], BlockCalibObs[iv][0], iblk));
      ObsCalibRatio[iv][1].push_back(DoObsRatio(CalibObs[iv][1], BlockCalibObs[iv][1], iblk));
      ObsCalib3sRatio[iv][0].push_back(DoObsRatio(Calib3sObs[iv][0], BlockCalib3sObs[iv][0], iblk));
      ObsCalib3sRatio[iv][1].push_back(DoObsRatio(Calib3sObs[iv][1], BlockCalib3sObs[iv][1], iblk));
      ObsCalibOverallRatio[iv][0].push_back(DoObsRatio(CalibObs[iv][0], BlockCalibOverallObs[iv][0], iblk));
      ObsCalibOverallRatio[iv][1].push_back(DoObsRatio(CalibObs[iv][1], BlockCalibOverallObs[iv][1], iblk));
      ObsCalib3sOverallRatio[iv][0].push_back(DoObsRatio(Calib3sObs[iv][0], BlockCalib3sOverallObs[iv][0], iblk));
      ObsCalib3sOverallRatio[iv][1].push_back(DoObsRatio(Calib3sObs[iv][1], BlockCalib3sOverallObs[iv][1], iblk));

      FillGraphPair(gNentriesRawRatio[iv],
          gSpotRawRatio[iv],
          gWidthRawRatio[iv], gWidthCoreRawRatio[iv], gWidthTailRawRatio[iv], gSingleWidthCoreRawRatio[iv],
          gChargeRawRatio[iv], gMeanQRawRatio[iv],
          i, iblk,
          ObsRawRatio[iv]
      );

      FillGraphPair(gNentriesCalibRatio[iv],
          gSpotCalibRatio[iv],
          gWidthCalibRatio[iv], gWidthCoreCalibRatio[iv], gWidthTailCalibRatio[iv], gSingleWidthCoreCalibRatio[iv],
          gChargeCalibRatio[iv], gMeanQCalibRatio[iv],
          i, iblk,
          ObsCalibRatio[iv]
      );

      FillGraphPair(gNentriesCalib3sRatio[iv],
          gSpotCalib3sRatio[iv],
          gWidthCalib3sRatio[iv], gWidthCoreCalib3sRatio[iv], gWidthTailCalib3sRatio[iv], gSingleWidthCoreCalib3sRatio[iv],
          gChargeCalib3sRatio[iv], gMeanQCalib3sRatio[iv],
          i, iblk,
          ObsCalib3sRatio[iv]
      );
      
      FillGraphPair(gNentriesCalibOverallRatio[iv],
          gSpotCalibOverallRatio[iv],
          gWidthCalibOverallRatio[iv], gWidthCoreCalibOverallRatio[iv], gWidthTailCalibOverallRatio[iv], gSingleWidthCoreCalibOverallRatio[iv],
          gChargeCalibOverallRatio[iv], gMeanQCalibOverallRatio[iv],
          i, iblk,
          ObsCalibOverallRatio[iv]
      );

      FillGraphPair(gNentriesCalib3sOverallRatio[iv],
          gSpotCalib3sOverallRatio[iv],
          gWidthCalib3sOverallRatio[iv], gWidthCoreCalib3sOverallRatio[iv], gWidthTailCalib3sOverallRatio[iv], gSingleWidthCoreCalib3sOverallRatio[iv],
          gChargeCalib3sOverallRatio[iv], gMeanQCalib3sOverallRatio[iv],
          i, iblk,
          ObsCalib3sOverallRatio[iv]
      );
    
    }
  }

  fout->cd();
  dGraphsOut->cd();

  WriteMeasurementGraphs(
    dGraphsRawOut,
    gNentriesRaw,
    gSpotRaw,
    gWidthRaw,
    gWidthCoreRaw,
    gWidthTailRaw,
    gSingleWidthCoreRaw,
    gChargeRaw,
    gMeanQRaw
  );

  WriteMeasurementGraphs(
    dGraphsCalibOut,
    gNentriesCalib,
    gSpotCalib,
    gWidthCalib,
    gWidthCoreCalib,
    gWidthTailCalib,
    gSingleWidthCoreCalib,
    gChargeCalib,
    gMeanQCalib
  );

  WriteMeasurementGraphs(
    dGraphsCalib3sOut,
    gNentriesCalib3s,
    gSpotCalib3s,
    gWidthCalib3s,
    gWidthCoreCalib3s,
    gWidthTailCalib3s,
    gSingleWidthCoreCalib3s,
    gChargeCalib3s,
    gMeanQCalib3s
  );

  WriteMeasurementGraphs(
    dGraphsCalibOverallOut,
    gNentriesCalibOverall,
    gSpotCalibOverall,
    gWidthCalibOverall,
    gWidthCoreCalibOverall,
    gWidthTailCalibOverall,
    gSingleWidthCoreCalibOverall,
    gChargeCalibOverall,
    gMeanQCalibOverall
  );

  WriteMeasurementGraphs(
    dGraphsCalib3sOverallOut,
    gNentriesCalib3sOverall,
    gSpotCalib3sOverall,
    gWidthCalib3sOverall,
    gWidthCoreCalib3sOverall,
    gWidthTailCalib3sOverall,
    gSingleWidthCoreCalib3sOverall,
    gChargeCalib3sOverall,
    gMeanQCalib3sOverall
  );

WriteMeasurementGraphs(
  dRatioRawOut,
  gNentriesRawRatio,
  gSpotRawRatio,
  gWidthRawRatio,
  gWidthCoreRawRatio,
  gWidthTailRawRatio,
  gSingleWidthCoreRawRatio,
  gChargeRawRatio,
  gMeanQRawRatio
);

  WriteMeasurementGraphs(
    dRatioCalibOut,
    gNentriesCalibRatio,
    gSpotCalibRatio,
    gWidthCalibRatio,
    gWidthCoreCalibRatio,
    gWidthTailCalibRatio,
    gSingleWidthCoreCalibRatio,
    gChargeCalibRatio,
    gMeanQCalibRatio
  );

  WriteMeasurementGraphs(
    dRatioCalib3sOut,
    gNentriesCalib3sRatio,
    gSpotCalib3sRatio,
    gWidthCalib3sRatio,
    gWidthCoreCalib3sRatio,
    gWidthTailCalib3sRatio,
    gSingleWidthCoreCalib3sRatio,
    gChargeCalib3sRatio,
    gMeanQCalib3sRatio
  );

  WriteMeasurementGraphs(
    dRatioCalibOverallOut,
    gNentriesCalibOverallRatio,
    gSpotCalibOverallRatio,
    gWidthCalibOverallRatio,
    gWidthCoreCalibOverallRatio,
    gWidthTailCalibOverallRatio,
    gSingleWidthCoreCalibOverallRatio,
    gChargeCalibOverallRatio,
    gMeanQCalibOverallRatio
  );

  WriteMeasurementGraphs(
    dRatioCalib3sOverallOut,
    gNentriesCalib3sOverallRatio,
    gSpotCalib3sOverallRatio,
    gWidthCalib3sOverallRatio,
    gWidthCoreCalib3sOverallRatio,
    gWidthTailCalib3sOverallRatio,
    gSingleWidthCoreCalib3sOverallRatio,
    gChargeCalib3sOverallRatio,
    gMeanQCalib3sOverallRatio
  );

  // fout->Write();
  fout->Close();
  fin->Close();

  cout << "AnalysisTMM output written to " << outName << endl;

}