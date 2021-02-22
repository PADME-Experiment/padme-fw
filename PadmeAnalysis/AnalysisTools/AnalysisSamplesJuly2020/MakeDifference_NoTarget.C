#include <iostream>
#include <cmath>

#include "Rtypes.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TRandom.h"
#include "TGraphErrors.h"

using namespace std;


void MakeDifference_NoTarget(std::string hname, double scalef=-1, double xmin=-999, double xmax=999, 
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
  
  //gStyle->SetOptTitle(0);
  std::cout<<" pointer to data file and mc file ... "<<fData<<" "<<fMC<<"    processing histo "<<hname <<std::endl;
  
  TH1D* hData = (TH1D*)fData->Get(hname.c_str());
  TH1D* hMC   = (TH1D*)fMC->Get(hname.c_str());
  
  if (scalef<0) 
    {
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
  
   TCanvas* c = new TCanvas("data","data",500,500);
   c->cd();
   TH1D *DifferencePositive = hData->Clone();
   DifferencePositive->Add(hsMC,-1);
   DifferencePositive->SetTitle(hname.c_str());
   DifferencePositive->GetXaxis()->SetRangeUser(xmin,xmax);
   
   Int_t Diff=DifferencePositive->GetEntries();
  /* DifferencePositive->GetXaxis()->SetRange(400, 600);
   Int_t Integral = DifferencePositive->GetIntegral();
   Double_t binW= DifferencePositive->GetBinWidth(150);*/
   
   int k = DifferencePositive->GetXaxis()->FindBin(300);
   int k1= DifferencePositive->GetXaxis()->FindBin(700);
   Int_t Integral2=DifferencePositive->Integral(k,k1);
   //std::cout<<"diff " << Diff << " integral " << Integral << " n gg " << Integral/binW  << " integra2 " << Integral2 << std::endl;
   
   
   
   DifferencePositive->Draw();
   Double_t maxhisto= DifferencePositive->GetMaximum();
    Double_t minhisto= DifferencePositive->GetMinimum();
    Int_t posY=maxhisto-(maxhisto-minhisto)/4;
    Int_t posY2=maxhisto-(maxhisto-minhisto)/3;
   std::ostringstream mytxt;
    mytxt<< "N entries  " << Diff;
    //mytxt<< "Sigma" << gausSigma;
    TLatex*latex=new TLatex(50, posY, mytxt.str().c_str());
    latex->Draw();
    std::ostringstream mytxt1;
    mytxt1<< "UnderPeak  " << Integral2;
    //mytxt<< "Sigma" << gausSigma;
    TLatex*latex1=new TLatex(50, posY2, mytxt1.str().c_str());
    latex1->Draw();
   
  c->SaveAs(("outpng/"+hname+".png").c_str());
  c->Print("AnalysisHisto.pdf(");
  //if(/*hname=="SS2gYEw_passDtDphiCogDsume"||hname=="ECal_NCluster" || */hname=="ECAL_tgg_10ns3CoGUnderPeakEThr90") c->Print("AnalysisHisto.pdf)");
 
   //c->SaveAs("myPlot.png");
   //c->SaveAs("myPlot.eps");
   //return;

   
   //delete c;
   
   std::cout<<"Bye, bye ................."<<std::endl<<std::endl;
}




void MakeDifference_NoTarget(std::string hname, double scalef=-1, double xmin=-999, double xmax=999, 
	       std::string xtitle="E(#gamma_{1})+E(#gamma_{2}) [MeV]", 
	       TFile* fData=_file0, TFile* fMC=_file1, Bool_t useDTimeNormalization, Int_t end)
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
  
  //gStyle->SetOptTitle(0);
  std::cout<<" pointer to data file and mc file ... "<<fData<<" "<<fMC<<"    processing histo "<<hname <<std::endl;
  
  TH1D* hData = (TH1D*)fData->Get(hname.c_str());
  TH1D* hMC   = (TH1D*)fMC->Get(hname.c_str());
  
  if (scalef<0) 
    {
      
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
  
   TCanvas* c = new TCanvas("data","data",500,500);
   c->cd();
   TH1D *DifferencePositive = hData->Clone();
   DifferencePositive->Add(hsMC,-1);
   DifferencePositive->SetTitle(hname.c_str());
   DifferencePositive->GetXaxis()->SetRangeUser(xmin,xmax);
   
   Int_t Diff=DifferencePositive->GetEntries();
  /* DifferencePositive->GetXaxis()->SetRange(400, 600);
   Int_t Integral = DifferencePositive->GetIntegral();
   Double_t binW= DifferencePositive->GetBinWidth(150);*/
   
   int k = DifferencePositive->GetXaxis()->FindBin(300);
   int k1= DifferencePositive->GetXaxis()->FindBin(700);
   Int_t Integral2=DifferencePositive->Integral(k,k1);
   //std::cout<<"diff " << Diff << " integral " << Integral << " n gg " << Integral/binW  << " integra2 " << Integral2 << std::endl;
   
   
   
   DifferencePositive->Draw();
   Double_t maxhisto= DifferencePositive->GetMaximum();
    Double_t minhisto= DifferencePositive->GetMinimum();
    Int_t posY=maxhisto-(maxhisto-minhisto)/4;
    Int_t posY2=maxhisto-(maxhisto-minhisto)/3;
   std::ostringstream mytxt;
    mytxt<< "N entries  " << Diff;
    //mytxt<< "Sigma" << gausSigma;
    TLatex*latex=new TLatex(50, posY, mytxt.str().c_str());
    latex->Draw();
    std::ostringstream mytxt1;
    mytxt1<< "UnderPeak  " << Integral2;
    //mytxt<< "Sigma" << gausSigma;
    TLatex*latex1=new TLatex(50, posY2, mytxt1.str().c_str());
    latex1->Draw();
   
  c->SaveAs(("outpng/"+hname+".png").c_str());
  //c->Print("AnalysisHisto.pdf(");
  //if(/*hname=="SS2gYEw_passDtDphiCogDsume"||hname=="ECal_NCluster" || */hname=="ECAL_tgg_10ns3CoGUnderPeakEThr90") c->Print("AnalysisHisto.pdf)");
  if(end==1)c->Print("AnalysisHisto.pdf)"); 
   //c->SaveAs("myPlot.png");
   //c->SaveAs("myPlot.eps");
   //return;

   
   //delete c;
   
   std::cout<<"Bye, bye ................."<<std::endl<<std::endl;
}
