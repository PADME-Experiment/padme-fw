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
#include <algorithm>
using namespace std;


void MyCompare(std::string hname, double scalef=-1, double xmin=-999, double xmax=999, 
	       std::string xtitle="E(#gamma_{1})+E(#gamma_{2}) [MeV]", 
	       TFile* fMC1, TFile* fMC2, TFile* fMC3, TFile *fData, std::string ratio, Int_t lastSet)
{
 
  gStyle->SetOptTitle(0);
  std::cout<<" pointer to data file and mc file ... "<<fData<<" "<<fMC1<<"    processing histo "<<hname <<std::endl;
  
  TH1D* hData  = (TH1D*)fData->Get(hname.c_str());
  TH1D* hMC1   = (TH1D*)fMC1->Get(hname.c_str());
  TH1D* hMC2   = (TH1D*)fMC2->Get(hname.c_str());
  TH1D* hMC3   = (TH1D*)fMC3->Get(hname.c_str());
  
  TCanvas* cData = new TCanvas("data","data",500,500);
  hData->Draw();
  TCanvas* cMC = new TCanvas("mc","mc",500,500);
  hData->Draw();
  if(hname=="ECal_ClusterEnergy"||hname=="ECal_HitEnergy" || hname=="ECal_EHitTot" || hname=="ECal_EClTot"){
    cData->SetLogy();
    cMC->SetLogy();
  }
  
  if(hData->GetEntries()==0)return;
  if(hMC1->GetEntries()==0)return;
  if(hMC2->GetEntries()==0)return;
  if(hMC2->GetEntries()==0)return;

  //  return;

  if (scalef<0) 
    {
      // scale factor is not given; compute it 
      TH1D* hDN = (TH1D*)fData->Get("nPOT");
      TH1D* hMCN = (TH1D*)fMC1->Get("nPOT");
      double nPOTdata = hDN->GetBinContent(2);
      double nPOTMC = hMCN->GetBinContent(2);
      TH1D* hPOTdisMC=(TH1D*)fMC1->Get("NposInBunch_beam");
      //double sc = 20000./hPOTdisMC->GetMean();
      /* from ideal MC, 20k POT gaussian fit of target nPOT/bunch
	 Mean         1.76871e+04   
	 Sigma        1.82364e+02
      */
      double sc =20000./17687.1; //1.18;//23000./hPOTdisMC->GetMean();
      TH1D* hPOTdisDT=(TH1D*)fData->Get("NposInBunch_beam"); 
      std::cout<<"Total num. of POTs is data = "<<nPOTdata<<std::endl;
      std::cout<<"Total num. of POTs is MC   = "<<nPOTMC  <<std::endl;
      std::cout<<"Average nPOT/bunch in data = "<<hPOTdisDT->GetMean()<<std::endl;
      std::cout<<"Average nPOT/bunch in MC   = "<<hPOTdisMC->GetMean()<<std::endl; 
      nPOTMC =nPOTMC*sc;
      scalef = nPOTdata/nPOTMC;
      std::cout<<"Total CORR num. of POTs is MC   = "<<nPOTMC<<std::endl;
      // scalef = nPOTMC;//nPOTdata/( hMCN->GetBinContent(2));
      std::cout<<"Scaling factor for MC is "<<scalef<<std::endl;
    }

  TH1D* hsMC1 = hMC1->Clone();
  hsMC1->Scale(scalef);
  double ymaxMC1 = hsMC1->GetMaximum();
  
  TH1D* hsMC2 = hMC2->Clone();
  hsMC2->Scale(scalef);
  double ymaxMC2 = hsMC2->GetMaximum();
  
  TH1D* hsMC3 = hMC3->Clone();
  hsMC3->Scale(scalef);
  double ymaxMC3 = hsMC3->GetMaximum();
  
  double ymaxDT = hData->GetMaximum();
  double ymax  = ymaxDT;
  double ymax1 = ymaxMC1;
  double ymax2 = ymaxMC2;
  double ymax3 = ymaxMC3;
 
  if (ymax < ymax1) ymax=ymax1;
  if (ymax < ymax2) ymax=ymax2;
  if (ymax < ymax3) ymax=ymax3;
  ymax = 1.3*ymax;

  TH1D* hFrame = hMC1->Clone();
  hFrame->Scale(0.);
  hFrame->SetMaximum(ymax);


  hFrame->Draw();
  hsMC1->SetLineColor(kRed);
  hsMC1->Draw("SAME");
  hsMC2->SetLineColor(kBlue);
  hsMC2->Draw("SAME");
  hsMC3->SetLineColor(kYellow);
  hsMC3->Draw("SAME");
  hData->SetMarkerStyle(20);
  //hData->SetMarkerSize(0.5);
  hData->SetMarkerColor(kBlack);
  hData->Draw("SAMEPE");
  cout<<"Data/MC ratio = "<<hData->GetEntries()/hMC1->GetEntries()<<std::endl;
  cout<<"Data/sMC ratio = "<<hData->GetEntries()/(scalef/**hMC->GetEntries()*/)<<std::endl;

//return;

  TH1D* hratio1 = hMC1->Clone();
  TH1D* hratio2 = hMC2->Clone();
  TH1D* hratio3 = hMC3->Clone();
  hratio1->Scale(0.);
  hratio2->Scale(0.);
  hratio3->Scale(0.);
  int nbinx = hratio1->GetNbinsX();
  double rat1, erat1;
  double rat2, erat2;
  double rat3, erat3;
  for (int i=1; i<nbinx+1; ++i)
    {
      rat1 =0.;
      erat1 =0.;
      rat2 =0.;
      erat2 =0.;
      rat2 =0.;
      erat2 =0.;
      if (fabs(hsMC1->GetBinContent(i))>1e-10) {
	rat1 = hData->GetBinContent(i)/hsMC1->GetBinContent(i);
	erat1 = sqrt(hData->GetBinContent(i)+scalef*scalef*hMC1->GetBinContent(i))/hsMC1->GetBinContent(i);
      }
      hratio1->SetBinContent(i, rat1); 
      hratio1->SetBinError(i, erat1); 

      if (fabs(hsMC2->GetBinContent(i))>1e-10) {
	rat2 = hData->GetBinContent(i)/hsMC2->GetBinContent(i);
	erat2 = sqrt(hData->GetBinContent(i)+scalef*scalef*hMC2->GetBinContent(i))/hsMC2->GetBinContent(i);
      }
      hratio2->SetBinContent(i, rat2); 
      hratio2->SetBinError(i, erat2); 

      if (fabs(hsMC3->GetBinContent(i))>1e-10) {
	rat3 = hData->GetBinContent(i)/hsMC3->GetBinContent(i);
	erat3 = sqrt(hData->GetBinContent(i)+scalef*scalef*hMC3->GetBinContent(i))/hsMC3->GetBinContent(i);
      }
      hratio3->SetBinContent(i, rat3); 
      hratio3->SetBinError(i, erat3); 
      //std::cout<<"Bin "<<i<<"  ratio = "<<hratio->GetBinContent(i)<<std::endl;
      //std::cout<<"Bin "<<i<<" eratio = "<<hratio->GetBinError(i)<<std::endl;
    }
  TCanvas* c2 = new TCanvas("ratio","ratio",600,300);
  c2->Divide(1,3);
  c2->cd(1);
  hratio1->Draw();
  c2->cd(2);
  hratio2->Draw();
  c2->cd(3);
  hratio3->Draw();

   // Define the Canvas
  TCanvas *c = new TCanvas("c", "My Final Plot", 50, 50, 600,600);
 
   // Upper plot will be in pad1
   TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.);
   pad1->SetBottomMargin(0.02); // Upper and lower plot are joined
   pad1->SetTopMargin(1); // Upper and lower plot are joined
   if(hname=="ECal_ClusterEnergy"||hname=="ECal_HitEnergy" || hname=="ECal_EHitTot" || hname=="ECal_EClTot"){
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
   ymin = hMC1->GetYaxis()->GetXmin();
   if (pad1->GetLogy()) {
     ymin = 1.;
     ymax = 10*ymax;
   }
   
   if (fabs(xmin+999)<0.0001 && fabs(xmax-999)<0.0001) {
     xmin = hMC1->GetXaxis()->GetXmin();
     xmax = hMC1->GetXaxis()->GetXmax();
   }
   //   Double_t xmin=60.00;  Double_t xmax=3500.;
   TH1F *hFrame1 = pad1->DrawFrame(xmin,ymin,xmax,ymax);
   std::cout<<"X range = "<<xmin<<" "<<xmax<<std::endl;
   std::cout<<"Y range = "<<ymin<<" "<<ymax<<std::endl;
   
   hFrame1->GetXaxis()->SetLabelSize(0.);
   hFrame1->SetStats(0);  
   hsMC1->SetStats(0);
   hsMC2->SetStats(0);
   hsMC3->SetStats(0);  
   hData->SetStats(0);  
   //hFrame1->Draw("SAME");
   //return;

   hsMC1->Draw("SAME");
   hsMC2->Draw("SAME");
   hsMC3->Draw("SAME");
   hData->Draw("SAMEPE");



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
   if(hname=="ECal_ClusterEnergy"||hname=="ECal_HitEnergy" || hname=="ECal_EHitTot" || hname=="ECal_EClTot"){
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
   hratio1->GetXaxis()->SetTitle("Y strip number");
   // pad2->SetLogy();
   TH1F *hFrameRat = pad2->DrawFrame(xmin,-1,xmax,12.);
   hFrameRat->GetXaxis()->SetTitle(xtitle.c_str());
   hFrameRat->Draw();
   hratio1->SetStats(0);  
   hratio1->SetMaximum(12.);
   hratio1->SetMinimum(-1);
   //hratio->SetMinimum(0.1);
   hratio1->SetMarkerStyle(20);
   //hratio->SetMarkerSize(0.5);
   hratio1->SetLineColor(kRed);
   hratio1->SetMarkerSize(0.5);
   hratio1->SetMarkerColor(kRed);
   hratio1->Draw("epsame");
   hratio2->SetLineColor(kBlue);
   hratio2->SetMarkerSize(0.5);
   hratio2->SetMarkerColor(kBlue);
   hratio2->Draw("epsame");
   hratio3->SetMarkerColor(kYellow);
   hratio3->SetMarkerSize(0.5);
   hratio3->SetLineColor(kYellow);
   hratio3->Draw("epsame");

   
   
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
   hratio1->SetTitle("");

   
   // Y axis ratio plot settings
   hFrameRat->GetYaxis()->SetTitle("Data/MC");
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
   string newName = hname+ ratio;
   const char *a=newName.c_str();
   myText( 0.25,0.91,1,a, 0.039); 
  //   ATLASLabel(0.2,0.8,"Work in progress");//,"Preliminary");
   //   myText(       0.2, 0.7 , 1, "nPOT=9.3#times10^{9}");
   std::ostringstream s;
   s<<"nPOT=" << nPOTdata;
   std::string snpot= s.str();
   const char *b=snpot.c_str();
   myText(       0.2, 0.7 , 1, b);

   // myText(       0.63, 0.7 , 1, "Diamond Target");
   // myText(       0.57, 0.8, 1, "Average beam Y profile");

   //myMarkerText( 0.25, 0.5, 1, 20, "Data July 2019",1.3);
   //myBoxText(    0.25, 0.45, 0.05, kBlack, "MC");
   TLegend* t = new TLegend(0.55,0.7,0.80,0.85);
   t->AddEntry(hData,"data July 2019","ep");
   //t->AddEntry(hData,"MCfr 23k e^{+}/bunch (SH+Ecorr+VCon)","ep");
   //t->AddEntry(hsMC,"MCle 20k e^{+}/bunch (SH+Ecorr+VCon)","l");
   t->AddEntry(hsMC1,"MCfr 23k e^{+}/bunch on target, int","l");
   t->AddEntry(hsMC2,"MCfr 23k e^{+}/bunch on target, out","l");
   t->AddEntry(hsMC3,"MCfr 23k e^{+}/bunch on target, int+out","l");
   t->SetTextAlign(12);
   t->SetBorderSize(0);
   t->SetFillStyle(0);
   t->SetFillColor(0);
   t->SetTextFont(43);
   t->SetTextSize(15);

   t->Draw();
   std::cout<<"ok fin qui " << hname.c_str()<<std::endl;
   c->SaveAs(("outpng/"+hname+".png").c_str());
   c->Print("AnalysisIneffHisto_EthrSup.pdf(");
   //std::cout<<"is the last " << lastSet<< std::endl;
   if(lastSet/* && hname=="ECAL_Rgg_3ns1CoG20DegreeDeltaThetaEThr90""ECAL_Egg_3ns1CoG20DegreeDeltaThetaUnderPeak"*/){std::cout<<"End of job" << std::endl; c->Print("AnalysisIneffHisto_EthrSup.pdf)");}
 
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


//for multeplicity studies

void MyCompare(std::string hname, double scalef=-1, double xmin=-999, double xmax=999, 
	       std::string xtitle="E(#gamma_{1})+E(#gamma_{2}) [MeV]", 
	       TFile* fT5, TFile* fT10, TFile* fT15, TFile *fT20, TFile *fT25, TFile* fnoT5, TFile* fnoT10, TFile* fnoT15, TFile *fnoT20, TFile *fnoT25, Int_t lastSet)
{
 
  gStyle->SetOptTitle(0);
  double scalef5=-1;
  double scalef10=-1;
  double scalef15=-1;
  double scalef20=-1;
  double scalef25=-1;
  double scalefno5=-1;
  double scalefno10=-1;
  double scalefno15=-1;
  double scalefno20=-1;
  double scalefno25=-1;
  
  TH1D* hT5    = (TH1D*)fT5->Get(hname.c_str());
  TH1D* hT10   = (TH1D*)fT10->Get(hname.c_str());
  TH1D* hT15   = (TH1D*)fT15->Get(hname.c_str());
  TH1D* hT20   = (TH1D*)fT20->Get(hname.c_str());
  TH1D* hT25   = (TH1D*)fT25->Get(hname.c_str());
  TH1D* hnoT5    = (TH1D*)fnoT5->Get(hname.c_str());
  TH1D* hnoT10   = (TH1D*)fnoT10->Get(hname.c_str());
  TH1D* hnoT15   = (TH1D*)fnoT15->Get(hname.c_str());
  TH1D* hnoT20   = (TH1D*)fnoT20->Get(hname.c_str());
  TH1D* hnoT25   = (TH1D*)fnoT25->Get(hname.c_str());
  
  /*
  TCanvas* cData = new TCanvas("data","data",500,500);
  hData->Draw();
  TCanvas* cMC = new TCanvas("mc","mc",500,500);
  hData->Draw();
  if(hname=="ECal_ClusterEnergy"||hname=="ECal_HitEnergy"){
    cData->SetLogy();
    cMC->SetLogy();
  }*/
  
  if(hT5->GetEntries()==0)return;
  if(hT10->GetEntries()==0)return;
  if(hT15->GetEntries()==0)return;
  if(hT20->GetEntries()==0)return;
  if(hT25->GetEntries()==0)return;

  //  return;

  if (scalef<0) 
    {
      Bool_t NormUsingDTime  = false;
      Bool_t NormUsingEnergy = false;
      Bool_t NormUsingCorrnPOT = true;
      //i'll normalise the #POT of run witn 20kPOT/bunch
      if(NormUsingDTime){
      double nPOTfromTarget = ((TH1D*)fT20->Get("nPOT"))->GetBinContent(2);
      //I'm using the delta time background to estimate the normalization factor
      TH1D * DTime5 = (TH1D*)fT5->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_5=DTime5->FindBin(-10);
      Double_t bin2_5=DTime5->FindBin(-6);
      Double_t bin4_5=DTime5->FindBin(10);
      Double_t bin3_5=DTime5->FindBin(6);
      Double_t IntegralLeft5=DTime5->Integral(bin1_5,bin2_5);
      Double_t IntegralRight5=DTime5->Integral(bin3_5,bin4_5);
      
      TH1D * DTime10 = (TH1D*)fT10->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_10=DTime10->FindBin(-10);
      Double_t bin2_10=DTime10->FindBin(-6);
      Double_t bin4_10=DTime10->FindBin(10);
      Double_t bin3_10=DTime10->FindBin(6);
      Double_t IntegralLeft10=DTime10->Integral(bin1_10,bin2_10);
      Double_t IntegralRight10=DTime10->Integral(bin3_10,bin4_10);
      
      TH1D * DTime15 = (TH1D*)fT15->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_15=DTime15->FindBin(-10);
      Double_t bin2_15=DTime15->FindBin(-6);
      Double_t bin4_15=DTime15->FindBin(10);
      Double_t bin3_15=DTime15->FindBin(6);
      Double_t IntegralLeft15=DTime15->Integral(bin1_15,bin2_15);
      Double_t IntegralRight15=DTime15->Integral(bin3_15,bin4_15);
      
      
      TH1D * DTime20 = (TH1D*)fT20->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_20=DTime20->FindBin(-10);
      Double_t bin2_20=DTime20->FindBin(-6);
      Double_t bin4_20=DTime20->FindBin(10);
      Double_t bin3_20=DTime20->FindBin(6);
      Double_t IntegralLeft20=DTime20->Integral(bin1_20,bin2_20);
      Double_t IntegralRight20=DTime20->Integral(bin3_20,bin4_20);
      
      
      TH1D * DTime25 = (TH1D*)fT25->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_25=DTime25->FindBin(-10);
      Double_t bin2_25=DTime25->FindBin(-6);
      Double_t bin4_25=DTime25->FindBin(10);
      Double_t bin3_25=DTime25->FindBin(6);
      Double_t IntegralLeft25=DTime25->Integral(bin1_25,bin2_25);
      Double_t IntegralRight25=DTime25->Integral(bin3_25,bin4_25);
      
      //NO TARGET
      
      TH1D * DTimeno5 = (TH1D*)fnoT5->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_no5=DTimeno5->FindBin(-10);
      Double_t bin2_no5=DTimeno5->FindBin(-6);
      Double_t bin4_no5=DTimeno5->FindBin(10);
      Double_t bin3_no5=DTimeno5->FindBin(6);
      Double_t IntegralLeftno5=DTimeno5->Integral(bin1_no5,bin2_no5);
      Double_t IntegralRightno5=DTimeno5->Integral(bin3_no5,bin4_no5);
      
      TH1D * DTimeno10 = (TH1D*)fnoT10->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_no10=DTimeno10->FindBin(-10);
      Double_t bin2_no10=DTimeno10->FindBin(-6);
      Double_t bin4_no10=DTimeno10->FindBin(10);
      Double_t bin3_no10=DTimeno10->FindBin(6);
      Double_t IntegralLeftno10=DTimeno10->Integral(bin1_no10,bin2_no10);
      Double_t IntegralRightno10=DTimeno10->Integral(bin3_no10,bin4_no10);
      
      TH1D * DTimeno15 = (TH1D*)fnoT15->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_no15=DTimeno15->FindBin(-10);
      Double_t bin2_no15=DTimeno15->FindBin(-6);
      Double_t bin4_no15=DTimeno15->FindBin(10);
      Double_t bin3_no15=DTimeno15->FindBin(6);
      Double_t IntegralLeftno15=DTimeno15->Integral(bin1_no15,bin2_no15);
      Double_t IntegralRightno15=DTimeno15->Integral(bin3_no15,bin4_no15);
      
      
      TH1D * DTimeno20 = (TH1D*)fnoT20->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_no20=DTimeno20->FindBin(-10);
      Double_t bin2_no20=DTimeno20->FindBin(-6);
      Double_t bin4_no20=DTimeno20->FindBin(10);
      Double_t bin3_no20=DTimeno20->FindBin(6);
      Double_t IntegralLeftno20=DTimeno20->Integral(bin1_no20,bin2_no20);
      Double_t IntegralRightno20=DTimeno20->Integral(bin3_no20,bin4_no20);
      
      
      TH1D * DTimeno25 = (TH1D*)fnoT25->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_no25=DTimeno25->FindBin(-10);
      Double_t bin2_no25=DTimeno25->FindBin(-6);
      Double_t bin4_no25=DTimeno25->FindBin(10);
      Double_t bin3_no25=DTimeno25->FindBin(6);
      Double_t IntegralLeftno25=DTimeno25->Integral(bin1_no5,bin2_no5);
      Double_t IntegralRightno25=DTimeno25->Integral(bin3_no5,bin4_no5);

      scalef=IntegralLeft20/IntegralLeft20;
      scalef5=IntegralLeft20/IntegralLeft5;
      scalef10=IntegralLeft20/IntegralLeft10;
      scalef15=IntegralLeft20/IntegralLeft15;
      scalef20=IntegralLeft20/IntegralLeft20;
      scalef25=IntegralLeft20/IntegralLeft25;
      
      scalefno5=IntegralLeft20/IntegralLeftno5;
      scalefno10=IntegralLeft20/IntegralLeftno10;
      scalefno15=IntegralLeft20/IntegralLeftno15;
      scalefno20=IntegralLeft20/IntegralLeftno20;
      scalefno25=IntegralLeft20/IntegralLeftno25;

      } 

      if(NormUsingEnergy){
      double nPOTfromTarget = ((TH1D*)fT20->Get("nPOT"))->GetBinContent(2);
      //I'm using the delta time background to estimate the normalization factor
      TH1D * DTime5 = (TH1D*)fT5->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_5=DTime5->FindBin(300);
      Double_t bin2_5=DTime5->FindBin(370);
      Double_t IntegralLeft5=DTime5->Integral(bin1_5,bin2_5);
      
      TH1D * DTime10 = (TH1D*)fT10->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_10=DTime10->FindBin(300);
      Double_t bin2_10=DTime10->FindBin(370);
      Double_t IntegralLeft10=DTime10->Integral(bin1_10,bin2_10);
      
      TH1D * DTime15 = (TH1D*)fT15->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_15=DTime15->FindBin(300);
      Double_t bin2_15=DTime15->FindBin(370);
      Double_t IntegralLeft15=DTime15->Integral(bin1_15,bin2_15);
      
      
      TH1D * DTime20 = (TH1D*)fT20->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_20=DTime20->FindBin(300);
      Double_t bin2_20=DTime20->FindBin(370);
      Double_t IntegralLeft20=DTime20->Integral(bin1_20,bin2_20);
      
      
      TH1D * DTime25 = (TH1D*)fT25->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_25=DTime25->FindBin(300);
      Double_t bin2_25=DTime25->FindBin(370);
      Double_t IntegralLeft25=DTime25->Integral(bin1_25,bin2_25);
      
      //NO TARGET
      
      TH1D * DTimeno5 = (TH1D*)fnoT5->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_no5=DTimeno5->FindBin(300);
      Double_t bin2_no5=DTimeno5->FindBin(370);
      Double_t IntegralLeftno5=DTimeno5->Integral(bin1_no5,bin2_no5);
      
      TH1D * DTimeno10 = (TH1D*)fnoT10->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_no10=DTimeno10->FindBin(300);
      Double_t bin2_no10=DTimeno10->FindBin(370);
      Double_t IntegralLeftno10=DTimeno10->Integral(bin1_no10,bin2_no10);
      
      TH1D * DTimeno15 = (TH1D*)fnoT15->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_no15=DTimeno15->FindBin(300);
      Double_t bin2_no15=DTimeno15->FindBin(370);
      Double_t IntegralLeftno15=DTimeno15->Integral(bin1_no15,bin2_no15);
      
      TH1D * DTimeno20 = (TH1D*)fnoT20->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_no20=DTimeno20->FindBin(300);
      Double_t bin2_no20=DTimeno20->FindBin(370);
      Double_t IntegralLeftno20=DTimeno20->Integral(bin1_no20,bin2_no20);
      
      TH1D * DTimeno25 = (TH1D*)fnoT25->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_no25=DTimeno25->FindBin(300);
      Double_t bin2_no25=DTimeno25->FindBin(370);
      Double_t IntegralLeftno25=DTimeno25->Integral(bin1_no5,bin2_no5);


      scalef=IntegralLeft20/IntegralLeft20;
      scalef5=IntegralLeft20/IntegralLeft5;
      scalef10=IntegralLeft20/IntegralLeft10;
      scalef15=IntegralLeft20/IntegralLeft15;
      scalef20=IntegralLeft20/IntegralLeft20;
      scalef25=IntegralLeft20/IntegralLeft25;
      
      scalefno5=IntegralLeft20/IntegralLeftno5;
      scalefno10=IntegralLeft20/IntegralLeftno10;
      scalefno15=IntegralLeft20/IntegralLeftno15;
      scalefno20=IntegralLeft20/IntegralLeftno20;
      scalefno25=IntegralLeft20/IntegralLeftno25;
      } 
      if(!NormUsingCorrnPOT){
	std::cout << "Normalisation at : "<<IntegralLeft20 << std::endl;
	std::cout<<"5kPOT/bunch ... target : " <<IntegralLeft5 << "( scale factor "<< scalef5 << ") no Target : " <<  IntegralLeftno5<< " (scalef " << scalefno5 <<" )" << std::endl;
	std::cout<<"10kPOT/bunch ... target : " <<IntegralLeft10 << "( scale factor "<< scalef10 << ") no Target : " <<  IntegralLeftno10<< " (scalef " << scalefno10 <<" )" << std::endl;
	std::cout<<"15kPOT/bunch ... target : " <<IntegralLeft15 << "( scale factor "<< scalef15 << ") no Target : " <<  IntegralLeftno15<< " (scalef " << scalefno15 <<" )" << std::endl;
	std::cout<<"20kPOT/bunch ... target : " <<IntegralLeft20 << "( scale factor "<< scalef20 << ") no Target : " <<  IntegralLeftno20<< " (scalef " << scalefno20 <<" )" << std::endl;
	std::cout<<"25kPOT/bunch ... target : " <<IntegralLeft25 << "( scale factor "<< scalef25 << ") no Target : " <<  IntegralLeftno25<< " (scalef " << scalefno25 <<" )" << std::endl;
	//std::cout<<" data left " << IntegralLeft << " right " << IntegralRight << " ....mc left " << IntegralLeftmc << " right " << IntegralRightmc << " ratio data/mc left " << IntegralLeft/IntegralLeftmc << std::endl;
      }
      if(NormUsingCorrnPOT){
	double nPOTfromTarget = ((TH1D*)fT15->Get("nPOT"))->GetBinContent(2);
	Double_t scalenPOT20= corrNPOT(fT20, 20000);
	Double_t scalenPOT25= corrNPOT(fT20, 25000);
	double nPOT5 = ((TH1D*)fT5->Get("nPOT"))->GetBinContent(2);
	double nPOT10 = ((TH1D*)fT10->Get("nPOT"))->GetBinContent(2);
	double nPOT15 = ((TH1D*)fT15->Get("nPOT"))->GetBinContent(2);
	double nPOT20 = ((TH1D*)fT20->Get("nPOT"))->GetBinContent(2);
	double nPOT25 = ((TH1D*)fT25->Get("nPOT"))->GetBinContent(2);
	scalef5=nPOT15/nPOT5;
	scalef10=nPOT15/nPOT10;
	scalef15=nPOT15/nPOT15;
	scalef20=nPOT15/(nPOT20*scalenPOT20);
	scalef25=nPOT15/(nPOT25*scalenPOT25);
	scalefno5=TargetNoTargetScaleFactor(fT5,fnoT5,scalef5);
	scalefno10=TargetNoTargetScaleFactor(fT10,fnoT10,scalef10);
	//std::cout<<"normalising ... scale 5 " << scalef5 << " no taregt " << scalefno5 << std::endl;
	//scalefno5=TargetNoTargetScaleFactor(fT5,fnoT5)*scalef5;
	//scalefno10=TargetNoTargetScaleFactor(fT10,fnoT10)*scalef10;
	scalefno15=TargetNoTargetScaleFactor(fT15,fnoT15,scalef15);
	scalefno20=TargetNoTargetScaleFactor(fT20,fnoT20,scalef20);
	scalefno25=TargetNoTargetScaleFactor(fT25,fnoT25,scalef25);
	std::cout<<"npot target "<< nPOT5  <<" scale 5 "  << scalef5  << " no taregt " << scalefno5  << std::endl; 
	std::cout<<"npot target "<< nPOT10 <<" scale 10 " << scalef10 << " no taregt " << scalefno10 << std::endl; 
	std::cout<<"npot target "<< nPOT15 <<" scale 15 " << scalef15 << " no taregt " << scalefno15 << std::endl; 
	std::cout<<"npot target "<< nPOT20 <<" scale 20 " << scalef20 << " no taregt " << scalefno20 << std::endl; 
	std::cout<<"npot target "<< nPOT25 <<" scale 25 " << scalef25 << " no taregt " << scalefno25 << std::endl; 
      }
    }
  // std::cout<<"scalefnoT 5 " << scalefno5 << std::endl;
  std::vector<double> ymax;

  TH1D* hsT5 = hT5->Clone();
  hsT5->Scale(scalef5);
  ymax.push_back(hsT5->GetMaximum());
  
  TH1D* hsnoT5 = hnoT5->Clone();
  hsnoT5->Scale(scalefno5);
  ymax.push_back(hsnoT5->GetMaximum());
  
  TH1D* hsT10 = hT10->Clone();
  hsT10->Scale(scalef10);
  ymax.push_back(hsT10->GetMaximum());
  
  TH1D* hsnoT10 = hnoT10->Clone();
  hsnoT10->Scale(scalefno10);
  ymax.push_back(hsnoT10->GetMaximum());
  
  TH1D* hsT15 = hT15->Clone();
  hsT15->Scale(scalef15);
  ymax.push_back(hsT15->GetMaximum());
  
  TH1D* hsnoT15 = hnoT15->Clone();
  hsnoT15->Scale(scalefno15);
  ymax.push_back(hsnoT15->GetMaximum());
  
  TH1D* hsT20 = hT20->Clone();
  hsT20->Scale(scalef20);
  ymax.push_back(hsT20->GetMaximum());
  
  TH1D* hsnoT20 = hnoT20->Clone();
  hsnoT20->Scale(scalefno20);
  ymax.push_back(hsnoT20->GetMaximum());
  
  TH1D* hsT25 = hT25->Clone();
  hsT25->Scale(scalef25);
  ymax.push_back(hsT25->GetMaximum());
  
  TH1D* hsnoT25 = hnoT25->Clone();
  hsnoT25->Scale(scalefno25);
  ymax.push_back(hsnoT25->GetMaximum());
  Double_t maxVec= *std::max_element(ymax.begin(),ymax.end());
  Double_t ymaxD=1.3*maxVec;
  
  /*
  double ymax  = ymaxDT;
  double ymax1 = ymaxMC1;
  double ymax2 = ymaxMC2;
  double ymax3 = ymaxMC3;
 
  if (ymax < ymax1) ymax=ymax1;
  if (ymax < ymax2) ymax=ymax2;
  if (ymax < ymax3) ymax=ymax3;
  ymax = 1.3*ymax;
*/
  TH1D* hFrame = hT5->Clone();
  hFrame->Scale(0.);
  hFrame->SetMaximum(ymaxD);

  hFrame->Draw();
  hsT5->SetMarkerColor(kRed);
  hsT5->SetLineColor(kRed);
  hsT5->SetMarkerStyle(20);
  hsT5->SetMarkerSize(0.5);
  hsT5->Draw("SAMEPE");
  hsT10->SetMarkerColor(kBlue);
  hsT10->SetMarkerStyle(20);
  hsT10->SetMarkerSize(0.5);
  hsT10->Draw("SAMEPE");
  hsT15->SetMarkerColor(kYellow);
  hsT15->SetMarkerStyle(20);
  hsT15->SetMarkerSize(0.5);
  hsT15->Draw("SAMEPE");
  hsT20->SetMarkerStyle(20);
  //hData->SetMarkerSize(0.5);
  hsT20->SetMarkerColor(kBlack);
  hsT20->SetMarkerSize(0.5);
  hsT20->Draw("SAMEPE");
  hsT25->SetMarkerColor(kGreen);
  hsT25->SetMarkerStyle(20);
  hsT25->SetMarkerSize(0.5);
  hsT25->Draw("SAMEPE");
  
  
  hsnoT5->SetLineColor(kRed);
  hsnoT5->Draw("SAME");
  hsnoT10->SetLineColor(kBlue);
  hsnoT10->Draw("SAME");
  hsnoT15->SetLineColor(kYellow);
  hsnoT15->Draw("SAME");
  hsnoT20->SetLineColor(kBlack);
  hsnoT20->Draw("SAME");
  hsnoT25->SetLineColor(kGreen);
  hsnoT25->Draw("SAME");
  
  
  //cout<<"Data/MC ratio = "<<hData->GetEntries()/hMC1->GetEntries()<<std::endl;
  //cout<<"Data/sMC ratio = "<<hData->GetEntries()/(scalef/**hMC->GetEntries()*/)<<std::endl;

//return;

  TH1D* hratio1 = hT5->Clone();
  TH1D* hratio2 = hT10->Clone();
  TH1D* hratio3 = hT15->Clone();
  TH1D* hratio4 = hT20->Clone();
  TH1D* hratio5 = hT25->Clone();
  hratio1->Scale(0.);
  hratio2->Scale(0.);
  hratio3->Scale(0.);
  hratio4->Scale(0.);
  hratio5->Scale(0.);
  int nbinx = hratio1->GetNbinsX();
  double rat1, erat1;
  double rat2, erat2;
  double rat3, erat3;
  double rat4, erat4;
  double rat5, erat5;
  for (int i=1; i<nbinx+1; ++i)
    {
      rat1 =0.;
      erat1 =0.;
      rat2 =0.;
      erat2 =0.;
      rat3 =0.;
      erat3 =0.;
      rat4 =0.;
      erat4 =0.;
      rat5 =0.;
      erat5 =0.;
      if (fabs(hsnoT5->GetBinContent(i))>1e-10) {
	rat1 = hsT5->GetBinContent(i)/hsnoT5->GetBinContent(i);
	erat1 = sqrt(hsT5->GetBinContent(i)+hsnoT5->GetBinContent(i))/hsnoT5->GetBinContent(i);
      }
      hratio1->SetBinContent(i, rat1); 
      hratio1->SetBinError(i, erat1); 

      if (fabs(hsnoT10->GetBinContent(i))>1e-10) {
	rat2 = hsT10->GetBinContent(i)/hsnoT10->GetBinContent(i);
	erat2 = sqrt(hsT10->GetBinContent(i)+hsnoT10->GetBinContent(i))/hsnoT10->GetBinContent(i);
      }
      hratio2->SetBinContent(i, rat2); 
      hratio2->SetBinError(i, erat2); 

      if (fabs(hsnoT15->GetBinContent(i))>1e-10) {
	rat3 = hsT15->GetBinContent(i)/hsnoT15->GetBinContent(i);
	erat3 = sqrt(hsT15->GetBinContent(i)+hsnoT15->GetBinContent(i))/hsnoT15->GetBinContent(i);
      }
      hratio3->SetBinContent(i, rat3); 
      hratio3->SetBinError(i, erat3); 
      //std::cout<<"Bin "<<i<<"  ratio = "<<hratio->GetBinContent(i)<<std::endl;
      //std::cout<<"Bin "<<i<<" eratio = "<<hratio->GetBinError(i)<<std::endl;
      
      if (fabs(hsnoT20->GetBinContent(i))>1e-10) {
	rat4 = hsT20->GetBinContent(i)/hsnoT20->GetBinContent(i);
	erat4 = sqrt(hsT20->GetBinContent(i)+hsnoT20->GetBinContent(i))/hsnoT20->GetBinContent(i);
      }
      hratio4->SetBinContent(i, rat4); 
      hratio4->SetBinError(i, erat4); 

      if (fabs(hsnoT25->GetBinContent(i))>1e-10) {
	rat5 = hsT25->GetBinContent(i)/hsnoT25->GetBinContent(i);
	erat5 = sqrt(hsT25->GetBinContent(i)+hsnoT25->GetBinContent(i))/hsnoT25->GetBinContent(i);
      }
      hratio5->SetBinContent(i, rat5); 
      hratio5->SetBinError(i, erat5); 
    }
  TCanvas* c2 = new TCanvas("ratio","ratio",600,300);
  c2->Divide(2,3);
  c2->cd(1);
  hratio1->Draw();
  c2->cd(2);
  hratio2->Draw();
  c2->cd(3);
  hratio3->Draw();
  c2->cd(4);
  hratio4->Draw();
  c2->cd(5);
  hratio5->Draw();

   // Define the Canvas
  TCanvas *c = new TCanvas("c", "My Final Plot", 50, 50, 600,600);
 
   // Upper plot will be in pad1
   TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.);
   pad1->SetBottomMargin(0.02); // Upper and lower plot are joined
   pad1->SetTopMargin(1); // Upper and lower plot are joined
   if(hname=="ECal_ClusterEnergy"||hname=="ECal_HitEnergy" || hname=="ECal_EHitTot" || hname=="ECal_EClTot"){
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
   ymin = hT5->GetYaxis()->GetXmin();
   if (pad1->GetLogy()) {
     ymin = 1.;
     ymaxD = 10*ymaxD;
   }
   
   if (fabs(xmin+999)<0.0001 && fabs(xmax-999)<0.0001) {
     xmin = hT5->GetXaxis()->GetXmin();
     xmax = hT5->GetXaxis()->GetXmax();
   }
   //   Double_t xmin=60.00;  Double_t xmax=3500.;
   //TH1F *hFrame1 = pad1->DrawFrame(xmin,ymin,xmax,ymax);
   TH1F *hFrame1 = pad1->DrawFrame(xmin,ymin,xmax,ymaxD);
   std::cout<<"X range = "<<xmin<<" "<<xmax<<std::endl;
   std::cout<<"Y range = "<<ymin<<" "<<ymaxD<<std::endl;
   
   hFrame1->GetXaxis()->SetLabelSize(0.);
   hFrame1->SetStats(0);  
   hsT5->SetStats(0);
   hsT10->SetStats(0);
   hsT15->SetStats(0);  
   hsT20->SetStats(0); 
   hsT25->SetStats(0);   
   hsnoT5->SetStats(0);
   hsnoT10->SetStats(0);
   hsnoT15->SetStats(0);  
   hsnoT20->SetStats(0); 
   hsnoT25->SetStats(0);   
   //hFrame1->Draw("SAME");
   //return;

   hsT5->Draw("SAMEPE");
   hsT10->Draw("SAMEPE");
   hsT15->Draw("SAMEPE");
   hsT20->Draw("SAMEPE");
   hsT25->Draw("SAMEPE");
   hsnoT5->Draw("SAME");
   hsnoT10->Draw("SAME");
   hsnoT15->Draw("SAME");
   hsnoT20->Draw("SAME");
   hsnoT25->Draw("SAME");


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
   if(hname=="ECal_ClusterEnergy"||hname=="ECal_HitEnergy" || hname=="ECal_EHitTot" || hname=="ECal_EClTot"){
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
   hratio1->GetXaxis()->SetTitle("Y strip number");
   pad2->SetLogy();
   TH1F *hFrameRat = pad2->DrawFrame(xmin,0.1,xmax,12.);
   hFrameRat->GetXaxis()->SetTitle(xtitle.c_str());
   hFrameRat->Draw();
   hratio1->SetStats(0);  
   hratio1->SetMaximum(12.);
   //hratio1->SetMinimum(-1);
   hratio1->SetMinimum(0.1);
   hratio1->SetMarkerStyle(20);
   //hratio->SetMarkerSize(0.5);
   hratio1->SetLineColor(kRed);
   hratio1->SetMarkerSize(0.5);
   hratio1->SetMarkerColor(kRed);
   hratio1->Draw("epsame");
   hratio2->SetLineColor(kBlue);
   hratio2->SetMarkerSize(0.5);
   hratio2->SetMarkerColor(kBlue);
   hratio2->Draw("epsame");
   hratio3->SetMarkerColor(kYellow);
   hratio3->SetMarkerSize(0.5);
   hratio3->SetLineColor(kYellow);
   hratio3->Draw("epsame");
   hratio4->SetLineColor(kBlack);
   hratio4->SetMarkerSize(0.5);
   hratio4->SetMarkerColor(kBlack);
   hratio4->Draw("epsame");
   hratio5->SetMarkerColor(kGreen);
   hratio5->SetMarkerSize(0.5);
   hratio5->SetLineColor(kGreen);
   hratio5->Draw("epsame");

   
   
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
   hratio1->SetTitle("");

   
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
   string newName = hname;
   const char *a=newName.c_str();
   myText( 0.25,0.91,1,a, 0.039); 
  //   ATLASLabel(0.2,0.8,"Work in progress");//,"Preliminary");
   //   myText(       0.2, 0.7 , 1, "nPOT=9.3#times10^{9}");

   std::ostringstream s;
   s<<"nPOT=" << nPOTfromTarget;
   std::string snpot= s.str();
   const char *b=snpot.c_str();
   myText(       0.2, 0.7 , 1, b);


   // myText(       0.63, 0.7 , 1, "Diamond Target");
   // myText(       0.57, 0.8, 1, "Average beam Y profile");

   //myMarkerText( 0.25, 0.5, 1, 20, "Data July 2019",1.3);
   //myBoxText(    0.25, 0.45, 0.05, kBlack, "MC");
   TLegend* t = new TLegend(0.55,0.55,0.80,0.85);
   t->AddEntry(hsT5,"target 5k POT/bunch","ep");
   t->AddEntry(hsnoT5,"no target 5k POT/bunch","l");
   t->AddEntry(hsT10,"target 10k POT/bunch","ep");
   t->AddEntry(hsnoT10,"no target 10k POT/bunch","l");
   t->AddEntry(hsT15,"target 15k POT/bunch","ep");
   t->AddEntry(hsnoT15,"no target 15k POT/bunch","l");
   t->AddEntry(hsT20,"target 20k POT/bunch","ep");
   t->AddEntry(hsnoT20,"no target 20k POT/bunch","l");
   t->AddEntry(hsT25,"target 25k POT/bunch","ep");
   t->AddEntry(hsnoT25,"no target 25k POT/bunch","l");
   
   t->SetTextAlign(12);
   t->SetBorderSize(0);
   t->SetFillStyle(0);
   t->SetFillColor(0);
   t->SetTextFont(43);
   t->SetTextSize(15);

   t->Draw();

   std::cout<<"ok fin qui " << hname.c_str()<<std::endl;
   c->SaveAs(("outpng/"+hname+".png").c_str());
   c->Print("AnalysisMulteplicityStudies.pdf(");
   //std::cout<<"is the last " << lastSet<< std::endl;
   if(lastSet/* && hname=="ECAL_Rgg_3ns1CoG20DegreeDeltaThetaEThr90""ECAL_Egg_3ns1CoG20DegreeDeltaThetaUnderPeak"*/){std::cout<<"End of job" << std::endl; c->Print("AnalysisMulteplicityStudies.pdf)");}
 
   //c->SaveAs("myPlot.png");
   //c->SaveAs("myPlot.eps");
   //return;

   
   delete pad1;
   delete pad2;
  // delete cData;
  // delete cMC;
   delete c;
   delete c2;
   delete t;
   
   std::cout<<"Bye, bye ................."<<std::endl<<std::endl;
}





void MyCompare(std::string hname, double scalef=-1, double xmin=-999, double xmax=999, 
	       std::string xtitle="E(#gamma_{1})+E(#gamma_{2}) [MeV]", 
	       TFile* fT5, TFile* fT10, TFile* fT15, TFile *fT20, TFile *fT25, TFile* fnoT5, TFile* fnoT10, TFile* fnoT15, TFile *fnoT20, TFile *fnoT25, Int_t lastSet, double minInt, double maxInt)
{
 
  gStyle->SetOptTitle(0);
  double scalef5=-1;
  double scalef10=-1;
  double scalef15=-1;
  double scalef20=-1;
  double scalef25=-1;
  double scalefno5=-1;
  double scalefno10=-1;
  double scalefno15=-1;
  double scalefno20=-1;
  double scalefno25=-1;
  
  TH1D* hT5    = (TH1D*)fT5->Get(hname.c_str());
  TH1D* hT10   = (TH1D*)fT10->Get(hname.c_str());
  TH1D* hT15   = (TH1D*)fT15->Get(hname.c_str());
  TH1D* hT20   = (TH1D*)fT20->Get(hname.c_str());
  TH1D* hT25   = (TH1D*)fT25->Get(hname.c_str());
  TH1D* hnoT5    = (TH1D*)fnoT5->Get(hname.c_str());
  TH1D* hnoT10   = (TH1D*)fnoT10->Get(hname.c_str());
  TH1D* hnoT15   = (TH1D*)fnoT15->Get(hname.c_str());
  TH1D* hnoT20   = (TH1D*)fnoT20->Get(hname.c_str());
  TH1D* hnoT25   = (TH1D*)fnoT25->Get(hname.c_str());
  
  /*
  TCanvas* cData = new TCanvas("data","data",500,500);
  hData->Draw();
  TCanvas* cMC = new TCanvas("mc","mc",500,500);
  hData->Draw();
  if(hname=="ECal_ClusterEnergy"||hname=="ECal_HitEnergy"){
    cData->SetLogy();
    cMC->SetLogy();
  }*/
  
  if(hT5->GetEntries()==0)return;
  if(hT10->GetEntries()==0)return;
  if(hT15->GetEntries()==0)return;
  if(hT20->GetEntries()==0)return;
  if(hT25->GetEntries()==0)return;

  //  return;

  if (scalef<0) 
    {
      Bool_t NormUsingDTime  = false;
      Bool_t NormUsingEnergy = false;
      Bool_t NormUsingCorrnPOT = true;
      //i'll normalise the #POT of run witn 20kPOT/bunch
      if(NormUsingDTime){
      double nPOTfromTarget = ((TH1D*)fT20->Get("nPOT"))->GetBinContent(2);
      //I'm using the delta time background to estimate the normalization factor
      TH1D * DTime5 = (TH1D*)fT5->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_5=DTime5->FindBin(-10);
      Double_t bin2_5=DTime5->FindBin(-6);
      Double_t bin4_5=DTime5->FindBin(10);
      Double_t bin3_5=DTime5->FindBin(6);
      Double_t IntegralLeft5=DTime5->Integral(bin1_5,bin2_5);
      Double_t IntegralRight5=DTime5->Integral(bin3_5,bin4_5);
      
      TH1D * DTime10 = (TH1D*)fT10->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_10=DTime10->FindBin(-10);
      Double_t bin2_10=DTime10->FindBin(-6);
      Double_t bin4_10=DTime10->FindBin(10);
      Double_t bin3_10=DTime10->FindBin(6);
      Double_t IntegralLeft10=DTime10->Integral(bin1_10,bin2_10);
      Double_t IntegralRight10=DTime10->Integral(bin3_10,bin4_10);
      
      TH1D * DTime15 = (TH1D*)fT15->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_15=DTime15->FindBin(-10);
      Double_t bin2_15=DTime15->FindBin(-6);
      Double_t bin4_15=DTime15->FindBin(10);
      Double_t bin3_15=DTime15->FindBin(6);
      Double_t IntegralLeft15=DTime15->Integral(bin1_15,bin2_15);
      Double_t IntegralRight15=DTime15->Integral(bin3_15,bin4_15);
      
      
      TH1D * DTime20 = (TH1D*)fT20->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_20=DTime20->FindBin(-10);
      Double_t bin2_20=DTime20->FindBin(-6);
      Double_t bin4_20=DTime20->FindBin(10);
      Double_t bin3_20=DTime20->FindBin(6);
      Double_t IntegralLeft20=DTime20->Integral(bin1_20,bin2_20);
      Double_t IntegralRight20=DTime20->Integral(bin3_20,bin4_20);
      
      
      TH1D * DTime25 = (TH1D*)fT25->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_25=DTime25->FindBin(-10);
      Double_t bin2_25=DTime25->FindBin(-6);
      Double_t bin4_25=DTime25->FindBin(10);
      Double_t bin3_25=DTime25->FindBin(6);
      Double_t IntegralLeft25=DTime25->Integral(bin1_25,bin2_25);
      Double_t IntegralRight25=DTime25->Integral(bin3_25,bin4_25);
      
      //NO TARGET
      
      TH1D * DTimeno5 = (TH1D*)fnoT5->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_no5=DTimeno5->FindBin(-10);
      Double_t bin2_no5=DTimeno5->FindBin(-6);
      Double_t bin4_no5=DTimeno5->FindBin(10);
      Double_t bin3_no5=DTimeno5->FindBin(6);
      Double_t IntegralLeftno5=DTimeno5->Integral(bin1_no5,bin2_no5);
      Double_t IntegralRightno5=DTimeno5->Integral(bin3_no5,bin4_no5);
      
      TH1D * DTimeno10 = (TH1D*)fnoT10->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_no10=DTimeno10->FindBin(-10);
      Double_t bin2_no10=DTimeno10->FindBin(-6);
      Double_t bin4_no10=DTimeno10->FindBin(10);
      Double_t bin3_no10=DTimeno10->FindBin(6);
      Double_t IntegralLeftno10=DTimeno10->Integral(bin1_no10,bin2_no10);
      Double_t IntegralRightno10=DTimeno10->Integral(bin3_no10,bin4_no10);
      
      TH1D * DTimeno15 = (TH1D*)fnoT15->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_no15=DTimeno15->FindBin(-10);
      Double_t bin2_no15=DTimeno15->FindBin(-6);
      Double_t bin4_no15=DTimeno15->FindBin(10);
      Double_t bin3_no15=DTimeno15->FindBin(6);
      Double_t IntegralLeftno15=DTimeno15->Integral(bin1_no15,bin2_no15);
      Double_t IntegralRightno15=DTimeno15->Integral(bin3_no15,bin4_no15);
      
      
      TH1D * DTimeno20 = (TH1D*)fnoT20->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_no20=DTimeno20->FindBin(-10);
      Double_t bin2_no20=DTimeno20->FindBin(-6);
      Double_t bin4_no20=DTimeno20->FindBin(10);
      Double_t bin3_no20=DTimeno20->FindBin(6);
      Double_t IntegralLeftno20=DTimeno20->Integral(bin1_no20,bin2_no20);
      Double_t IntegralRightno20=DTimeno20->Integral(bin3_no20,bin4_no20);
      
      
      TH1D * DTimeno25 = (TH1D*)fnoT25->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
      Double_t bin1_no25=DTimeno25->FindBin(-10);
      Double_t bin2_no25=DTimeno25->FindBin(-6);
      Double_t bin4_no25=DTimeno25->FindBin(10);
      Double_t bin3_no25=DTimeno25->FindBin(6);
      Double_t IntegralLeftno25=DTimeno25->Integral(bin1_no5,bin2_no5);
      Double_t IntegralRightno25=DTimeno25->Integral(bin3_no5,bin4_no5);

      scalef=IntegralLeft20/IntegralLeft20;
      scalef5=IntegralLeft20/IntegralLeft5;
      scalef10=IntegralLeft20/IntegralLeft10;
      scalef15=IntegralLeft20/IntegralLeft15;
      scalef20=IntegralLeft20/IntegralLeft20;
      scalef25=IntegralLeft20/IntegralLeft25;
      
      scalefno5=IntegralLeft20/IntegralLeftno5;
      scalefno10=IntegralLeft20/IntegralLeftno10;
      scalefno15=IntegralLeft20/IntegralLeftno15;
      scalefno20=IntegralLeft20/IntegralLeftno20;
      scalefno25=IntegralLeft20/IntegralLeftno25;

      } 

      if(NormUsingEnergy){
      double nPOTfromTarget = ((TH1D*)fT20->Get("nPOT"))->GetBinContent(2);
      //I'm using the delta time background to estimate the normalization factor
      TH1D * DTime5 = (TH1D*)fT5->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_5=DTime5->FindBin(300);
      Double_t bin2_5=DTime5->FindBin(370);
      Double_t IntegralLeft5=DTime5->Integral(bin1_5,bin2_5);
      
      TH1D * DTime10 = (TH1D*)fT10->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_10=DTime10->FindBin(300);
      Double_t bin2_10=DTime10->FindBin(370);
      Double_t IntegralLeft10=DTime10->Integral(bin1_10,bin2_10);
      
      TH1D * DTime15 = (TH1D*)fT15->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_15=DTime15->FindBin(300);
      Double_t bin2_15=DTime15->FindBin(370);
      Double_t IntegralLeft15=DTime15->Integral(bin1_15,bin2_15);
      
      
      TH1D * DTime20 = (TH1D*)fT20->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_20=DTime20->FindBin(300);
      Double_t bin2_20=DTime20->FindBin(370);
      Double_t IntegralLeft20=DTime20->Integral(bin1_20,bin2_20);
      
      
      TH1D * DTime25 = (TH1D*)fT25->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_25=DTime25->FindBin(300);
      Double_t bin2_25=DTime25->FindBin(370);
      Double_t IntegralLeft25=DTime25->Integral(bin1_25,bin2_25);
      
      //NO TARGET
      
      TH1D * DTimeno5 = (TH1D*)fnoT5->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_no5=DTimeno5->FindBin(300);
      Double_t bin2_no5=DTimeno5->FindBin(370);
      Double_t IntegralLeftno5=DTimeno5->Integral(bin1_no5,bin2_no5);
      
      TH1D * DTimeno10 = (TH1D*)fnoT10->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_no10=DTimeno10->FindBin(300);
      Double_t bin2_no10=DTimeno10->FindBin(370);
      Double_t IntegralLeftno10=DTimeno10->Integral(bin1_no10,bin2_no10);
      
      TH1D * DTimeno15 = (TH1D*)fnoT15->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_no15=DTimeno15->FindBin(300);
      Double_t bin2_no15=DTimeno15->FindBin(370);
      Double_t IntegralLeftno15=DTimeno15->Integral(bin1_no15,bin2_no15);
      
      TH1D * DTimeno20 = (TH1D*)fnoT20->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_no20=DTimeno20->FindBin(300);
      Double_t bin2_no20=DTimeno20->FindBin(370);
      Double_t IntegralLeftno20=DTimeno20->Integral(bin1_no20,bin2_no20);
      
      TH1D * DTimeno25 = (TH1D*)fnoT25->Get("ECAL_twoPhotonInTime10nsUnderPeak");
      Double_t bin1_no25=DTimeno25->FindBin(300);
      Double_t bin2_no25=DTimeno25->FindBin(370);
      Double_t IntegralLeftno25=DTimeno25->Integral(bin1_no5,bin2_no5);


      scalef=IntegralLeft20/IntegralLeft20;
      scalef5=IntegralLeft20/IntegralLeft5;
      scalef10=IntegralLeft20/IntegralLeft10;
      scalef15=IntegralLeft20/IntegralLeft15;
      scalef20=IntegralLeft20/IntegralLeft20;
      scalef25=IntegralLeft20/IntegralLeft25;
      
      scalefno5=IntegralLeft20/IntegralLeftno5;
      scalefno10=IntegralLeft20/IntegralLeftno10;
      scalefno15=IntegralLeft20/IntegralLeftno15;
      scalefno20=IntegralLeft20/IntegralLeftno20;
      scalefno25=IntegralLeft20/IntegralLeftno25;
      } 
      if(!NormUsingCorrnPOT){
	std::cout << "Normalisation at : "<<IntegralLeft20 << std::endl;
	std::cout<<"5kPOT/bunch ... target : " <<IntegralLeft5 << "( scale factor "<< scalef5 << ") no Target : " <<  IntegralLeftno5<< " (scalef " << scalefno5 <<" )" << std::endl;
	std::cout<<"10kPOT/bunch ... target : " <<IntegralLeft10 << "( scale factor "<< scalef10 << ") no Target : " <<  IntegralLeftno10<< " (scalef " << scalefno10 <<" )" << std::endl;
	std::cout<<"15kPOT/bunch ... target : " <<IntegralLeft15 << "( scale factor "<< scalef15 << ") no Target : " <<  IntegralLeftno15<< " (scalef " << scalefno15 <<" )" << std::endl;
	std::cout<<"20kPOT/bunch ... target : " <<IntegralLeft20 << "( scale factor "<< scalef20 << ") no Target : " <<  IntegralLeftno20<< " (scalef " << scalefno20 <<" )" << std::endl;
	std::cout<<"25kPOT/bunch ... target : " <<IntegralLeft25 << "( scale factor "<< scalef25 << ") no Target : " <<  IntegralLeftno25<< " (scalef " << scalefno25 <<" )" << std::endl;
	//std::cout<<" data left " << IntegralLeft << " right " << IntegralRight << " ....mc left " << IntegralLeftmc << " right " << IntegralRightmc << " ratio data/mc left " << IntegralLeft/IntegralLeftmc << std::endl;
      }
      if(NormUsingCorrnPOT){
	double nPOTfromTarget = ((TH1D*)fT15->Get("nPOT"))->GetBinContent(2);
	Double_t scalenPOT20= corrNPOT(fT20, 20000);
	Double_t scalenPOT25= corrNPOT(fT20, 25000);
	double nPOT5 = ((TH1D*)fT5->Get("nPOT"))->GetBinContent(2);
	double nPOT10 = ((TH1D*)fT10->Get("nPOT"))->GetBinContent(2);
	double nPOT15 = ((TH1D*)fT15->Get("nPOT"))->GetBinContent(2);
	double nPOT20 = ((TH1D*)fT20->Get("nPOT"))->GetBinContent(2);
	double nPOT25 = ((TH1D*)fT25->Get("nPOT"))->GetBinContent(2);
	scalef5=nPOT15/nPOT5;
	scalef10=nPOT15/nPOT10;
	scalef15=nPOT15/nPOT15;
	scalef20=nPOT15/(nPOT20*scalenPOT20);
	scalef25=nPOT15/(nPOT25*scalenPOT25);
	scalefno5=TargetNoTargetScaleFactor(fT5,fnoT5,scalef5);
	scalefno10=TargetNoTargetScaleFactor(fT10,fnoT10,scalef10);
       	//scalefno5=TargetNoTargetScaleFactor(fT5,fnoT5)*scalef5;
	//std::cout<<"normalising ... scale 5 " << scalef5 << " no taregt " << scalefno5  << " TARGETNOTARGET " << TargetNoTargetScaleFactor(fT5,fnoT5) << std::endl;
	//scalefno10=TargetNoTargetScaleFactor(fT10,fnoT10)*scalef10;
	scalefno15=TargetNoTargetScaleFactor(fT15,fnoT15,scalef15);
	scalefno20=TargetNoTargetScaleFactor(fT20,fnoT20,scalef20);
	scalefno25=TargetNoTargetScaleFactor(fT25,fnoT25,scalef25);
	std::cout<<"npot target "<< nPOT5  <<" scale 5 "  << scalef5  << " no taregt " << scalefno5  << std::endl; 
	std::cout<<"npot target "<< nPOT10 <<" scale 10 " << scalef10 << " no taregt " << scalefno10 << std::endl; 
	std::cout<<"npot target "<< nPOT15 <<" scale 15 " << scalef15 << " no taregt " << scalefno15 << std::endl; 
	std::cout<<"npot target "<< nPOT20 <<" scale 20 " << scalef20 << " no taregt " << scalefno20 << std::endl; 
	std::cout<<"npot target "<< nPOT25 <<" scale 25 " << scalef25 << " no taregt " << scalefno25 << std::endl; 
      }
     
    }
  // std::cout<<"scalefnoT 5 " << scalefno5 << std::endl;
  std::vector<double> ymax;

  TH1D* hsT5 = hT5->Clone();
  hsT5->Scale(scalef5);
  ymax.push_back(hsT5->GetMaximum());
  
  TH1D* hsnoT5 = hnoT5->Clone();
  hsnoT5->Scale(scalefno5);
  ymax.push_back(hsnoT5->GetMaximum());

  TH1D* hsT10 = hT10->Clone();
  hsT10->Scale(scalef10);
  ymax.push_back(hsT10->GetMaximum());
  
  TH1D* hsnoT10 = hnoT10->Clone();
  hsnoT10->Scale(scalefno10);
  ymax.push_back(hsnoT10->GetMaximum());
  
  TH1D* hsT15 = hT15->Clone();
  hsT15->Scale(scalef15);
  ymax.push_back(hsT15->GetMaximum());
  
  TH1D* hsnoT15 = hnoT15->Clone();
  hsnoT15->Scale(scalefno15);
  ymax.push_back(hsnoT15->GetMaximum());
  
  TH1D* hsT20 = hT20->Clone();
  hsT20->Scale(scalef20);
  ymax.push_back(hsT20->GetMaximum());
  
  TH1D* hsnoT20 = hnoT20->Clone();
  hsnoT20->Scale(scalefno20);
  ymax.push_back(hsnoT20->GetMaximum());
  
  TH1D* hsT25 = hT25->Clone();
  hsT25->Scale(scalef25);
  ymax.push_back(hsT25->GetMaximum());
  
  TH1D* hsnoT25 = hnoT25->Clone();
  hsnoT25->Scale(scalefno25);
  ymax.push_back(hsnoT25->GetMaximum());
  Double_t maxVec= *std::max_element(ymax.begin(),ymax.end());
  Double_t ymaxD=1.3*maxVec;
  
  /*
  double ymax  = ymaxDT;
  double ymax1 = ymaxMC1;
  double ymax2 = ymaxMC2;
  double ymax3 = ymaxMC3;
 
  if (ymax < ymax1) ymax=ymax1;
  if (ymax < ymax2) ymax=ymax2;
  if (ymax < ymax3) ymax=ymax3;
  ymax = 1.3*ymax;
*/
  TH1D* hFrame = hT5->Clone();
  hFrame->Scale(0.);
  hFrame->SetMaximum(ymaxD);

  hFrame->Draw();
  hsT5->SetMarkerColor(kRed);
  hsT5->SetLineColor(kRed);
  hsT5->SetMarkerStyle(20);
  hsT5->SetMarkerSize(0.5);
  hsT5->Draw("SAMEPE");
  hsT10->SetMarkerColor(kBlue);
  hsT10->SetMarkerStyle(20);
  hsT10->SetMarkerSize(0.5);
  hsT10->Draw("SAMEPE");
  hsT15->SetMarkerColor(kYellow);
  hsT15->SetMarkerStyle(20);
  hsT15->SetMarkerSize(0.5);
  hsT15->Draw("SAMEPE");
  hsT20->SetMarkerStyle(20);
  //hData->SetMarkerSize(0.5);
  hsT20->SetMarkerColor(kBlack);
  hsT20->SetMarkerSize(0.5);
  hsT20->Draw("SAMEPE");
  hsT25->SetMarkerColor(kGreen);
  hsT25->SetMarkerStyle(20);
  hsT25->SetMarkerSize(0.5);
  hsT25->Draw("SAMEPE");
  
  
  hsnoT5->SetLineColor(kRed);
  hsnoT5->Draw("SAME");
  hsnoT10->SetLineColor(kBlue);
  hsnoT10->Draw("SAME");
  hsnoT15->SetLineColor(kYellow);
  hsnoT15->Draw("SAME");
  hsnoT20->SetLineColor(kBlack);
  hsnoT20->Draw("SAME");
  hsnoT25->SetLineColor(kGreen);
  hsnoT25->Draw("SAME");
  
  
  //cout<<"Data/MC ratio = "<<hData->GetEntries()/hMC1->GetEntries()<<std::endl;
  //cout<<"Data/sMC ratio = "<<hData->GetEntries()/(scalef/**hMC->GetEntries()*/)<<std::endl;

return;

  TH1D* hratio1 = hT5->Clone();
  TH1D* hratio2 = hT10->Clone();
  TH1D* hratio3 = hT15->Clone();
  TH1D* hratio4 = hT20->Clone();
  TH1D* hratio5 = hT25->Clone();
  hratio1->Scale(0.);
  hratio2->Scale(0.);
  hratio3->Scale(0.);
  hratio4->Scale(0.);
  hratio5->Scale(0.);
  int nbinx = hratio1->GetNbinsX();
  double rat1, erat1;
  double rat2, erat2;
  double rat3, erat3;
  double rat4, erat4;
  double rat5, erat5;
  for (int i=1; i<nbinx+1; ++i)
    {
      rat1 =0.;
      erat1 =0.;
      rat2 =0.;
      erat2 =0.;
      rat3 =0.;
      erat3 =0.;
      rat4 =0.;
      erat4 =0.;
      rat5 =0.;
      erat5 =0.;
      if (fabs(hsnoT5->GetBinContent(i))>1e-10) {
	rat1 = hsT5->GetBinContent(i)/hsnoT5->GetBinContent(i);
	erat1 = sqrt(hsT5->GetBinContent(i)+hsnoT5->GetBinContent(i))/hsnoT5->GetBinContent(i);
      }
      hratio1->SetBinContent(i, rat1); 
      hratio1->SetBinError(i, erat1); 

      if (fabs(hsnoT10->GetBinContent(i))>1e-10) {
	rat2 = hsT10->GetBinContent(i)/hsnoT10->GetBinContent(i);
	erat2 = sqrt(hsT10->GetBinContent(i)+hsnoT10->GetBinContent(i))/hsnoT10->GetBinContent(i);
      }
      hratio2->SetBinContent(i, rat2); 
      hratio2->SetBinError(i, erat2); 

      if (fabs(hsnoT15->GetBinContent(i))>1e-10) {
	rat3 = hsT15->GetBinContent(i)/hsnoT15->GetBinContent(i);
	erat3 = sqrt(hsT15->GetBinContent(i)+hsnoT15->GetBinContent(i))/hsnoT15->GetBinContent(i);
      }
      hratio3->SetBinContent(i, rat3); 
      hratio3->SetBinError(i, erat3); 
      //std::cout<<"Bin "<<i<<"  ratio = "<<hratio->GetBinContent(i)<<std::endl;
      //std::cout<<"Bin "<<i<<" eratio = "<<hratio->GetBinError(i)<<std::endl;
      
      if (fabs(hsnoT20->GetBinContent(i))>1e-10) {
	rat4 = hsT20->GetBinContent(i)/hsnoT20->GetBinContent(i);
	erat4 = sqrt(hsT20->GetBinContent(i)+hsnoT20->GetBinContent(i))/hsnoT20->GetBinContent(i);
      }
      hratio4->SetBinContent(i, rat4); 
      hratio4->SetBinError(i, erat4); 

      if (fabs(hsnoT25->GetBinContent(i))>1e-10) {
	rat5 = hsT25->GetBinContent(i)/hsnoT25->GetBinContent(i);
	erat5 = sqrt(hsT25->GetBinContent(i)+hsnoT25->GetBinContent(i))/hsnoT25->GetBinContent(i);
      }
      hratio5->SetBinContent(i, rat5); 
      hratio5->SetBinError(i, erat5); 
    }
  TCanvas* c2 = new TCanvas("ratio","ratio",600,300);
  c2->Divide(2,3);
  c2->cd(1);
  hratio1->Draw();
  c2->cd(2);
  hratio2->Draw();
  c2->cd(3);
  hratio3->Draw();
  c2->cd(4);
  hratio4->Draw();
  c2->cd(5);
  hratio5->Draw();

   // Define the Canvas
  TCanvas *c = new TCanvas("c", "My Final Plot", 50, 50, 600,600);
 
   // Upper plot will be in pad1
   TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.);
   pad1->SetBottomMargin(0.02); // Upper and lower plot are joined
   pad1->SetTopMargin(1); // Upper and lower plot are joined
   if(hname=="ECal_ClusterEnergy"||hname=="ECal_HitEnergy" || hname=="ECal_EHitTot" || hname=="ECal_EClTot"){
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
   ymin = hT5->GetYaxis()->GetXmin();
   if (pad1->GetLogy()) {
     ymin = 1.;
     ymaxD = 10*ymaxD;
   }
   
   if (fabs(xmin+999)<0.0001 && fabs(xmax-999)<0.0001) {
     xmin = hT5->GetXaxis()->GetXmin();
     xmax = hT5->GetXaxis()->GetXmax();
   }
   //   Double_t xmin=60.00;  Double_t xmax=3500.;
   //TH1F *hFrame1 = pad1->DrawFrame(xmin,ymin,xmax,ymax);
   TH1F *hFrame1 = pad1->DrawFrame(xmin,ymin,xmax,ymaxD);
   std::cout<<"X range = "<<xmin<<" "<<xmax<<std::endl;
   std::cout<<"Y range = "<<ymin<<" "<<ymaxD<<std::endl;
   
   hFrame1->GetXaxis()->SetLabelSize(0.);
   hFrame1->SetStats(0);  
   hsT5->SetStats(0);
   hsT10->SetStats(0);
   hsT15->SetStats(0);  
   hsT20->SetStats(0); 
   hsT25->SetStats(0);   
   hsnoT5->SetStats(0);
   hsnoT10->SetStats(0);
   hsnoT15->SetStats(0);  
   hsnoT20->SetStats(0); 
   hsnoT25->SetStats(0);   
   //hFrame1->Draw("SAME");
   //return;

   hsT5->Draw("SAMEPE");
   hsT10->Draw("SAMEPE");
   hsT15->Draw("SAMEPE");
   hsT20->Draw("SAMEPE");
   hsT25->Draw("SAMEPE");
   hsnoT5->Draw("SAME");
   hsnoT10->Draw("SAME");
   hsnoT15->Draw("SAME");
   hsnoT20->Draw("SAME");
   hsnoT25->Draw("SAME");


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
   if(hname=="ECal_ClusterEnergy"||hname=="ECal_HitEnergy" || hname=="ECal_EHitTot" || hname=="ECal_EClTot"){
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
   hratio1->GetXaxis()->SetTitle("Y strip number");
   pad2->SetLogy();
   TH1F *hFrameRat = pad2->DrawFrame(xmin,0.1,xmax,12.);
   hFrameRat->GetXaxis()->SetTitle(xtitle.c_str());
   hFrameRat->Draw();
   hratio1->SetStats(0);  
   hratio1->SetMaximum(12.);
   //hratio1->SetMinimum(-1);
   hratio1->SetMinimum(0.1);
   hratio1->SetMarkerStyle(20);
   //hratio->SetMarkerSize(0.5);
   hratio1->SetLineColor(kRed);
   hratio1->SetMarkerSize(0.5);
   hratio1->SetMarkerColor(kRed);
   hratio1->Draw("epsame");
   hratio2->SetLineColor(kBlue);
   hratio2->SetMarkerSize(0.5);
   hratio2->SetMarkerColor(kBlue);
   hratio2->Draw("epsame");
   hratio3->SetMarkerColor(kYellow);
   hratio3->SetMarkerSize(0.5);
   hratio3->SetLineColor(kYellow);
   hratio3->Draw("epsame");
   hratio4->SetLineColor(kBlack);
   hratio4->SetMarkerSize(0.5);
   hratio4->SetMarkerColor(kBlack);
   hratio4->Draw("epsame");
   hratio5->SetMarkerColor(kGreen);
   hratio5->SetMarkerSize(0.5);
   hratio5->SetLineColor(kGreen);
   hratio5->Draw("epsame");

   
   
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
   hratio1->SetTitle("");

   
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
   string newName = hname;
   const char *a=newName.c_str();
   myText( 0.25,0.91,1,a, 0.039); 
  //   ATLASLabel(0.2,0.8,"Work in progress");//,"Preliminary");
   //   myText(       0.2, 0.7 , 1, "nPOT=9.3#times10^{9}");

   std::ostringstream s;
   s<<"nPOT=" << nPOTfromTarget;
   std::string snpot= s.str();
   const char *b=snpot.c_str();
   myText(       0.2, 0.7 , 1, b);


   // myText(       0.63, 0.7 , 1, "Diamond Target");
   // myText(       0.57, 0.8, 1, "Average beam Y profile");

   //myMarkerText( 0.25, 0.5, 1, 20, "Data July 2019",1.3);
   //myBoxText(    0.25, 0.45, 0.05, kBlack, "MC");
   TLegend* t = new TLegend(0.55,0.55,0.80,0.85);
   t->AddEntry(hsT5,"target 5k POT/bunch","ep");
   t->AddEntry(hsnoT5,"no target 5k POT/bunch","l");
   t->AddEntry(hsT10,"target 10k POT/bunch","ep");
   t->AddEntry(hsnoT10,"no target 10k POT/bunch","l");
   t->AddEntry(hsT15,"target 15k POT/bunch","ep");
   t->AddEntry(hsnoT15,"no target 15k POT/bunch","l");
   t->AddEntry(hsT20,"target 20k POT/bunch","ep");
   t->AddEntry(hsnoT20,"no target 20k POT/bunch","l");
   t->AddEntry(hsT25,"target 25k POT/bunch","ep");
   t->AddEntry(hsnoT25,"no target 25k POT/bunch","l");
   
   t->SetTextAlign(12);
   t->SetBorderSize(0);
   t->SetFillStyle(0);
   t->SetFillColor(0);
   t->SetTextFont(43);
   t->SetTextSize(15);

   t->Draw();
   Int_t gg5=MakeDifference(hsT5, hsnoT5, minInt, maxInt);
   Int_t gg10=MakeDifference(hsT10, hsnoT10, minInt, maxInt);
   Int_t gg15=MakeDifference(hsT15, hsnoT15, minInt, maxInt);
   Int_t gg20=MakeDifference(hsT20, hsnoT20, minInt, maxInt);
   Int_t gg25=MakeDifference(hsT25, hsnoT25, minInt, maxInt);
   std::fstream fs;
   fs.open("ggCount.txt", std::fstream::in | std::fstream::out | std::fstream::app);
   fs<<hname.c_str() << " " << gg5 << " " << gg10 << " " << gg15 << " " << " " <<gg20 << " " << gg25 << std::endl;
   fs.close();
   std::cout<<"ok fin qui " << hname.c_str()<<std::endl;
   c->SaveAs(("outpng/"+hname+".png").c_str());
   c->Print("AnalysisMulteplicityStudies.pdf(");
   //std::cout<<"is the last " << lastSet<< std::endl;
   if(lastSet/* && hname=="ECAL_Rgg_3ns1CoG20DegreeDeltaThetaEThr90""ECAL_Egg_3ns1CoG20DegreeDeltaThetaUnderPeak"*/){std::cout<<"End of job" << std::endl; c->Print("AnalysisMulteplicityStudies.pdf)");}
 
   //c->SaveAs("myPlot.png");
   //c->SaveAs("myPlot.eps");
   //return;

   
   delete pad1;
   delete pad2;
  // delete cData;
  // delete cMC;
   delete c;
   delete c2;
   delete t;
   
   std::cout<<"Bye, bye ................."<<std::endl<<std::endl;
}





Double_t TargetNoTargetScaleFactor( TFile* fTarget, TFile* fnoTarget, Double_t scalef){
  TH1D * DTime = (TH1D*)fTarget->Get("ECAL_dTime_10ns20DegreeDeltaTheta");

  Double_t bin1_5=DTime->FindBin(-10);
  Double_t bin2_5=DTime->FindBin(-6);
  Double_t bin4_5=DTime->FindBin(10);
  Double_t bin3_5=DTime->FindBin(6);
  DTime->Scale(scalef);
  Double_t IntegralLeft5=DTime->Integral(bin1_5,bin2_5);
  Double_t IntegralRight5=DTime->Integral(bin3_5,bin4_5);
      
  TH1D * DTime10 = (TH1D*)fnoTarget->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
  Double_t bin1_10=DTime10->FindBin(-10);
  Double_t bin2_10=DTime10->FindBin(-6);
  Double_t bin4_10=DTime10->FindBin(10);
  Double_t bin3_10=DTime10->FindBin(6);
  Double_t IntegralLeft10=DTime10->Integral(bin1_10,bin2_10);
  Double_t IntegralRight10=DTime10->Integral(bin3_10,bin4_10);
  Double_t scaleFactor = IntegralLeft5/IntegralLeft10;
  DTime->Scale(1./scalef);
  // std::cout<<"scalef " << scalef <<" integral left target " << IntegralLeft5 << " integral left Notarget " << IntegralLeft10  << " scale factor " << scaleFactor << std::endl;
  return scaleFactor;
}

Double_t TargetNoTargetScaleFactor( TFile* fTarget, TFile* fnoTarget){
  TH1D * DTime = (TH1D*)fData->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
  Double_t bin1_5=DTime->FindBin(-10);
  Double_t bin2_5=DTime->FindBin(-6);
  Double_t bin4_5=DTime->FindBin(10);
  Double_t bin3_5=DTime->FindBin(6);
  Double_t IntegralLeft5=DTime->Integral(bin1_5,bin2_5);
  Double_t IntegralRight5=DTime->Integral(bin3_5,bin4_5);
      
  TH1D * DTime10 = (TH1D*)fMC->Get("ECAL_dTime_10ns20DegreeDeltaTheta");
  Double_t bin1_10=DTime10->FindBin(-10);
  Double_t bin2_10=DTime10->FindBin(-6);
  Double_t bin4_10=DTime10->FindBin(10);
  Double_t bin3_10=DTime10->FindBin(6);
  Double_t IntegralLeft10=DTime10->Integral(bin1_10,bin2_10);
  Double_t IntegralRight10=DTime10->Integral(bin3_10,bin4_10);
  scalef=IntegralLeft5/IntegralLeft10;
  //std::cout<<"integral left target " << IntegralLeft5 << " integral left Notarget " << IntegralLeft10  << " scale factor " << scaleFactor << std::endl;
  return scaleFactor;
}

Double_t corrNPOT(TFile* fData, Double_t nPOTbtf){
  Double_t firstMean20=((TH1D*)fData->Get("TargetBeam_nPOT"))->GetMean(); 
  TH1D *h1=((TH1D*)fData->Get("TargetBeam_nPOT"));
  h1->GetXaxis()->SetRangeUser(firstMean20-7000, firstMean20+15000);
  Double_t Mean20 = h1->GetMean();
  Double_t fcorr=nPOTbtf/Mean20;
  return fcorr;
}


Int_t MakeDifference(TH1D* hTarget, TH1D* hnoTarget, double min, double max){
  TH1D *DifferencePositive = hTarget->Clone();
  DifferencePositive->Add(hnoTarget,-1);
  int k = DifferencePositive->GetXaxis()->FindBin(min);
  int k1= DifferencePositive->GetXaxis()->FindBin(max);
  Int_t Integral=DifferencePositive->Integral(k,k1);
  return Integral;
}


