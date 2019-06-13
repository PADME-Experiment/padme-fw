#include "ECalTemplate.hh"
#include <iostream>
#include "cstdlib"
#include "TProfile.h"
#include "TMath.h"

ECalTemplate* ECalTemplate::fInstance = 0;

ECalTemplate* ECalTemplate::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new ECalTemplate(); }
  return fInstance;
}

void ECalTemplate::Init()
{
  std::cout<<"Creating template root file "<<std::endl;
  fileOut    = new TFile("ECalTemplate.root", "RECREATE");
  hListTemplate   = new TList();  
  ECal = new TTree("ECAL","ECAL");
  ECal->Branch("VMax",&fVMax);
  hListTemplate->Add(hSample   = new TH1D("hSample","hSample",1000,0.,1000.));
  //  hListTemplate->Add(hSignal = new TH1D("hSignal","hSignal",1000,0.,1000.));
  hListTemplate->Add(hTMax    = new TH1D("hTimeMax","hTimeMax",1000,0.,1000.));
  hListTemplate->Add(hprof= new TProfile("hprof","Signal Profile",1000,0,1000,0,4096));
}


ECalTemplate::ECalTemplate()
{
  std::cout<<"Creating template class "<<std::endl;
}
ECalTemplate::~ECalTemplate(){
}

void ECalTemplate::WriteHist()
{
  std::cout<<"Saving trees "<<std::endl;
  fileOut->cd();
  ECal->Write();
  hListTemplate->Write();
  fileOut->Close();
}

void ECalTemplate::PrepareTemplate(Short_t * fSample, Double_t Time)
{
  //  for(Int_t kk=0;kk<200;kk++) std::cout<<"Preparing template *******************************"<<fSample[kk]<<std::endl;
  //  std::cout<<"List "<<hListTemplate<<std::endl;
  //  hListTemplate->Print();
  TH1D * HistoTemp = (TH1D*) hListTemplate->FindObject("hSample");
  TH1D * hTemplate = (TH1D*) hListTemplate->FindObject("hprof");
  TH1D * hTMax = (TH1D*) hListTemplate->FindObject("hTimeMax");
  //  HistoTemp->Reset();
  hTMax->Fill(Time);
  //for(Int_t kk=0;kk<1001;kk++) HistoTemp->SetBinContent(kk,fSample[kk]);
//  for(Int_t kk=0;kk<1001;kk++){ 
//    if(kk<=100) hTemplate->Fill(kk,3700.);
//    if(kk>100) hTemplate->Fill(kk-(Int_t)Time+100,fSample[kk]); //sistemare
//  }


  double templateVec[1000];
  double Baseline = TMath::Mean(200,&fSample[0]);
  for(Int_t kk=0;kk<1000;kk++){ 
    if (kk+(Time-400) > 0 && kk+(Time-400) < 1000) {
      // Double_t temp = Smooth[j+(minj-200)];
      //temp = (baseline - temp)/(baseline - minj);
      // temp = (4096 - temp)/4096;
      if (kk < Time-80) templateVec[kk] = Baseline;
      else              templateVec[kk] = fSample[kk+((Int_t)Time-400)];
      //      numTraces[j]++;
    }else{
      templateVec[kk] = Baseline;
    }
  }
  
  for (int j = 0; j < 1000; j++) {
    //  hTemplate->SetBinContent(j+1, templateVec[j]);
    HistoTemp->SetBinContent(j+1,templateVec[j]);
    hTemplate->Fill(j+1,templateVec[j]); //sistemare
    // std::cout << "j: " << j << ", template: " << templateVec[j] << std::endl;
  }
  
  fileOut->cd();
  HistoTemp->Write();
}

void ECalTemplate::ReadTemplate()
{
  std::cout<<"Reading template "<<std::endl;
}

