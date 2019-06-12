#include "ECalTemplate.hh"
#include <iostream>
#include "cstdlib"
#include "TProfile.h"

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
//  hListCal->Add(hSat    = new TH1F("hSat","hSat",1000,0.,1000.));
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

void ECalTemplate::PrepareTemplate(Short_t * fSample)
{
  //  for(Int_t kk=0;kk<200;kk++) std::cout<<"Preparing template *******************************"<<fSample[kk]<<std::endl;
  //  std::cout<<"List "<<hListTemplate<<std::endl;
  //  hListTemplate->Print();
  TH1D * HistoTemp = (TH1D*) hListTemplate->FindObject("hSample");
  //  HistoTemp->Reset();
  for(Int_t kk=0;kk<1001;kk++) HistoTemp->SetBinContent(kk,fSample[kk]);
//  histo->Write();
}

void ECalTemplate::ReadTemplate()
{
  std::cout<<"Reading template "<<std::endl;
}

