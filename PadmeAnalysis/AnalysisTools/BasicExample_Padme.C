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

void BasicExample_Padme()
{


  #ifdef __CINT__
  gROOT->LoadMacro("PadmeLabels.C");
  gROOT->LoadMacro("PadmeUtils.C");
  gROOT->LoadMacro("PadmeStyle.C");
  #endif
 
  SetPadmeStyle();

  
  TFile *f1 = TFile::Open("ValidationSample.root");
    if (!f1) 
      { 
	std::cout<<"Reference file (ValidationSample.root) not found; exit"<<std::endl;
	return;
      }

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
  //c1->Divide(2,2);
 
  
  
  for(Int_t iList = 0; iList < ObjList->GetEntries(); iList++)
  {
        TString ObjName = ObjList->At(iList)->GetName();
        std::cout<<"Obj at " << iList << " is " << ObjName << std::endl;
        TString ObjClass =  ((TKey*)ObjList->At(iList))->GetClassName();
        if (ObjClass=="TH1D")  
       {
         
         TH1D *temp1Hist_DATA = (TH1D*)f1->Get(ObjName);
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
         myTitle( 0.3,  0.85, 1, Form("%s",ObjName.Data()));
       
         PadmeText();
         c1->Print("ValidationHisto.pdf(",Form("Title:%s",ObjName.Data()));
         
       }
        if (ObjClass=="TH2F")  
       {
         TH2F *Hist   = (TH2F*)f1->Get(ObjName);
         //cout<<"ObjName"<< ObjName << endl;
         c1->cd(1);
         temp2Hist->Draw("COLORZ");
         c1->Print("ValidationHisto.pdf(",Form("Title:%s",ObjName.Data()));
       }
  }
  c1->Print("ValidationHisto.pdf)");
  exit(0);
}
void PadmeText()
{        
         //myText(       0.22,  0.78, 1, "POT= 10^{11}");
         //myText(       0.22,  0.28, 1, "Normalized");
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
