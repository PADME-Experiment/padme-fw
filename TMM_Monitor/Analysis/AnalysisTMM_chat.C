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

struct SliceFitResult {
  TH1D *amp = nullptr;
  TH1D *mean = nullptr;
  TH1D *sigma = nullptr;
};

struct ObservableResult {
  double charge = 0.;
  double chargeErr = 0.;
  double meanCharge = 0.;
  double meanChargeErr = 0.;
  double spot = 0.;
  double spotErr = 0.;
  double width = 0.;
  double widthErr = 0.;
  double entries = 0.;
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

TH1D* BuildChargeProjectionX(TH2F *h2, const TString &name)
{
  if (!h2) return nullptr;

  const int nx = h2->GetNbinsX();
  const int ny = h2->GetNbinsY();

  TH1D *h = new TH1D(name, Form("%s charge;x_{strip};integrated charge", h2->GetTitle()), nx, h2->GetXaxis()->GetXmin(), h2->GetXaxis()->GetXmax());

  h->SetDirectory(0);

  for (int ix = 1; ix <= nx; ++ix) {
    double qsum = 0.;
    double q2sum = 0.;

    for (int iy = 1; iy <= ny; ++iy) {
      const double n = h2->GetBinContent(ix, iy);
      const double q = h2->GetYaxis()->GetBinCenter(iy);

      qsum  += n * q;
      q2sum += n * q * q;
    }

    h->SetBinContent(ix, qsum);
    h->SetBinError(ix, sqrt(fabs(q2sum)));
  }

  return h;
}

SliceFitResult RunFitSlicesY(TH2F *h2, TString tag){
  
  SliceFitResult out;

  if (!h2) return out;

  h2->FitSlicesY();

  TH1D *h0 = (TH1D*)gDirectory->Get(Form("%s_0", h2->GetName()));
  TH1D *h1 = (TH1D*)gDirectory->Get(Form("%s_1", h2->GetName()));
  TH1D *h2s = (TH1D*)gDirectory->Get(Form("%s_2", h2->GetName()));

  if (!h0 || !h1 || !h2s) {
    cerr << "ERROR: FitSlicesY failed for " << h2->GetName() << endl;
    return out;
  }

  out.amp   = (TH1D*)h0->Clone(Form("hAmpSlice_%s", tag.Data()));
  out.mean  = (TH1D*)h1->Clone(Form("hMeanSlice_%s", tag.Data()));
  out.sigma = (TH1D*)h2s->Clone(Form("hSigmaSlice_%s", tag.Data()));

  out.amp->SetDirectory(0);
  out.mean->SetDirectory(0);
  out.sigma->SetDirectory(0);

  return out;
}

TF1* FitDoubleGaussian(TH1D *h, TString name, double xmin, double xmax){
  
  TF1 *prefit = new TF1(Form("prefit_%s", name.Data()), "gaus", xmin, xmax);
  h->Fit(prefit, "RQ0");

  double A0  = prefit->GetParameter(0);
  double mu0 = prefit->GetParameter(1);
  double s0  = fabs(prefit->GetParameter(2));
  if (s0 <= 0) s0 = 10.;

  TF1 *fit = new TF1( Form("f_%s", name.Data()), "[0]*exp(-0.5*((x-[1])/[2])^2)+[3]*exp(-0.5*((x-[1])/[4])^2)", xmin, xmax );

  fit->SetParNames("A_1", "mean", "sigma_1", "A_2", "sigma_2");
  fit->SetParameters(A0/2, mu0, s0, A0, s0/2);

  fit->SetParLimits(0, 0., 10.*A0);
  fit->SetParLimits(1, xmin, xmax);
  fit->SetParLimits(2, 1., xmax-xmin);
  fit->SetParLimits(3, 0., 10.*A0);
  fit->SetParLimits(4, 1., 3.*(xmax-xmin));

  h->Fit(fit, "RQ");

  delete prefit;
  return fit;
}

ObservableResult ComputeObservables(TH2F *h2, TString tag, TDirectory *outdir)
{
  ObservableResult obs;
  if (!h2) return obs;

  const int nx = h2->GetNbinsX();
  const int ny = h2->GetNbinsY();

  double sumN = 0.;
  double sumQ = 0.;
  double sumQ2 = 0.;
  double sumXQ = 0.;
  double sumX2Q = 0.;

  for (int ix = 1; ix <= nx; ++ix) {
    const double x = h2->GetXaxis()->GetBinCenter(ix);

    for (int iy = 1; iy <= ny; ++iy) {
      const double n = h2->GetBinContent(ix, iy);
      const double q = h2->GetYaxis()->GetBinCenter(iy);

      sumN   += n;
      sumQ   += n * q;
      sumQ2  += n * q * q;
      sumXQ  += n * q * x;
      sumX2Q += n * q * x * x;
    }
  }
 
  obs.entries = sumN;
  obs.charge = sumQ;
  obs.chargeErr = sqrt(fabs(sumQ2));

  if (sumN > 0.) {
    obs.meanCharge = sumQ / sumN;
    const double varQ = sumQ2 / sumN - obs.meanCharge * obs.meanCharge;
    obs.meanChargeErr = sqrt(fabs(varQ) / sumN);
  }

  if (sumQ > 0.) {
    obs.spot = sumXQ / sumQ;
    const double varX = sumX2Q / sumQ - obs.spot * obs.spot;
    obs.width = sqrt(fabs(varX));
    obs.spotErr = obs.width / sqrt(max(1.0, sumN));
    obs.widthErr = obs.width / sqrt(2. * max(1.0, sumN));
  }

  if (outdir) outdir->cd();

  TH1D *hChargeX = BuildChargeProjectionX(h2, Form("hChargeProjectionX_%s", tag.Data()));

  if (hChargeX) {
    hChargeX->Write();

    const double xmin = hChargeX->GetXaxis()->GetXmin();
    const double xmax = hChargeX->GetXaxis()->GetXmax();

    if (hChargeX->Integral() > 0.) {
      TF1 *f = FitDoubleGaussian(hChargeX, Form("BeamFit_%s", tag.Data()), xmin, xmax);

      if (f) {
        obs.spot = f->GetParameter(1);
        obs.spotErr = f->GetParError(1);

        const double s1 = fabs(f->GetParameter(2));
        const double s2 = fabs(f->GetParameter(4));
        const double e1 = f->GetParError(2);
        const double e2 = f->GetParError(4);

        // Uso la sigma più stretta come core beam width.
        if (s1 <= s2) {
          obs.width = s1;
          obs.widthErr = e1;
        } else {
          obs.width = s2;
          obs.widthErr = e2;
        }

        f->Write();
      }
    }
  }

  return obs;
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
}

void AnalyzeOneHistogram(TH2F *h2, TString tag, TDirectory *outdir)
{
  if (!h2 || !outdir) return;

  outdir->cd();

  TH2F *hc = (TH2F*)h2->Clone(Form("h2_%s", tag.Data()));
  hc->SetDirectory(outdir);
  hc->Write();

  WriteSliceFit(hc, tag, outdir);

  ObservableResult obs = ComputeObservables(hc, tag, outdir);

  TTree *t = new TTree(Form("tObs_%s", tag.Data()), Form("Observables %s", tag.Data()));

  t->Branch("entries",        &obs.entries,        "entries/D");
  t->Branch("charge",         &obs.charge,         "charge/D");
  t->Branch("chargeErr",      &obs.chargeErr,      "chargeErr/D");
  t->Branch("meanCharge",     &obs.meanCharge,     "meanCharge/D");
  t->Branch("meanChargeErr",  &obs.meanChargeErr,  "meanChargeErr/D");
  t->Branch("spot",           &obs.spot,           "spot/D");
  t->Branch("spotErr",        &obs.spotErr,        "spotErr/D");
  t->Branch("width",          &obs.width,          "width/D");
  t->Branch("widthErr",       &obs.widthErr,       "widthErr/D");

  t->Fill();
  t->Write();
}

vector<int> FindBlockNumbers(TDirectory *dir, TString view)
{
  set<int> blocks;

  if (!dir) return vector<int>();

  TIter next(dir->GetListOfKeys());
  TKey *key = nullptr;

  while ((key = (TKey*)next())) {
    TString name = key->GetName();

    TString prefix = Form("hBlockqmaxstrip%s_block", view.Data());

    if (!name.BeginsWith(prefix)) continue;
    if (name.Contains("Full")) continue;
    if (name.Contains("_cal")) continue;

    int iblk = -1;
    sscanf(name.Data(), Form("hBlockqmaxstrip%s_block%%d", view.Data()), &iblk);

    if (iblk >= 0) blocks.insert(iblk);
  }

  return vector<int>(blocks.begin(), blocks.end());
}

void AnalysisTMM(char *InputFileName)
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

  if (!fout || fout->IsZombie()) {
    cerr << "ERROR: cannot create output file " << outName << endl;
    fin->Close();
    return;
  }

  TDirectory *dOverallIn = (TDirectory*)fin->Get("Overall");
  TDirectory *dCalibIn   = (TDirectory*)fin->Get("Calib");
  TDirectory *dBlocksIn  = (TDirectory*)fin->Get("Blocks");

  TDirectory *dOverallOut = fout->mkdir("Overall");
  TDirectory *dCalibOut   = fout->mkdir("Calib");
  TDirectory *dBlocksOut  = fout->mkdir("Blocks");
  TDirectory *dGraphsOut  = fout->mkdir("Graphs");

  TString views[TMMCH_N_Readout] = {"X", "Y"};

  // ============================================================
  // Overall + Calib
  // ============================================================

  for (int iv = 0; iv < TMMCH_N_Readout; ++iv) {
    TString v = views[iv];

    if (dOverallIn) {
      TDirectory *dv = dOverallOut->mkdir(v);
      dv->cd();

      AnalyzeOneHistogram(
        GetH2F(dOverallIn, Form("hqmaxstrip%s", v.Data())),
        Form("%s_even", v.Data()),
        dv
      );

      AnalyzeOneHistogram(
        GetH2F(dOverallIn, Form("hqmaxstripFull%s", v.Data())),
        Form("%s_full", v.Data()),
        dv
      );
    }

    if (dCalibIn) {
      TDirectory *dv = dCalibOut->mkdir(v);
      dv->cd();

      AnalyzeOneHistogram(
        GetH2F(dCalibIn, Form("hqmaxstrip_cal%s", v.Data())),
        Form("%s_cal", v.Data()),
        dv
      );

      AnalyzeOneHistogram(
        GetH2F(dCalibIn, Form("hqmaxstripFull_cal%s", v.Data())),
        Form("%s_full_cal", v.Data()),
        dv
      );

      AnalyzeOneHistogram(
        GetH2F(dCalibIn, Form("hqmaxstrip_cal3s%s", v.Data())),
        Form("%s_cal3s", v.Data()),
        dv
      );


      AnalyzeOneHistogram(
        GetH2F(dCalibIn, Form("hqmaxstripFull_cal3s%s", v.Data())),
        Form("%s_full_cal3s", v.Data()),
        dv
      );
    }
  }

  // ============================================================
  // Blocks
  // ============================================================

  for (int iv = 0; iv < TMMCH_N_Readout; ++iv) {
    TString v = views[iv];

    vector<int> blocks = FindBlockNumbers(dBlocksIn, v);

    TGraphErrors *gSpot      = new TGraphErrors();
    TGraphErrors *gWidth     = new TGraphErrors();
    TGraphErrors *gCharge    = new TGraphErrors();
    TGraphErrors *gMeanQ     = new TGraphErrors();

    TGraphAttribute(gSpot,   Form("g_BlockBeamSpot_%s",   v.Data()), "block", "beam spot [strip]", 20, iv + 1);
    TGraphAttribute(gWidth,  Form("g_BlockBeamSpread_%s", v.Data()), "block", "beam width [strip]", 20, iv + 1);
    TGraphAttribute(gCharge, Form("g_BlockBeamCharge_%s", v.Data()), "block", "integrated charge",  20, iv + 1);
    TGraphAttribute(gMeanQ,  Form("g_BlockMeanCharge_%s", v.Data()), "block", "mean charge/event", 20, iv + 1);

    TDirectory *dv = dBlocksOut->mkdir(v);

    for (size_t i = 0; i < blocks.size(); ++i) {
      const int iblk = blocks[i];

      TString hname = Form("hBlockqmaxstrip%s_block%04d", v.Data(), iblk);
      TH2F *h2 = GetH2F(dBlocksIn, hname);

      if (!h2) {
        cerr << "WARNING: missing " << hname << endl;
        continue;
      }

      TDirectory *db = dv->mkdir(Form("block%04d", iblk));
      db->cd();

      TString tag = Form("%s_block%d", v.Data(), iblk);

      TH2F *hc = (TH2F*)h2->Clone(Form("h2_%s", tag.Data()));
      hc->SetDirectory(db);
      hc->Write();

      WriteSliceFit(hc, tag, db);

      ObservableResult obs = ComputeObservables(hc, tag, db);

      AddGraphPoint(gSpot,   i, iblk, obs.spot,       obs.spotErr);
      AddGraphPoint(gWidth,  i, iblk, obs.width,      obs.widthErr);
      AddGraphPoint(gCharge, i, iblk, obs.charge,     obs.chargeErr);
      AddGraphPoint(gMeanQ,  i, iblk, obs.meanCharge, obs.meanChargeErr);

      TTree *t = new TTree(Form("tObs_block%04d_%s", iblk, v.Data()), "Block observables");

      t->Branch("block",          (void*)&iblk,        "block/I");
      t->Branch("entries",        &obs.entries,        "entries/D");
      t->Branch("charge",         &obs.charge,         "charge/D");
      t->Branch("chargeErr",      &obs.chargeErr,      "chargeErr/D");
      t->Branch("meanCharge",     &obs.meanCharge,     "meanCharge/D");
      t->Branch("meanChargeErr",  &obs.meanChargeErr,  "meanChargeErr/D");
      t->Branch("spot",           &obs.spot,           "spot/D");
      t->Branch("spotErr",        &obs.spotErr,        "spotErr/D");
      t->Branch("width",          &obs.width,          "width/D");
      t->Branch("widthErr",       &obs.widthErr,       "widthErr/D");

      t->Fill();
      t->Write();
    }

    dGraphsOut->cd();

    gSpot->Write();
    gWidth->Write();
    gCharge->Write();
    gMeanQ->Write();
  }

  fout->Write();
  fout->Close();
  fin->Close();

  cout << "AnalysisTMM output written to " << outName << endl;

}