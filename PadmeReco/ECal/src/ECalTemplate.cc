#include "ECalTemplate.hh"
#include <iostream>
#include "cstdlib"
#include "TProfile.h"
#include "TSpline.h"
#include "TMath.h"
#include "TF1.h"
#include <vector>

ECalTemplate* ECalTemplate::fInstance = 0;

static Double_t fitf(Double_t * x, Double_t * par) {
   TSpline3 *TemplateSpline =  ECalTemplate::GetInstance()->GetTemplateSpline();
  double arg = (x[0] - par[1])/par[3];
  return par[0]*TemplateSpline->Eval(arg)+par[2];
}

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
  hListTemplate->Add(hTMax    = new TH1D("hTimeMax","hTimeMax",1000,0.,1000.));
  hListTemplate->Add(hprof= new TProfile("hprof","Signal Profile",1000,0,1000,0,4096));
}

ECalTemplate::ECalTemplate()
{
  std::cout<<"Creating template class "<<std::endl;
}
ECalTemplate::~ECalTemplate(){
  if(hListTemplate) {hListTemplate->Delete();}
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
  fileOut->cd();
  //  hListTemplate->Print();
  HistoTemp = (TH1D*) hListTemplate->FindObject("hSample");
  hTemplate = (TH1D*) hListTemplate->FindObject("hprof");
  hTMax     = (TH1D*) hListTemplate->FindObject("hTimeMax");
  hTMax->Fill(Time);

  static Int_t PeakPos    = 400;
  static Int_t RiseMargin = 25;

  double Baseline = TMath::Mean(200,&fSample[0]);
  // double Baseline = TMath::Mean(40,&fSample[0]);

  Int_t shift = Time-PeakPos;
  for(Int_t kk=0;kk<1000;kk++){ 
    if (kk+shift > 0 && kk+shift < 1000) {
      //      if (kk < Time-RiseMargin) fTemplateVec[kk] = Baseline;
      if (kk < Time-RiseMargin) fTemplateVec[kk] = fSample[kk];
      else fTemplateVec[kk] = fSample[kk+shift];
    }else{
      fTemplateVec[kk] = Baseline;
      //      std::cout<<"aho "<<fSample[1000-((Int_t)Time-PeakPos)]<<std::endl;

      if(shift>0){ 
	double LastAvg  = TMath::Mean(200,&fSample[0]);
	fTemplateVec[kk] =  fSample[1000-shift];  // prone to noise fluctuation better to use avg and scale with e-^(t/tau)
      }
    }
  }
  TH1D* htmp    = new TH1D(Form("hVtemp%d",NHist),Form("hVtemp%d",NHist),1000,0.,1000.);
  TH1D* h = new TH1D(Form("hSig%d",NHist),Form("hSig%d",NHist),1000,0.,1000.);
  hVSig.push_back(h);
  hVTemp.push_back(htmp);
  for (int j = 0; j < 1000; j++) {
    hVTemp.at(NHist)->SetBinContent(j+1,fTemplateVec[j]);
    //    hVSig.at(NHist)->SetBinContent(j+1,templateVec[j]);
    hVSig.at(NHist)->SetBinContent(j+1,fSample[j]);
    hTemplate->Fill(j+1,fTemplateVec[j]); //sistemare
    // std::cout << "j: " << j << ", template: " << templateVec[j] << std::endl;
  }  
  fileOut->cd();
  hVSig.at(NHist)->Write();
  hVTemp.at(NHist)->Write();
  NHist++;
}

// reads the template function
void ECalTemplate::ReadTemplate()
{
  fileOut = new TFile("ECalTemplate.root","update");
  if(fileOut->IsOpen()){ 
    printf("File opened successfully\n");
  }else{
    printf("failed to open Template File \n");
    exit(-1);
  }
  std::cout<<"Reading template from File ECalTemplate.root"<<std::endl;
  TProfile* hECalTempl = (TProfile *) fileOut->Get("hprof");
  if(hECalTempl==0){ 
    std::cout<<"ERROR: Missing profile function"<<hECalTempl<<std::endl;
    std::cout<<"go to the config file and run prepate template mode"<<std::endl;
    exit(-1);
  }
  TH1D* hTest = (TH1D *) fileOut->Get("hSample");
  //  hECalTempl->Draw();
  Int_t Nent = hECalTempl->GetEntries();
  for(Int_t kk=0;kk<1000;kk++){ 
    fTemplateVec[kk] = hECalTempl->GetBinContent(kk);
    //   std::cout<<" "<<templateVec[kk]<<std::endl;
  }
  fTemplateSpline = new TSpline3("templateSpline", -1, 1000, fTemplateVec, 1000);
  fileOut->cd();
  std::cout<<"Writing template "<<std::endl;
  fTemplateSpline->Write();
}

void ECalTemplate::BuildFitFunction(){
  fECalFitFunction = new TF1("fitf",fitf,20,1000,4);
  fECalFitFunction -> SetParameters(1,0,0,1);
}
