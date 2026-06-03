#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <array>

#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TMath.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TString.h"
#include "TF1.h"
#include "TMinuit.h"
#include "TGraphErrors.h"
#include "TRandom3.h"
#include "TCanvas.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TFitter.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TLine.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TDecompSVD.h"

using namespace std;

// Helper: TGraphErrors cosmetics
int TGraphAttribute(TGraphErrors *Graph, TString title, TString xlabel, TString ylabel,
                    int markerstyle, int color){
  Graph->SetTitle(title);
  Graph->SetName(title);
  Graph->GetXaxis()->SetTitle(xlabel);
  Graph->GetYaxis()->SetTitle(ylabel);
  Graph->SetMarkerStyle(markerstyle);
  Graph->SetMarkerColor(color);
  return 0;
}

void Aging() {
  const int NRUNS     = 11;
  int runids[NRUNS]   = {  80344,   80357,   80415,   80427,   80472,   80534,   80581,   80615,   80632,   80649,  80677};
  int run[NRUNS]      = {    344,     357,     415,     427,     472,     534,     581,     615,     632,     649,    677};
  double ebeam[NRUNS] = {  293.5,     292,     283,   281.5,     271,  294.25,  289.75,  285.25,  280.75,  277.75, 270.25};
  double NPOT[NRUNS]  = {1.65e10, 2.81e10, 1.61e11, 1.84e11, 3.94e11, 4.43e11, 5.07e11, 5.97e11, 6.75e11, 7.01e11, 8.4e11};

  // const int NRUNS     = 9;
  // int runids[NRUNS]   = {80344,  80357,  80415,  80427,  80472,  80534,  80615,  80632,  80677};
  // int run[NRUNS]      = {  344,    357,    415,    427,    472,    534,    615,    632,    677};
  // double ebeam[NRUNS] = {293.5,    292,    283,  281.5,    271, 294.25, 285.25, 280.75, 270.25};
  // double NPOT[NRUNS]  = {1.65e10, 2.81e10, 1.61e11, 1.84e11, 3.94e11, 4.4e11, 5.97e11, 6.88e11, 8.42e11};

  
  const string gName_raw_period = "gQratio_raw";
  const string gName_raw_ECorr_period = "gQratio_raw_ECorr";
  const string gName_cal_period = "gQratio_cal";
  const string gName_cal_ECorr_period = "gQratio_cal_ECorr";

  const string gName_raw = "gR_raw";
  const string gName_raw_ECorr = "gR_raw_ECorr";
  const string gName_cal = "gR_cal";
  const string gName_cal_ECorr = "gR_cal_ECorr";

  const string gName_raw_fitted = "gR_fitted_raw";
  const string gName_raw_ECorr_fitted = "gR_fitted_raw_ECorr";
  const string gName_cal_fitted = "gR_fitted_cal";
  const string gName_cal_ECorr_fitted = "gR_fitted_cal_ECorr";

  TFile *fout = new TFile("/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/CalibTarget/outputCalibration/Run4Monitor/AgingResult_NewCharge3.root", "RECREATE");

  TGraphErrors *gAging_raw_period = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_raw_ECorr_period = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_cal_period = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_cal_ECorr_period = new TGraphErrors(NRUNS);

  TGraphErrors *gAging_raw = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_raw_ECorr = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_cal = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_cal_ECorr = new TGraphErrors(NRUNS);

  TGraphErrors *gAging_raw_fitted = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_raw_ECorr_fitted = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_cal_fitted = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_cal_ECorr_fitted = new TGraphErrors(NRUNS);

  TGraphErrors *gAging_raw_period_POT = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_raw_ECorr_period_POT = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_cal_period_POT = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_cal_ECorr_period_POT = new TGraphErrors(NRUNS);

  TGraphErrors *gAging_raw_POT = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_raw_ECorr_POT = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_cal_POT = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_cal_ECorr_POT = new TGraphErrors(NRUNS);

  TGraphErrors *gAging_raw_fitted_POT = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_raw_ECorr_fitted_POT = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_cal_fitted_POT = new TGraphErrors(NRUNS);
  TGraphErrors *gAging_cal_ECorr_fitted_POT = new TGraphErrors(NRUNS);

  for (int i = 0; i < NRUNS; ++i) {
    const int runid = runids[i];

    TString inFileName = Form("/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/CalibTarget/outputCalibration/Run4Monitor/TargetAnalysis_run_00%d.root", runid);

    TFile *fin = TFile::Open(inFileName, "READ");
    if (!fin || fin->IsZombie()) {
      cerr << "[WARN] Cannot open file: " << inFileName << " (skipping)\n";
      if (fin) { fin->Close(); delete fin; }
      continue;
    }

    TGraphErrors *gIn_raw_period = (TGraphErrors*) fin->Get( gName_raw_period.c_str());
    TGraphErrors *gIn_raw_ECorr_period = (TGraphErrors*) fin->Get( gName_raw_ECorr_period.c_str());
    TGraphErrors *gIn_cal_period = (TGraphErrors*) fin->Get( gName_cal_period.c_str());
    TGraphErrors *gIn_cal_ECorr_period = (TGraphErrors*) fin->Get( gName_cal_ECorr_period.c_str());

    TGraphErrors *gIn_raw = (TGraphErrors*) fin->Get( gName_raw.c_str());
    TGraphErrors *gIn_raw_ECorr = (TGraphErrors*) fin->Get( gName_raw_ECorr.c_str());
    TGraphErrors *gIn_cal = (TGraphErrors*) fin->Get( gName_cal.c_str());
    TGraphErrors *gIn_cal_ECorr = (TGraphErrors*) fin->Get( gName_cal_ECorr.c_str());

    TGraphErrors *gIn_raw_fitted = (TGraphErrors*) fin->Get( gName_raw_fitted.c_str());
    TGraphErrors *gIn_raw_ECorr_fitted = (TGraphErrors*) fin->Get( gName_raw_ECorr_fitted.c_str());
    TGraphErrors *gIn_cal_fitted = (TGraphErrors*) fin->Get( gName_cal_fitted.c_str());
    TGraphErrors *gIn_cal_ECorr_fitted = (TGraphErrors*) fin->Get( gName_cal_ECorr_fitted.c_str());

    if (!gIn_raw_period || !gIn_raw_ECorr_period || !gIn_cal_period || !gIn_cal_ECorr_period) {
      cerr << "[WARN] Cannot find TGraphErrors period in " << inFileName << " (skipping)\n";
      fin->Close(); delete fin;
      continue;
    }
    if (!gIn_raw || !gIn_raw_ECorr || !gIn_cal || !gIn_cal_ECorr) {
      cerr << "[WARN] Cannot find TGraphErrors reco in " << inFileName << " (skipping)\n";
      fin->Close(); delete fin;
      continue;
    }
    if (!gIn_raw_fitted || !gIn_raw_ECorr_fitted || !gIn_cal_fitted || !gIn_cal_ECorr_fitted) {
      cerr << "[WARN] Cannot find TGraphErrors fitted in " << inFileName << " (skipping)\n";
      fin->Close(); delete fin;
      continue;
    }

    // Clone so you can write the fitted graph to the output file
    TGraphErrors *g_raw_period = (TGraphErrors*) gIn_raw_period->Clone(Form("gQratio_raw%d_clone", runid));
    TGraphErrors *g_raw_ECorr_period = (TGraphErrors*) gIn_raw_ECorr->Clone(Form("gQratio_raw_ECorr_run%d_clone", runid));
    TGraphErrors *g_cal_period = (TGraphErrors*) gIn_cal->Clone(Form("gQratio_cal_run%d_clone", runid));
    TGraphErrors *g_cal_ECorr_period = (TGraphErrors*) gIn_cal_ECorr->Clone(Form("gQratio_cal_ECorr_run%d_clone", runid));

    // Fit with your pol0 procedure
    TF1* fpol0_raw = new TF1(Form("fpol0_raw_run%d", runid), "pol0");
    g_raw_period->Fit(fpol0_raw);  // add "R" or other options if you really need them
    const double c  = fpol0_raw->GetParameter(0);
    const double ec = fpol0_raw->GetParError(0);
    // ---- THIS is the fit parameter obtained (constant and its error)
    cout << "runid " << runid << " (run " << run[i] << "):  pol0 = " << c << " +/- " << ec << endl;
    // Fill summary graph (x=run number, y=constant)
    gAging_raw_period->SetPoint(i, run[i], c);
    gAging_raw_period->SetPointError(i, 0.0, ec);
    gAging_raw_period_POT->SetPoint(i, NPOT[i], c);
    gAging_raw_period_POT->SetPointError(i, 0.0, ec);

    TF1* fpol0_raw_ECorr = new TF1(Form("fpol0_raw_ECorr_run%d", runid), "pol0");
    g_raw_ECorr_period->Fit(fpol0_raw_ECorr);  // add "R" or other options if you really need them
    const double c_ECorr  = fpol0_raw_ECorr->GetParameter(0);
    const double ec_ECorr = fpol0_raw_ECorr->GetParError(0);
    // ---- THIS is the fit parameter obtained (constant and its error)
    cout << "runid " << runid << " (run " << run[i] << "):  pol0 = " << c_ECorr << " +/- " << ec_ECorr << endl;
    // Fill summary graph (x=run number, y=constant)
    gAging_raw_ECorr_period->SetPoint(i, run[i], c_ECorr);
    gAging_raw_ECorr_period->SetPointError(i, 0.0, ec_ECorr);
    gAging_raw_ECorr_period_POT->SetPoint(i, NPOT[i], c_ECorr);
    gAging_raw_ECorr_period_POT->SetPointError(i, 0.0, ec_ECorr);

    TF1* fpol0_cal = new TF1(Form("fpol0_cal_run%d", runid), "pol0");
    g_cal_period->Fit(fpol0_cal);  // add "R" or other options if you really need them
    const double c_cal  = fpol0_cal->GetParameter(0);
    const double ec_cal = fpol0_cal->GetParError(0);
    // ---- THIS is the fit parameter obtained (constant and its error)
    cout << "runid " << runid << " (run " << run[i] << "):  pol0 = " << c_cal << " +/- " << ec_cal << endl;
    // Fill summary graph (x=run number, y=constant)
    gAging_cal_period->SetPoint(i, run[i], c_cal);
    gAging_cal_period->SetPointError(i, 0.0, ec_cal);
    gAging_cal_period_POT->SetPoint(i, NPOT[i], c_cal);
    gAging_cal_period_POT->SetPointError(i, 0.0, ec_cal);

    TF1* fpol0_cal_ECorr = new TF1(Form("fpol0_cal_ECorr_run%d", runid), "pol0");
    g_cal_ECorr_period->Fit(fpol0_cal_ECorr);  // add "R" or other options if you really need them
    const double c_cal_ECorr  = fpol0_cal_ECorr->GetParameter(0);
    const double ec_cal_ECorr = fpol0_cal_ECorr->GetParError(0);
    // ---- THIS is the fit parameter obtained (constant and its error)
    cout << "runid " << runid << " (run " << run[i] << "):  pol0 = " << c_cal_ECorr << " +/- " << ec_cal_ECorr << endl;
    // Fill summary graph (x=run number, y=constant)
    gAging_cal_ECorr_period->SetPoint(i, run[i], c_cal_ECorr);
    gAging_cal_ECorr_period->SetPointError(i, 0.0, ec_cal_ECorr);
    gAging_cal_ECorr_period_POT->SetPoint(i, NPOT[i], c_cal_ECorr);
    gAging_cal_ECorr_period_POT->SetPointError(i, 0.0, ec_cal_ECorr);

    // Clone graph for the fitted charge aging evaluation 
    TGraphErrors *g_raw = (TGraphErrors*) gIn_raw->Clone(Form("gR_raw%d_clone", runid));
    TGraphErrors *g_raw_ECorr = (TGraphErrors*) gIn_raw_ECorr->Clone(Form("gR_raw_ECorr_run%d_clone", runid));
    TGraphErrors *g_cal = (TGraphErrors*) gIn_cal->Clone(Form("gR_cal_run%d_clone", runid));
    TGraphErrors *g_cal_ECorr = (TGraphErrors*) gIn_cal_ECorr->Clone(Form("gR_cal_ECorr_run%d_clone", runid));

    TGraphErrors *g_raw_fitted = (TGraphErrors*) gIn_raw_fitted->Clone(Form("gR_raw_fitted_run%d_clone", runid));
    TGraphErrors *g_raw_ECorr_fitted = (TGraphErrors*) gIn_raw_ECorr_fitted->Clone(Form("gR_raw_ECorr_fitted_run%d_clone", runid));
    TGraphErrors *g_cal_fitted = (TGraphErrors*) gIn_cal_fitted->Clone(Form("gR_cal_fitted_run%d_clone", runid));
    TGraphErrors *g_cal_ECorr_fitted = (TGraphErrors*) gIn_cal_ECorr_fitted->Clone(Form("gR_cal_ECorr_fitted_run%d_clone", runid));
    
    gAging_raw->SetPoint(i, run[i], g_raw->GetY()[0]);
    gAging_raw->SetPointError(i, 0.0, g_raw->GetEY()[0]);
    gAging_raw_ECorr->SetPoint(i, run[i], g_raw_ECorr->GetY()[0]);
    gAging_raw_ECorr->SetPointError(i, 0.0, g_raw_ECorr->GetEY()[0]);
    gAging_cal->SetPoint(i, run[i], g_cal->GetY()[0]);
    gAging_cal->SetPointError(i, 0.0, g_cal->GetEY()[0]);
    gAging_cal_ECorr->SetPoint(i, run[i], g_cal_ECorr->GetY()[0]);
    gAging_cal_ECorr->SetPointError(i, 0.0, g_cal_ECorr->GetEY()[0]);

    gAging_raw_fitted->SetPoint(i, run[i], g_raw_fitted->GetY()[0]);
    gAging_raw_fitted->SetPointError(i, 0.0, g_raw_fitted->GetEY()[0]);
    gAging_raw_ECorr_fitted->SetPoint(i, run[i], g_raw_ECorr_fitted->GetY()[0]);
    gAging_raw_ECorr_fitted->SetPointError(i, 0.0, g_raw_ECorr_fitted->GetEY()[0]);
    gAging_cal_fitted->SetPoint(i, run[i], g_cal_fitted->GetY()[0]);
    gAging_cal_fitted->SetPointError(i, 0.0, g_cal_fitted->GetEY()[0]);
    gAging_cal_ECorr_fitted->SetPoint(i, run[i], g_cal_ECorr_fitted->GetY()[0]);
    gAging_cal_ECorr_fitted->SetPointError(i, 0.0, g_cal_ECorr_fitted->GetEY()[0]);

    gAging_raw_POT->SetPoint(i, NPOT[i], g_raw->GetY()[0]);
    gAging_raw_POT->SetPointError(i, 0.0, g_raw->GetEY()[0]);
    gAging_raw_ECorr_POT->SetPoint(i, NPOT[i], g_raw_ECorr->GetY()[0]);
    gAging_raw_ECorr_POT->SetPointError(i, 0.0, g_raw_ECorr->GetEY()[0]);
    gAging_cal_POT->SetPoint(i, NPOT[i], g_cal->GetY()[0]);
    gAging_cal_POT->SetPointError(i, 0.0, g_cal->GetEY()[0]);
    gAging_cal_ECorr_POT->SetPoint(i, NPOT[i], g_cal_ECorr->GetY()[0]);
    gAging_cal_ECorr_POT->SetPointError(i, 0.0, g_cal_ECorr->GetEY()[0]);

    gAging_raw_fitted_POT->SetPoint(i, NPOT[i], g_raw_fitted->GetY()[0]);
    gAging_raw_fitted_POT->SetPointError(i, 0.0, g_raw_fitted->GetEY()[0]);
    gAging_raw_ECorr_fitted_POT->SetPoint(i, NPOT[i], g_raw_ECorr_fitted->GetY()[0]);
    gAging_raw_ECorr_fitted_POT->SetPointError(i, 0.0, g_raw_ECorr_fitted->GetEY()[0]);
    gAging_cal_fitted_POT->SetPoint(i, NPOT[i], g_cal_fitted->GetY()[0]);
    gAging_cal_fitted_POT->SetPointError(i, 0.0, g_cal_fitted->GetEY()[0]);
    gAging_cal_ECorr_fitted_POT->SetPoint(i, NPOT[i], g_cal_ECorr_fitted->GetY()[0]);
    gAging_cal_ECorr_fitted_POT->SetPointError(i, 0.0, g_cal_ECorr_fitted->GetEY()[0]);

    // Write fitted graph
    // fout->cd();
    // g_raw_period->Write();
    // g_raw_ECorr_period->Write();
    // g_cal_period->Write();
    // g_cal_ECorr_period->Write();

    // g_raw->Write();
    // g_raw_ECorr->Write();
    // g_cal->Write();
    // g_cal_ECorr->Write();

    // g_raw_fitted->Write();
    // g_raw_ECorr_fitted->Write();
    // g_cal_fitted->Write();
    // g_cal_ECorr_fitted->Write();

    delete g_raw_period;
    delete g_raw_ECorr_period;
    delete g_cal_period;
    delete g_cal_ECorr_period;
    delete g_raw;
    delete g_raw_ECorr;
    delete g_cal;
    delete g_cal_ECorr;
    delete g_raw_fitted;
    delete g_raw_ECorr_fitted;
    delete g_cal_fitted;
    delete g_cal_ECorr_fitted; 
    fin->Close();
    delete fin;
  }

  TGraphAttribute(gAging_raw_period, "gAging_raw Period", "Run Num", "Q_{X-tar}/Q_{LG} raw", 20, kRed+2);
  TGraphAttribute(gAging_raw_ECorr_period, "gAging_raw_ECorr Period", "Run Num", "Q_{X-tar}/Q_{LG} raw ECorr", 20, kRed+2);
  TGraphAttribute(gAging_cal_period, "gAging_cal Period", "Run Num", "Q_{X-tar}/Q_{LG} cal", 20, kBlue+2);
  TGraphAttribute(gAging_cal_ECorr_period, "gAging_cal_Corr Period", "Run Num", "Q_{X-tar}/Q_{LG} cal ECorr", 20, kBlue+2);

  TGraphAttribute(gAging_raw, "gAging_raw", "Run Num", "Q_{X-tar}/Q_{LG} raw", 20, kRed+2);
  TGraphAttribute(gAging_raw_ECorr, "gAging_raw_ECorr", "Run Num", "Q_{X-tar}/Q_{LG} raw ECorr", 20, kRed+2);
  TGraphAttribute(gAging_cal, "gAging_cal", "Run Num", "Q_{X-tar}/Q_{LG} cal", 20, kBlue+2);
  TGraphAttribute(gAging_cal_ECorr, "gAging_cal_ECorr", "Run Num", "Q_{X-tar}/Q_{LG} cal ECorr", 20, kBlue+2);

  TGraphAttribute(gAging_raw_fitted, "gAging_raw_fitted", "Run Num", "Q_{X-tar}/Q_{LG} raw", 20, kRed+2);
  TGraphAttribute(gAging_raw_ECorr_fitted, "gAging_raw_ECorr_fitted", "Run Num", "Q_{X-tar}/Q_{LG} raw ECorr", 20, kRed+2);
  TGraphAttribute(gAging_cal_fitted, "gAging_cal_fitted", "Run Num", "Q_{X-tar}/Q_{LG} cal", 20, kBlue+2);
  TGraphAttribute(gAging_cal_ECorr_fitted, "gAging_cal_ECorr_fitted", "Run Num", "Q_{X-tar}/Q_{LG} cal ECorr", 20, kBlue+2);

  TGraphAttribute(gAging_raw_period_POT, "gAging_raw_POT Period", "Integrated N_{POT}", "Q_{X-tar}/Q_{LG} raw", 20, kRed+2);
  TGraphAttribute(gAging_raw_ECorr_period_POT, "gAging_raw_ECorr_POT Period", "Integrated N_{POT}", "Q_{X-tar}/Q_{LG} raw ECorr", 20, kRed+2);
  TGraphAttribute(gAging_cal_period_POT, "gAging_cal_POT Period", "Integrated N_{POT}", "Q_{X-tar}/Q_{LG} cal", 20, kBlue+2);
  TGraphAttribute(gAging_cal_ECorr_period_POT, "gAging_cal_ECorr_POT Period", "Integrated N_{POT}", "Q_{X-tar}/Q_{LG} cal ECorr", 20, kBlue+2);

  TGraphAttribute(gAging_raw_POT, "gAging_raw_POT", "Integrated N_{POT}", "Q_{X-tar}/Q_{LG} raw", 20, kRed+2);
  TGraphAttribute(gAging_raw_ECorr_POT, "gAging_raw_ECorr_POT", "Integrated N_{POT}", "Q_{X-tar}/Q_{LG} raw ECorr", 20, kRed+2);
  TGraphAttribute(gAging_cal_POT, "gAging_cal_POT", "Integrated N_{POT}", "Q_{X-tar}/Q_{LG} cal", 20, kBlue+2);
  TGraphAttribute(gAging_cal_ECorr_POT, "gAging_cal_ECorr_POT", "Integrated N_{POT}", "Q_{X-tar}/Q_{LG} cal ECorr", 20, kBlue+2);

  TGraphAttribute(gAging_raw_fitted_POT, "gAging_raw_fitted_POT", "Integrated N_{POT}", "Q_{X-tar}/Q_{LG} raw", 20, kRed+2);
  TGraphAttribute(gAging_raw_ECorr_fitted_POT, "gAging_raw_ECorr_fitted_POT", "Integrated N_{POT}", "Q_{X-tar}/Q_{LG} raw ECorr", 20, kRed+2);
  TGraphAttribute(gAging_cal_fitted_POT, "gAging_cal_fitted_POT", "Integrated N_{POT}", "Q_{X-tar}/Q_{LG} cal", 20, kBlue+2);
  TGraphAttribute(gAging_cal_ECorr_fitted_POT, "gAging_cal_ECorr_fitted_POT", "Integrated N_{POT}", "Q_{X-tar}/Q_{LG} cal ECorr", 20, kBlue+2);

  fout->cd();
  gAging_raw_period->Write();
  gAging_raw_ECorr_period->Write();
  gAging_cal_period->Write();
  gAging_cal_ECorr_period->Write();

  gAging_raw->Write();
  gAging_raw_ECorr->Write();
  gAging_cal->Write();
  gAging_cal_ECorr->Write();

  gAging_raw_fitted->Write();
  gAging_raw_ECorr_fitted->Write();
  gAging_cal_fitted->Write();
  gAging_cal_ECorr_fitted->Write();

  gAging_raw_period_POT->Write();
  gAging_raw_ECorr_period_POT->Write();
  gAging_cal_period_POT->Write();
  gAging_cal_ECorr_period_POT->Write();

  gAging_raw_POT->Write();
  gAging_raw_ECorr_POT->Write();
  gAging_cal_POT->Write();
  gAging_cal_ECorr_POT->Write();

  gAging_raw_fitted_POT->Write();
  gAging_raw_ECorr_fitted_POT->Write();
  gAging_cal_fitted_POT->Write();
  gAging_cal_ECorr_fitted_POT->Write();

  fout->Close();

  cout << "[OK] Wrote gAging TGraphErrors to AgingResult_NewCharge3.root" << endl;
  // " << gAging_raw->GetN() << " points to AgingResult.root\n";
  // cout << "[OK] Wrote gAging_raw_ECorr with " << gAging_raw_ECorr->GetN() << " points to AgingResult.root\n";
  // cout << "[OK] Wrote gAging_cal with " << gAging_cal->GetN() << " points to AgingResult.root\n";
  // cout << "[OK] Wrote gAging_cal_ECorr with " << gAging_cal_ECorr->GetN() << " points to AgingResult.root\n";
}
