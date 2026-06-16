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

  h2->FitSlicesY();

  TH1D *h0  = (TH1D*)gDirectory->Get(Form("%s_0"   , h2->GetName()));
  TH1D *h1  = (TH1D*)gDirectory->Get(Form("%s_1"   , h2->GetName()));
  TH1D *h2s = (TH1D*)gDirectory->Get(Form("%s_2"   , h2->GetName()));
  TH1D *hC2 = (TH1D*)gDirectory->Get(Form("%s_chi2", h2->GetName()));

  if (!h0 || !h1 || !h2s || !hC2) {
    cerr << "ERROR: FitSlicesY failed for " << h2->GetName() << endl;
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

// void ExtractDoubleGaussianObservables(TF1 *f,TH1D *h, ObservableResult &obs) {
//   if (!f || !h) return;

//   const double xmin = h->GetXaxis()->GetXmin();
//   const double xmax = h->GetXaxis()->GetXmax();
//   const double binWidth = h->GetXaxis()->GetBinWidth(1);

//   const double A1 = f->GetParameter(0);
//   const double mu = f->GetParameter(1);
//   const double s1 = fabs(f->GetParameter(2));
//   const double A2 = f->GetParameter(3);
//   const double s2 = fabs(f->GetParameter(4));

//   const double eMu = f->GetParError(1);
//   const double eS1 = f->GetParError(2);
//   const double eS2 = f->GetParError(4);

//   obs.spot = mu;
//   obs.spotErr = eMu;

//   obs.charge = NEvtBLOCKS * f->Integral(xmin, xmax) / binWidth;
//   obs.chargeErr = sqrt(NEvtBLOCKS * max(0.0, obs.charge));

//   obs.meancharge = f->Integral(xmin, xmax) / binWidth;
//   obs.meanChargeErr = sqrt(max(0.0, obs.charge));

//   if (A1 * s1 + A2 * s2 > 0.) {
//     // weighted sum of the single variance
//     obs.width = sqrt((A1 * s1 * s1 * s1 + A2 * s2 * s2 * s2) / (A1 * s1 + A2 * s2));
//     obs.widthErr = 0.5 / obs.width * sqrt(
//         pow(((3.*A1*s1*s1*(A1*s1 + A2*s2) - A1*(A1*s1*s1*s1 + A2*s2*s2*s2)) / pow(A1*s1 + A2*s2, 2)) * eS1, 2) +
//         pow(((3.*A2*s2*s2*(A1*s1 + A2*s2) - A2*(A1*s1*s1*s1 + A2*s2*s2*s2)) / pow(A1*s1 + A2*s2, 2)) * eS2, 2)
//     );  
//   }

// }

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

  obs.charge = f->Integral(xmin, xmax) / binWidth;
  obs.chargeErr = sqrt(max(0.0, obs.chargeFit));

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
    mean_charge = charge / entries;
    const double variance = charge2 / entries - obs.meanCharge * obs.meanCharge;
    err_mean_charge = sqrt(max(0.0, variance) / entries);
  }

  obs.charge = charge;
  obs.chargeErr = err_charge;
  obs.meanCharge = mean_charge; //da capire!!!
  obs.meanChargeErr = err_mean_charge;
  obs.spot = h2->GetMean();
  obs.spotErr = 0;
  obs.width = h2->GetRMS();
  obs.widthErr = 0;
  return obs;
}

ObservableResult ComputeObservablesFitSlice(TH2F *h2, TString tag)
{
  ObservableResult obs;
  if (!h2) return obs;
 
  SliceFitResult s = RunFitSlicesY(h2, tag);

  TF1 *f = new TF1("f", "gaus", s.mean->GetBinCenter(123), s.mean->GetBinCenter(231));
  // This can be improved with the double gaussian fit 
  // NB: understand how to derive physics information from the 
  s.mean->Fit(f, "RQ");
  
  obs.charge = f->Integral(s.mean->GetBinCenter(0), s.mean->GetBinCenter(s.mean->GetNbinsX())); //when charge derived from this quantity, one must multiply for the number of recosntructed events
  obs.chargeErr = sqrt(f->Integral(s.mean->GetBinCenter(0), s.mean->GetBinCenter(s.mean->GetNbinsX())));
  obs.meanCharge = f->GetParameter(0);
  obs.meanChargeErr = f->GetParError(0);
  obs.spot = f->GetParameter(1);
  obs.spotErr = f->GetParError(1);
  obs.width = f->GetParameter(2);
  obs.widthErr = f->GetParError(2);
  delete f;
  return obs;
}

void InitGraphPair(TGraphErrors *g[2])
{
  g[0] = new TGraphErrors();
  g[1] = new TGraphErrors();
}

TDirectory* GetOrMakeDir(TDirectory *parent, const TString &name)
{
  if (!parent) return nullptr;

  TObject *obj = parent->Get(name);

  if (obj) {
    TDirectory *dir = dynamic_cast<TDirectory*>(obj);
    if (!dir) {
      cerr << "ERROR: object " << name << " exists but is not a TDirectory" << endl;
      return nullptr;
    }
    return dir;
  }

  return parent->mkdir(name);
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
  }
  
  /////////////FINO A QUI POTREBBE PERFINO FUNZIONARE

  // write out the overall results and keep them as reference for the other coparison block by block
  // maybe write them out to a txt file

  // ============================================================
  // Blocks
  // ============================================================

  //prepare TGraphs and struct for the block analysis (runs over possible analysis methods)
  TGraphErrors *gSpotRaw[TMMCH_N_Readout][2];
  TGraphErrors *gWidthRaw[TMMCH_N_Readout][2];
  TGraphErrors *gChargeRaw[TMMCH_N_Readout][2];
  TGraphErrors *gMeanQRaw[TMMCH_N_Readout][2];

  TGraphErrors *gSpotCalib[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCalib[TMMCH_N_Readout][2];
  TGraphErrors *gChargeCalib[TMMCH_N_Readout][2];
  TGraphErrors *gMeanQCalib[TMMCH_N_Readout][2];

  TGraphErrors *gSpotCalib3s[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCalib3s[TMMCH_N_Readout][2];
  TGraphErrors *gChargeCalib3s[TMMCH_N_Readout][2];
  TGraphErrors *gMeanQCalib3s[TMMCH_N_Readout][2];

  TGraphErrors *gSpotCalibOverall[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCalibOverall[TMMCH_N_Readout][2];
  TGraphErrors *gChargeCalibOverall[TMMCH_N_Readout][2];
  TGraphErrors *gMeanQCalibOverall[TMMCH_N_Readout][2];

  TGraphErrors *gSpotCalib3sOverall[TMMCH_N_Readout][2];
  TGraphErrors *gWidthCalib3sOverall[TMMCH_N_Readout][2];
  TGraphErrors *gChargeCalib3sOverall[TMMCH_N_Readout][2];
  TGraphErrors *gMeanQCalib3sOverall[TMMCH_N_Readout][2];

  vector <ObservableResult> BlockRawObs[TMMCH_N_Readout][2];
  vector <ObservableResult> BlockCalibObs[TMMCH_N_Readout][2], BlockCalib3sObs[TMMCH_N_Readout][2];
  vector <ObservableResult> BlockCalibOverallObs[TMMCH_N_Readout][2], BlockCalib3sOverallObs[TMMCH_N_Readout][2];

  vector<int> blocks = FindBlockNumbers(dBlocksIn);
  cout << "DEBUG: found " << blocks.size() << " blocks" << endl;
  
  for (int iv = 0; iv < TMMCH_N_Readout; ++iv) {
    TString v = views[iv];

    InitGraphPair(gSpotRaw[iv]);
    InitGraphPair(gWidthRaw[iv]);
    InitGraphPair(gChargeRaw[iv]);
    InitGraphPair(gMeanQRaw[iv]);

    InitGraphPair(gSpotCalib[iv]);
    InitGraphPair(gWidthCalib[iv]);
    InitGraphPair(gChargeCalib[iv]);
    InitGraphPair(gMeanQCalib[iv]);

    InitGraphPair(gSpotCalib3s[iv]);
    InitGraphPair(gWidthCalib3s[iv]);
    InitGraphPair(gChargeCalib3s[iv]);
    InitGraphPair(gMeanQCalib3s[iv]);

    InitGraphPair(gSpotCalibOverall[iv]);
    InitGraphPair(gWidthCalibOverall[iv]);
    InitGraphPair(gChargeCalibOverall[iv]);
    InitGraphPair(gMeanQCalibOverall[iv]);

    InitGraphPair(gSpotCalib3sOverall[iv]);
    InitGraphPair(gWidthCalib3sOverall[iv]);
    InitGraphPair(gChargeCalib3sOverall[iv]);
    InitGraphPair(gMeanQCalib3sOverall[iv]);

    TGraphAttribute(gSpotRaw[iv][0],   Form("g_BlockBeamSpotRaw_%s", v.Data()), "block", "beam spot [strip]", 20, iv + 1);
    TGraphAttribute(gWidthRaw[iv][0],  Form("g_BlockBeamSpreadRaw_%s", v.Data()), "block", "beam width [strip]", 20, iv + 1);
    TGraphAttribute(gChargeRaw[iv][0], Form("g_BlockBeamChargeRaw_%s", v.Data()), "block", "integrated charge", 20, iv + 1);
    TGraphAttribute(gMeanQRaw[iv][0],  Form("g_BlockMeanChargeRaw_%s", v.Data()), "block", "mean charge/event", 20, iv + 1);

    TGraphAttribute(gSpotRaw[iv][1],   Form("g_BlockBeamSpotRawFitSlices_%s", v.Data()), "block", "beam spot [strip]", 20, iv + 1);
    TGraphAttribute(gWidthRaw[iv][1],  Form("g_BlockBeamSpreadRawFitSlices_%s", v.Data()), "block", "beam width [strip]", 20, iv + 1);
    TGraphAttribute(gChargeRaw[iv][1], Form("g_BlockBeamChargeRawFitSlices_%s", v.Data()), "block", "integrated charge", 20, iv + 1);
    TGraphAttribute(gMeanQRaw[iv][1],  Form("g_BlockMeanChargeRawFitSlices_%s", v.Data()), "block", "mean charge/event", 20, iv + 1);

    TGraphAttribute(gSpotCalib[iv][0],   Form("g_BlockBeamSpotCalib_%s", v.Data()), "block", "beam spot [strip]", 20, iv + 1);
    TGraphAttribute(gWidthCalib[iv][0],  Form("g_BlockBeamSpreadCalib_%s", v.Data()), "block", "beam width [strip]", 20, iv + 1);
    TGraphAttribute(gChargeCalib[iv][0], Form("g_BlockBeamChargeCalib_%s", v.Data()), "block", "integrated charge", 20, iv + 1);
    TGraphAttribute(gMeanQCalib[iv][0],  Form("g_BlockMeanChargeCalib_%s", v.Data()), "block", "mean charge/event", 20, iv + 1);

    TGraphAttribute(gSpotCalib[iv][1],   Form("g_BlockBeamSpotCalibFitSlices_%s", v.Data()), "block", "beam spot [strip]", 20, iv + 1);
    TGraphAttribute(gWidthCalib[iv][1],  Form("g_BlockBeamSpreadCalibFitSlices_%s", v.Data()), "block", "beam width [strip]", 20, iv + 1);
    TGraphAttribute(gChargeCalib[iv][1], Form("g_BlockBeamChargeCalibFitSlices_%s", v.Data()), "block", "integrated charge", 20, iv + 1);
    TGraphAttribute(gMeanQCalib[iv][1],  Form("g_BlockMeanChargeCalibFitSlices_%s", v.Data()), "block", "mean charge/event", 20, iv + 1);

    TGraphAttribute(gSpotCalib3s[iv][0],   Form("g_BlockBeamSpotCalib3s_%s", v.Data()), "block", "beam spot [strip]", 20, iv + 1);
    TGraphAttribute(gWidthCalib3s[iv][0],  Form("g_BlockBeamSpreadCalib3s_%s", v.Data()), "block", "beam width [strip]", 20, iv + 1);
    TGraphAttribute(gChargeCalib3s[iv][0], Form("g_BlockBeamChargeCalib3s_%s", v.Data()), "block", "integrated charge", 20, iv + 1);
    TGraphAttribute(gMeanQCalib3s[iv][0],  Form("g_BlockMeanChargeCalib3s_%s", v.Data()), "block", "mean charge/event", 20, iv + 1);

    TGraphAttribute(gSpotCalib3s[iv][1],   Form("g_BlockBeamSpotCalib3sFitSlices_%s", v.Data()), "block", "beam spot [strip]", 20, iv + 1);
    TGraphAttribute(gWidthCalib3s[iv][1],  Form("g_BlockBeamSpreadCalib3sFitSlices_%s", v.Data()), "block", "beam width [strip]", 20, iv + 1);
    TGraphAttribute(gChargeCalib3s[iv][1], Form("g_BlockBeamChargeCalib3sFitSlices_%s", v.Data()), "block", "integrated charge", 20, iv + 1);
    TGraphAttribute(gMeanQCalib3s[iv][1],  Form("g_BlockMeanChargeCalib3sFitSlices_%s", v.Data()), "block", "mean charge/event", 20, iv + 1);

    TGraphAttribute(gSpotCalibOverall[iv][0],   Form("g_BlockBeamSpotCalibOverall_%s", v.Data()), "block", "beam spot [strip]", 20, iv + 1);
    TGraphAttribute(gWidthCalibOverall[iv][0],  Form("g_BlockBeamSpreadCalibOverall_%s", v.Data()), "block", "beam width [strip]", 20, iv + 1);
    TGraphAttribute(gChargeCalibOverall[iv][0], Form("g_BlockBeamChargeCalibOverall_%s", v.Data()), "block", "integrated charge", 20, iv + 1);
    TGraphAttribute(gMeanQCalibOverall[iv][0],  Form("g_BlockMeanChargeCalibOverall_%s", v.Data()), "block", "mean charge/event", 20, iv + 1);

    TGraphAttribute(gSpotCalibOverall[iv][1],   Form("g_BlockBeamSpotCalibOverallFitSlices_%s", v.Data()), "block", "beam spot [strip]", 20, iv + 1);
    TGraphAttribute(gWidthCalibOverall[iv][1],  Form("g_BlockBeamSpreadCalibOverallFitSlices_%s", v.Data()), "block", "beam width [strip]", 20, iv + 1);
    TGraphAttribute(gChargeCalibOverall[iv][1], Form("g_BlockBeamChargeCalibOverallFitSlices_%s", v.Data()), "block", "integrated charge", 20, iv + 1);
    TGraphAttribute(gMeanQCalibOverall[iv][1],  Form("g_BlockMeanChargeCalibOverallFitSlices_%s", v.Data()), "block", "mean charge/event", 20, iv + 1);

    TGraphAttribute(gSpotCalib3sOverall[iv][0],   Form("g_BlockBeamSpotCalib3sOverall_%s", v.Data()), "block", "beam spot [strip]", 20, iv + 1);
    TGraphAttribute(gWidthCalib3sOverall[iv][0],  Form("g_BlockBeamSpreadCalib3sOverall_%s", v.Data()), "block", "beam width [strip]", 20, iv + 1);
    TGraphAttribute(gChargeCalib3sOverall[iv][0], Form("g_BlockBeamChargeCalib3sOverall_%s", v.Data()), "block", "integrated charge", 20, iv + 1);
    TGraphAttribute(gMeanQCalib3sOverall[iv][0],  Form("g_BlockMeanChargeCalib3sOverall_%s", v.Data()), "block", "mean charge/event", 20, iv + 1);

    TGraphAttribute(gSpotCalib3sOverall[iv][1],   Form("g_BlockBeamSpotCalib3sOverallFitSlices_%s", v.Data()), "block", "beam spot [strip]", 20, iv + 1);
    TGraphAttribute(gWidthCalib3sOverall[iv][1],  Form("g_BlockBeamSpreadCalib3sOverallFitSlices_%s", v.Data()), "block", "beam width [strip]", 20, iv + 1);
    TGraphAttribute(gChargeCalib3sOverall[iv][1], Form("g_BlockBeamChargeCalib3sOverallFitSlices_%s", v.Data()), "block", "integrated charge", 20, iv + 1);
    TGraphAttribute(gMeanQCalib3sOverall[iv][1],  Form("g_BlockMeanChargeCalib3sOverallFitSlices_%s", v.Data()), "block", "mean charge/event", 20, iv + 1);
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

      // ObservableResult raw0 = ComputeObservables(h2RawBlock, v);
      // ObservableResult raw1 = ComputeObservablesFitSlice(h2RawBlock, v);

      // ObservableResult cal0 = ComputeObservables(h2CalBlock, v);
      // ObservableResult cal1 = ComputeObservablesFitSlice(h2CalBlock, v);

      // ObservableResult cal3s0 = ComputeObservables(h2Cal3sBlock, v);
      // ObservableResult cal3s1 = ComputeObservablesFitSlice(h2Cal3sBlock, v);

      // ObservableResult calOv0 = ComputeObservables(h2CalOverallBlock, v);
      // ObservableResult calOv1 = ComputeObservablesFitSlice(h2CalOverallBlock, v);

      // ObservableResult cal3sOv0 = ComputeObservables(h2Cal3sOverallBlock, v);
      // ObservableResult cal3sOv1 = ComputeObservablesFitSlice(h2Cal3sOverallBlock, v);

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

      AddGraphPoint(gSpotRaw[iv][0],   i, iblk, BlockRawObs[iv][0].back().spot, BlockRawObs[iv][0].back().spotErr);
      AddGraphPoint(gWidthRaw[iv][0],  i, iblk, BlockRawObs[iv][0].back().width, BlockRawObs[iv][0].back().widthErr);
      AddGraphPoint(gChargeRaw[iv][0], i, iblk, BlockRawObs[iv][0].back().charge, BlockRawObs[iv][0].back().chargeErr);
      AddGraphPoint(gMeanQRaw[iv][0],  i, iblk, BlockRawObs[iv][0].back().meanCharge, BlockRawObs[iv][0].back().meanChargeErr);

      AddGraphPoint(gSpotRaw[iv][1],   i, iblk, BlockRawObs[iv][1].back().spot, BlockRawObs[iv][1].back().spotErr);
      AddGraphPoint(gWidthRaw[iv][1],  i, iblk, BlockRawObs[iv][1].back().width, BlockRawObs[iv][1].back().widthErr);
      AddGraphPoint(gChargeRaw[iv][1], i, iblk, BlockRawObs[iv][1].back().charge, BlockRawObs[iv][1].back().chargeErr);
      AddGraphPoint(gMeanQRaw[iv][1],  i, iblk, BlockRawObs[iv][1].back().meanCharge, BlockRawObs[iv][1].back().meanChargeErr);

      AddGraphPoint(gSpotCalib[iv][0],   i, iblk, BlockCalibObs[iv][0].back().spot, BlockCalibObs[iv][0].back().spotErr);
      AddGraphPoint(gWidthCalib[iv][0],  i, iblk, BlockCalibObs[iv][0].back().width, BlockCalibObs[iv][0].back().widthErr);
      AddGraphPoint(gChargeCalib[iv][0], i, iblk, BlockCalibObs[iv][0].back().charge, BlockCalibObs[iv][0].back().chargeErr);
      AddGraphPoint(gMeanQCalib[iv][0],  i, iblk, BlockCalibObs[iv][0].back().meanCharge, BlockCalibObs[iv][0].back().meanChargeErr);

      AddGraphPoint(gSpotCalib[iv][1],   i, iblk, BlockCalibObs[iv][1].back().spot, BlockCalibObs[iv][1].back().spotErr);
      AddGraphPoint(gWidthCalib[iv][1],  i, iblk, BlockCalibObs[iv][1].back().width, BlockCalibObs[iv][1].back().widthErr);
      AddGraphPoint(gChargeCalib[iv][1], i, iblk, BlockCalibObs[iv][1].back().charge, BlockCalibObs[iv][1].back().chargeErr);
      AddGraphPoint(gMeanQCalib[iv][1],  i, iblk, BlockCalibObs[iv][1].back().meanCharge, BlockCalibObs[iv][1].back().meanChargeErr);

      AddGraphPoint(gSpotCalib3s[iv][0],   i, iblk, BlockCalib3sObs[iv][0].back().spot, BlockCalib3sObs[iv][0].back().spotErr);
      AddGraphPoint(gWidthCalib3s[iv][0],  i, iblk, BlockCalib3sObs[iv][0].back().width, BlockCalib3sObs[iv][0].back().widthErr);
      AddGraphPoint(gChargeCalib3s[iv][0], i, iblk, BlockCalib3sObs[iv][0].back().charge, BlockCalib3sObs[iv][0].back().chargeErr);
      AddGraphPoint(gMeanQCalib3s[iv][0],  i, iblk, BlockCalib3sObs[iv][0].back().meanCharge, BlockCalib3sObs[iv][0].back().meanChargeErr);

      AddGraphPoint(gSpotCalib3s[iv][1],   i, iblk, BlockCalib3sObs[iv][1].back().spot, BlockCalib3sObs[iv][1].back().spotErr);
      AddGraphPoint(gWidthCalib3s[iv][1],  i, iblk, BlockCalib3sObs[iv][1].back().width, BlockCalib3sObs[iv][1].back().widthErr);
      AddGraphPoint(gChargeCalib3s[iv][1], i, iblk, BlockCalib3sObs[iv][1].back().charge, BlockCalib3sObs[iv][1].back().chargeErr);
      AddGraphPoint(gMeanQCalib3s[iv][1],  i, iblk, BlockCalib3sObs[iv][1].back().meanCharge, BlockCalib3sObs[iv][1].back().meanChargeErr);

      AddGraphPoint(gSpotCalibOverall[iv][0],   i, iblk, BlockCalibOverallObs[iv][0].back().spot, BlockCalibOverallObs[iv][0].back().spotErr);
      AddGraphPoint(gWidthCalibOverall[iv][0],  i, iblk, BlockCalibOverallObs[iv][0].back().width, BlockCalibOverallObs[iv][0].back().widthErr);
      AddGraphPoint(gChargeCalibOverall[iv][0], i, iblk, BlockCalibOverallObs[iv][0].back().charge, BlockCalibOverallObs[iv][0].back().chargeErr);
      AddGraphPoint(gMeanQCalibOverall[iv][0],  i, iblk, BlockCalibOverallObs[iv][0].back().meanCharge, BlockCalibOverallObs[iv][0].back().meanChargeErr);

      AddGraphPoint(gSpotCalibOverall[iv][1],   i, iblk, BlockCalibOverallObs[iv][1].back().spot, BlockCalibOverallObs[iv][1].back().spotErr);
      AddGraphPoint(gWidthCalibOverall[iv][1],  i, iblk, BlockCalibOverallObs[iv][1].back().width, BlockCalibOverallObs[iv][1].back().widthErr);
      AddGraphPoint(gChargeCalibOverall[iv][1], i, iblk, BlockCalibOverallObs[iv][1].back().charge, BlockCalibOverallObs[iv][1].back().chargeErr);
      AddGraphPoint(gMeanQCalibOverall[iv][1],  i, iblk, BlockCalibOverallObs[iv][1].back().meanCharge, BlockCalibOverallObs[iv][1].back().meanChargeErr);

      AddGraphPoint(gSpotCalib3sOverall[iv][0],   i, iblk, BlockCalib3sOverallObs[iv][0].back().spot, BlockCalib3sOverallObs[iv][0].back().spotErr);
      AddGraphPoint(gWidthCalib3sOverall[iv][0],  i, iblk, BlockCalib3sOverallObs[iv][0].back().width, BlockCalib3sOverallObs[iv][0].back().widthErr);
      AddGraphPoint(gChargeCalib3sOverall[iv][0], i, iblk, BlockCalib3sOverallObs[iv][0].back().charge, BlockCalib3sOverallObs[iv][0].back().chargeErr);
      AddGraphPoint(gMeanQCalib3sOverall[iv][0],  i, iblk, BlockCalib3sOverallObs[iv][0].back().meanCharge, BlockCalib3sOverallObs[iv][0].back().meanChargeErr);

      AddGraphPoint(gSpotCalib3sOverall[iv][1],   i, iblk, BlockCalib3sOverallObs[iv][1].back().spot, BlockCalib3sOverallObs[iv][1].back().spotErr);
      AddGraphPoint(gWidthCalib3sOverall[iv][1],  i, iblk, BlockCalib3sOverallObs[iv][1].back().width, BlockCalib3sOverallObs[iv][1].back().widthErr);
      AddGraphPoint(gChargeCalib3sOverall[iv][1], i, iblk, BlockCalib3sOverallObs[iv][1].back().charge, BlockCalib3sOverallObs[iv][1].back().chargeErr);
      AddGraphPoint(gMeanQCalib3sOverall[iv][1],  i, iblk, BlockCalib3sOverallObs[iv][1].back().meanCharge, BlockCalib3sOverallObs[iv][1].back().meanChargeErr);
    }
  }

  fout->cd();
  dGraphsOut->cd();

  for (int iv = 0; iv < TMMCH_N_Readout; ++iv) {

    gSpotRaw[iv][0]->Write();
    gWidthRaw[iv][0]->Write();
    gChargeRaw[iv][0]->Write();
    gMeanQRaw[iv][0]->Write();

    gSpotRaw[iv][1]->Write();
    gWidthRaw[iv][1]->Write();
    gChargeRaw[iv][1]->Write();
    gMeanQRaw[iv][1]->Write();

    gSpotCalib[iv][0]->Write();
    gWidthCalib[iv][0]->Write();
    gChargeCalib[iv][0]->Write();
    gMeanQCalib[iv][0]->Write();

    gSpotCalib[iv][1]->Write();
    gWidthCalib[iv][1]->Write();
    gChargeCalib[iv][1]->Write();
    gMeanQCalib[iv][1]->Write();

    gSpotCalib3s[iv][0]->Write();
    gWidthCalib3s[iv][0]->Write();
    gChargeCalib3s[iv][0]->Write();
    gMeanQCalib3s[iv][0]->Write();

    gSpotCalib3s[iv][1]->Write();
    gWidthCalib3s[iv][1]->Write();
    gChargeCalib3s[iv][1]->Write();
    gMeanQCalib3s[iv][1]->Write();

    gSpotCalibOverall[iv][0]->Write();
    gWidthCalibOverall[iv][0]->Write();
    gChargeCalibOverall[iv][0]->Write();
    gMeanQCalibOverall[iv][0]->Write();

    gSpotCalibOverall[iv][1]->Write();
    gWidthCalibOverall[iv][1]->Write();
    gChargeCalibOverall[iv][1]->Write();
    gMeanQCalibOverall[iv][1]->Write();

    gSpotCalib3sOverall[iv][0]->Write();
    gWidthCalib3sOverall[iv][0]->Write();
    gChargeCalib3sOverall[iv][0]->Write();
    gMeanQCalib3sOverall[iv][0]->Write();

    gSpotCalib3sOverall[iv][1]->Write();
    gWidthCalib3sOverall[iv][1]->Write();
    gChargeCalib3sOverall[iv][1]->Write();
    gMeanQCalib3sOverall[iv][1]->Write();
  }

  // fout->Write();
  fout->Close();
  fin->Close();

  cout << "AnalysisTMM output written to " << outName << endl;

}