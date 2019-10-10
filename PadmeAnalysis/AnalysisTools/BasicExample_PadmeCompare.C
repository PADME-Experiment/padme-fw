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
#include "TRandom.h"
#include "TGraphErrors.h"

void BasicExample_PadmeCompare()
{


  #ifdef __CINT__
  gROOT->LoadMacro("PadmeLabels.C");
  gROOT->LoadMacro("PadmeUtils.C");
  gROOT->LoadMacro("PadmeStyle.C");
  #endif
 
  SetPadmeStyle();

  //      TFile *f1 = TFile::Open("/afs/le.infn.it/project/padme/Padme-fw_28_02_2019/PadmeAnalysis/RecoFromRawOutputFile_AnalysisRef.root");
  //      TFile *f2 = TFile::Open("/afs/le.infn.it/project/padme/Padme-fw_28_02_2019/PadmeAnalysis/RecoFromRawOutputFile_AnalysisTest.root");
  TFile *f1 = TFile::Open("RefValidationSample.root");
  TFile *f2 = TFile::Open("TestValidationSample.root");
    if (!f1) 
      { 
	std::cout<<"Reference file (RefValidationSample.root) not found; exit"<<std::endl;
	return;
      }
    if (!f2) 
      { 
	std::cout<<"Reference file (TestValidationSample.root) not found; exit"<<std::endl;
	return;
      }
    std::cout<<"Comparing input files: "<<f1->GetName()<<" and "<<f2->GetName()<<std::endl;
    

    //TFile *f1 = TFile::Open("/afs/le.infn.it/user/i/isabella/padme/Padme-fw_28_02_2019/PadmeAnalysis/RecoFromRawOutputFile_AnalysisRef.root");
    //TFile *f2 = TFile::Open("/afs/le.infn.it/user/i/isabella/padme/Padme-fw_28_02_2019/PadmeAnalysis/RecoFromRawOutputFile_AnalysisTest.root");
  //TFile *f1 = TFile::Open("/afs/le.infn.it/project/padme/Padme-fw_28_02_2019/PadmeReco/outControlFile.root");
  //TFile *f2 = TFile::Open("/afs/le.infn.it/project/padme/Padme-fw_28_02_2019/PadmeReco/outControlFileMC.root");
  
  TList *ObjList = f1->GetListOfKeys();
  
  TCanvas *c0 = new TCanvas("c1", "c1", 600,800);
  TPad *newpad=new TPad("newpad","a transparent pad",0,0,1,1);
  newpad->SetFillStyle(4000);
  newpad->Draw();
  newpad->cd();
  TPaveLabel *title = new TPaveLabel(0.1,0.94,0.9,0.98,"Dimension of PADME variables");
  title->SetFillColor(5);
  title->SetTextFont(52);
  title->Draw();
  
  myTextInit( 0.1,  0.78, 1, "Energy   #rightarrow MeV ");
  myTextInit( 0.1,  0.68, 1, "Charge   #rightarrow pC ");
  myTextInit( 0.1,  0.58, 1, "Time     #rightarrow ns"); 
  myTextInit( 0.1,  0.48, 1, "Position #rightarrow mm");
  myTextInit( 0.1,  0.38, 1, "Ch Id    #rightarrow adimensional");
  myTextInit( 0.1,  0.28, 1, "POT      #rightarrow adimensional");  
  c0->Print("ValidationHisto.pdf(");
  TCanvas *c1 = new TCanvas("c1", "c1", 600,800);
  c1->Divide(2,2);
 
  
  
  for(Int_t iList = 0; iList < ObjList->GetEntries(); iList++)
  {
        TString ObjName = ObjList->At(iList)->GetName();
        std::cout<<"Obj at " << iList << " is " << ObjName << std::endl;
        TString ObjClass =  ((TKey*)ObjList->At(iList))->GetClassName();
        if (ObjClass=="TH1D")  
       {
         
         TH1D *temp1Hist_DATA = (TH1D*)f1->Get(ObjName);
         TH1D *temp1Hist_MC = (TH1D*)f2->Get(ObjName);
         //cout<<"ObjName"<< ObjName << endl;
         
         c1->cd(1);
         gPad->SetLogy();
         temp1Hist_DATA->SetLineColor(4);
         temp1Hist_DATA->SetLineWidth(1);
         temp1Hist_DATA->SetYTitle("Counts");
         temp1Hist_DATA->SetXTitle("VariableUnit");
         temp1Hist_DATA->GetYaxis()->SetTitleOffset(1.5);
         temp1Hist_DATA->GetXaxis()->SetTitleOffset(0.9);
         double EntryDATA = temp1Hist_DATA->GetEntries();
         if (EntryDATA!=0) temp1Hist_DATA->Scale(1./EntryDATA);
         temp1Hist_DATA->Draw();
         //temp1Hist_DATA->DrawNormalized();
         myText( 0.3,  0.78, 1, "Ref");
         myTitle( 0.3,  0.85, 1, Form("%s",ObjName.Data()));
         
         
         c1->cd(2); 
         gPad->SetLogy();
         temp1Hist_MC->SetLineColor(2);
         temp1Hist_MC->SetLineWidth(1);
         temp1Hist_MC->SetYTitle("Counts");
         temp1Hist_MC->SetXTitle("VariableUnit");
         temp1Hist_MC->GetYaxis()->SetTitleOffset(1.5);
         temp1Hist_MC->GetXaxis()->SetTitleOffset(0.9);
         double EntryMC = temp1Hist_MC->GetEntries();
         if (EntryMC!=0) temp1Hist_MC->Scale(1./EntryMC);
         temp1Hist_MC->Draw();
         myText( 0.3,  0.78, 1, "Test");
         myTitle( 0.3,  0.85, 1, Form("%s",ObjName.Data()));
         
         
         c1->cd(3);
         
         gPad->SetLogy();
         temp1Hist_MC->Draw();
         temp1Hist_DATA->SetMarkerStyle(20);
         temp1Hist_DATA->SetMarkerColor(4);
         temp1Hist_DATA->SetMarkerSize(0.6);
         temp1Hist_DATA->Draw("p SAME");
         
         
         /*myText(       0.36,  0.78, 1, "POT= 10^{11}");
         myText(       0.6, 0.78, 1, "E_{e^{+}} = 550 MeV");
         //myMarkerText( 0.55, 0.75, 1, 22, "RUN I, 2018",0.7);*/
         myBoxText(    0.6  , 0.57, 0.05,4 , "Ref ");
        // myBoxText(    0.6  , 0.62, 0.05, 2, "Data Cluster");
         myBoxText(    0.6  , 0.67, 0.05, 2, "Test ");

         PADMELabel(0.2,0.2,"Preliminary");
         PadmeText();
         c1->cd(4);
        
         
         gPad->SetLogy();
         TH1* Ratio = (TH1*)temp1Hist_DATA->Clone();
         Ratio->SetYTitle("Ref/Test");
         Ratio->Divide(temp1Hist_MC);
         
         Ratio->SetMarkerColor(2);
         Ratio->SetMarkerStyle(7);
         Ratio->Draw("p");
         float xmin = temp1Hist_MC->GetXaxis()->GetXmin();
         float xmax = temp1Hist_MC->GetXaxis()->GetXmax();
     
         TLine *line = new TLine(xmin,1, xmax,1);
         line->SetLineColor(1);
         line->SetLineWidth(2);
         line->SetLineStyle(3);
         
         line->Draw();
         //myText(       0.36,  0.88, 1, "Ratio DATA/MC");
         PADMELabel(0.2,0.2,"Preliminary");
         PadmeText();
         c1->Print("ValidationHisto.pdf(",Form("Title:%s",ObjName.Data()));
         
       }
        if (ObjClass=="TH2F")  
       {
         TH2F *Hist   = (TH2F*)f1->Get(ObjName);
         TH2F *temp2Hist_1 = (TH2F*)f2->Get(ObjName);
         //cout<<"ObjName"<< ObjName << endl;
         c1->cd(1);
         temp2Hist->Draw("COLORZ");
         c1->cd(2);
         temp2Hist_1->Draw("COLORZ");
         c1->Print("ValidationHisto.pdf(",Form("Title:%s",ObjName.Data()));
       }
  }
  c1->Print("ValidationHisto.pdf)");


 /* TCanvas* c1 = new TCanvas("c1","ECal",50,50,600,600);
  //TPad* thePad = (TPad*)c1->cd();
  //thePad->SetLogy();

  TH1D *h1 = (TH1D*)f1->Get("ECal_HitEnergy");
  //int N_h1=h1->GetEntries();
  //h1->Scale(N_h1);
  h1->SetYTitle("Counts");
  h1->SetXTitle("E (MeV)");
  h1->GetYaxis()->SetTitleOffset(1.4);
  h1->GetXaxis()->SetTitleOffset(1.4);
  h1->SetLineColor(4);
  //h1->GetXaxis()->SetNdivisions(5);
  h1->DrawNormalized();

  
  TH1D *h2 = (TH1D*)f1->Get("ECal_ClusterEnergy");
  h2->SetLineColor(2);
  h2->DrawNormalized("SAME");
  
  TH1D *h3 = (TH1D*)f2->Get("ECal_ClusterEnergy");
  h3->SetLineColor(7);
  h3->DrawNormalized("SAME");


  myText(       0.36,  0.78, 1, "POT= 10^{11}");
  myText(       0.6, 0.78, 1, "E_{e^{+}} = 550 MeV");
  myMarkerText( 0.55, 0.75, 1, 20, "Phase I, 2018",1.3);
  myBoxText(    0.6  , 0.57, 0.05,4 , "Data Hit");
  myBoxText(    0.6  , 0.62, 0.05, 2, "Data Cluster");
  myBoxText(    0.6  , 0.67, 0.05, 7, "MC Cluster");
 

  PADMELabel(0.2,0.2,"Preliminary");

  c1->Print("PadmeExample.eps");
  c1->Print("PadmeExample.png");
  c1->Print("PadmeExample.pdf");*/
  

  //return;
  exit(0);
}
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
#endif
