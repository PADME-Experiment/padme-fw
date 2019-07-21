#include <iostream>
#include <cmath>

#include "Rtypes.h"
#include "TLatex.h"

/*
#include "PadmeUtils.h"
#include "PadmeStyle.h"
#include "PadmeLabels.h"

#ifdef __CLING__
// these are not headers - do not treat them as such - needed for ROOT6
#include "PadmeLabels.C"
#include "PadmeUtils.C"
#endif
*/

#include "TCanvas.h"
#include "TFile.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TRandom.h"
#include "TGraphErrors.h"
#include <fstream>
#include "TString.h"


//#include "PadmeStyle.h"

//void DATAqualityTarget(char *filename)
void EventSelectionPlots()
{

  /*

  #ifdef __CINT__
  gROOT->LoadMacro("PadmeLabels.C");
  gROOT->LoadMacro("PadmeUtils.C");
  gROOT->LoadMacro("PadmeStyle.C");
  
  #endif
  */

 
   /*
   SetPadmeStyle();
   */
   gStyle->SetOptTitle(0);
   gStyle->SetOptFit(1111);
   gStyle->SetOptStat(1110);
   gStyle->SetMarkerStyle(2);
   gStyle->SetMarkerSize(1.);

   gStyle->SetOptStat(0);

     
   //TFile *f1 = TFile::Open(filename);
   std::string fName = "run_0000000_20190228_131527_recoFromRaw.AnalysisOut1.root";
   std::string fNameToOpen = ""+fName;
   TFile *f1 = TFile::Open(fNameToOpen.c_str());
   

   TCanvas *c1 = new TCanvas("Beam_Target", "Beam_Target", 600,700);
   c1->SetFixedAspectRatio(true);

   //title->Draw();

   TText *t = new TText(0.5,0.5,"Primary beam; nPOT=10^10");
   t->SetTextAlign(22);
   //t->SetTextColor(kRed+2);
   t->SetTextFont(43);
   t->SetTextSize(13);
   t->SetTextAngle(0);


   //return;
   //gStyle->SetPadTopMargin(300);
   c1->Divide(2,3,0,0);
   c1->cd(1);
   gPad->SetTopMargin(30);
   gPad->SetBottomMargin(10);
   gPad->SetRightMargin(10);
   TH1D *Target_HitXprofile = (TH1D*)f1->Get("Target_HitXprofile");
   Target_HitXprofile->SetXTitle("Target X profile");
   Target_HitXprofile->SetYTitle("Charge [pC]");
   Target_HitXprofile->Draw();

   c1->cd(2);
   gPad->SetTopMargin(30);
   gPad->SetLeftMargin(30);
   gPad->SetBottomMargin(10);
   TH1D *Target_HitYprofile = (TH1D*)f1->Get("Target_HitYprofile");
   Target_HitYprofile->SetXTitle("Target Y profile");
   Target_HitYprofile->SetYTitle("Charge [pC]");
   Target_HitYprofile->Draw();
   t->Draw();

   c1->cd(3);
   gPad->SetRightMargin(10);
   gPad->SetBottomMargin(10);
   TH1D *TargetBeam_XCfit = (TH1D*)f1->Get("TargetBeam_XCfit");
   TargetBeam_XCfit->SetXTitle("Target X centroid (fit) [mm]");
   TargetBeam_XCfit->SetYTitle("Counts");
   TargetBeam_XCfit->Draw();
   c1->cd(4);
   gPad->SetBottomMargin(10);
   gPad->SetLeftMargin(10);
   TH1D *TargetBeam_YCfit = (TH1D*)f1->Get("TargetBeam_YCfit");
   TargetBeam_YCfit->SetXTitle("Target Y centroid (fit) [mm]");
   TargetBeam_YCfit->SetYTitle("Counts");
   TargetBeam_YCfit->Draw();
   c1->cd(5);
   gPad->SetBottomMargin(10);
   gPad->SetRightMargin(10);
   TH1D *TargetBeam_nPOT = (TH1D*)f1->Get("TargetBeam_nPOT");
   TargetBeam_nPOT->SetXTitle("Target nPOT");
   TargetBeam_nPOT->SetYTitle("Counts");
   TargetBeam_nPOT->Draw();
   c1->cd(6);
   gPad->SetBottomMargin(10);
   gPad->SetLeftMargin(10);
   TH1D *TargetBeam_XCharge = (TH1D*)f1->Get("TargetBeam_XCharge");
   TargetBeam_XCharge->SetXTitle("Target X Charge [pC]");
   TargetBeam_XCharge->SetYTitle("Counts");
   TargetBeam_XCharge->Draw();
   
   //t->Draw();
   
   c1->Print("DATAqualityTarget.pdf(", Form("TargetSummaryInfo"));


   TCanvas *c2 = new TCanvas("c2", "c2", 600,600);
   
   TH1D *ECal_TwoPhotonEnergy_TimeCoincidenceRequest3ns = (TH1D*)f1->Get("ECal_TwoPhotonEnergy_TimeCoincidenceRequest3ns");
   ECal_TwoPhotonEnergy_TimeCoincidenceRequest3ns->SetXTitle("E1+E2 [MeV]");
   //ECal_TwoPhotonEnergy_TimeCoincidenceRequest3ns->SetYTitle("Counts");
   ECal_TwoPhotonEnergy_TimeCoincidenceRequest3ns->Draw();
   TText *t1 = new TText(700.,5000.,"Two ECal clusters [Delta] t<3ns");
   t1->SetTextAlign(20);
   //t->SetTextColor(kRed+2);
   t1->SetTextFont(43);
   t1->SetTextSize(20);
   t1->SetTextAngle(0);
   t1->Draw();


   TCanvas *c3 = new TCanvas("Invariant Mass", "Invariant Mass", 600,600);
   
   TH1D *ECal2gsearch_MinvDt3 = (TH1D*)f1->Get("ECal2gsearch_MinvDt3");
   ECal_TwoPhotonEnergy_TimeCoincidenceRequest3ns->SetXTitle("E1+E2 [MeV]");
   ECal2gsearch_MinvDt3->SetXTitle("Invariant mass [MeV]");
   gPad->SetLogy();
   ECal2gsearch_MinvDt3->SetLineColor(kBlack);
   ECal2gsearch_MinvDt3->Draw();
   TH1D *ECal2gsearch_MinvDt3Phi100 = (TH1D*)f1->Get("ECal2gsearch_MinvDt3Phi100"); 
   ECal2gsearch_MinvDt3Phi100->SetLineColor(kBlue);
   ECal2gsearch_MinvDt3Phi100->Draw("SAME");
   TH1D *ECal2gsearch_MinvDt3Phi75 = (TH1D*)f1->Get("ECal2gsearch_MinvDt3Phi75"); 
   ECal2gsearch_MinvDt3Phi75->SetLineColor(kRed);
   ECal2gsearch_MinvDt3Phi75->Draw("SAME");
   TH1D *ECal2gsearch_MinvDt3Phi50 = (TH1D*)f1->Get("ECal2gsearch_MinvDt3Phi50"); 
   ECal2gsearch_MinvDt3Phi50->SetLineColor(kMagenta);
   ECal2gsearch_MinvDt3Phi50->Draw("SAME");
   TH1D *ECal2gsearch_MinvDt3Phi25 = (TH1D*)f1->Get("ECal2gsearch_MinvDt3Phi25"); 
   ECal2gsearch_MinvDt3Phi25->SetLineColor(kGreen);
   ECal2gsearch_MinvDt3Phi25->Draw("SAME");
   TText *t2 = new TText(20.,10000.,"Two ECal clusters Dt<3ns");
   t2->SetTextAlign(12);
   //t->SetTextColor(kRed+2);
   t2->SetTextFont(43);
   t2->SetTextSize(20);
   t2->SetTextAngle(0);
   t2->Draw();
   t2->DrawText(20,15000, "Primary e+ beam; nPOT=10^10");
   TLegend* L2 = new TLegend(0.65,0.55,0.9,0.75);
   L2->SetFillStyle(0);
   L2->SetBorderSize(0);
   L2->SetTextSize(0.03);
   L2->SetTextAlign(12);
   L2->AddEntry(ECal2gsearch_MinvDt3,"no Phi cut", "L");
   L2->AddEntry(ECal2gsearch_MinvDt3Phi100,"DPhi<10", "L");
   L2->AddEntry(ECal2gsearch_MinvDt3Phi75, "DPhi<7.5", "L");
   L2->AddEntry(ECal2gsearch_MinvDt3Phi50, "DPhi<5", "L");
   L2->AddEntry(ECal2gsearch_MinvDt3Phi25, "DPhi<2.5", "L");
   L2->Draw();


   TCanvas *c32 = new TCanvas("2g_Energy_Sum", "2g Energy Sum", 600,600);

   TH1D *ECal2gsearch_ESumDt3 = (TH1D*)f1->Get("ECal2gsearch_ESumDt3");
   ECal2gsearch_ESumDt3->SetXTitle("Two cluster Energy Sum [MeV]");
   gPad->SetLogy();
   ECal2gsearch_ESumDt3->SetLineColor(kBlack);
   ECal2gsearch_ESumDt3->Draw();
   TH1D *ECal2gsearch_ESumDt3Phi100 = (TH1D*)f1->Get("ECal2gsearch_ESumDt3Phi100"); 
   ECal2gsearch_ESumDt3Phi100->SetLineColor(kBlue);
   ECal2gsearch_ESumDt3Phi100->Draw("SAME");
   TH1D *ECal2gsearch_ESumDt3Phi75 = (TH1D*)f1->Get("ECal2gsearch_ESumDt3Phi75"); 
   ECal2gsearch_ESumDt3Phi75->SetLineColor(kRed);
   ECal2gsearch_ESumDt3Phi75->Draw("SAME");
   TH1D *ECal2gsearch_ESumDt3Phi50 = (TH1D*)f1->Get("ECal2gsearch_ESumDt3Phi50"); 
   ECal2gsearch_ESumDt3Phi50->SetLineColor(kMagenta);
   ECal2gsearch_ESumDt3Phi50->Draw("SAME");
   TH1D *ECal2gsearch_ESumDt3Phi25 = (TH1D*)f1->Get("ECal2gsearch_ESumDt3Phi25"); 
   ECal2gsearch_ESumDt3Phi25->SetLineColor(kGreen);
   ECal2gsearch_ESumDt3Phi25->Draw("SAME");
   t2->DrawText(400,3000, "Two ECal clusters Dt<3ns");
   t2->DrawText(400,5000, "Primary e+ beam; nPOT=10^10");
   TLegend* L32 = new TLegend(0.65,0.55,0.9,0.75);
   L32->SetFillStyle(0);
   L32->SetBorderSize(0);
   L32->SetTextSize(0.03);
   L32->SetTextAlign(12);
   L32->AddEntry(ECal2gsearch_ESumDt3,"no Phi cut", "L");
   L32->AddEntry(ECal2gsearch_ESumDt3Phi100,"DPhi<10", "L");
   L32->AddEntry(ECal2gsearch_ESumDt3Phi75, "DPhi<7.5", "L");
   L32->AddEntry(ECal2gsearch_ESumDt3Phi50, "DPhi<5", "L");
   L32->AddEntry(ECal2gsearch_ESumDt3Phi25, "DPhi<2.5", "L");
   L32->Draw();


   TCanvas *c35 = new TCanvas("SAC_Ehit_vs_PVetoCh", "SAC_Ehit_vs_PVetoCh", 1000,400);
   TH1D *SACClEVsPVetoHitChId_1ns_linearCorr= (TH1D*)f1->Get("SACClEVsPVetoHitChId_1ns_linearCorr");
   TH1D *SACClE21VsPVetoHitChId_1ns_linearCorr= (TH1D*)f1->Get("SACClE21VsPVetoHitChId_1ns_linearCorr");
   c35->Divide(2,1);
   c35->cd(1);
   SACClEVsPVetoHitChId_1ns_linearCorr->Draw("ZCOL");
   SACClEVsPVetoHitChId_1ns_linearCorr->SetXTitle("PVeto Ch Id - after PVeto t-linCorr");
   SACClEVsPVetoHitChId_1ns_linearCorr->SetYTitle("All SAC hit E [MeV]");
   c35->cd(2);
   SACClE21VsPVetoHitChId_1ns_linearCorr->Draw("ZCOL");
   SACClE21VsPVetoHitChId_1ns_linearCorr->SetXTitle("PVeto Ch Id - after PVeto t-linCorr");
   SACClE21VsPVetoHitChId_1ns_linearCorr->SetYTitle("Ch21 SAC hit E [MeV]");
   


   TCanvas *c33 = new TCanvas("ECal dPhiVsdT", "ECal dPhiVsdT", 1000,400);

   c33->Divide(2,1);
   c33->cd(1);
   TH1D *ECal2gsearch_dPhiVsDt = (TH1D*)f1->Get("ECal2gsearch_dPhiVsDt");
   ECal2gsearch_dPhiVsDt->SetXTitle("Two cluster dT [ns]");
   ECal2gsearch_dPhiVsDt->SetYTitle("Two cluster dPhi [rad]");
   //gPad->SetLiny();
   ECal2gsearch_dPhiVsDt->Draw("ZCOL");
   c33->cd(2);
   TH1D *ECal2gsearch_dPhiVsDt = (TH1D*)f1->Get("ECal2gsearch_dRVsDt");
   ECal2gsearch_dRVsDt->SetXTitle("Two cluster dT [ns]");
   ECal2gsearch_dRVsDt->SetYTitle("Two cluster dR [mm]");
   //gPad->SetLiny();
   ECal2gsearch_dRVsDt->Draw("ZCOL");


   TCanvas *c34 = new TCanvas("ECal2gsearch_MinvVsRcog", "ECal2gsearch_MinvVsRcog", 1000,600);
   TH1D* ECal2gsearch_MinvVsRcogDt3       = (TH1D*)f1->Get("ECal2gsearch_MinvVsRcogDt3");
   TH1D* ECal2gsearch_MinvVsRcogDt3Phi100 = (TH1D*)f1->Get("ECal2gsearch_MinvVsRcogDt3Phi100");
   TH1D* ECal2gsearch_MinvVsRcogDt3Phi75  = (TH1D*)f1->Get("ECal2gsearch_MinvVsRcogDt3Phi75" );
   TH1D* ECal2gsearch_MinvVsRcogDt3Phi50  = (TH1D*)f1->Get("ECal2gsearch_MinvVsRcogDt3Phi50" );
   TH1D* ECal2gsearch_MinvVsRcogDt3Phi25  = (TH1D*)f1->Get("ECal2gsearch_MinvVsRcogDt3Phi25" );
   TH1D* ECal2gsearch_MinvVsRcogDt2       = (TH1D*)f1->Get("ECal2gsearch_MinvVsRcogDt2");
   TH1D* ECal2gsearch_MinvVsRcogDt2Phi100 = (TH1D*)f1->Get("ECal2gsearch_MinvVsRcogDt2Phi100");
   TH1D* ECal2gsearch_MinvVsRcogDt2Phi75  = (TH1D*)f1->Get("ECal2gsearch_MinvVsRcogDt2Phi75" );
   TH1D* ECal2gsearch_MinvVsRcogDt2Phi50  = (TH1D*)f1->Get("ECal2gsearch_MinvVsRcogDt2Phi50" );
   TH1D* ECal2gsearch_MinvVsRcogDt2Phi25  = (TH1D*)f1->Get("ECal2gsearch_MinvVsRcogDt2Phi25" );
   TH1D* ECal2gsearch_MinvVsRcogDt1       = (TH1D*)f1->Get("ECal2gsearch_MinvVsRcogDt2");
   TH1D* ECal2gsearch_MinvVsRcogDt1Phi100 = (TH1D*)f1->Get("ECal2gsearch_MinvVsRcogDt1Phi100");
   TH1D* ECal2gsearch_MinvVsRcogDt1Phi75  = (TH1D*)f1->Get("ECal2gsearch_MinvVsRcogDt1Phi75" );
   TH1D* ECal2gsearch_MinvVsRcogDt1Phi50  = (TH1D*)f1->Get("ECal2gsearch_MinvVsRcogDt1Phi50" );
   TH1D* ECal2gsearch_MinvVsRcogDt1Phi25  = (TH1D*)f1->Get("ECal2gsearch_MinvVsRcogDt1Phi25" );
   c34->Divide(5,3,0,0);
   c34->cd(1);
   t2->SetTextSize(10);
   ECal2gsearch_MinvVsRcogDt3->Draw("ZCOL");
   t2->DrawText(200.,45.,"dT= 3ns");
   c34->cd(2);
   ECal2gsearch_MinvVsRcogDt3Phi100->Draw("ZCOL");
   t2->DrawText(120.,45.,"dT= 3ns, dPhi=10deg");
   c34->cd(3);
   ECal2gsearch_MinvVsRcogDt3Phi75->Draw("ZCOL");
   t2->DrawText(120.,45.,"dT= 3ns, dPhi=7.5deg");
   c34->cd(4);
   ECal2gsearch_MinvVsRcogDt3Phi50->Draw("ZCOL");
   t2->DrawText(120.,45.,"dT= 3ns, dPhi=5.0deg");
   c34->cd(5);
   ECal2gsearch_MinvVsRcogDt3Phi25->Draw("ZCOL");
   t2->DrawText(120.,45.,"dT= 3ns, dPhi=2.5deg");
   c34->cd(6);
   ECal2gsearch_MinvVsRcogDt2->Draw("ZCOL");
   t2->DrawText(30.,40.,"Invariant mass [MeV] (y-axix)");
   t2->DrawText(30.,35.,"CoG-distance from 0,0 [mm] (x-axis)");
   t2->DrawText(30.,25.,"Primary e+ beam");
   t2->DrawText(30.,20.,"10^10 POT");
   c34->cd(7);
   ECal2gsearch_MinvVsRcogDt2Phi100->Draw("ZCOL");
   t2->DrawText(120.,45.,"dT= 2ns, dPhi=10deg");
   c34->cd(8);
   ECal2gsearch_MinvVsRcogDt2Phi75->Draw("ZCOL");
   t2->DrawText(120.,45.,"dT= 2ns, dPhi=7.5deg");
   c34->cd(9);
   ECal2gsearch_MinvVsRcogDt2Phi50->Draw("ZCOL");
   t2->DrawText(120.,45.,"dT= 2ns, dPhi=5.0deg");
   c34->cd(10);
   ECal2gsearch_MinvVsRcogDt2Phi25->Draw("ZCOL");
   t2->DrawText(120.,45.,"dT= 2ns, dPhi=2.5deg");
   c34->cd(11);
   ECal2gsearch_MinvVsRcogDt1->Draw("ZCOL");
   c34->cd(12);
   ECal2gsearch_MinvVsRcogDt1Phi100->Draw("ZCOL");
   t2->DrawText(120.,45.,"dT= 1ns, dPhi=10deg");
   c34->cd(13);
   ECal2gsearch_MinvVsRcogDt1Phi75->Draw("ZCOL");
   t2->DrawText(120.,45.,"dT= 1ns, dPhi=7.5deg");
   c34->cd(14);
   ECal2gsearch_MinvVsRcogDt1Phi50->Draw("ZCOL");
   t2->DrawText(120.,45.,"dT= 1ns, dPhi=5.0deg");
   c34->cd(15);
   ECal2gsearch_MinvVsRcogDt1Phi25->Draw("ZCOL");
   t2->DrawText(120.,45.,"dT= 1ns, dPhi=2.5deg");

  
   /*
   TLatex *text = new TLatex();
   text->DrawTextNDC(0.12, 0.14, "D#slash{0} Preliminary");
   c2->SaveAs("c2.pdf");
   */

   return;

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
