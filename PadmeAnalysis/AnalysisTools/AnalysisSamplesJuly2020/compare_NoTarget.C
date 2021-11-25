#include <iostream>
#include <cmath>

#include "Rtypes.h"

#include "PadmeUtils.h"
#include "PadmeStyle.h"
#include "PadmeLabels.h"

/*
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
#include "TRandom.h"
#include "TGraphErrors.h"

using namespace std;


void compare_NoTarget(std::string hname, double scalef=-1, double xmin=-999, double xmax=999, 
	       std::string xtitle="E(#gamma_{1})+E(#gamma_{2}) [MeV]", 
	       TFile* fData=_file0, TFile* fMC=_file1)
{
  //  TFile* fData = _file0;
  //  TFile* fMC   = _file1;

  
  /*    
  #ifdef __CINT__
    gROOT->LoadMacro("PadmeLabels.C");
    gROOT->LoadMacro("PadmeUtils.C");
  #endif

  SetPadmeStyle();
  */
  
  gStyle->SetOptTitle(0);
  std::cout<<" pointer to data file and mc file ... "<<fData<<" "<<fMC<<"    processing histo "<<hname <<std::endl;
  
  TH1D* hData = (TH1D*)fData->Get(hname.c_str());
  TH1D* hMC   = (TH1D*)fMC->Get(hname.c_str());
  
  TCanvas* cData = new TCanvas("data","data",500,500);
  hData->Draw();
  TCanvas* cMC = new TCanvas("mc","mc",500,500);
  hData->Draw();
  if(hname=="ECal_ClusterEnergy"||hname=="ECal_HitEnergy"){
    cData->SetLogy();
    cMC->SetLogy();
  }
  
  //  return;

  if (scalef<0) 
    {
      double nPOTfromTarget = ((TH1D*)fData->Get("nPOT"))->GetBinContent(2);
      double nPOTTarget = ((TH1D*)fData->Get("ECal_EHitTot"))->GetEntries();
      double nPOTNoTarget =  ((TH1D*)fMC->Get("ECal_EHitTot"))->GetEntries();
      std::cout<<"Target: " << nPOTTarget << " No Target "<< nPOTNoTarget << " POT " << std::endl;
      scalef=nPOTTarget/nPOTNoTarget;
    }

  TH1D* hsMC = hMC->Clone();
  hsMC->Scale(scalef);
  double ymaxMC = hsMC->GetMaximum();
  double ymaxDT = hData->GetMaximum();

  double ymax = ymaxMC;
  if (ymaxDT>ymax) ymax=ymaxDT;
  ymax = 1.3*ymax;

  TH1D* hFrame = hMC->Clone();
  hFrame->Scale(0.);
  hFrame->SetMaximum(ymax);


  hFrame->Draw();
  hsMC->SetLineColor(kRed);
  hsMC->Draw("SAME");
  hData->SetMarkerStyle(20);
  //hData->SetMarkerSize(0.5);
  //hData->SetMarkerColor(kBlack);
  hData->SetLineColor(kBlack);
  hData->Draw("SAME");
  if(hMC->GetEntries()>0.001)cout<<"Data/MC ratio = "<<hData->GetEntries()/hMC->GetEntries()<<std::endl;
  cout<<"Data/sMC ratio = "<<hData->GetEntries()/(scalef/**hMC->GetEntries()*/)<<std::endl;

//return;

  TH1D* hratio = hMC->Clone();
  hratio->Scale(0.);
  int nbinx = hratio->GetNbinsX();
  double rat, erat;
  for (int i=1; i<nbinx+1; ++i)
    {
      rat =0.;
      erat =0.;
      if (fabs(hsMC->GetBinContent(i))>1e-10) {
	rat = hData->GetBinContent(i)/hsMC->GetBinContent(i);
	erat = sqrt(hData->GetBinContent(i)+scalef*scalef*hMC->GetBinContent(i))/hsMC->GetBinContent(i);
      }
      hratio->SetBinContent(i, rat); 
      hratio->SetBinError(i, erat); 
      //std::cout<<"Bin "<<i<<"  ratio = "<<hratio->GetBinContent(i)<<std::endl;
      //std::cout<<"Bin "<<i<<" eratio = "<<hratio->GetBinError(i)<<std::endl;
    }
  TCanvas* c2 = new TCanvas("ratio","ratio",600,300);
  hratio->Draw();

   // Define the Canvas
  TCanvas *c = new TCanvas("c", "My Final Plot", 50, 50, 600,600);
 
   // Upper plot will be in pad1
   TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.);
   pad1->SetBottomMargin(0.02); // Upper and lower plot are joined
   pad1->SetTopMargin(1); // Upper and lower plot are joined
   if(hname=="ECal_ClusterEnergy"||hname=="ECal_HitEnergy"){
     pad1->SetLogy();
     pad1->SetLogx();
   }
   //pad1->SetLogy();
   //pad1->SetGridx();         // Vertical grid
   pad1->Draw();             // Draw the upper pad: pad1
   pad1->cd();               // pad1 becomes the current pad

   /*
   h1->SetStats(0);          // No statistics on upper plot
   h1->Draw();               // Draw h1
   h2->Draw("same");         // Draw h2 on top of h1
   */

   Double_t ymin=0.;
   ymin = hMC->GetYaxis()->GetXmin();
   if (pad1->GetLogy()) {
     ymin = 1.;
     ymax = 10*ymax;
   }
   
   if (fabs(xmin+999)<0.0001 && fabs(xmax-999)<0.0001) {
     xmin = hMC->GetXaxis()->GetXmin();
     xmax = hMC->GetXaxis()->GetXmax();
   }
   //   Double_t xmin=60.00;  Double_t xmax=3500.;
   TH1F *hFrame1 = pad1->DrawFrame(xmin,ymin,xmax,ymax);
   std::cout<<"X range = "<<xmin<<" "<<xmax<<std::endl;
   std::cout<<"Y range = "<<ymin<<" "<<ymax<<std::endl;
   
   hFrame1->GetXaxis()->SetLabelSize(0.);
   hFrame1->SetStats(0);  
   hsMC->SetStats(0);  
   hData->SetStats(0);  
   //hFrame1->Draw("SAME");
   //return;

   hsMC->Draw("SAME");
   hData->Draw("SAME");



   // Do not draw the Y axis label on the upper plot and redraw a small
   // axis instead, in order to avoid the first label (0) to be clipped.
   //////////////hFrame->GetYaxis()->SetLabelSize(0.);
   //TGaxis *axis = new TGaxis( -5, 20, -5, 220, 20,220,510,"");
   //axis->SetLabelFont(43); // Absolute font size in pixel (precision 3)
   //axis->SetLabelSize(15);
   //axis->Draw();

   // lower plot will be in pad
   c->cd();          // Go back to the main canvas before defining pad2
   TPad *pad2 = new TPad("pad2", "pad2", 0, 0.0, 1, 0.3);
   pad2->SetTopMargin(0.02);
   pad2->SetBottomMargin(0.3);
   if(hname=="ECal_ClusterEnergy"||hname=="ECal_HitEnergy"){
     // pad1->SetLogy();
     pad2->SetLogx();
   }
   //pad2->SetGridx(); // vertical grid
   pad2->SetGridy(); // horizontal grid
   pad2->Draw();
   pad2->cd();       // pad2 becomes the current pad

   /*
   // Define the ratio plot
   TH1F *h3 = (TH1F*)h1->Clone("h3");
   h3->SetLineColor(kBlack);
   h3->SetMinimum(0.8);  // Define Y ..
   h3->SetMaximum(1.35); // .. range
   h3->Sumw2();
   h3->SetStats(0);      // No statistics on lower plot
   h3->Divide(h2);
   h3->SetMarkerStyle(21);
   h3->Draw("ep");       // Draw the ratio plot
   */
   hratio->GetXaxis()->SetTitle("Y strip number");
   pad2->SetLogy();
   TH1F *hFrameRat = pad2->DrawFrame(xmin,0.1,xmax,12.);
   hFrameRat->GetXaxis()->SetTitle(xtitle.c_str());
   hFrameRat->Draw();
   hratio->SetStats(0);  
   hratio->SetMaximum(12.);
   hratio->SetMinimum(0.1);
   //hratio->SetMinimum(0.1);
   hratio->SetMarkerStyle(20);
   hratio->SetMarkerSize(0.5);
   hratio->Draw("epsame");
   
   /*
   // h1 settings
   h1->SetLineColor(kBlue+1);
   h1->SetLineWidth(2);

   // Y axis h1 plot settings
   h1->GetYaxis()->SetTitleSize(20);
   h1->GetYaxis()->SetTitleFont(43);
   h1->GetYaxis()->SetTitleOffset(1.55);

   // h2 settings
   h2->SetLineColor(kRed);
   h2->SetLineWidth(2);
   */

   // Ratio plot (h3) settings
   //h3->SetTitle(""); // Remove the ratio title
   hratio->SetTitle("");

   
   // Y axis ratio plot settings
   hFrameRat->GetYaxis()->SetTitle("Target/NoTarget");
   hFrameRat->GetYaxis()->SetNdivisions(505);
   hFrameRat->GetYaxis()->SetTitleSize(20);
   hFrameRat->GetYaxis()->SetTitleFont(43);
   hFrameRat->GetYaxis()->SetTitleOffset(1.55);
   hFrameRat->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
   hFrameRat->GetYaxis()->SetLabelSize(20);

   // X axis ratio plot settings
   hFrameRat->GetXaxis()->SetTitleSize(20);
   hFrameRat->GetXaxis()->SetTitleFont(43);
   hFrameRat->GetXaxis()->SetTitleOffset(3.5);
   hFrameRat->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
   hFrameRat->GetXaxis()->SetLabelSize(20);
   

   pad1->cd();
   PADME_LABEL(0.2,0.8); myText( 0.33,0.8,1,"Internal");
   const char *a=hname.c_str();
   myText( 0.25,0.91,1,a, 0.039); 
  //   ATLASLabel(0.2,0.8,"Work in progress");//,"Preliminary");
   myText(       0.2, 0.7 , 1, "nPOT=2.69#times10^{10}");

   // myText(       0.63, 0.7 , 1, "Diamond Target");
   // myText(       0.57, 0.8, 1, "Average beam Y profile");

   //myMarkerText( 0.25, 0.5, 1, 20, "Data July 2019",1.3);
   //myBoxText(    0.25, 0.45, 0.05, kBlack, "MC");
   TLegend* t = new TLegend(0.55,0.7,0.80,0.85);
   t->AddEntry(hData,"Target","ep");
   //t->AddEntry(hData,"MCfr 23k e^{+}/bunch (SH+Ecorr+VCon)","ep");
   //t->AddEntry(hsMC,"MCle 20k e^{+}/bunch (SH+Ecorr+VCon)","l");
   t->AddEntry(hsMC,"No Target","l");
   t->SetTextAlign(12);
   t->SetBorderSize(0);
   t->SetFillStyle(0);
   t->SetFillColor(0);
   t->SetTextFont(43);
   t->SetTextSize(15);

   t->Draw();

   
  c->SaveAs(("outpng/"+hname+".png").c_str());
  c->Print("AnalysisHisto.pdf(");
   if(/*hname=="SS2gYEw_passDtDphiCogDsume"||hname=="ECal_NCluster" || */hname=="ECAL_tgg_10ns3CoGUnderPeakEThr90") c->Print("AnalysisHisto.pdf)");
 
   //c->SaveAs("myPlot.png");
   //c->SaveAs("myPlot.eps");
   //return;

   
   delete pad1;
   delete pad2;
   delete cData;
   delete cMC;
   delete c;
   delete c2;
   delete t;
   
   std::cout<<"Bye, bye ................."<<std::endl<<std::endl;
}




void compare_NoTarget(std::string hname, double scalef=-1, double xmin=-999, double xmax=999, 
	       std::string xtitle="E(#gamma_{1})+E(#gamma_{2}) [MeV]", 
		      TFile* fData=_file0, TFile* fMC=_file1, Bool_t useDTimeNormalization)
{
  //  TFile* fData = _file0;
  //  TFile* fMC   = _file1;

  
  /*    
  #ifdef __CINT__
    gROOT->LoadMacro("PadmeLabels.C");
    gROOT->LoadMacro("PadmeUtils.C");
  #endif

  SetPadmeStyle();
  */
  
  gStyle->SetOptTitle(0);
  std::cout<<" pointer to data file and mc file ... "<<fData<<" "<<fMC<<"    processing histo "<<hname <<std::endl;
  
  TH1D* hData = (TH1D*)fData->Get(hname.c_str());
  TH1D* hMC   = (TH1D*)fMC->Get(hname.c_str());
  
  TCanvas* cData = new TCanvas("data","data",500,500);
  hData->Draw();
  TCanvas* cMC = new TCanvas("mc","mc",500,500);
  hData->Draw();
  if(hname=="ECal_ClusterEnergy"||hname=="ECal_HitEnergy"){
    cData->SetLogy();
    cMC->SetLogy();
  }
  
  //  return;

  if (scalef<0) 
    {
      double nPOTfromTarget = ((TH1D*)fData->Get("nPOT"))->GetBinContent(2);
      double nPOTTarget = ((TH1D*)fData->Get("ECal_EHitTot"))->GetEntries();
      double nPOTNoTarget =  ((TH1D*)fMC->Get("ECal_EHitTot"))->GetEntries();
      std::cout<<"From EHitTot ecal........n events Target: " << nPOTTarget << " No Target "<< nPOTNoTarget << " factor scale factor "<<nPOTTarget/nPOTNoTarget << std::endl;
      //I'm using the delta time background to estimate the normalization factor
      TH1D * dataDTime = (TH1D*)fData->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1Data=dataDTime->FindBin(-10);
      Double_t bin2Data=dataDTime->FindBin(-6);
      Double_t bin4Data=dataDTime->FindBin(10);
      Double_t bin3Data=dataDTime->FindBin(6);
      Double_t IntegralLeft=dataDTime->Integral(bin1Data,bin2Data);
      Double_t IntegralRight=dataDTime->Integral(bin3Data,bin4Data);

      TH1D * mcDTime = (TH1D*)fMC->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1MC=mcDTime->FindBin(-10);
      Double_t bin2MC=mcDTime->FindBin(-6);
      Double_t bin4MC=mcDTime->FindBin(10);
      Double_t bin3MC=mcDTime->FindBin(6);
      Double_t IntegralLeftmc=mcDTime->Integral(bin1Data,bin2Data);
      Double_t IntegralRightmc=mcDTime->Integral(bin3Data,bin4Data);
      if(useDTimeNormalization)scalef=IntegralLeft/IntegralLeftmc;
      else scalef=nPOTTarget/nPOTNoTarget;
      std::cout<<" data left " << IntegralLeft << " right " << IntegralRight << " ....mc left " << IntegralLeftmc << " right " << IntegralRightmc << " ratio data/mc left " << IntegralLeft/IntegralLeftmc << std::endl;
    }

  TH1D* hsMC = hMC->Clone();
  hsMC->Scale(scalef);
  double ymaxMC = hsMC->GetMaximum();
  double ymaxDT = hData->GetMaximum();

  double ymax = ymaxMC;
  if (ymaxDT>ymax) ymax=ymaxDT;
  ymax = 1.3*ymax;

  TH1D* hFrame = hMC->Clone();
  hFrame->Scale(0.);
  hFrame->SetMaximum(ymax);


  hFrame->Draw();
  hsMC->SetLineColor(kRed);
  hsMC->Draw("SAME");
  hData->SetMarkerStyle(20);
  //hData->SetMarkerSize(0.5);
  //hData->SetMarkerColor(kBlack);
  hData->SetLineColor(kBlack);
  hData->Draw("SAME");
  if(hMC->GetEntries()>0.001) cout<<"Data/MC ratio = "<<hData->GetEntries()/hMC->GetEntries()<<std::endl;
  cout<<"Data/sMC ratio = "<<hData->GetEntries()/(scalef/**hMC->GetEntries()*/)<<std::endl;

//return;

  TH1D* hratio = hMC->Clone();
  hratio->Scale(0.);
  int nbinx = hratio->GetNbinsX();
  double rat, erat;
  for (int i=1; i<nbinx+1; ++i)
    {
      rat =0.;
      erat =0.;
      if (fabs(hsMC->GetBinContent(i))>1e-10) {
	rat = hData->GetBinContent(i)/hsMC->GetBinContent(i);
	erat = sqrt(hData->GetBinContent(i)+scalef*scalef*hMC->GetBinContent(i))/hsMC->GetBinContent(i);
      }
      hratio->SetBinContent(i, rat); 
      hratio->SetBinError(i, erat); 
      //std::cout<<"Bin "<<i<<"  ratio = "<<hratio->GetBinContent(i)<<std::endl;
      //std::cout<<"Bin "<<i<<" eratio = "<<hratio->GetBinError(i)<<std::endl;
    }
  TCanvas* c2 = new TCanvas("ratio","ratio",600,300);
  hratio->Draw();

   // Define the Canvas
  TCanvas *c = new TCanvas("c", "My Final Plot", 50, 50, 600,600);
 
   // Upper plot will be in pad1
   TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.);
   pad1->SetBottomMargin(0.02); // Upper and lower plot are joined
   pad1->SetTopMargin(1); // Upper and lower plot are joined
   if(hname=="ECal_ClusterEnergy"||hname=="ECal_HitEnergy"){
     pad1->SetLogy();
     pad1->SetLogx();
   }
   //pad1->SetLogy();
   //pad1->SetGridx();         // Vertical grid
   pad1->Draw();             // Draw the upper pad: pad1
   pad1->cd();               // pad1 becomes the current pad

   /*
   h1->SetStats(0);          // No statistics on upper plot
   h1->Draw();               // Draw h1
   h2->Draw("same");         // Draw h2 on top of h1
   */

   Double_t ymin=0.;
   ymin = hMC->GetYaxis()->GetXmin();
   if (pad1->GetLogy()) {
     ymin = 1.;
     ymax = 10*ymax;
   }
   
   if (fabs(xmin+999)<0.0001 && fabs(xmax-999)<0.0001) {
     xmin = hMC->GetXaxis()->GetXmin();
     xmax = hMC->GetXaxis()->GetXmax();
   }
   //   Double_t xmin=60.00;  Double_t xmax=3500.;
   TH1F *hFrame1 = pad1->DrawFrame(xmin,ymin,xmax,ymax);
   std::cout<<"X range = "<<xmin<<" "<<xmax<<std::endl;
   std::cout<<"Y range = "<<ymin<<" "<<ymax<<std::endl;
   
   hFrame1->GetXaxis()->SetLabelSize(0.);
   hFrame1->SetStats(0);  
   hsMC->SetStats(0);  
   hData->SetStats(0);  
   //hFrame1->Draw("SAME");
   //return;

   hsMC->Draw("SAME");
   hData->Draw("SAME");



   // Do not draw the Y axis label on the upper plot and redraw a small
   // axis instead, in order to avoid the first label (0) to be clipped.
   //////////////hFrame->GetYaxis()->SetLabelSize(0.);
   //TGaxis *axis = new TGaxis( -5, 20, -5, 220, 20,220,510,"");
   //axis->SetLabelFont(43); // Absolute font size in pixel (precision 3)
   //axis->SetLabelSize(15);
   //axis->Draw();

   // lower plot will be in pad
   c->cd();          // Go back to the main canvas before defining pad2
   TPad *pad2 = new TPad("pad2", "pad2", 0, 0.0, 1, 0.3);
   pad2->SetTopMargin(0.02);
   pad2->SetBottomMargin(0.3);
   if(hname=="ECal_ClusterEnergy"||hname=="ECal_HitEnergy"){
     // pad1->SetLogy();
     pad2->SetLogx();
   }
   //pad2->SetGridx(); // vertical grid
   pad2->SetGridy(); // horizontal grid
   pad2->Draw();
   pad2->cd();       // pad2 becomes the current pad

   /*
   // Define the ratio plot
   TH1F *h3 = (TH1F*)h1->Clone("h3");
   h3->SetLineColor(kBlack);
   h3->SetMinimum(0.8);  // Define Y ..
   h3->SetMaximum(1.35); // .. range
   h3->Sumw2();
   h3->SetStats(0);      // No statistics on lower plot
   h3->Divide(h2);
   h3->SetMarkerStyle(21);
   h3->Draw("ep");       // Draw the ratio plot
   */
   hratio->GetXaxis()->SetTitle("Y strip number");
   pad2->SetLogy();
   TH1F *hFrameRat = pad2->DrawFrame(xmin,0.1,xmax,12.);
   hFrameRat->GetXaxis()->SetTitle(xtitle.c_str());
   hFrameRat->Draw();
   hratio->SetStats(0);  
   hratio->SetMaximum(12.);
   hratio->SetMinimum(0.1);
   //hratio->SetMinimum(0.1);
   hratio->SetMarkerStyle(20);
   hratio->SetMarkerSize(0.5);
   hratio->Draw("epsame");
   
   /*
   // h1 settings
   h1->SetLineColor(kBlue+1);
   h1->SetLineWidth(2);

   // Y axis h1 plot settings
   h1->GetYaxis()->SetTitleSize(20);
   h1->GetYaxis()->SetTitleFont(43);
   h1->GetYaxis()->SetTitleOffset(1.55);

   // h2 settings
   h2->SetLineColor(kRed);
   h2->SetLineWidth(2);
   */

   // Ratio plot (h3) settings
   //h3->SetTitle(""); // Remove the ratio title
   hratio->SetTitle("");

   
   // Y axis ratio plot settings
   hFrameRat->GetYaxis()->SetTitle("Target/NoTarget");
   hFrameRat->GetYaxis()->SetNdivisions(505);
   hFrameRat->GetYaxis()->SetTitleSize(20);
   hFrameRat->GetYaxis()->SetTitleFont(43);
   hFrameRat->GetYaxis()->SetTitleOffset(1.55);
   hFrameRat->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
   hFrameRat->GetYaxis()->SetLabelSize(20);

   // X axis ratio plot settings
   hFrameRat->GetXaxis()->SetTitleSize(20);
   hFrameRat->GetXaxis()->SetTitleFont(43);
   hFrameRat->GetXaxis()->SetTitleOffset(3.5);
   hFrameRat->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
   hFrameRat->GetXaxis()->SetLabelSize(20);
   

   pad1->cd();
   PADME_LABEL(0.2,0.8); myText( 0.33,0.8,1,"Internal");
   const char *a=hname.c_str();
   myText( 0.25,0.91,1,a, 0.039); 
  //   ATLASLabel(0.2,0.8,"Work in progress");//,"Preliminary");
   const char nnpot= nPOTfromTarget;
   myText(       0.2, 0.7 , 1, "nPOT=2.69#times10^{10}");

   // myText(       0.63, 0.7 , 1, "Diamond Target");
   // myText(       0.57, 0.8, 1, "Average beam Y profile");

   //myMarkerText( 0.25, 0.5, 1, 20, "Data July 2019",1.3);
   //myBoxText(    0.25, 0.45, 0.05, kBlack, "MC");
   TLegend* t = new TLegend(0.55,0.7,0.80,0.85);
   t->AddEntry(hData,"Target","ep");
   //t->AddEntry(hData,"MCfr 23k e^{+}/bunch (SH+Ecorr+VCon)","ep");
   //t->AddEntry(hsMC,"MCle 20k e^{+}/bunch (SH+Ecorr+VCon)","l");
   t->AddEntry(hsMC,"No Target","l");
   t->SetTextAlign(12);
   t->SetBorderSize(0);
   t->SetFillStyle(0);
   t->SetFillColor(0);
   t->SetTextFont(43);
   t->SetTextSize(15);

   t->Draw();

   
  c->SaveAs(("outpng/"+hname+".png").c_str());
  c->Print("AnalysisHisto.pdf(");
   if(/*hname=="SS2gYEw_passDtDphiCogDsume"||hname=="ECal_NCluster" || */hname=="ECAL_tgg_10ns3CoGUnderPeakEThr90") c->Print("AnalysisHisto.pdf)");
 
   //c->SaveAs("myPlot.png");
   //c->SaveAs("myPlot.eps");
   //return;

   
   delete pad1;
   delete pad2;
   delete cData;
   delete cMC;
   delete c;
   delete c2;
   delete t;
   
   std::cout<<"Bye, bye ................."<<std::endl<<std::endl;
}
