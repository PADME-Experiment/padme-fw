#include "ECalTemplate.hh"
#include <iostream>
#include "cstdlib"
#include "TProfile.h"
#include "TSpline.h"
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
  //  hListTemplate->Print();
  TH1D * HistoTemp = (TH1D*) hListTemplate->FindObject("hSample");
  TH1D * hTemplate = (TH1D*) hListTemplate->FindObject("hprof");
  TH1D * hTMax = (TH1D*) hListTemplate->FindObject("hTimeMax");
  //  HistoTemp->Reset();
  hTMax->Fill(Time);

  double templateVec[1000];
  Int_t PeakPos = 400;
  Int_t RiseMargin = 80;

  double Baseline = TMath::Mean(200,&fSample[0]);
  for(Int_t kk=0;kk<1000;kk++){ 
    if (kk+(Time-PeakPos) > 0 && kk+(Time-PeakPos) < 1000) {
      if (kk < Time-RiseMargin) templateVec[kk] = Baseline;
      else              templateVec[kk] = fSample[kk+((Int_t)Time-PeakPos)];
    }else{
      templateVec[kk] = Baseline;
    }
  }
  
  for (int j = 0; j < 1000; j++) {
    HistoTemp->SetBinContent(j+1,templateVec[j]);
    hTemplate->Fill(j+1,templateVec[j]); //sistemare
    // std::cout << "j: " << j << ", template: " << templateVec[j] << std::endl;
  }  
  fileOut->cd();
  HistoTemp->Write();
}

void ECalTemplate::ReadTemplate()
{
  fileOut = new TFile("ECalTemplate.root");
  if(fileOut->IsOpen()){ 
    printf("File opened successfully\n");
  }else{
    printf("failed to open Template File \n");
    exit(-1);
  }
  std::cout<<"Reading template from File ECalTemplate.root"<<std::endl;
  TProfile* hECalTempl = (TProfile *) fileOut->Get("hprof");
  //  hECalTempl->Draw();
  Double_t templateVec[1000];
  Int_t Nent = hECalTempl->GetEntries();
  for(Int_t kk=0;kk<1000;kk++){ 
    templateVec[kk] = hECalTempl->GetBinContent(kk);
    //   std::cout<<" "<<templateVec[kk]<<std::endl;
  }
  TSpline3 * templateSpline = new TSpline3("templateSpline", -1, 1000, templateVec, 1000);
  //  templateSpline->Draw("LCPSAME");
}

