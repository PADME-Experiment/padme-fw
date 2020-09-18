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

void compareSideBySide(std::string hname="SS2gEvsR_passDt", TFile* f0=_file0, TFile* f1=_file1)
{
  gStyle->SetOptStat(0);
  Double_t scalef=-1; 
  TH1D* hDN = (TH1D*)f0->Get("nPOT");
  TH1D* hMCN = (TH1D*)f1->Get("nPOT");
  double nPOTdata = hDN->GetBinContent(2);
  double nPOTMC = hMCN->GetBinContent(2);
  TH1D* hPOTdisMC=(TH1D*)f1->Get("NposInBunch_beam");
  /* from ideal MC, 20k POT gaussian fit of target nPOT/bunch
     Mean         1.76871e+04   
     Sigma        1.82364e+02
  */
  double sc =20000./17687.1; //1.18;//23000./hPOTdisMC->GetMean();
  TH1D* hPOTdisDT=(TH1D*)f0->Get("NposInBunch_beam"); 
  std::cout<<"Total num. of POTs is data = "<<nPOTdata<<std::endl;
  std::cout<<"Total num. of POTs is MC   = "<<nPOTMC  <<std::endl;
  //std::cout<<"Average nPOT/bunch in data = "<<hPOTdisDT->GetMean()<<std::endl;
  //std::cout<<"Average nPOT/bunch in MC   = "<<hPOTdisMC->GetMean()<<std::endl;
  std::cout<<"Average nPOT/bunch in data = "<<hPOTdisDT->GetMean()<<std::endl;
  std::cout<<"Average nPOT/bunch in MC   = "<<hPOTdisMC->GetMean()<<std::endl; 
  nPOTMC =nPOTMC*sc;
  scalef = nPOTdata/nPOTMC;
  std::cout<<"Total CORR num. of POTs is MC   = "<<nPOTMC<<std::endl;
  // scalef = nPOTMC;//nPOTdata/( hMCN->GetBinContent(2));
  std::cout<<"Scaling factor for MC is "<<scalef<<std::endl;

  new TCanvas("c1","c1",1600,600);
  //c1->SetTitle(hname.c_str());
  c1->Divide(2,1); 
  c1->cd(1);
  TH2D* h0; _file0->GetObject(hname.c_str(),h0);
  if (h0 != NULL){
    //h0->GetYaxis()->SetRange(-400, 700);
    h0->SetMaximum(30);
    h0->GetXaxis()->SetTitle("x_{#gamma} [mm]");
    h0->GetYaxis()->SetTitle("y_{#gamma} [mm]");
    h0->Draw("COLORZ");
   PADME_LABEL(0.2,0.8); myText( 0.33,0.8,1,"Internal");
   myText(       0.2, 0.72 , 1, "nPOT=9.3#times10^{9}");
   myText(       0.2, 0.20 , 1, "E_{beam}=490 MeV, 23k e^{+}/ 150 ns bunch");
   myText(0.6, 0.8,1,"Data July 2019");
  }
  else {
    std::cout<<" histogram named <"<<hname<<"> not found in file named "<<f0->GetName()<<std::endl;
    delete c1;
    return;
  }
  c1->cd(2);
  TH2D* h1; _file1->GetObject(hname.c_str(),h1);
  TH2D* hsMC = h1->Clone();
  hsMC->Scale(scalef);
  hsMC->GetXaxis()->SetTitle("x_{#gamma} [mm]");
  hsMC->GetYaxis()->SetTitle("y_{#gamma} [mm]");
  //hsMC->SetAxisRange(-400, 700,"Y");
  if (hsMC != NULL){
    hsMC->SetMaximum(30); hsMC->Draw("COLORZ");
   PADME_LABEL(0.2,0.8); myText( 0.33,0.8,1,"Internal");
   myText(       0.2, 0.72 , 1, "nPOT=9.3#times10^{9}");
   myText(       0.2, 0.20 , 1, "E_{beam}=490 MeV, 23k e^{+}/ 150 ns bunch"); 
   myText(0.6, 0.8,1,"MC");
   }
  else {
    std::cout<<" histogram named <"<<hname<<"> not found in file named "<<f1->GetName()<<std::endl;
    delete c1;
    return;
  }

  c1->SaveAs(("Plot_"+hname+".C").c_str());
  c1->Print("AnalysisHisto.pdf(");
  delete c1;
}



void compareSideBySide(std::string hname="SS2gEvsR_passDt", TFile* f0=_file0, TFile* f1=_file1, Bool_t MakeDifference)
{
  gStyle->SetOptStat(0);
  Double_t scalef=-1; 
  TH1D* hDN = (TH1D*)f0->Get("nPOT");
  TH1D* hMCN = (TH1D*)f1->Get("nPOT");
  double nPOTdata = hDN->GetBinContent(2);
  double nPOTMC = hMCN->GetBinContent(2);
  TH1D* hPOTdisMC=(TH1D*)f1->Get("NposInBunch_beam");
  /* from ideal MC, 20k POT gaussian fit of target nPOT/bunch
     Mean         1.76871e+04   
     Sigma        1.82364e+02
  */
  double sc =20000./17687.1; //1.18;//23000./hPOTdisMC->GetMean();
  TH1D* hPOTdisDT=(TH1D*)f0->Get("NposInBunch_beam"); 
  std::cout<<"Total num. of POTs is data = "<<nPOTdata<<std::endl;
  std::cout<<"Total num. of POTs is MC   = "<<nPOTMC  <<std::endl;
  //std::cout<<"Average nPOT/bunch in data = "<<hPOTdisDT->GetMean()<<std::endl;
  //std::cout<<"Average nPOT/bunch in MC   = "<<hPOTdisMC->GetMean()<<std::endl;
  std::cout<<"Average nPOT/bunch in data = "<<hPOTdisDT->GetMean()<<std::endl;
  std::cout<<"Average nPOT/bunch in MC   = "<<hPOTdisMC->GetMean()<<std::endl; 
  nPOTMC =nPOTMC*sc;
  scalef = nPOTdata/nPOTMC;
  std::cout<<"Total CORR num. of POTs is MC   = "<<nPOTMC<<std::endl;
  // scalef = nPOTMC;//nPOTdata/( hMCN->GetBinContent(2));
  std::cout<<"Scaling factor for MC is "<<scalef<<std::endl;

  new TCanvas("c1","c1",1100,1100);
  c1->SetTitle(hname.c_str());
  c1->Divide(2,2); 
  c1->cd(1);
  TH2D* h0; _file0->GetObject(hname.c_str(),h0);
  if (h0 != NULL){h0->SetMaximum(30); h0->Draw("COLORZ");}
  else {
    std::cout<<" histogram named <"<<hname<<"> not found in file named "<<f0->GetName()<<std::endl;
    delete c1;
    return;
  }
  c1->cd(2);
  TH2D* h1; _file1->GetObject(hname.c_str(),h1);
  TH2D* hsMC = h1->Clone();
  hsMC->Scale(scalef);
  if (hsMC != NULL){hsMC->SetMaximum(30); hsMC->Draw("COLORZ");}
  else {
    std::cout<<" histogram named <"<<hname<<"> not found in file named "<<f1->GetName()<<std::endl;
    delete c1;
    return;
  }

  c1->cd(3);
  TH2D *DifferencePositive = hsMC->Clone();
  DifferencePositive->Add(h0,-1);
  DifferencePositive->SetMinimum(0);
  DifferencePositive->SetMaximum(15);
  DifferencePositive->SetTitle("MC-Data>0");
  if (DifferencePositive != NULL){DifferencePositive->Draw("COLORZ");}
  c1->cd(4);
  TH2D *DifferenceNegative = hsMC->Clone();
  DifferenceNegative->Add(h0,-1);
  //gStile->SetPalette(52);
  DifferenceNegative->SetMaximum(0);
  DifferenceNegative->SetMinimum(-15);
  DifferenceNegative->SetTitle("MC-Data<0");
   if (DifferenceNegative != NULL){DifferenceNegative->Draw("colz");}


  c1->SaveAs(("outpng/"+hname+".png").c_str());
  c1->Print("AnalysisHisto.pdf(");
  delete c1;
}

void compareSideBySide(std::string hname, std::string hname2, TFile* f0=_file0, TFile* f1=_file1,  Bool_t MakeDifference)
{
  gStyle->SetOptStat(0);
  Double_t scalef=-1; 
  TH1D* hDN = (TH1D*)f0->Get("nPOT");
  TH1D* hMCN = (TH1D*)f1->Get("nPOT");
  double nPOTdata = hDN->GetBinContent(2);
  double nPOTMC = hMCN->GetBinContent(2);
  TH1D* hPOTdisMC=(TH1D*)f1->Get("NposInBunch_beam");
  /* from ideal MC, 20k POT gaussian fit of target nPOT/bunch
     Mean         1.76871e+04   
     Sigma        1.82364e+02
  */
  double sc =20000./17687.1; //1.18;//23000./hPOTdisMC->GetMean();
  TH1D* hPOTdisDT=(TH1D*)f0->Get("NposInBunch_beam"); 
  std::cout<<"Total num. of POTs is data = "<<nPOTdata<<std::endl;
  std::cout<<"Total num. of POTs is MC   = "<<nPOTMC  <<std::endl;
  //std::cout<<"Average nPOT/bunch in data = "<<hPOTdisDT->GetMean()<<std::endl;
  //std::cout<<"Average nPOT/bunch in MC   = "<<hPOTdisMC->GetMean()<<std::endl;
  std::cout<<"Average nPOT/bunch in data = "<<hPOTdisDT->GetMean()<<std::endl;
  std::cout<<"Average nPOT/bunch in MC   = "<<hPOTdisMC->GetMean()<<std::endl; 
  nPOTMC =nPOTMC*sc;
  scalef = nPOTdata/nPOTMC;
  std::cout<<"Total CORR num. of POTs is MC   = "<<nPOTMC<<std::endl;
  // scalef = nPOTMC;//nPOTdata/( hMCN->GetBinContent(2));
  std::cout<<"Scaling factor for MC is "<<scalef<<std::endl;

  new TCanvas("c1","c1",1100,800);
  c1->SetTitle(hname.c_str());
  c1->Divide(2,1); 
  c1->cd(1);
  TH2D* h0Data; _file0->GetObject(hname.c_str(),h0Data);
  TH2D* h1Data; _file0->GetObject(hname2.c_str(),h1Data);
  TH2D* h0; _file1->GetObject(hname.c_str(),h0);
  TH2D* h1; _file1->GetObject(hname2.c_str(),h1);
  
  
  TH2D* hsMC  = h0->Clone();
  TH2D* hsMC1 = h1->Clone();
  hsMC->Scale(scalef);
  hsMC1->Scale(scalef);
  
  TH2D *DifferencePositive = h0Data->Clone();
  DifferencePositive->Add(h1Data,-1);
  DifferencePositive->SetMinimum(0);
  DifferencePositive->SetMaximum(20);
  DifferencePositive->SetTitle(hname.c_str());
  if (DifferencePositive != NULL){DifferencePositive->Draw("COLORZ");}
  
  c1->cd(2);
  TH2D *DifferencePositiveMC = hsMC->Clone();
  DifferencePositiveMC->Add(hsMC1,-1);
  DifferencePositiveMC->SetMinimum(0);
  DifferencePositiveMC->SetMaximum(20);
  DifferencePositiveMC->SetTitle("MC");
  if (DifferencePositiveMC != NULL){DifferencePositiveMC->Draw("COLORZ");}

  c1->SaveAs(("outpng/"+hname+".png").c_str());
  c1->Print("AnalysisHisto.pdf(");
  delete c1;
}
