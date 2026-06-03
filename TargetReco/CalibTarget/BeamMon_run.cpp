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

void BeamMon_run(){
  vector<pair<TString,TString>> graphInfo = {
                      {"gQLGTot",               "Q_{LG} Total [pC]"},
                      {"gQLGTot_ECorr",         "Q_{LG} Total ECorr [pC]"},
                      {"gQXTot_raw",            "Q_{X-Tar} Raw Total [pC]"},
                      {"gQYTot_raw",            "Q_{Y-Tar} Raw Total [pC]"},
                      {"gQXTot_cal",            "Q_{X-Tar} Cal Total [pC]"},
                      {"gQXTot_fitted_raw",     "Q_{X-Tar} Fitted Raw Total [pC]"},
                      {"gQYTot_fitted_raw",     "Q_{Y-Tar} Fitted Raw Total [pC]"},
                      {"gQXTot_fitted_cal",     "Q_{X-Tar} Fitted Cal Total [pC]"},

                      {"gmeanXTot_raw",         "<X_{wmean}> Raw [mm]"},
                      {"gmeanYTot_raw",         "<Y_{wmean}> Raw [mm]"},
                      {"gmeanXTot_cal",         "<X_{wmean}> Cal [mm]"},
                      {"gmeanXTot_fitted_raw",  "<X_{fit}> Fitted Raw [mm]"},
                      {"gmeanYTot_fitted_raw",  "<Y_{fit}> Fitted Raw [mm]"},
                      {"gmeanXTot_fitted_cal",  "<X_{fit}> Fitted Cal [mm]"},

                      {"gsigmaXTot_fitted_raw", "<#sigma_{X}> Fitted Raw [mm]"},
                      {"gsigmaYTot_fitted_raw", "<#sigma_{Y}> Fitted Raw [mm]"},
                      {"gsigmaXTot_fitted_cal", "<#sigma_{X}> Fitted Cal [mm]"},

                      {"gRX_raw",               "Q_{X-Tar}/Q_{LG} Raw"},
                      {"gRY_raw",               "Q_{Y-Tar}/Q_{LG} Raw"},
                      {"gRX_cal",               "Q_{X-Tar}/Q_{LG} Cal"},
                      {"gXR_fitted_raw",        "Q_{X-Tar}/Q_{LG} Fitted Raw"},
                      {"gYR_fitted_raw",        "Q_{Y-Tar}/Q_{LG} Fitted Raw"},
                      {"gXR_fitted_cal",        "Q_{X-Tar}/Q_{LG} Fitted Cal"},

                      {"gRX_raw_ECorr",         "Q_{X-Tar}/Q_{LG} Raw ECorr"},
                      {"gRY_raw_ECorr",         "Q_{Y-Tar}/Q_{LG} Raw ECorr"},
                      {"gRX_cal_ECorr",         "Q_{X-Tar}/Q_{LG} Cal ECorr"},
                      {"gXR_fitted_raw_ECorr",  "Q_{X-Tar}/Q_{LG} Fitted Raw ECorr"},
                      {"gYR_fitted_raw_ECorr",  "Q_{Y-Tar}/Q_{LG} Fitted Raw ECorr"},
                      {"gXR_fitted_cal_ECorr",  "Q_{X-Tar}/Q_{LG} Fitted Cal ECorr"}
  };
  const int NRun = 10;
  const int runNumbers[NRun] = {80344, 80357, 80415, 80427, 80472, 80534, 80615, 80649, 80651, 80677};
  const int NG = graphInfo.size();
  double m_e = 0.511; // MeV - electron mass

  // summary graphs
  vector<TGraphErrors*> gVsEbeam;
  vector<TGraphErrors*> gVsSqrtS;
  vector<TGraphErrors*> gVsRun;

  gVsEbeam.reserve(NG);
  gVsSqrtS.reserve(NG);
  gVsRun.reserve(NG);

  for (int ig=0; ig<NG; ++ig) {

    TGraphErrors* gE = new TGraphErrors(NRun);
    gE->SetName(graphInfo[ig].first + "_vsEBeam");

    TGraphErrors* gS = new TGraphErrors(NRun);
    gS->SetName(graphInfo[ig].first + "_vsSqrtS");

    TGraphErrors* gR = new TGraphErrors(NRun);
    gR->SetName(graphInfo[ig].first + "_vsRun");

    gVsEbeam.push_back(gE);
    gVsSqrtS.push_back(gS);
    gVsRun.push_back(gR);
  }

  // loop over runs
  for (int irun=0; irun<NRun; ++irun) {

    int runid = runNumbers[irun];

    TString inFileName = Form("/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/CalibTarget/outputCalibration/Run4Monitor/TargetAnalysis_run_00%i_NewReco_100files.root", runid);
    TFile* fin = TFile::Open(inFileName,"READ");

    if (!fin || fin->IsZombie()) {
      cerr<<"[WARN] Cannot open "<<inFileName<<endl;
      if(fin){fin->Close(); delete fin;}
      continue;
    }

    // loop over graphs
    for (int ig=0; ig<NG; ++ig) {
      TGraphErrors* gIn = (TGraphErrors*) fin->Get("summary/" + graphInfo[ig].first);
      if (!gIn) {
        cerr<<"[WARN] Missing "<<graphInfo[ig].first<<" in "<<inFileName<<endl;
        continue;
      }

      // take first point (assuming one point per graph)
      double x,y;
      gIn->GetPoint(0,x,y);

      double ex = gIn->GetErrorX(0);
      double ey = gIn->GetErrorY(0);

      // summary vs Ebeam
      gVsEbeam[ig]->SetPoint(irun,x,y);
      gVsEbeam[ig]->SetPointError(irun,ex,ey);

      // summary vs SqrtS
      gVsSqrtS[ig]->SetPoint(irun,sqrt(2*x*m_e),y);
      gVsSqrtS[ig]->SetPointError(irun,0,ey);

      // summary vs Run
      gVsRun[ig]->SetPoint(irun,runid,y);
      gVsRun[ig]->SetPointError(irun,0,ey);
    }

    fin->Close();
    delete fin;
  }

  // output file
  TFile* fout = new TFile("/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/CalibTarget/outputCalibration/Run4Monitor/BeamMonitorEntireRun_NewReco_100files.root","RECREATE");

  fout->cd();
  TDirectory *EBeamDir = fout->mkdir("EBeam");
  TDirectory *SqrtSDir = fout->mkdir("SqrtS");
  TDirectory *NRunDir = fout->mkdir("NRun");

  for (int ig=0; ig<NG; ++ig) {

    TGraphAttribute(gVsEbeam[ig], graphInfo[ig].first+" vs EBeam", "E_{beam} [MeV]", graphInfo[ig].second, 20, kRed+2);
    TGraphAttribute(gVsSqrtS[ig], graphInfo[ig].first+" vs #sqrt{s}", "#sqrt{s} [MeV]", graphInfo[ig].second, 20, kRed+2);
    TGraphAttribute(gVsRun[ig], graphInfo[ig].first+" vs Run", "Run number", graphInfo[ig].second, 20, kBlue+2);

    fout->cd();
    EBeamDir->cd();
    gVsEbeam[ig]->Write();
    fout->cd();
    SqrtSDir->cd();
    gVsSqrtS[ig]->Write();
    fout->cd();
    NRunDir->cd();
    gVsRun[ig]->Write();
  }

  fout->Close();

  cout<<"[OK] Wrote "<<NG*3<<" summary graphs"<<endl;
}