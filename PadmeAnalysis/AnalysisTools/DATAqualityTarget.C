#include <iostream>
#include <cmath>

#include "Rtypes.h"

#include "PadmeUtils.h"
#include "PadmeStyle.h"
#include "PadmeLabels.h"

#ifdef __CLING__
// these are not headers - do not treat them as such - needed for ROOT6
#include "PadmeLabels.C"
#include "PadmeUtils.C"
#endif

#include "TCanvas.h"
#include "TFile.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TRandom.h"
#include "TGraphErrors.h"
#include <fstream>
#include "TString.h"

#include "PadmeStyle.h"

//void DATAqualityTarget(char *filename)
void DATAqualityTarget()
{


  #ifdef __CINT__
  gROOT->LoadMacro("PadmeLabels.C");
  gROOT->LoadMacro("PadmeUtils.C");
  gROOT->LoadMacro("PadmeStyle.C");
  
  #endif
 
   SetPadmeStyle();
   gStyle->SetOptTitle(1);
   gStyle->SetOptFit(1111);
   gStyle->SetOptStat(1110);
   gStyle->SetMarkerStyle(2);
   gStyle->SetMarkerSize(1.);
     
   //TFile *f1 = TFile::Open(filename);
   TFile *f1 = TFile::Open("DATAqualityTarget.root");
   
    
   TCanvas *c0 = new TCanvas("c1", "c1", 600,800);
   

   TPaveLabel *title = new TPaveLabel(0.1,0.7,0.9,0.98,"Targer DATA quality ");
   title->SetFillColor(8);
   title->SetTextFont(42);
   title->Draw();    

   //myTextInit( 0.1,  0.5, 1, "PADME Preliminary ");
   myTextInit( 0.1,  0.4, 1, "TargetBeam Charge X and Y   ");
   myTextInit( 0.1,  0.35, 1, "TargetBeam Centroid X and Y ");
   myTextInit( 0.1,  0.3, 1, "TargetBeam Xc and Yc Fit    ");
   myTextInit( 0.1,  0.25, 1, "X and Y Profiles and  Npot  ");
   myTextInit( 0.1,  0.2, 1, "Trends histos  ");

   c0->Print("DATAqualityTarget.pdf(");

   TF1 *gaus0 = new TF1("gaus","gaus",-0.5, 0.5);
   gaus0->SetLineColor(2);

   TF1 *gaus = new TF1("gaus","gaus", 0, 30000);
   gaus->SetLineColor(2);

   float  meanX0,  meanY0,  meanXY0;
   float sigmaX0, sigmaY0, sigmaXY0;

   TCanvas *c1 = new TCanvas("c1", "c1", 600,800);
   c1->Divide(1,2);
   c1->cd(1);
   TH1D *TargetBeam_XCharge = (TH1D*)f1->Get("TargetBeam_XCharge");
   TargetBeam_XCharge->SetXTitle("Q [pC]");
   TargetBeam_XCharge->SetYTitle("Counts");
   TargetBeam_XCharge->Draw();
   

   c1->cd(2);
   TH1D *TargetBeam_YCharge = (TH1D*)f1->Get("TargetBeam_YCharge");
   TargetBeam_YCharge->SetXTitle("Q [pC]");
   TargetBeam_YCharge->SetYTitle("Counts");
   TargetBeam_YCharge->Draw();
   c1->Print("DATAqualityTarget.pdf(", Form("TargetBeam_Charge"));

   c1->cd(1);
   TH1D *TargetBeam_XCharge_BTFtrigger = (TH1D*)f1->Get("TargetBeam_XCharge_BTFtrigger");
   TargetBeam_XCharge_BTFtrigger->Draw();
   TargetBeam_XCharge_BTFtrigger->SetXTitle("Q [pC]");
   TargetBeam_XCharge_BTFtrigger->SetYTitle("Counts");
   /*TF1 *gausX = new TF1("gaus","gaus",-0.2, 0.2);
   TargetBeam_XCharge_BTFtrigger->Fit(gaus, "","",  -0.2, 0.2);
   float meanX0 =gausX->GetParameter(1);
   float sigmaX0=gausX->GetParameter(2);
   std::cout<<"\n \n The mean value of the fit 0 for Charge X is   "<<meanX0<<std::endl;
   std::cout<<"\n \n The sigma value of the fit 0 for Charge X is  "<<sigmaX0<<std::endl;*/
   
   c1->cd(2);
   TH1D *TargetBeam_YCharge_BTFtrigger = (TH1D*)f1->Get("TargetBeam_YCharge_BTFtrigger");
   TargetBeam_YCharge_BTFtrigger->SetXTitle("Q [pC]");
   TargetBeam_YCharge_BTFtrigger->SetYTitle("Counts");
   TargetBeam_YCharge_BTFtrigger->Draw();
   /*TF1 *gausY = new TF1("gaus","gaus",-0.2, 0.2);
   TargetBeam_YCharge_BTFtrigger->Fit(gaus, "","",  -0.2, 0.2);
   float meanY0 =gausY->GetParameter(1);
   float sigmaY0=gausY->GetParameter(2); 
   std::cout<<"\n \n The mean value of the fit 0 for Charge Y is   "<<meanY0<<std::endl;
   std::cout<<"\n \n The sigma value of the fit 0 for Charge Y is  "<<sigmaY0<<std::endl;*/
   c1->Print("DATAqualityTarget.pdf(", Form("TargetBeam_Charge_BTFtrigger"));

   c1->cd(1);
   TH1D *TargetBeam_XCharge_CRtrigger = (TH1D*)f1->Get("TargetBeam_XCharge_CRtrigger");
   TargetBeam_XCharge_CRtrigger->SetXTitle("Q [pC]");
   TargetBeam_XCharge_CRtrigger->SetYTitle("Counts");
   TargetBeam_XCharge_CRtrigger->Draw();
   TargetBeam_XCharge_CRtrigger->Fit(gaus0, "","",  -0.5, 0.5);
   c1->cd(2);
   TH1D *TargetBeam_YCharge_CRtrigger = (TH1D*)f1->Get("TargetBeam_YCharge_CRtrigger");
   TargetBeam_YCharge_CRtrigger->SetXTitle("Q [pC]");
   TargetBeam_YCharge_CRtrigger->SetYTitle("Counts");
   TargetBeam_YCharge_CRtrigger->Draw();
   TargetBeam_YCharge_CRtrigger->Fit(gaus0, "","",  -0.5, 0.5);
   c1->Print("DATAqualityTarget.pdf(", Form("TargetBeam_Charge_CRtrigger"));

   c1->cd(1);
   TH1D *TargetBeam_XCharge_AUTOtrigger = (TH1D*)f1->Get("TargetBeam_XCharge_AUTOtrigger");
   TargetBeam_XCharge_AUTOtrigger->SetXTitle("Q [pC]");
   TargetBeam_XCharge_AUTOtrigger->SetYTitle("Counts");
   TargetBeam_XCharge_AUTOtrigger->Draw();
   TargetBeam_XCharge_AUTOtrigger->Fit(gaus0, "","",  -0.5, 0.5);
   TargetBeam_XCharge_AUTOtrigger->Draw();
   meanX0  = gaus0->GetParameter(1);
   sigmaX0 = gaus0->GetParameter(2);
   c1->cd(2);
   TH1D *TargetBeam_YCharge_AUTOtrigger = (TH1D*)f1->Get("TargetBeam_YCharge_AUTOtrigger");
   TargetBeam_YCharge_AUTOtrigger->SetXTitle("Q [pC]");
   TargetBeam_YCharge_AUTOtrigger->SetYTitle("Counts");
   TargetBeam_YCharge_AUTOtrigger->Draw();
   TargetBeam_YCharge_AUTOtrigger->Fit(gaus0, "","",  -0.2, 0.2);
   meanY0= gaus0->GetParameter(1);
   sigmaY0=gaus0->GetParameter(2);
   std::cout<<"\n \n The mean value of the fit 0 for Charge Y is   "<<meanY0<<std::endl;
   c1->Print("DATAqualityTarget.pdf(", Form("TargetBeam_Charge_AUTOtrigger"));

   c1->cd(1);
   TH1D *TargetBeam_MeanXYCharge = (TH1D*)f1->Get("TargetBeam_MeanXYCharge");
   TargetBeam_MeanXYCharge->SetXTitle("Q [pC]");
   TargetBeam_MeanXYCharge->SetYTitle("Counts");
   TargetBeam_MeanXYCharge->Draw();
   TargetBeam_MeanXYCharge->Fit(gaus0, "","",  0.5, 2);
   c1->cd(2);
   TH1D *TargetBeam_MeanXYCharge_BTFtrigger = (TH1D*)f1->Get("TargetBeam_MeanXYCharge_BTFtrigger");
   TargetBeam_MeanXYCharge_BTFtrigger->SetXTitle("Q [pC]");
   TargetBeam_MeanXYCharge_BTFtrigger->SetYTitle("Counts");
   TargetBeam_MeanXYCharge_BTFtrigger->Draw();
   c1->Print("DATAqualityTarget.pdf(", Form("TargetBeam_XYCharge_BTFtrigger"));

   c1->cd(1);
   TH1D *TargetBeam_nPOT= (TH1D*)f1->Get("TargetBeam_MeanXYCharge_AUTOtrigger");
   TargetBeam_MeanXYCharge_AUTOtrigger->SetXTitle("Q [pC]");
   TargetBeam_MeanXYCharge_AUTOtrigger->SetYTitle("Counts");
   TargetBeam_MeanXYCharge_AUTOtrigger->Draw();
   TargetBeam_MeanXYCharge_AUTOtrigger->Fit(gaus0, "","",  -0.5, 0.5);
   c1->cd(2);
   TH1D *TargetBeam_MeanXYCharge_CRtrigger = (TH1D*)f1->Get("TargetBeam_MeanXYCharge_CRtrigger");
   TargetBeam_MeanXYCharge_CRtrigger->SetXTitle("Q [pC]");
   TargetBeam_MeanXYCharge_CRtrigger->SetYTitle("Counts");
   TargetBeam_MeanXYCharge_CRtrigger->Draw();
   c1->Print("DATAqualityTarget.pdf(", Form("TargetBeam_XYCharge_CRtrigger"));

   c1->cd(1);
   TH1D *Target_HitXprofile = (TH1D*)f1->Get("Target_HitXprofile");
   Target_HitXprofile->SetXTitle("X strip [mm]");
   Target_HitXprofile->SetYTitle("Q [pC]");
   Target_HitXprofile->Draw();
   c1->cd(2);
   TH1D *Target_HitYprofile = (TH1D*)f1->Get("Target_HitYprofile");
   Target_HitYprofile->SetXTitle("Y strip [mm]");
   Target_HitYprofile->SetYTitle("Q [pC]");
   Target_HitYprofile->Draw();
   c1->Print("DATAqualityTarget.pdf(", Form("TargetBeam_Profiles"));

   c1->cd(1);
   TH1D *TargetBeam_XCentroid = (TH1D*)f1->Get("TargetBeam_XCentroid");
   TargetBeam_XCentroid->SetXTitle("X [mm]");
   TargetBeam_XCentroid->SetYTitle("Counts");
   TargetBeam_XCentroid->Draw();
   c1->cd(2);
   TH1D *TargetBeam_YCentroid = (TH1D*)f1->Get("TargetBeam_YCentroid");
   TargetBeam_YCentroid->SetXTitle("Y [mm]");
   TargetBeam_YCentroid->SetYTitle("Counts");
   TargetBeam_YCentroid->Draw();
   c1->Print("DATAqualityTarget.pdf(", Form("TargetBeam_Centroid"));

   c1->cd(1);
   TH1D *TargetBeam_XCentroidError = (TH1D*)f1->Get("TargetBeam_XCentroidError");
   TargetBeam_XCentroidError->SetXTitle("X [mm]");
   TargetBeam_XCentroidError->SetYTitle("Counts");
   TargetBeam_XCentroidError->Draw();
   c1->cd(2);
   TH1D *TargetBeam_YCentroidError = (TH1D*)f1->Get("TargetBeam_YCentroidError");
   TargetBeam_YCentroidError->SetXTitle("Y [mm]");
   TargetBeam_YCentroidError->SetYTitle("Counts");
   TargetBeam_YCentroidError->Draw();
   c1->Print("DATAqualityTarget.pdf(", Form("TargetBeam_CentroidError"));

   c1->cd(1);
   TH1D *TargetBeam_XCfit = (TH1D*)f1->Get("TargetBeam_XCfit");
   TargetBeam_XCfit->SetXTitle("X [mm]");
   TargetBeam_XCfit->SetYTitle("Counts");
   TargetBeam_XCfit->Draw();
   c1->cd(2);
   TH1D *TargetBeam_YCfit = (TH1D*)f1->Get("TargetBeam_YCfit");
   TargetBeam_YCfit->SetXTitle("Y [mm]");
   TargetBeam_YCfit->SetYTitle("Counts");
   TargetBeam_YCfit->Draw();
   c1->Print("DATAqualityTarget.pdf(", Form("TargetBeam_XYfit"));

   c1->cd(1);
   TH1D *TargetBeam_XCfitError = (TH1D*)f1->Get("TargetBeam_XCfitError");
   TargetBeam_XCfitError->Draw();
   TargetBeam_XCfitError->SetXTitle("X [mm]");
   TargetBeam_XCfitError->SetYTitle("Counts");
   c1->cd(2);
   TH1D *TargetBeam_YCfitError = (TH1D*)f1->Get("TargetBeam_YCfitError");
   TargetBeam_YCfitError->SetXTitle("Y [mm]");
   TargetBeam_YCfitError->SetYTitle("Counts");
   TargetBeam_YCfitError->Draw();
   c1->Print("DATAqualityTarget.pdf(", Form("TargetBeam_XYfitError"));

   c1->cd(1);
   TH1D *TargetBeam_nPOT = (TH1D*)f1->Get("TargetBeam_nPOT");
   TargetBeam_nPOT->SetXTitle("Multiplicity [e^{+} ]");
   TargetBeam_nPOT->SetYTitle("Counts");
   TargetBeam_nPOT->Draw();
   TargetBeam_nPOT->Fit(gaus, "","",   3000, 30000);
   c1->cd(2);
   TH1D *TargetBeam_nPOTError = (TH1D*)f1->Get("TargetBeam_nPOTError");
   TargetBeam_nPOTError->SetXTitle("Multiplicity [e^{+} ]");
   TargetBeam_nPOTError->SetYTitle("Counts");
   TargetBeam_nPOTError->Draw();
   c1->Print("DATAqualityTarget.pdf(", Form("TargetBeam_nPOT"));

   c1->cd(1);
   TH1D *TargetBeam_nPOT = (TH1D*)f1->Get("TargetBeam_nPOT_BTFtrigger");
   TargetBeam_nPOT_BTFtrigger->SetXTitle("Multiplicity [e^{+} ]");
   TargetBeam_nPOT_BTFtrigger->SetYTitle("Counts");
   TargetBeam_nPOT_BTFtrigger->Draw();
   TargetBeam_nPOT->Fit(gaus, "","",   3000, 30000);
   c1->cd(2);
   TH1D *TargetBeam_nPOT_AUTOtrigger = (TH1D*)f1->Get("TargetBeam_nPOT_AUTOtrigger");
   TargetBeam_nPOT_AUTOtrigger->SetXTitle("Multiplicity [e^{+} ]");
   TargetBeam_nPOT_AUTOtrigger->SetYTitle("Counts");
   TargetBeam_nPOT_AUTOtrigger->Draw();
   c1->Print("DATAqualityTarget.pdf(", Form("TargetBeam_POT_BTFtrigger"));

   c1->cd(1);
   TH2D *Trend_ChargeXvsEventNumber = (TH2D*)f1->Get("Trend_ChargeXvsEventNumber");
   Trend_ChargeXvsEventNumber->SetXTitle("Event Number");
   Trend_ChargeXvsEventNumber->SetYTitle("Q [pC]");
   Trend_ChargeXvsEventNumber->Draw();
   c1->cd(2);
   TH2D *Trend_ChargeYvsEventNumber = (TH2D*)f1->Get("Trend_ChargeYvsEventNumber");
   Trend_ChargeYvsEventNumber->SetXTitle("Event Number");
   Trend_ChargeYvsEventNumber->SetYTitle("Q [pC]");
   Trend_ChargeYvsEventNumber->Draw();
   c1->Print("DATAqualityTarget.pdf(", Form("Trend_ChargevsEventNumber"));

   c1->cd(1);
   TH2D *Trend_CentroidXvsEventNumber = (TH2D*)f1->Get("Trend_CentroidXvsEventNumber");
   Trend_CentroidXvsEventNumber->SetXTitle("Event Number");
   Trend_CentroidXvsEventNumber->SetYTitle("X [mm]");
   Trend_CentroidXvsEventNumber->Draw();
   c1->cd(2);
   TH2D *Trend_CentroidYvsEventNumber = (TH2D*)f1->Get("Trend_CentroidYvsEventNumber");
   Trend_CentroidYvsEventNumber->SetXTitle("Event Number");
   Trend_CentroidYvsEventNumber->SetYTitle("Y [mm]");
   Trend_CentroidYvsEventNumber->Draw();
   c1->Print("DATAqualityTarget.pdf(", Form("Trend_CentroidvsEventNumber"));

   c1->cd(1);
   TH2D *Trend_XCfitvsEventNumber = (TH2D*)f1->Get("Trend_XCfitvsEventNumber");
   Trend_XCfitvsEventNumber->SetXTitle("Event Number");
   Trend_XCfitvsEventNumber->SetYTitle("X [mm]");
   Trend_XCfitvsEventNumber->Draw();
   c1->cd(2);
   TH2D *Trend_YCfitvsEventNumber = (TH2D*)f1->Get("Trend_YCfitvsEventNumber");
   Trend_YCfitvsEventNumber->SetXTitle("Event Number");
   Trend_YCfitvsEventNumber->SetYTitle("Y [mm]");
   Trend_YCfitvsEventNumber->Draw();
   c1->Print("DATAqualityTarget.pdf)", Form("Trend_CentroidvsEventNumber"));
   

   //return;
   
   exit(0);
}

/*
void PadmeText()
{        
         //myText(       0.22,  0.78, 1, "POT= 10^{11}");
         myText(       0.22,  0.28, 1, "Normalized");
         myText(       0.22, 0.23, 1, "E_{e^{+}} = 550 MeV");
         //myMarkerText( 0.55, 0.75, 1, 22, "RUN I, 2018",0.7);       
}
#ifndef __CINT__
int main() { 
  BasicExample_Padme();
  gPad->Print("basic.png");
  return 0;
}
#endif*/
