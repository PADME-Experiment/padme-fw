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
#include <TMatrixDSym.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
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
#define NEvtBLOCKS 1000 // this quantity must be passed as a parameter or read from somewhere else

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

  int fitStatus = -9999;
  int covStatus = -9999;
  bool fitAccepted = false;
};

void SetBadObservableResult(ObservableResult &obs){
  obs.spot = -99.;
  obs.spotErr = -99.;
  obs.width = -99.;
  obs.widthErr = -99.;
  obs.widthCore = -99.;
  obs.widthCoreErr = -99.;
  obs.widthTail = -99.;
  obs.widthTailErr = -99.;
  obs.widthSingleCore = -99.;
  obs.widthSingleCoreErr = -99.;
  obs.charge = -99.;
  obs.chargeErr = -99.;

  int fitStatus = -99;
  int covStatus = -99;
  bool fitAccepted = false;
}

int TGraphAttribute(TGraphErrors *Graph, TString title, TString xlabel, TString ylabel, int markerstyle, int color){
  Graph->SetTitle(title);
  Graph->SetName(title);
  Graph->GetXaxis()->SetTitle(xlabel);
  Graph->GetYaxis()->SetTitle(ylabel);
  Graph->SetMarkerStyle(markerstyle);
  Graph->SetMarkerColor(color);
  return 0;
}

TH2F* GetH2F(TDirectory *dir, const TString &name){
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

vector<int> FindBlockNumbers(TDirectory *dir){
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

double PropagateError(TMatrixDSym &cov, const double grad[5]){
  double var = 0.;
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      var += grad[i] * cov(i, j) * grad[j];
    }
  }
  return sqrt(max(0.0, var));
}

double RatioValue(double num, double den){
  if (den == 0.) return -9999.;
  if (num == -9999. || den == -9999.) return -9999.;
  return num / den;
}

double RatioError(double num, double numErr, double den, double denErr){
  if (den == 0.) return -9999.;
  if (num == -9999. || den == -9999.) return -9999.;
  if (numErr < 0. || denErr < 0.) return -9999.;

  return sqrt(
    pow(numErr / den, 2) +
    pow(num * denErr / (den * den), 2)
  );
}

double ClampToLimits(double x, double low, double up){
  if (x < low) return 0.5 * (low + up);
  if (x > up) return 0.5 * (low + up);
  return x;
}

double VoigtIntegralPDF(double *x, double *par){
  const double I     = par[0];          // total integral
  const double mu    = par[1];
  const double sigma = fabs(par[2]);    // Gaussian sigma
  const double gamma = fabs(par[3]);    // Lorentzian width

  if (sigma <= 0. || gamma <= 0.) return 0.;

  return I * TMath::Voigt(x[0] - mu, sigma, gamma);
}

TF1* FitDoubleGaussian(TH1D *h, TString name, double xmin, double xmax, TFitResultPtr &fitResult){
  fitResult = TFitResultPtr();

  if (!h) {
    cerr << "ERROR: FitDoubleGaussian received null histogram: " << name << endl;
    return nullptr;
  }

  if (h->GetEntries() <= 0 || h->Integral() <= 0.) {
    cerr << "WARNING: FitDoubleGaussian skipped empty histogram: " << h->GetName() << " view = " << name << endl;
    return nullptr;
  }

  if (xmax <= xmin) {
    cerr << "ERROR: FitDoubleGaussian invalid fit range for " << h->GetName() << " tag = " << name << " xmin = " << xmin << " xmax = " << xmax << endl;
    return nullptr;
  }

  const int maxBin = h->GetMaximumBin();
  const double maxContent = h->GetBinContent(maxBin);

  if (maxContent <= 0.) {
    cerr << "WARNING: FitDoubleGaussian skipped histogram with non-positive maximum: " << h->GetName() << " view = " << name << endl;
    return nullptr;
  }

  TF1 *prefit = new TF1(Form("prefit_%s", name.Data()), "gaus", xmin, xmax);

  TFitResultPtr prefitResult = h->Fit(prefit, "RQS0");

  double amp0 = prefit->GetParameter(0);
  double mu0  = prefit->GetParameter(1);
  double s0   = fabs(prefit->GetParameter(2));

  if ((int)prefitResult != 0) {
    cerr << "WARNING: Gaussian prefit failed for " << h->GetName() << " view =" << name << " status =" << (int)prefitResult << ". Using histogram maximum/RMS seeds." << endl;
    amp0 = maxContent;
    mu0  = h->GetBinCenter(maxBin);
    s0   = h->GetRMS();
  }

  if (amp0 <= 0.) amp0 = maxContent;
  if (mu0 < xmin || mu0 > xmax) mu0 = h->GetBinCenter(maxBin);
  if (s0 <= 0.) s0 = 0.1 * (xmax - xmin);

  const double xrange = xmax - xmin;

  if (s0 <= 0. || xrange <= 0.) {
    cerr << "ERROR: FitDoubleGaussian invalid seed/range for " << h->GetName() << " tag=" << name << " s0=" << s0 << " xrange=" << xrange << endl;
    delete prefit;
    return nullptr;
  }

  const double area0 = max(1.0, amp0 * sqrt(2. * TMath::Pi()) * s0);

  TF1 *fit = new TF1(
    Form("f_%s", name.Data()),
    "[0]/(sqrt(2*TMath::Pi())*[2])*exp(-0.5*((x-[1])/[2])^2)"
    "+[3]/(sqrt(2*TMath::Pi())*[4])*exp(-0.5*((x-[1])/[4])^2)",
    xmin,
    xmax
  );

  fit->SetParNames("I_1", "mean", "sigma_1", "I_2", "sigma_2");

  const double I_low = 0.;
  const double I_up  = max(1.0, 10. * area0);

  const double s_low = 0.1;
  const double s1_up = max(s_low * 2., xrange);
  const double s2_up = max(s_low * 2., 5. * xrange);

  double I1_0 = ClampToLimits(0.7 * area0, I_low, I_up);
  double I2_0 = ClampToLimits(0.3 * area0, I_low, I_up);
  double s1_0 = ClampToLimits(0.7 * s0,   s_low, s1_up);
  double s2_0 = ClampToLimits(2.0 * s0,   s_low, s2_up);
  double mu_0 = ClampToLimits(mu0,        xmin,  xmax);

  fit->SetParameters(I1_0, mu_0, s1_0, I2_0, s2_0);

  fit->SetParLimits(0, I_low, I_up);
  fit->SetParLimits(1, xmin, xmax);
  fit->SetParLimits(2, s_low, s1_up);
  fit->SetParLimits(3, I_low, I_up);
  fit->SetParLimits(4, s_low, s2_up);

  fitResult = h->Fit(fit, "RQS");

  const int status = (int)fitResult;

  if (status > 1) {
    cerr << "WARNING: Double Gaussian fit rejected for " << h->GetName() << " view =" << name << " status =" << status << endl;
    // delete prefit;
    // delete fit;
    // fitResult = TFitResultPtr();
    // return nullptr;
  }

  if (status == 1) {
    cerr << "WARNING: Double Gaussian fit accepted with status = 1 for " << h->GetName() << " view =" << name << "; covariance matrix may be non-ideal" << endl;
  }

  if (!fitResult.Get() || !fitResult->IsValid()) {
    cerr << "WARNING: Double Gaussian fit result is not valid for " << h->GetName() << " view = " << name << endl;

    // delete prefit;
    // delete fit;
    // fitResult = TFitResultPtr();
    // return nullptr;
  }

  const int covStatus = fitResult->CovMatrixStatus();

  if (covStatus < 2) {
    cerr << "WARNING: Double Gaussian covariance matrix rejected for " << h->GetName() << " view = " << name << " CovMatrixStatus = " << covStatus << endl;

    // delete prefit;
    // delete fit;
    // fitResult = TFitResultPtr();
    // return nullptr;
  }

  if (covStatus == 2) {
    cerr << "WARNING: Double Gaussian covariance matrix accepted with CovMatrixStatus = 2 for " << h->GetName() << " view = " << name << " ; forced positive definite covariance" << endl;
  }
  delete prefit;
  return fit;
}

TF1* FitVoigt(TH1D *h, TString name, double xmin, double xmax, TFitResultPtr &fitResult){
  if (!h) return nullptr;

  TF1 *prefit = new TF1(Form("prefitVoigt_%s", name.Data()), "gaus", xmin, xmax);
  h->Fit(prefit, "RQ0");

  double amp0 = prefit->GetParameter(0);
  double mu0  = prefit->GetParameter(1);
  double s0   = fabs(prefit->GetParameter(2));

  if (amp0 <= 0.) amp0 = h->GetMaximum();
  if (mu0 < xmin || mu0 > xmax) mu0 = h->GetBinCenter(h->GetMaximumBin());
  if (s0 <= 0.) s0 = 0.1 * (xmax - xmin);

  const double area0 = amp0 * sqrt(2. * TMath::Pi()) * s0;
  const double xrange = xmax - xmin;

  TF1 *fit = new TF1(Form("fVoigt_%s", name.Data()), VoigtIntegralPDF, xmin, xmax, 4);
  fit->SetParNames("I", "mean", "sigmaG", "gammaL");

  const double I_low = 0.;
  const double I_up  = max(1.0, 10. * area0);

  const double s_low = 0.1;
  const double s_up  = max(2. * s_low, xrange);

  const double g_low = 0.001;
  const double g_up  = max(2. * g_low, xrange);

  double I_0 = ClampToLimits(area0, I_low, I_up);
  double s_0 = ClampToLimits(s0, s_low, s_up);
  double g_0 = ClampToLimits(0.5 * s0, g_low, g_up);

  fit->SetParameters(I_0, mu0, s_0, g_0);

  fit->SetParLimits(0, I_low, I_up);
  fit->SetParLimits(1, xmin, xmax);
  fit->SetParLimits(2, s_low, s_up);
  fit->SetParLimits(3, g_low, g_up);

  fitResult = h->Fit(fit, "RQS");

  delete prefit;
  return fit;
}

bool IsFitAccepted(TFitResultPtr fitResult, int maxFitStatusAccepted = 1,int minCovMatrixStatusAccepted = 2){
  if (!fitResult.Get()) return false;
  const int status = (int)fitResult;
  const int covStatus = fitResult->CovMatrixStatus();
  if (status > maxFitStatusAccepted) return false;
  if (covStatus < minCovMatrixStatusAccepted) return false;
  return true;
}

void ExtractDoubleGaussianObservables(TF1 *f, TFitResultPtr fitResult, ObservableResult &obs){
  if (!f) return;

  const double I1 = f->GetParameter(0);
  const double mu = f->GetParameter(1);
  const double s1 = fabs(f->GetParameter(2));
  const double I2 = f->GetParameter(3);
  const double s2 = fabs(f->GetParameter(4));

  TMatrixDSym cov = fitResult->GetCovarianceMatrix();

  obs.spot = mu;
  obs.spotErr = sqrt(max(0.0, cov(1, 1)));

  obs.charge = I1 + I2;

  double gradCharge[5] = {1., 0., 0., 1., 0.};
  obs.chargeErr = PropagateError(cov, gradCharge);

  obs.widthCore = s1;
  obs.widthCoreErr = sqrt(max(0.0, cov(2, 2)));

  obs.widthTail = s2;
  obs.widthTailErr = sqrt(max(0.0, cov(4, 4)));

  if (s2 < s1) {
    obs.widthCore = s2;
    obs.widthCoreErr = sqrt(max(0.0, cov(4, 4)));

    obs.widthTail = s1;
    obs.widthTailErr = sqrt(max(0.0, cov(2, 2)));
  }

  const double den = I1 + I2;
  const double num = I1 * s1 * s1 + I2 * s2 * s2;

  if (den > 0. && num > 0.) {
    obs.width = sqrt(num / den);

    double gradWidth[5] = {0., 0., 0., 0., 0.};

    const double w = obs.width;

    gradWidth[0] = 0.5 / w * ((s1 * s1 * den - num) / (den * den));
    gradWidth[2] = 0.5 / w * (2. * I1 * s1 / den);

    gradWidth[3] = 0.5 / w * ((s2 * s2 * den - num) / (den * den));
    gradWidth[4] = 0.5 / w * (2. * I2 * s2 / den);

    obs.widthErr = PropagateError(cov, gradWidth);
  }
}

void ExtractVoigtObservables(TF1 *f, TFitResultPtr fitResult, ObservableResult &obs){
  if (!f) return;

  const double I     = f->GetParameter(0);
  const double mu    = f->GetParameter(1);
  const double sigma = fabs(f->GetParameter(2));
  const double gamma = fabs(f->GetParameter(3));

  TMatrixDSym cov = fitResult->GetCovarianceMatrix();

  obs.charge = I;
  obs.chargeErr = sqrt(max(0.0, cov(0, 0)));

  obs.spot = mu;
  obs.spotErr = sqrt(max(0.0, cov(1, 1)));

  obs.widthCore = sigma;
  obs.widthCoreErr = sqrt(max(0.0, cov(2, 2)));

  obs.widthTail = gamma;
  obs.widthTailErr = sqrt(max(0.0, cov(3, 3)));

  // Practical Voigt FWHM approximation:
  const double fG = 2.354820045 * sigma;
  const double fL = 2.0 * gamma;

  // Olivero and Longbothum FWHM approximation --> accuracy of 0.02% [Journal of Quantitative Spectroscopy and Radiative Transfer. 17 (2): 233–236]
  const double fV = 0.5346 * fL + sqrt(0.2166 * fL * fL + fG * fG);

  obs.width = fV / 2.354820045;

  // Error propagation with covariance between sigma and gamma
  const double D = sqrt(0.2166 * fL * fL + fG * fG);

  double grad[4] = {0., 0., 0., 0.};

  const double dfG_dsigma = 2.354820045;
  const double dfL_dgamma = 2.0;

  const double dfV_dfG = fG / D;
  const double dfV_dfL = 0.5346 + 0.2166 * fL / D;

  grad[2] = (dfV_dfG * dfG_dsigma) / 2.354820045;
  grad[3] = (dfV_dfL * dfL_dgamma) / 2.354820045;

  obs.widthErr = PropagateError(cov, grad);
}

void AddGraphPoint(TGraphErrors *g, int ip, double x, double y, double ey){
  if (!g) return;
  g->SetPoint(ip, x, y);
  g->SetPointError(ip, 0., ey);
}

void WriteSliceFit(TH2F *h2, TString tag, TDirectory *outdir){
  if (!h2 || !outdir) return;

  outdir->cd();

  SliceFitResult s = RunFitSlicesY(h2, tag);

  if (s.amp)   s.amp->Write();
  if (s.mean)  s.mean->Write();
  if (s.sigma) s.sigma->Write();
  if (s.chi2)  s.chi2->Write();
}

ObservableResult ComputeObservables(TH2F *h2, TString tag){
  ObservableResult obs;
  if (!h2) return obs;

  double entries = 0.;
  double charge = 0.;
  double err_charge = 0.;
  double charge2 = 0.;
  double mean_charge = 0.;
  double err_mean_charge = 0.;
 
  for (int ix = 1; ix <= h2->GetNbinsX(); ++ix) {
    for (int iy = 1; iy <= h2->GetNbinsY(); ++iy) {
      const double n = h2->GetBinContent(ix, iy);
      const double q = h2->GetYaxis()->GetBinCenter(iy);

      entries += n;
      charge  += n * q;
      charge2 += n * q * q;
    }
  }

  if (entries > 0.) {
    mean_charge = charge / NEvtBLOCKS;
    err_charge = sqrt(max(0.0, charge2));
    // const double variance = charge2 / NEvtBLOCKS - mean_charge * mean_charge;    
    err_mean_charge = err_charge / NEvtBLOCKS;
  }

  obs.entries = entries;
  obs.charge = mean_charge;
  obs.chargeErr = err_mean_charge;
  obs.spot = h2->GetMean();
  obs.spotErr = h2->GetMeanError();
  obs.spotErrCons = 0; // to be determined
  obs.width = h2->GetRMS();
  obs.widthErr = h2->GetRMSError();
  return obs;
}

ObservableResult ComputeObservablesFitSlice(TH2F *h2, TString tag, bool applyFitSelection = false, int maxFitStatusAccepted = 1, int minCovMatrixStatusAccepted = 2){  
  ObservableResult obs;
  if (!h2) {
    SetBadObservableResult(obs);
    return obs;
  }

  SliceFitResult s = RunFitSlicesY(h2, tag);

  if (!s.mean) {
    SetBadObservableResult(obs);
    delete s.amp;
    delete s.sigma;
    delete s.chi2;
    return obs;
  }

  const double xmin = s.mean->GetXaxis()->GetXmin();
  const double xmax = s.mean->GetXaxis()->GetXmax();

  TFitResultPtr fitResult;

  TF1 *f = FitDoubleGaussian(s.mean, Form("DoubleGaussian_%s", tag.Data()), xmin, xmax, fitResult);

  // TF1 *f = FitVoigt(s.mean, Form("Voigt_%s", tag.Data()), xmin, xmax, fitResult);

  // if (f) {
  //   ExtractVoigtObservables(f, fitResult, obs);
  //   delete f;
  // }

  bool fitOK = false;

  if (f && fitResult.Get()) {
    obs.fitStatus = (int)fitResult;
    obs.covStatus = fitResult->CovMatrixStatus();

    fitOK = IsFitAccepted(fitResult, maxFitStatusAccepted, minCovMatrixStatusAccepted);
    obs.fitAccepted = fitOK;

    if (!applyFitSelection || fitOK) {
      ExtractDoubleGaussianObservables(f, fitResult, obs);
    } else {
      cerr << "WARNING: FitSlicesY double-Gaussian fit rejected for view = " << tag << " fitStatus = " << obs.fitStatus << " covStatus = " << obs.covStatus << endl;
      SetBadObservableResult(obs);

      obs.fitStatus = (int)fitResult;
      obs.covStatus = fitResult->CovMatrixStatus();
      obs.fitAccepted = false;
    }

    delete f;
  } else {
    cerr << "WARNING: FitSlicesY double-Gaussian fit failed technically for view = " << tag << endl;
    SetBadObservableResult(obs);
  }

// Single-core Gaussian fit only if the main fit produced usable mu/sigma
  if (obs.spot > -9000. && obs.width > 0.) {
    const double mu = obs.spot;
    const double sigma = obs.width;

    TF1 *fg = new TF1(Form("fg_singleCore_%s", tag.Data()), "gaus", mu - 2. * sigma, mu + 2. * sigma);
    TFitResultPtr fgResult = s.mean->Fit(fg, "RQS0");

    if (fg && fgResult.Get() && (int)fgResult <= 1) {
      obs.widthSingleCore = fabs(fg->GetParameter(2));
      obs.widthSingleCoreErr = fg->GetParError(2);
    } else {
      obs.widthSingleCore = -99.;
      obs.widthSingleCoreErr = -99.;
      cerr << "WARNING: single-core Gaussian fit failed for view = " << tag << " status = " << (int)fgResult << endl;
    }

    delete fg;
  } else {
    obs.widthSingleCore = -99.;
    obs.widthSingleCoreErr = -99.;
  }

  delete s.amp;
  delete s.mean;
  delete s.sigma;
  delete s.chi2;

  return obs;
}

ObservableResult DoObsRatio(ObservableResult obs_tot, vector<ObservableResult> obs_block, int iblk){
  ObservableResult obsratio;
      
  obsratio.entries = 0.;
  obsratio.charge = fabs(obs_tot.charge)>0 ? obs_block.at(iblk).charge/obs_tot.charge : -99;
  obsratio.chargeErr = 0.;
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

ObservableResult DoObsRatioBlocks(vector<ObservableResult> obs_block1, vector<ObservableResult> obs_block2, int iblk){
  ObservableResult obsratio;
      
  obsratio.entries = 0.;
  obsratio.charge = fabs(obs_block1.at(iblk).charge)>0 ? obs_block2.at(iblk).charge/obs_block1.at(iblk).charge : -99;
  obsratio.chargeErr = 0.;
  obsratio.spot = fabs(obs_block1.at(iblk).spot)>0 ? obs_block2.at(iblk).spot/obs_block1.at(iblk).spot : -99;
  obsratio.spotErr = 0.;
  obsratio.spotErrCons = 0.;
  obsratio.width = fabs(obs_block1.at(iblk).width)>0 ? obs_block2.at(iblk).width/obs_block1.at(iblk).width : -99;
  obsratio.widthErr = 0.;
  obsratio.widthCore = fabs(obs_block1.at(iblk).widthCore)>0 ? obs_block2.at(iblk).widthCore/obs_block1.at(iblk).widthCore : -99;
  obsratio.widthCoreErr = 0.;
  obsratio.widthTail = fabs(obs_block1.at(iblk).widthTail)>0 ? obs_block2.at(iblk).widthTail/obs_block1.at(iblk).widthTail : -99;
  obsratio.widthTailErr = 0.;
  obsratio.widthSingleCore = fabs(obs_block1.at(iblk).widthSingleCore)>0 ? obs_block2.at(iblk).widthSingleCore/obs_block1.at(iblk).widthSingleCore : -99;
  obsratio.widthSingleCoreErr = 0.;

  return obsratio;
}

void InitGraphPair(TGraphErrors *g[2]){
  g[0] = new TGraphErrors();
  g[1] = new TGraphErrors();
}

void InitTH2Pair(TH2F *h[2], TString baseName, TString title, int xbin, double xmin, double xmax, int ybin, double ymin, double ymax, TString xtitle, TString ytitle){
  h[0] = new TH2F(
    Form("%s", baseName.Data()),
    Form("%s;%s;%s", title.Data(), xtitle.Data(), ytitle.Data()),
    xbin, xmin, xmax,
    ybin, ymin, ymax
  );

  h[1] = new TH2F(
    Form("%sFitSlices", baseName.Data()),
    Form("%s FitSlices;%s;%s", title.Data(), xtitle.Data(), ytitle.Data()),
    xbin, xmin, xmax,
    ybin, ymin, ymax
  );
}

void FillTH2Pair(TH2F *h[2], double x_direct, double y_direct, double x_fitslices, double y_fitslices){
  if (h[0]) h[0]->Fill(x_direct, y_direct);
  if (h[1]) h[1]->Fill(x_fitslices, y_fitslices);
}

void TGraphAttributePair(TGraphErrors *g[2], TString baseName, TString view, TString ytitle, int markerstyle, int color){
  TGraphAttribute(g[0], Form("g_Block%s_%s", baseName.Data(), view.Data()), "block", ytitle, markerstyle, color);
  TGraphAttribute(g[1], Form("g_Block%sFitSlices_%s", baseName.Data(), view.Data()), "block", ytitle, markerstyle, color);
}

void FillGraphPair(TGraphErrors *gSpot[2], TGraphErrors *gWidth[2], TGraphErrors *gSingleWidthCore[2], TGraphErrors *gCharge[2], int ip, int iblk, const vector<ObservableResult> obs[2]){
  for (int im = 0; im < 2; ++im) {
    if (obs[im].empty()) continue;

    const ObservableResult &o = obs[im].back();

    AddGraphPoint(gSpot[im],            ip, iblk, o.spot,            o.spotErr);
    AddGraphPoint(gWidth[im],           ip, iblk, o.width,           o.widthErr);
    AddGraphPoint(gSingleWidthCore[im], ip, iblk, o.widthSingleCore, o.widthSingleCoreErr);
    AddGraphPoint(gCharge[im],          ip, iblk, o.charge,          o.chargeErr);
  }
}

void FillRatioGraph(TGraphErrors *g, int ip, int iblk, double num, double numErr, double den, double denErr){
  if (!g) return;

  const double r  = RatioValue(num, den);
  const double er = RatioError(num, numErr, den, denErr);

  AddGraphPoint(g, ip, iblk, r, er);
}

void FillXYRatioGraphs(TGraphErrors *gXYratio[2], TGraphErrors *gSigmaXsigmaYratio[2], TGraphErrors *gQXQYratio[2], int ip, int iblk, const vector<ObservableResult> obs[TMMCH_N_Readout][2]){
  for (int im = 0; im < 2; ++im) {
    if (obs[0][im].empty()) continue;
    if (obs[1][im].empty()) continue;

    const ObservableResult &ox = obs[0][im].back();
    const ObservableResult &oy = obs[1][im].back();

    FillRatioGraph(gXYratio[im], ip, iblk, ox.spot, ox.spotErr, oy.spot, oy.spotErr);
    FillRatioGraph(gSigmaXsigmaYratio[im], ip, iblk, ox.width, ox.widthErr, oy.width, oy.widthErr);
    FillRatioGraph(gQXQYratio[im], ip, iblk, ox.charge, ox.chargeErr, oy.charge, oy.chargeErr);
  }
}

void FillTH2ObservablePair(TH2F *hSpotvsSigma[2], TH2F *hSpotvsSigmaSCore[2], TH2F *hSpotvsQ[2], TH2F *hSigmavsQ[2], TH2F *hSigmaSCorevsQ[2], const vector<ObservableResult> obs[2]){
  for (int im = 0; im < 2; ++im) {
    if (obs[im].empty()) continue;

    const ObservableResult &o = obs[im].back();
    if (hSpotvsSigma[im]) hSpotvsSigma[im]->Fill(o.spot, o.width);
    if (hSpotvsSigmaSCore[im]) hSpotvsSigmaSCore[im]->Fill(o.spot, o.widthSingleCore);
    if (hSpotvsQ[im]) hSpotvsQ[im]->Fill(o.spot, o.charge);
    if (hSigmavsQ[im]) hSigmavsQ[im]->Fill(o.width, o.charge);
    if (hSigmaSCorevsQ[im]) hSigmaSCorevsQ[im]->Fill(o.widthSingleCore, o.charge);
  }
}

void FillTH2XYComparison(TH2F *hXvsY[2], TH2F *hSigmaXvsSigmaY[2], TH2F *hSigmaSCoreXvsSigmaSCoreY[2], TH2F *hQXvsQY[2], const vector<ObservableResult> obsX[2], const vector<ObservableResult> obsY[2]){
  for (int im = 0; im < 2; ++im) {
    if (obsX[im].empty()) continue;
    if (obsY[im].empty()) continue;

    const ObservableResult &ox = obsX[im].back();
    const ObservableResult &oy = obsY[im].back();

    if (hXvsY[im]) hXvsY[im]->Fill(ox.spot, oy.spot);
    if (hSigmaXvsSigmaY[im]) hSigmaXvsSigmaY[im]->Fill(ox.width, oy.width);
    if (hSigmaSCoreXvsSigmaSCoreY[im]) hSigmaSCoreXvsSigmaSCoreY[im]->Fill(ox.widthSingleCore, oy.widthSingleCore);
    if (hQXvsQY[im]) hQXvsQY[im]->Fill(ox.charge, oy.charge);
  }
}

void WriteGraphPair(TGraphErrors *g[2]){
  if (g[0]) g[0]->Write();
  if (g[1]) g[1]->Write();
}

void WriteTH2Pair(TH2F *h[2]){
  if (h[0]) h[0]->Write();
  if (h[1]) h[1]->Write();
}

void WriteMeasurementGraphs(TDirectory *dir, TGraphErrors *gSpot[TMMCH_N_Readout][2], TGraphErrors *gWidth[TMMCH_N_Readout][2], TGraphErrors *gSingleWidthCore[TMMCH_N_Readout][2], TGraphErrors *gCharge[TMMCH_N_Readout][2]){
  if (!dir) return;

  dir->cd();

  for (int iv = 0; iv < TMMCH_N_Readout; ++iv) {
    WriteGraphPair(gSpot[iv]);
    WriteGraphPair(gWidth[iv]);
    gSingleWidthCore[iv][1]->Write();
    WriteGraphPair(gCharge[iv]);
  }
}

void WriteTH2Measurement(TDirectory *dir, TH2F *hXvsY[2], TH2F *hSigmaXvsSigmaY[2], TH2F *hSigmaSCoreXvsSigmaSCoreY[2], TH2F *hQXvsQY[2], TH2F *hSpotvsSigma[TMMCH_N_Readout][2], TH2F *hSpotvsSigmaSCore[TMMCH_N_Readout][2], TH2F *hSpotvsQ[TMMCH_N_Readout][2], TH2F *hSigmavsQ[TMMCH_N_Readout][2], TH2F *hSigmaSCorevsQ[TMMCH_N_Readout][2]){
  if (!dir) return;

  dir->cd();
  WriteTH2Pair(hXvsY);
  WriteTH2Pair(hSigmaXvsSigmaY);
  WriteTH2Pair(hSigmaSCoreXvsSigmaSCoreY);
  WriteTH2Pair(hQXvsQY);

  for (int iv = 0; iv < TMMCH_N_Readout; ++iv) {
    WriteTH2Pair(hSpotvsSigma[iv]);
    WriteTH2Pair(hSpotvsSigmaSCore[iv]);
    WriteTH2Pair(hSpotvsQ[iv]);
    WriteTH2Pair(hSigmavsQ[iv]);
    WriteTH2Pair(hSigmaSCorevsQ[iv]);
  }
}

void PrintObservableResult(const TString &label, const ObservableResult obs[2]){
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

  TString outName = Form("DoubleG_AnalysisTMM_%s.root", inName.Data());

  TFile *fout = TFile::Open(outName, "RECREATE");
  TDirectory *dGraphsOut = fout->mkdir("Graphs");
  TDirectory *dTH2Out = fout->mkdir("TH2");

  if (!dGraphsOut) {
    cerr << "ERROR: cannot create/get Graphs directory" << endl;
    fout->Close();
    fin->Close();
    return;
  }

  TDirectory *dGraphsRawOut          = dGraphsOut->mkdir("Raw");
  TDirectory *dGraphsCalibOut        = dGraphsOut->mkdir("Calib");
  TDirectory *dGraphsCalibOverallOut = dGraphsOut->mkdir("CalibOverall");
  TDirectory *dRatioOut              = dGraphsOut->mkdir("Ratio");
  TDirectory *dRatioRawOut           = dRatioOut->mkdir("Raw");
  TDirectory *dRatioCalibOut         = dRatioOut->mkdir("Calib");
  TDirectory *dRatioCalibOverallOut  = dRatioOut->mkdir("CalibOverall");
  TDirectory *dRatioCalib_CalibOverallOut  = dRatioOut->mkdir("Calib_CalibOverall");

  TDirectory *dTH2RawOut             = dTH2Out->mkdir("Raw");
  TDirectory *dTH2CalibOut           = dTH2Out->mkdir("Calib");
  TDirectory *dTH2CalibOverallOut    = dTH2Out->mkdir("CalibOverall");


  if (!dGraphsRawOut || !dGraphsCalibOut || !dGraphsCalibOverallOut ) {
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
  ObservableResult RawObs[TMMCH_N_Readout][2], CalibObs[TMMCH_N_Readout][2];
  
  for (int iv = 0; iv < TMMCH_N_Readout; ++iv) {
    TString v = views[iv];

    if (dOverlallInRaw) {
      TH2F *h2RawOverall = GetH2F(dOverlallInRaw, Form("hqmaxstripFull%s", v.Data()));
      RawObs[iv][0] = ComputeObservables(h2RawOverall, v);
      RawObs[iv][1] = ComputeObservablesFitSlice(h2RawOverall, v, false);    
    }

    if (dOverlallInCalib) {
      TH2F *h2CalibOverall = GetH2F(dOverlallInCalib, Form("hqmaxstripFull_cal%s", v.Data()));
      CalibObs[iv][0] = ComputeObservables(h2CalibOverall, v);
      CalibObs[iv][1] = ComputeObservablesFitSlice(h2CalibOverall, v, false);       
    }

    cout << "===================================================" << endl;
    cout << "======== Overall analysis results - view " << v << " ========" << endl;
    cout << "===================================================" << endl;

    PrintObservableResult("Raw observables", RawObs[iv]);
    PrintObservableResult("Calib observables", CalibObs[iv]);

    cout << "===================================================" << endl;
    cout << endl;

  }
  
  // write out the overall results and keep them as reference for the other coparison block by block
  // maybe write them out to a txt file

  // ============================================================
  // Blocks
  // ============================================================

  //prepare TGraphs and struct for the block analysis (runs over possible analysis methods)
  TGraphErrors *gSpotRaw[TMMCH_N_Readout][2];
  TGraphErrors *gWidthRaw[TMMCH_N_Readout][2];
  TGraphErrors *gSingleWidthCoreRaw[TMMCH_N_Readout][2];
  TGraphErrors *gChargeRaw[TMMCH_N_Readout][2];

  TGraphErrors *gSpotCalib[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCalib[TMMCH_N_Readout][2];
  TGraphErrors *gSingleWidthCoreCalib[TMMCH_N_Readout][2];
  TGraphErrors *gChargeCalib[TMMCH_N_Readout][2];

  TGraphErrors *gSpotCalibOverall[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCalibOverall[TMMCH_N_Readout][2];
  TGraphErrors *gSingleWidthCoreCalibOverall[TMMCH_N_Readout][2];
  TGraphErrors *gChargeCalibOverall[TMMCH_N_Readout][2];

  // TH2 for X-Y comparison 
  TH2F *hXvsYRaw[2];
  TH2F *hSigmaXvsSigmaYRaw[2];
  TH2F *hSigmaSCoreXvsSigmaSCoreYRaw[2];
  TH2F *hQXvsQYRaw[2];

  TH2F *hXvsYCalib[2];
  TH2F *hSigmaXvsSigmaYCalib[2];
  TH2F *hSigmaSCoreXvsSigmaSCoreYCalib[2];
  TH2F *hQXvsQYCalib[2];

  TH2F *hXvsYCalibOverall[2];
  TH2F *hSigmaXvsSigmaYCalibOverall[2];
  TH2F *hSigmaSCoreXvsSigmaSCoreYCalibOverall[2];
  TH2F *hQXvsQYCalibOverall[2];

  // TH2 for X-X and Y-Y different obs comparison 
  TH2F *hSpotvsSigmaRaw[TMMCH_N_Readout][2];
  TH2F *hSpotvsSigmaSCoreRaw[TMMCH_N_Readout][2];
  TH2F *hSpotvsQRaw[TMMCH_N_Readout][2];
  TH2F *hSigmavsQRaw[TMMCH_N_Readout][2];
  TH2F *hSigmaSCorevsQRaw[TMMCH_N_Readout][2];

  TH2F *hSpotvsSigmaCalib[TMMCH_N_Readout][2];
  TH2F *hSpotvsSigmaSCoreCalib[TMMCH_N_Readout][2];
  TH2F *hSpotvsQCalib[TMMCH_N_Readout][2];
  TH2F *hSigmavsQCalib[TMMCH_N_Readout][2];
  TH2F *hSigmaSCorevsQCalib[TMMCH_N_Readout][2];

  TH2F *hSpotvsSigmaCalibOverall[TMMCH_N_Readout][2];
  TH2F *hSpotvsSigmaSCoreCalibOverall[TMMCH_N_Readout][2];
  TH2F *hSpotvsQCalibOverall[TMMCH_N_Readout][2];
  TH2F *hSigmavsQCalibOverall[TMMCH_N_Readout][2];
  TH2F *hSigmaSCorevsQCalibOverall[TMMCH_N_Readout][2];

  TGraphErrors *gQFitSliceQDirRaw[TMMCH_N_Readout];
  TGraphErrors *gQFitSliceQDirCalib[TMMCH_N_Readout];
  TGraphErrors *gQFitSliceQDirCalibOverall[TMMCH_N_Readout];

  TGraphErrors *gXYratioRaw[2]; //one for direct one for FitSlice
  TGraphErrors *gXYratioCalib[2];
  TGraphErrors *gXYratioCalibOverall[2];

  TGraphErrors *gsigmaXsigmaYratioRaw[2];
  TGraphErrors *gsigmaXsigmaYratioCalib[2];
  TGraphErrors *gsigmaXsigmaYratioCalibOverall[2];

  TGraphErrors *gQXQYratioRaw[2];
  TGraphErrors *gQXQYratioCalib[2];
  TGraphErrors *gQXQYratioCalibOverall[2];

  vector <ObservableResult> BlockRawObs[TMMCH_N_Readout][2];
  vector <ObservableResult> BlockCalibObs[TMMCH_N_Readout][2];
  vector <ObservableResult> BlockCalibOverallObs[TMMCH_N_Readout][2];

  vector<int> blocks = FindBlockNumbers(dBlocksIn);
  cout << "### NBlocks to be analysed =  " << blocks.size() << endl;
  
  for (int iv = 0; iv < TMMCH_N_Readout; ++iv) {
    TString v = views[iv];

    InitGraphPair(gSpotRaw[iv]);
    InitGraphPair(gWidthRaw[iv]);
    InitGraphPair(gSingleWidthCoreRaw[iv]);
    InitGraphPair(gChargeRaw[iv]);

    InitGraphPair(gSpotCalib[iv]);
    InitGraphPair(gWidthCalib[iv]);
    InitGraphPair(gSingleWidthCoreCalib[iv]);
    InitGraphPair(gChargeCalib[iv]);

    InitGraphPair(gSpotCalibOverall[iv]);
    InitGraphPair(gWidthCalibOverall[iv]);
    InitGraphPair(gSingleWidthCoreCalibOverall[iv]);
    InitGraphPair(gChargeCalibOverall[iv]);

    gQFitSliceQDirRaw[iv] = new TGraphErrors();
    gQFitSliceQDirCalib[iv] = new TGraphErrors();
    gQFitSliceQDirCalibOverall[iv] = new TGraphErrors();

    TGraphAttributePair(gSpotRaw[iv],            "BeamSpotRaw",        v, Form("%s [strip]", v.Data()),       20, iv + 1);
    TGraphAttributePair(gWidthRaw[iv],           "BeamSpreadRaw",      v, Form("#sigma_{%s} [strip]", v.Data()),      20, iv + 1);
    TGraphAttributePair(gSingleWidthCoreRaw[iv], "SingleCoreWidthRaw", v, Form("single-core #sigma_{%s} [strip]", v.Data()), 20, iv + 1);
    TGraphAttributePair(gChargeRaw[iv],          "BeamChargeRaw",      v, Form("Q_{%s} [adc]", v.Data()),       20, iv + 1);

    TGraphAttributePair(gSpotCalib[iv],            "BeamSpotCalib",        v, Form("%s [strip]", v.Data()),       20, iv + 1);
    TGraphAttributePair(gWidthCalib[iv],           "BeamSpreadCalib",      v, Form("#sigma_{%s} [strip]", v.Data()),      20, iv + 1);
    TGraphAttributePair(gSingleWidthCoreCalib[iv], "SingleCoreWidthCalib", v, Form("single-core #sigma_{%s} [strip]", v.Data()), 20, iv + 1);
    TGraphAttributePair(gChargeCalib[iv],          "BeamChargeCalib",      v, Form("Q_{%s} [adc]", v.Data()),       20, iv + 1);

    TGraphAttributePair(gSpotCalibOverall[iv],            "BeamSpotCalibOverall",        v, Form("%s [strip]", v.Data()),       20, iv + 1);
    TGraphAttributePair(gWidthCalibOverall[iv],           "BeamSpreadCalibOverall",      v, Form("#sigma_{%s} [strip]", v.Data()),      20, iv + 1);
    TGraphAttributePair(gSingleWidthCoreCalibOverall[iv], "SingleCoreWidthCalibOverall", v, Form("single-core #sigma_{%s} [strip]", v.Data()), 20, iv + 1);
    TGraphAttributePair(gChargeCalibOverall[iv],          "BeamChargeCalibOverall",      v, Form("Q_{%s} [adc]", v.Data()),       20, iv + 1);
  
    // TGraphMethods
    TGraphAttribute(gQFitSliceQDirRaw[iv], "gQFitSliceQDirRaw_"+v, "block", "QFit/Qdir", 20, iv + 1);
    TGraphAttribute(gQFitSliceQDirCalib[iv], "gQFitSliceQDirCalib_"+v, "block", "QFit/Qdir", 20, iv + 1);
    TGraphAttribute(gQFitSliceQDirCalibOverall[iv], "gQFitSliceQDirCalibOverall_"+v, "block", "QFit/Qdir", 20, iv + 1);
    
    // initializing TH2 per view comparison
    InitTH2Pair(hSpotvsSigmaRaw[iv], Form("hSpotvsSigmaRaw_%s", v.Data()), Form("spot vs #sigma-Raw %s", v.Data()), 400, 149.5, 199.5, 300, -0.5, 149.5, Form("%s [strip]",v.Data()), Form("#sigma_{%s} [strip]",v.Data()));
    InitTH2Pair(hSpotvsSigmaSCoreRaw[iv], Form("hSpotvsSigmaSCoreRaw_%s", v.Data()), Form("spot vs Siglecore#sigma-Raw %s", v.Data()), 400, 149.5, 199.5, 300, -0.5, 149.5, Form("%s [strip]",v.Data()), Form("SingleCore #sigma_{%s} [strip]",v.Data()));
    InitTH2Pair(hSpotvsQRaw[iv], Form("hSpotvsQRaw_%s", v.Data()), Form("spot vs Q-Raw %s", v.Data()), 400, 149.5, 199.5, 1500, -0.5, 150000, Form("%s [strip]",v.Data()), Form("Q_{%s} [adc]",v.Data()));
    InitTH2Pair(hSigmavsQRaw[iv], Form("hSigmavsQRaw_%s", v.Data()), Form("#sigma vs Q-Raw %s", v.Data()), 300, -0.5, 149.5, 1500, -0.5, 150000, Form("#sigma_{%s} [strip]",v.Data()), Form("Q_{%s} [adc]",v.Data()));
    InitTH2Pair(hSigmaSCorevsQRaw[iv], Form("hSigmaSCorevsQRaw_%s", v.Data()), Form("SingleCore #sigma vs Q-Raw %s", v.Data()), 300, -0.5, 149.5, 1500, -0.5, 150000, Form("SingleCore #sigma_{%s} [strip]",v.Data()), Form("Q_{%s} [adc]",v.Data()));
  
    InitTH2Pair(hSpotvsSigmaCalib[iv], Form("hSpotvsSigmaCalib_%s", v.Data()), Form("spot vs #sigma-Calib %s", v.Data()), 400, 149.5, 199.5, 300, -0.5, 149.5, Form("%s [strip]",v.Data()), Form("#sigma_{%s} [strip]",v.Data()));
    InitTH2Pair(hSpotvsSigmaSCoreCalib[iv], Form("hSpotvsSigmaSCoreCalib_%s", v.Data()), Form("spot vs Siglecore#sigma-Calib %s", v.Data()), 400, 149.5, 199.5, 300, -0.5, 149.5, Form("%s [strip]",v.Data()), Form("SingleCore #sigma_{%s} [strip]",v.Data()));
    InitTH2Pair(hSpotvsQCalib[iv], Form("hSpotvsQCalib_%s", v.Data()), Form("spot vs Q-Calib %s", v.Data()), 400, 149.5, 199.5, 1500, -0.5, 150000, Form("%s [strip]",v.Data()), Form("Q_{%s} [adc]",v.Data()));
    InitTH2Pair(hSigmavsQCalib[iv], Form("hSigmavsQCalib_%s", v.Data()), Form("#sigma vs Q-Calib %s", v.Data()), 300, -0.5, 149.5, 1500, -0.5, 150000, Form("#sigma_{%s} [strip]",v.Data()), Form("Q_{%s} [adc]",v.Data()));
    InitTH2Pair(hSigmaSCorevsQCalib[iv], Form("hSigmaSCorevsQCalib_%s", v.Data()), Form("SingleCore #sigma vs Q-Calib %s", v.Data()), 300, -0.5, 149.5, 1500, -0.5, 150000, Form("SingleCore #sigma_{%s} [strip]",v.Data()), Form("Q_{%s} [adc]",v.Data()));
  
    InitTH2Pair(hSpotvsSigmaCalibOverall[iv], Form("hSpotvsSigmaCalibOverall_%s", v.Data()), Form("spot vs #sigma-CalibOverall %s", v.Data()), 400, 149.5, 199.5, 300, -0.5, 159.5, Form("%s [strip]",v.Data()), Form("#sigma_{%s} [strip]",v.Data()));
    InitTH2Pair(hSpotvsSigmaSCoreCalibOverall[iv], Form("hSpotvsSigmaSCoreCalibOverall_%s", v.Data()), Form("spot vs Siglecore#sigma-CalibOverall %s", v.Data()), 400, 149.5, 199.5, 300, -0.5, 149.5, Form("%s [strip]",v.Data()), Form("SingleCore #sigma_{%s} [strip]",v.Data()));
    InitTH2Pair(hSpotvsQCalibOverall[iv], Form("hSpotvsQCalibOverall_%s", v.Data()), Form("spot vs Q-CalibOverall %s", v.Data()), 400, 149.5, 199.5, 1500, -0.5, 150000, Form("%s [strip]",v.Data()), Form("Q_{%s} [adc]",v.Data()));
    InitTH2Pair(hSigmavsQCalibOverall[iv], Form("hSigmavsQCalibOverall_%s", v.Data()), Form("#sigma vs Q-CalibOverall %s", v.Data()), 300, -0.5, 149.5, 1500, -0.5, 150000, Form("#sigma_{%s} [strip]",v.Data()), Form("Q_{%s} [adc]",v.Data()));
    InitTH2Pair(hSigmaSCorevsQCalibOverall[iv], Form("hSigmaSCorevsQCalibOverall_%s", v.Data()), Form("SingleCore #sigma vs Q-CalibOverall %s", v.Data()), 300, -0.5, 149.5, 1500, -0.5, 150000, Form("SingleCore #sigma_{%s} [strip]",v.Data()), Form("Q_{%s} [adc]",v.Data()));
  }

  InitGraphPair(gXYratioRaw);
  InitGraphPair(gXYratioCalib);
  InitGraphPair(gXYratioCalibOverall);

  InitGraphPair(gsigmaXsigmaYratioRaw);
  InitGraphPair(gsigmaXsigmaYratioCalib);
  InitGraphPair(gsigmaXsigmaYratioCalibOverall);

  InitGraphPair(gQXQYratioRaw);
  InitGraphPair(gQXQYratioCalib);
  InitGraphPair(gQXQYratioCalibOverall);

  TGraphAttributePair(gXYratioRaw, "BeamSpotXYratioRaw", views[0]+views[1], "X/Y", 20, 1);
  TGraphAttributePair(gXYratioCalib, "BeamSpotXYratioCalib", views[0]+views[1], "X/Y", 20, 1);
  TGraphAttributePair(gXYratioCalibOverall, "BeamSpotXYratioCalibOverll", views[0]+views[1], "X/Y", 20, 1);

  TGraphAttributePair(gsigmaXsigmaYratioRaw, "BeamSpreadXYratioRaw", views[0]+views[1], "#sigma_{X}/#sigma_{Y}", 20, 1);
  TGraphAttributePair(gsigmaXsigmaYratioCalib, "BeamSpreadXYratioCalib", views[0]+views[1], "#sigma_{X}/#sigma_{Y}", 20, 1);
  TGraphAttributePair(gsigmaXsigmaYratioCalibOverall, "BeamSpreadXYratioCalibOverll", views[0]+views[1], "#sigma_{X}/#sigma_{Y}", 20, 1);

  TGraphAttributePair(gQXQYratioRaw, "BeamChargeXYratioRaw", views[0]+views[1], "Q_{X}/Q_{Y}", 20, 1);
  TGraphAttributePair(gQXQYratioCalib, "BeamChargeXYratioCalib", views[0]+views[1], "Q_{X}/Q_{Y}", 20, 1);
  TGraphAttributePair(gQXQYratioCalibOverall, "BeamChargeXYratioCalibOverall", views[0]+views[1], "Q_{X}/Q_{Y}", 20, 1);


  // Initializing TH2 X-y
  InitTH2Pair(hXvsYRaw, "hXvsYRaw", "X vs Y-Raw", 400, 149.5, 199.5, 400, 149.5, 199.5, "X [strip]", "Y [strip]");
  InitTH2Pair(hSigmaXvsSigmaYRaw, "hSigmaXvsSigmaYRaw", "#sigma_{X} vs #sigma_{Y}-Raw", 300, -0.5, 149.5, 300, -0.5, 149.5, "#sigma_{X} [strip]", "#sigma_{Y} [strip]");
  InitTH2Pair(hSigmaSCoreXvsSigmaSCoreYRaw, "hSigmaSCoreXvsSigmaSCoreYRaw", "SingleGaussian #sigma_{X} vs #sigma_{Y}-Raw", 300, -0.5, 149.5, 300, -0.5, 149.5, "X single-core width [strip]", "Y single-core width [strip]");
  InitTH2Pair(hQXvsQYRaw, "hQXvsQYRaw", "Q_{X} vs Q_{Y}-Raw", 1500, -0.5, 150000, 1500, -0.5, 150000, "Q_{X} [adc]", "Q_{Y} [adc]");

  InitTH2Pair(hXvsYCalib, "hXvsYCalib", "X vs Y-Calib", 400, 149.5, 199.5, 400, 149.5, 199.5, "X [strip]", "Y [strip]");
  InitTH2Pair(hSigmaXvsSigmaYCalib, "hSigmaXvsSigmaYCalib", "#sigma_{X} vs #sigma_{Y}-Calib", 300, -0.5, 149.5, 300, -0.5, 149.5, "#sigma_{X} [strip]", "#sigma_{Y} [strip]");
  InitTH2Pair(hSigmaSCoreXvsSigmaSCoreYCalib, "hSigmaSCoreXvsSigmaSCoreYCalib", "SingleGaussian #sigma_{X} vs #sigma_{Y}-Calib", 300, -0.5, 149.5, 300, -0.5, 149.5, "X single-core width [strip]", "Y single-core width [strip]");
  InitTH2Pair(hQXvsQYCalib, "hQXvsQYCalib", "Q_{X} vs Q_{Y}-Calib", 1500, -0.5, 150000, 1500, -0.5, 150000, "Q_{X} [adc]", "Q_{Y} [adc]");

  InitTH2Pair(hXvsYCalibOverall, "hXvsYCalibOverall", "X vs Y-CalibOverall", 400, 149.5, 199.5, 400, 149.5, 199.5, "X [strip]", "Y [strip]");
  InitTH2Pair(hSigmaXvsSigmaYCalibOverall, "hSigmaXvsSigmaYCalibOverall", "#sigma_{X} vs #sigma_{Y}-CalibOverall", 300, -0.5, 149.5, 300, -0.5, 149.5, "#sigma_{X} [strip]", "#sigma_{Y} [strip]");
  InitTH2Pair(hSigmaSCoreXvsSigmaSCoreYCalibOverall, "hSigmaSCoreXvsSigmaSCoreYCalibOverall", "SingleGaussian #sigma_{X} vs #sigma_{Y}-CalibOverall", 300, -0.5, 149.5, 300, -0.5, 149.5, "X single-core width [strip]", "Y single-core width [strip]");
  InitTH2Pair(hQXvsQYCalibOverall, "hQXvsQYCalibOverall", "Q_{X} vs Q_{Y}-CalibOverall", 1500, -0.5, 150000, 1500, -0.5, 150000, "Q_{X} [adc]", "Q_{Y} [adc]");

  
  for (size_t i = 0; i < blocks.size(); ++i) {

    const int iblk = blocks[i];
    cout << "Processing block: " << blocks[i] << endl;
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
      TH2F *h2CalOverallBlock = GetH2F(dBCalOvIn,Form("hBlockqmaxstripFull_Overallcal%s_block%04d", v.Data(), iblk));

      BlockRawObs[iv][0].push_back(ComputeObservables(h2RawBlock, v));
      BlockRawObs[iv][1].push_back(ComputeObservablesFitSlice(h2RawBlock, true, 1, 2));

      BlockCalibObs[iv][0].push_back(ComputeObservables(h2CalBlock, v));
      BlockCalibObs[iv][1].push_back(ComputeObservablesFitSlice(h2CalBlock, true, 1, 2));

      BlockCalibOverallObs[iv][0].push_back(ComputeObservables(h2CalOverallBlock, v));
      BlockCalibOverallObs[iv][1].push_back(ComputeObservablesFitSlice(h2CalOverallBlock, true, 1, 2));

      FillGraphPair(gSpotRaw[iv],
          gWidthRaw[iv], gSingleWidthCoreRaw[iv],
          gChargeRaw[iv],
          i, iblk,
          BlockRawObs[iv]
      );

      FillGraphPair(gSpotCalib[iv],
          gWidthCalib[iv], gSingleWidthCoreCalib[iv],
          gChargeCalib[iv], 
          i, iblk,
          BlockCalibObs[iv]
      );

      FillGraphPair(gSpotCalibOverall[iv],
          gWidthCalibOverall[iv], gSingleWidthCoreCalibOverall[iv],
          gChargeCalibOverall[iv],
          i, iblk,
          BlockCalibOverallObs[iv]
      );

      FillTH2ObservablePair(hSpotvsSigmaRaw[iv], hSpotvsSigmaSCoreRaw[iv], hSpotvsQRaw[iv],
        hSigmavsQRaw[iv], hSigmaSCorevsQRaw[iv],
        BlockRawObs[iv]
      );

      FillTH2ObservablePair(hSpotvsSigmaCalib[iv], hSpotvsSigmaSCoreCalib[iv], hSpotvsQCalib[iv],
        hSigmavsQCalib[iv], hSigmaSCorevsQCalib[iv],
        BlockCalibObs[iv]
      );

      FillTH2ObservablePair(hSpotvsSigmaCalibOverall[iv], hSpotvsSigmaSCoreCalibOverall[iv], hSpotvsQCalibOverall[iv],
        hSigmavsQCalibOverall[iv], hSigmaSCorevsQCalibOverall[iv],
        BlockCalibOverallObs[iv]
      );

      double eRaw = RatioError(BlockRawObs[iv][1].back().charge, BlockRawObs[iv][1].back().chargeErr, BlockRawObs[iv][0].back().charge, BlockRawObs[iv][0].back().chargeErr);
      double eCalib = RatioError(BlockCalibObs[iv][1].back().charge, BlockCalibObs[iv][1].back().chargeErr, BlockCalibObs[iv][0].back().charge, BlockCalibObs[iv][0].back().chargeErr);
      double eCalibOverall = RatioError(BlockCalibOverallObs[iv][1].back().charge, BlockCalibOverallObs[iv][1].back().chargeErr, BlockCalibOverallObs[iv][0].back().charge, BlockCalibOverallObs[iv][0].back().chargeErr);
      AddGraphPoint(gQFitSliceQDirRaw[iv], i, iblk, BlockRawObs[iv][1].back().charge/BlockRawObs[iv][0].back().charge, eRaw);
      AddGraphPoint(gQFitSliceQDirCalib[iv], i, iblk, BlockCalibObs[iv][1].back().charge/BlockCalibObs[iv][0].back().charge, eCalib);
      AddGraphPoint(gQFitSliceQDirCalibOverall[iv], i, iblk, BlockCalibOverallObs[iv][1].back().charge/BlockCalibOverallObs[iv][0].back().charge, eCalibOverall);

      dBlocksOut->cd();
      WriteSliceFit(h2RawBlock, Form("Raw_%s_block%04d", v.Data(), iblk), dBlocksRawOut);
      WriteSliceFit(h2CalBlock, Form("Calib_%s_block%04d", v.Data(), iblk), dBlocksCalOut);
      WriteSliceFit(h2CalOverallBlock, Form("CalibOverall_%s_block%04d", v.Data(), iblk), dBlocksCalOverallOut);
    }

    FillXYRatioGraphs(gXYratioRaw, gsigmaXsigmaYratioRaw, gQXQYratioRaw, i, iblk, BlockRawObs);
    FillXYRatioGraphs(gXYratioCalib, gsigmaXsigmaYratioCalib, gQXQYratioCalib, i, iblk, BlockCalibObs);
    FillXYRatioGraphs(gXYratioCalibOverall, gsigmaXsigmaYratioCalibOverall, gQXQYratioCalibOverall, i, iblk, BlockCalibOverallObs);
    
    FillTH2XYComparison(hXvsYRaw,
      hSigmaXvsSigmaYRaw, hSigmaSCoreXvsSigmaSCoreYRaw,
      hQXvsQYRaw,
      BlockRawObs[0], BlockRawObs[1]
    );

    FillTH2XYComparison(hXvsYCalib,
      hSigmaXvsSigmaYCalib, hSigmaSCoreXvsSigmaSCoreYCalib,
      hQXvsQYCalib,
      BlockCalibObs[0], BlockCalibObs[1]
    );

    FillTH2XYComparison(hXvsYCalibOverall,
      hSigmaXvsSigmaYCalibOverall, hSigmaSCoreXvsSigmaSCoreYCalibOverall,
      hQXvsQYCalibOverall,
      BlockCalibOverallObs[0], BlockCalibOverallObs[1]
    );
  }

  // do the comparison analysis to get the final plots
  // ratio wrt to the overall measurements (overall analysis) to verify the stability of the calibration procedure in time
  // TMultiGraph to se by eye the effect of the calibration
  // afterwards --> systematics evaluation

  TGraphErrors *gSpotRawRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthRawRatio[TMMCH_N_Readout][2];
  TGraphErrors *gSingleWidthCoreRawRatio[TMMCH_N_Readout][2];
  TGraphErrors *gChargeRawRatio[TMMCH_N_Readout][2];

  TGraphErrors *gSpotCalibRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCalibRatio[TMMCH_N_Readout][2];
  TGraphErrors *gSingleWidthCoreCalibRatio[TMMCH_N_Readout][2];
  TGraphErrors *gChargeCalibRatio[TMMCH_N_Readout][2];

  TGraphErrors *gSpotCalibOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCalibOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gSingleWidthCoreCalibOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gChargeCalibOverallRatio[TMMCH_N_Readout][2];

  TGraphErrors *gSpotCalib_CalibOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCalib_CalibOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gSingleWidthCoreCalib_CalibOverallRatio[TMMCH_N_Readout][2];
  TGraphErrors *gChargeCalib_CalibOverallRatio[TMMCH_N_Readout][2];

  vector<ObservableResult> ObsRawRatio[TMMCH_N_Readout][2];
  vector<ObservableResult> ObsCalibRatio[TMMCH_N_Readout][2];
  vector<ObservableResult> ObsCalibOverallRatio[TMMCH_N_Readout][2];
  vector<ObservableResult> ObsCalib_CalibOverallRatio[TMMCH_N_Readout][2];
  for (int iv = 0; iv < TMMCH_N_Readout; ++iv) {
    TString v = views[iv];

    InitGraphPair(gSpotRawRatio[iv]);
    InitGraphPair(gWidthRawRatio[iv]);
    InitGraphPair(gSingleWidthCoreRawRatio[iv]);
    InitGraphPair(gChargeRawRatio[iv]);

    InitGraphPair(gSpotCalibRatio[iv]);
    InitGraphPair(gWidthCalibRatio[iv]);
    InitGraphPair(gSingleWidthCoreCalibRatio[iv]);
    InitGraphPair(gChargeCalibRatio[iv]);

    InitGraphPair(gSpotCalibOverallRatio[iv]);
    InitGraphPair(gWidthCalibOverallRatio[iv]);
    InitGraphPair(gSingleWidthCoreCalibOverallRatio[iv]);
    InitGraphPair(gChargeCalibOverallRatio[iv]);

    InitGraphPair(gSpotCalib_CalibOverallRatio[iv]);
    InitGraphPair(gWidthCalib_CalibOverallRatio[iv]);
    InitGraphPair(gSingleWidthCoreCalib_CalibOverallRatio[iv]);
    InitGraphPair(gChargeCalib_CalibOverallRatio[iv]);

    TGraphAttributePair(gSpotRawRatio[iv],            "BeamSpotRawRatio",        v, Form("%s [strip]", v.Data()),       20, iv + 1);
    TGraphAttributePair(gWidthRawRatio[iv],           "BeamSpreadRawRatio",      v, Form("#sigma_{%s} [strip]", v.Data()),      20, iv + 1);
    TGraphAttributePair(gSingleWidthCoreRawRatio[iv], "SingleCoreWidthRawRatio", v, Form("single-core #sigma_{%s} [strip]", v.Data()), 20, iv + 1);
    TGraphAttributePair(gChargeRawRatio[iv],          "BeamChargeRawRatio",      v, Form("Q_{%s} [adc]", v.Data()),       20, iv + 1);

    TGraphAttributePair(gSpotCalibRatio[iv],            "BeamSpotCalibRatio",        v, Form("%s [strip]", v.Data()),       20, iv + 1);
    TGraphAttributePair(gWidthCalibRatio[iv],           "BeamSpreadCalibRatio",      v, Form("#sigma_{%s} [strip]", v.Data()),      20, iv + 1);
    TGraphAttributePair(gSingleWidthCoreCalibRatio[iv], "SingleCoreWidthCalibRatio", v, Form("single-core #sigma_{%s} [strip]", v.Data()), 20, iv + 1);
    TGraphAttributePair(gChargeCalibRatio[iv],          "BeamChargeCalibRatio",      v, Form("Q_{%s} [adc]", v.Data()),       20, iv + 1);

    TGraphAttributePair(gSpotCalibOverallRatio[iv],            "BeamSpotCalibOverallRatio",        v, Form("%s [strip]", v.Data()),       20, iv + 1);
    TGraphAttributePair(gWidthCalibOverallRatio[iv],           "BeamSpreadCalibOverallRatio",      v, Form("#sigma_{%s} [strip]", v.Data()),      20, iv + 1);
    TGraphAttributePair(gSingleWidthCoreCalibOverallRatio[iv], "SingleCoreWidthCalibOverallRatio", v, Form("single-core #sigma_{%s} [strip]", v.Data()), 20, iv + 1);
    TGraphAttributePair(gChargeCalibOverallRatio[iv],          "BeamChargeCalibOverallRatio",      v, Form("Q_{%s} [adc]", v.Data()),       20, iv + 1);

    TGraphAttributePair(gSpotCalib_CalibOverallRatio[iv],            "BeamSpotCalib_CalibOverallRatio",        v, Form("%s [strip]", v.Data()),       20, iv + 1);
    TGraphAttributePair(gWidthCalib_CalibOverallRatio[iv],           "BeamSpreadCalib_CalibOverallRatio",      v, Form("#sigma_{%s} [strip]", v.Data()),      20, iv + 1);
    TGraphAttributePair(gSingleWidthCoreCalib_CalibOverallRatio[iv], "SingleCoreWidthCalib_CalibOverallRatio", v, Form("single-core #sigma_{%s} [strip]", v.Data()), 20, iv + 1);
    TGraphAttributePair(gChargeCalib_CalibOverallRatio[iv],          "BeamChargeCalib_CalibOverallRatio",      v, Form("Q_{%s} [adc]", v.Data()),       20, iv + 1);

  }

  for (size_t i = 0; i < blocks.size(); ++i) {
    const int iblk = blocks[i];   

    for (int iv = 0; iv < TMMCH_N_Readout; ++iv) {
      TString v = views[iv];

      ObsRawRatio[iv][0].push_back(DoObsRatio(RawObs[iv][0], BlockRawObs[iv][0], iblk));
      ObsRawRatio[iv][1].push_back(DoObsRatio(RawObs[iv][1], BlockRawObs[iv][1], iblk));
      ObsCalibRatio[iv][0].push_back(DoObsRatio(CalibObs[iv][0], BlockCalibObs[iv][0], iblk));
      ObsCalibRatio[iv][1].push_back(DoObsRatio(CalibObs[iv][1], BlockCalibObs[iv][1], iblk));
      ObsCalibOverallRatio[iv][0].push_back(DoObsRatio(CalibObs[iv][0], BlockCalibOverallObs[iv][0], iblk));
      ObsCalibOverallRatio[iv][1].push_back(DoObsRatio(CalibObs[iv][1], BlockCalibOverallObs[iv][1], iblk));
      
      ObsCalib_CalibOverallRatio[iv][0].push_back(DoObsRatioBlocks(BlockCalibObs[iv][0], BlockCalibOverallObs[iv][0], iblk));
      ObsCalib_CalibOverallRatio[iv][1].push_back(DoObsRatioBlocks(BlockCalibObs[iv][1], BlockCalibOverallObs[iv][1], iblk));
      
      FillGraphPair(gSpotRawRatio[iv],
          gWidthRawRatio[iv], gSingleWidthCoreRawRatio[iv],
          gChargeRawRatio[iv], 
          i, iblk,
          ObsRawRatio[iv]
      );

      FillGraphPair(gSpotCalibRatio[iv],
          gWidthCalibRatio[iv], gSingleWidthCoreCalibRatio[iv],
          gChargeCalibRatio[iv], 
          i, iblk,
          ObsCalibRatio[iv]
      );
      
      FillGraphPair(gSpotCalibOverallRatio[iv],
          gWidthCalibOverallRatio[iv], gSingleWidthCoreCalibOverallRatio[iv],
          gChargeCalibOverallRatio[iv], 
          i, iblk,
          ObsCalibOverallRatio[iv]
      ); 
      
      FillGraphPair(gSpotCalib_CalibOverallRatio[iv],
          gWidthCalib_CalibOverallRatio[iv], gSingleWidthCoreCalib_CalibOverallRatio[iv],
          gChargeCalib_CalibOverallRatio[iv], 
          i, iblk,
          ObsCalib_CalibOverallRatio[iv]
      );
    }
  }

  fout->cd();
  dGraphsOut->cd();

  WriteMeasurementGraphs(
    dGraphsRawOut,
    gSpotRaw,
    gWidthRaw,
    gSingleWidthCoreRaw,
    gChargeRaw
  );

  WriteMeasurementGraphs(
    dGraphsCalibOut,
    gSpotCalib,
    gWidthCalib,
    gSingleWidthCoreCalib,
    gChargeCalib
  );

  WriteMeasurementGraphs(
    dGraphsCalibOverallOut,
    gSpotCalibOverall,
    gWidthCalibOverall,
    gSingleWidthCoreCalibOverall,
    gChargeCalibOverall
  );

  WriteMeasurementGraphs(
    dRatioRawOut,
    gSpotRawRatio,
    gWidthRawRatio,
    gSingleWidthCoreRawRatio,
    gChargeRawRatio
  );

  WriteMeasurementGraphs(
    dRatioCalibOut,
    gSpotCalibRatio,
    gWidthCalibRatio,
    gSingleWidthCoreCalibRatio,
    gChargeCalibRatio
  );

  WriteMeasurementGraphs(
    dRatioCalibOverallOut,
    gSpotCalibOverallRatio,
    gWidthCalibOverallRatio,
    gSingleWidthCoreCalibOverallRatio,
    gChargeCalibOverallRatio
  );

  WriteMeasurementGraphs(
    dRatioCalib_CalibOverallOut,
    gSpotCalib_CalibOverallRatio,
    gWidthCalib_CalibOverallRatio,
    gSingleWidthCoreCalib_CalibOverallRatio,
    gChargeCalib_CalibOverallRatio
  );

  dGraphsOut->cd();
  for (int iv = 0; iv < TMMCH_N_Readout; ++iv) {
    gQFitSliceQDirRaw[iv]->Write();
    gQFitSliceQDirCalib[iv]->Write();
    gQFitSliceQDirCalibOverall[iv]->Write();
  }
  WriteGraphPair(gXYratioRaw);
  WriteGraphPair(gXYratioCalib);
  WriteGraphPair(gXYratioCalibOverall);
  
  WriteGraphPair(gsigmaXsigmaYratioRaw);
  WriteGraphPair(gsigmaXsigmaYratioCalib);
  WriteGraphPair(gsigmaXsigmaYratioCalibOverall);

  WriteGraphPair(gQXQYratioRaw);
  WriteGraphPair(gQXQYratioCalib);
  WriteGraphPair(gQXQYratioCalibOverall);

  fout->cd();
  dTH2Out->cd();
  WriteTH2Measurement(
    dTH2RawOut,
    hXvsYRaw,
    hSigmaXvsSigmaYRaw,
    hSigmaSCoreXvsSigmaSCoreYRaw,
    hQXvsQYRaw,
    hSpotvsSigmaRaw,
    hSpotvsSigmaSCoreRaw,
    hSpotvsQRaw,
    hSigmavsQRaw,
    hSigmaSCorevsQRaw
  );

  dTH2Out->cd();
  WriteTH2Measurement(
    dTH2CalibOut,
    hXvsYCalib,
    hSigmaXvsSigmaYCalib,
    hSigmaSCoreXvsSigmaSCoreYCalib,
    hQXvsQYCalib,
    hSpotvsSigmaCalib,
    hSpotvsSigmaSCoreCalib,
    hSpotvsQCalib,
    hSigmavsQCalib,
    hSigmaSCorevsQCalib
  );

  dTH2Out->cd();
  WriteTH2Measurement(
    dTH2CalibOverallOut,
    hXvsYCalibOverall,
    hSigmaXvsSigmaYCalibOverall,
    hSigmaSCoreXvsSigmaSCoreYCalibOverall,
    hQXvsQYCalibOverall,
    hSpotvsSigmaCalibOverall,
    hSpotvsSigmaSCoreCalibOverall,
    hSpotvsQCalibOverall,
    hSigmavsQCalibOverall,
    hSigmaSCorevsQCalibOverall
  );

  // fout->Write();
  fout->Close();
  fin->Close();

  cout << "AnalysisTMM output written to " << outName << endl;

}