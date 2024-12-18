#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooFormulaVar.h"
#include "RooDataHist.h"
#include "RooGaussian.h"
#include "RooExponential.h"
#include "TCanvas.h"
#include "RooPlot.h"
#include "TTree.h"
#include "TH1D.h"
#include "TRandom.h"

using namespace RooFit;




Double_t fitFunc(Double_t *x, Double_t *par){
  
  return (par[0] + par[1]*exp(-par[2]*(400 - x[0])) + par[3]*exp(-0.5*pow(((x[0]-par[4])/2*par[5]),2)));
}



void MCData_analysis(){

  
  TFile *f20_1 = TFile::Open("MCData_1e_20MeV_ana.root");
  TFile *f19_1 = TFile::Open("MCData_1e_19MeV_ana.root");
  TFile *f18_1 = TFile::Open("MCData_1e_18MeV_ana.root");
  TFile *f16_1 = TFile::Open("MCData_1e_16MeV_ana.root");
  TFile *f14_1 = TFile::Open("MCData_1e_14MeV_ana.root");
  TFile *f12_1 = TFile::Open("MCData_1e_12MeV_ana.root");
  TFile *f10_1 = TFile::Open("MCData_1e_10MeV_ana.root");
  TFile *f8_1 = TFile::Open("MCData_1e_8MeV_ana.root");
  TFile *f6_1 = TFile::Open("MCData_1e_6MeV_ana.root");
  TFile *f4_1 = TFile::Open("MCData_1e_4MeV_ana.root");
  TFile *f2_1 = TFile::Open("MCData_1e_2MeV_ana.root");
  
  TFile *f20_10 = TFile::Open("MCData_10e_20MeV_ana.root");
  TFile *f19_10 = TFile::Open("MCData_10e_19MeV_ana.root");
  TFile *f18_10 = TFile::Open("MCData_10e_18MeV_ana.root");
  TFile *f16_10 = TFile::Open("MCData_10e_16MeV_ana.root");
  TFile *f14_10 = TFile::Open("MCData_10e_14MeV_ana.root");
  TFile *f12_10 = TFile::Open("MCData_10e_12MeV_ana.root");
  TFile *f10_10 = TFile::Open("MCData_10e_10MeV_ana.root");
  TFile *f8_10 = TFile::Open("MCData_10e_8MeV_ana.root");
  TFile *f6_10 = TFile::Open("MCData_10e_6MeV_ana.root");
  TFile *f4_10 = TFile::Open("MCData_10e_4MeV_ana.root");
  TFile *f2_10 = TFile::Open("MCData_10e_2MeV_ana.root");
    
  TFile *f20_100 = TFile::Open("MCData_100e_20MeV_ana.root");
  TFile *f19_100 = TFile::Open("MCData_100e_19MeV_ana.root");
  TFile *f18_100 = TFile::Open("MCData_100e_18MeV_ana.root");
  TFile *f16_100 = TFile::Open("MCData_100e_16MeV_ana.root");
  TFile *f14_100 = TFile::Open("MCData_100e_14MeV_ana.root");
  TFile *f12_100 = TFile::Open("MCData_100e_12MeV_ana.root");
  TFile *f10_100 = TFile::Open("MCData_100e_10MeV_ana.root");
  TFile *f8_100 = TFile::Open("MCData_100e_8MeV_ana.root");
  TFile *f6_100 = TFile::Open("MCData_100e_6MeV_ana.root");
  TFile *f4_100 = TFile::Open("MCData_100e_4MeV_ana.root");
  TFile *f2_100 = TFile::Open("MCData_100e_2MeV_ana.root");
      
  TFile *f20_1000 = TFile::Open("MCData_1000e_20MeV_ana.root");
  TFile *f19_1000 = TFile::Open("MCData_1000e_19MeV_ana.root");
  TFile *f18_1000 = TFile::Open("MCData_1000e_18MeV_ana.root");
  TFile *f16_1000 = TFile::Open("MCData_1000e_16MeV_ana.root");
  TFile *f14_1000 = TFile::Open("MCData_1000e_14MeV_ana.root");
  TFile *f12_1000 = TFile::Open("MCData_1000e_12MeV_ana.root");
  TFile *f10_1000 = TFile::Open("MCData_1000e_10MeV_ana.root");
  TFile *f8_1000 = TFile::Open("MCData_1000e_8MeV_ana.root");
  TFile *f6_1000 = TFile::Open("MCData_1000e_6MeV_ana.root");
  TFile *f4_1000 = TFile::Open("MCData_1000e_4MeV_ana.root");
  TFile *f2_1000 = TFile::Open("MCData_1000e_2MeV_ana.root");
  
  TFile *f20_10000 = TFile::Open("MCData_10000e_20MeV_ana.root");
  TFile *f19_10000 = TFile::Open("MCData_10000e_19MeV_ana.root");
  TFile *f18_10000 = TFile::Open("MCData_10000e_18MeV_ana.root");
  TFile *f16_10000 = TFile::Open("MCData_10000e_16MeV_ana.root");
  TFile *f14_10000 = TFile::Open("MCData_10000e_14MeV_ana.root");
  TFile *f12_10000 = TFile::Open("MCData_10000e_12MeV_ana.root");
  TFile *f10_10000 = TFile::Open("MCData_10000e_10MeV_ana.root");
  TFile *f8_10000 = TFile::Open("MCData_10000e_8MeV_ana.root");
  TFile *f6_10000 = TFile::Open("MCData_10000e_6MeV_ana.root");
  TFile *f4_10000 = TFile::Open("MCData_10000e_4MeV_ana.root");
  TFile *f2_10000 = TFile::Open("MCData_10000e_2MeV_ana.root");
  
  TFile *f20_25000 = TFile::Open("MCData_25000e_20MeV_ana.root");
  TFile *f19_25000 = TFile::Open("MCData_25000e_19MeV_ana.root");
  TFile *f18_25000 = TFile::Open("MCData_25000e_18MeV_ana.root");
  TFile *f16_25000 = TFile::Open("MCData_25000e_16MeV_ana.root");
  TFile *f14_25000 = TFile::Open("MCData_25000e_14MeV_ana.root");
  TFile *f12_25000 = TFile::Open("MCData_25000e_12MeV_ana.root");
  TFile *f10_25000 = TFile::Open("MCData_25000e_10MeV_ana.root");
  TFile *f8_25000 = TFile::Open("MCData_25000e_8MeV_ana.root");
  TFile *f6_25000 = TFile::Open("MCData_25000e_6MeV_ana.root");
  TFile *f4_25000 = TFile::Open("MCData_25000e_4MeV_ana.root");
  TFile *f2_25000 = TFile::Open("MCData_25000e_2MeV_ana.root");

  
  TDirectory* dir20_1 = (TDirectory*) f20_1->Get("SPA_background");
  TDirectory* dir19_1 = (TDirectory*) f19_1->Get("SPA_background");
  TDirectory* dir18_1 = (TDirectory*) f18_1->Get("SPA_background");
  TDirectory* dir16_1 = (TDirectory*) f16_1->Get("SPA_background");
  TDirectory* dir14_1 = (TDirectory*) f14_1->Get("SPA_background");
  TDirectory* dir12_1 = (TDirectory*) f12_1->Get("SPA_background");
  TDirectory* dir10_1 = (TDirectory*) f10_1->Get("SPA_background");
  TDirectory* dir8_1 = (TDirectory*) f8_1->Get("SPA_background");
  TDirectory* dir6_1 = (TDirectory*) f6_1->Get("SPA_background");
  TDirectory* dir4_1 = (TDirectory*) f4_1->Get("SPA_background");
  TDirectory* dir2_1 = (TDirectory*) f2_1->Get("SPA_background");
  
  TDirectory* dir20_10 = (TDirectory*) f20_10->Get("SPA_background");
  TDirectory* dir19_10 = (TDirectory*) f19_10->Get("SPA_background");
  TDirectory* dir18_10 = (TDirectory*) f18_10->Get("SPA_background");
  TDirectory* dir16_10 = (TDirectory*) f16_10->Get("SPA_background");
  TDirectory* dir14_10 = (TDirectory*) f14_10->Get("SPA_background");
  TDirectory* dir12_10 = (TDirectory*) f12_10->Get("SPA_background");
  TDirectory* dir10_10 = (TDirectory*) f10_10->Get("SPA_background");
  TDirectory* dir8_10 = (TDirectory*) f8_10->Get("SPA_background");
  TDirectory* dir6_10 = (TDirectory*) f6_10->Get("SPA_background");
  TDirectory* dir4_10 = (TDirectory*) f4_10->Get("SPA_background");
  TDirectory* dir2_10 = (TDirectory*) f2_10->Get("SPA_background");
  
  TDirectory* dir20_100 = (TDirectory*) f20_100->Get("SPA_background");
  TDirectory* dir19_100 = (TDirectory*) f19_100->Get("SPA_background");
  TDirectory* dir18_100 = (TDirectory*) f18_100->Get("SPA_background");
  TDirectory* dir16_100 = (TDirectory*) f16_100->Get("SPA_background");
  TDirectory* dir14_100 = (TDirectory*) f14_100->Get("SPA_background");
  TDirectory* dir12_100 = (TDirectory*) f12_100->Get("SPA_background");
  TDirectory* dir10_100 = (TDirectory*) f10_100->Get("SPA_background");
  TDirectory* dir8_100 = (TDirectory*) f8_100->Get("SPA_background");
  TDirectory* dir6_100 = (TDirectory*) f6_100->Get("SPA_background");
  TDirectory* dir4_100 = (TDirectory*) f4_100->Get("SPA_background");
  TDirectory* dir2_100 = (TDirectory*) f2_100->Get("SPA_background");

  TDirectory* dir20_1000 = (TDirectory*) f20_1000->Get("SPA_background");
  TDirectory* dir19_1000 = (TDirectory*) f19_1000->Get("SPA_background");
  TDirectory* dir18_1000 = (TDirectory*) f18_1000->Get("SPA_background");
  TDirectory* dir16_1000 = (TDirectory*) f16_1000->Get("SPA_background");
  TDirectory* dir14_1000 = (TDirectory*) f14_1000->Get("SPA_background");
  TDirectory* dir12_1000 = (TDirectory*) f12_1000->Get("SPA_background");
  TDirectory* dir10_1000 = (TDirectory*) f10_1000->Get("SPA_background");
  TDirectory* dir8_1000 = (TDirectory*) f8_1000->Get("SPA_background");
  TDirectory* dir6_1000 = (TDirectory*) f6_1000->Get("SPA_background");
  TDirectory* dir4_1000 = (TDirectory*) f4_1000->Get("SPA_background");
  TDirectory* dir2_1000 = (TDirectory*) f2_1000->Get("SPA_background");

  TDirectory* dir20_10000 = (TDirectory*) f20_10000->Get("SPA_background");
  TDirectory* dir19_10000 = (TDirectory*) f19_10000->Get("SPA_background");
  TDirectory* dir18_10000 = (TDirectory*) f18_10000->Get("SPA_background");
  TDirectory* dir16_10000 = (TDirectory*) f16_10000->Get("SPA_background");
  TDirectory* dir14_10000 = (TDirectory*) f14_10000->Get("SPA_background");
  TDirectory* dir12_10000 = (TDirectory*) f12_10000->Get("SPA_background");
  TDirectory* dir10_10000 = (TDirectory*) f10_10000->Get("SPA_background");
  TDirectory* dir8_10000 = (TDirectory*) f8_10000->Get("SPA_background");
  TDirectory* dir6_10000 = (TDirectory*) f6_10000->Get("SPA_background");
  TDirectory* dir4_10000 = (TDirectory*) f4_10000->Get("SPA_background");
  TDirectory* dir2_10000 = (TDirectory*) f2_10000->Get("SPA_background");

  TDirectory* dir20_25000 = (TDirectory*) f20_25000->Get("SPA_background");
  TDirectory* dir19_25000 = (TDirectory*) f19_25000->Get("SPA_background");
  TDirectory* dir18_25000 = (TDirectory*) f18_25000->Get("SPA_background");
  TDirectory* dir16_25000 = (TDirectory*) f16_25000->Get("SPA_background");
  TDirectory* dir14_25000 = (TDirectory*) f14_25000->Get("SPA_background");
  TDirectory* dir12_25000 = (TDirectory*) f12_25000->Get("SPA_background");
  TDirectory* dir10_25000 = (TDirectory*) f10_25000->Get("SPA_background");
  TDirectory* dir8_25000 = (TDirectory*) f8_25000->Get("SPA_background");
  TDirectory* dir6_25000 = (TDirectory*) f6_25000->Get("SPA_background");
  TDirectory* dir4_25000 = (TDirectory*) f4_25000->Get("SPA_background");
  TDirectory* dir2_25000 = (TDirectory*) f2_25000->Get("SPA_background");

  
  
  TFile *outRoot = new TFile("MCDataanalysis.root","RECREATE");

  TF1 *myfitFunc = new TF1("myfitFunc",fitFunc,-100,400,6);

  Float_t sigmas_1[11];
  Float_t means_1[11];
  Float_t entries_1[11];

  Float_t sigmas_10[11];
  Float_t means_10[11];
  Float_t entries_10[11];

  Float_t sigmas_100[11];
  Float_t means_100[11];
  Float_t entries_100[11];

  Float_t sigmas_1000[11];
  Float_t means_1000[11];
  Float_t entries_1000[11];

  Float_t sigmas_10000[11];
  Float_t means_10000[11];
  Float_t entries_10000[11];
  
  Float_t sigmas_25000[11];
  Float_t means_25000[11];
  Float_t entries_25000[11];
  
  TH1F *Histo20_1 = (TH1F*) dir20_1->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo19_1 = (TH1F*) dir19_1->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo18_1 = (TH1F*) dir18_1->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo16_1 = (TH1F*) dir16_1->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo14_1 = (TH1F*) dir14_1->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo12_1 = (TH1F*) dir12_1->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo10_1 = (TH1F*) dir10_1->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo8_1 = (TH1F*) dir8_1->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo6_1 = (TH1F*) dir6_1->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo4_1 = (TH1F*) dir4_1->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo2_1 = (TH1F*) dir2_1->Get("SPA_background_ECal_MissingMass_All");
  
  TH1F *Histo20_10 = (TH1F*) dir20_10->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo19_10 = (TH1F*) dir19_10->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo18_10 = (TH1F*) dir18_10->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo16_10 = (TH1F*) dir16_10->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo14_10 = (TH1F*) dir14_10->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo12_10 = (TH1F*) dir12_10->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo10_10 = (TH1F*) dir10_10->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo8_10 = (TH1F*) dir8_10->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo6_10 = (TH1F*) dir6_10->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo4_10 = (TH1F*) dir4_10->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo2_10 = (TH1F*) dir2_10->Get("SPA_background_ECal_MissingMass_All");
  
  TH1F *Histo20_100 = (TH1F*) dir20_100->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo19_100 = (TH1F*) dir19_100->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo18_100 = (TH1F*) dir18_100->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo16_100 = (TH1F*) dir16_100->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo14_100 = (TH1F*) dir14_100->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo12_100 = (TH1F*) dir12_100->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo10_100 = (TH1F*) dir10_100->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo8_100 = (TH1F*) dir8_100->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo6_100 = (TH1F*) dir6_100->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo4_100 = (TH1F*) dir4_100->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo2_100 = (TH1F*) dir2_100->Get("SPA_background_ECal_MissingMass_All");
  
  TH1F *Histo20_1000 = (TH1F*) dir20_1000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo19_1000 = (TH1F*) dir19_1000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo18_1000 = (TH1F*) dir18_1000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo16_1000 = (TH1F*) dir16_1000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo14_1000 = (TH1F*) dir14_1000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo12_1000 = (TH1F*) dir12_1000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo10_1000 = (TH1F*) dir10_1000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo8_1000 = (TH1F*) dir8_1000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo6_1000 = (TH1F*) dir6_1000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo4_1000 = (TH1F*) dir4_1000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo2_1000 = (TH1F*) dir2_1000->Get("SPA_background_ECal_MissingMass_All");
  
  TH1F *Histo20_10000 = (TH1F*) dir20_10000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo19_10000 = (TH1F*) dir19_10000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo18_10000 = (TH1F*) dir18_10000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo16_10000 = (TH1F*) dir16_10000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo14_10000 = (TH1F*) dir14_10000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo12_10000 = (TH1F*) dir12_10000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo10_10000 = (TH1F*) dir10_10000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo8_10000 = (TH1F*) dir8_10000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo6_10000 = (TH1F*) dir6_10000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo4_10000 = (TH1F*) dir4_10000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo2_10000 = (TH1F*) dir2_10000->Get("SPA_background_ECal_MissingMass_All");
  
  TH1F *Histo20_25000 = (TH1F*) dir20_25000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo19_25000 = (TH1F*) dir19_25000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo18_25000 = (TH1F*) dir18_25000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo16_25000 = (TH1F*) dir16_25000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo14_25000 = (TH1F*) dir14_25000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo12_25000 = (TH1F*) dir12_25000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo10_25000 = (TH1F*) dir10_25000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo8_25000 = (TH1F*) dir8_25000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo6_25000 = (TH1F*) dir6_25000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo4_25000 = (TH1F*) dir4_25000->Get("SPA_background_ECal_MissingMass_All");
  TH1F *Histo2_25000 = (TH1F*) dir2_25000->Get("SPA_background_ECal_MissingMass_All");



  Float_t masses[11];
  
  for(int i=0;i<9;i++){
    if(i==0) masses[i]=2;
    else masses[i]=masses[i-1]+2;
    std::cout<<"Masses["<<i<<"] = "<<masses[i]<<std::endl;
  }

  masses[9] = 19;
  masses[10] = 20;
  std::cout<<"Masses[9] = "<<masses[9]<<std::endl;
  std::cout<<"Masses[10] = "<<masses[10]<<std::endl;
  //Histo12->Fit(myfitFunc,"R");
  //return;

  
  
  // RooFormulaVar zlo("zlo", "mean2_1-2*(sqrt(sigma2_1_1*sigma2_1_1+sigma2_1_2*sigma2_1_2))", RooArgSet(mean2_1,sigma2_1_1,sigma2_1_2));
  // RooFormulaVar zhi("zlo", "mean2_1+2*(sqrt(sigma2_1_1*sigma2_1_1+sigma2_1_2*sigma2_1_2))", RooArgSet(mean2_1,sigma2_1_1,sigma2_1_2));
  
  // std::cout<<"zlo: "<<zlo.getVal()<<" zhi: "<<zhi.getVal()<<std::endl;
  // return;
   
  Double_t fpars2_1[6] = {0.,0.,0.02,200.,-1.28,26.};
  myfitFunc->SetParameters(fpars2_1);
  Histo2_1->Fit(myfitFunc,"R");
  sigmas_1[0] = myfitFunc->GetParameter(5);
  means_1[0] = myfitFunc->GetParameter(4);
  entries_1[0]=myfitFunc->Integral(500+(means_1[0]-2*sigmas_1[0]),500+(means_1[0]+2*sigmas_1[0]))/100000;

  Histo4_1->Fit(myfitFunc,"R");
  sigmas_1[1] = myfitFunc->GetParameter(5);
  means_1[1] = myfitFunc->GetParameter(4);
  entries_1[1]=myfitFunc->Integral(500+(means_1[1]-2*sigmas_1[1]),500+(means_1[1]+2*sigmas_1[1]))/100000;

  Histo6_1->Fit(myfitFunc,"R");
  sigmas_1[2] = myfitFunc->GetParameter(5);
  means_1[2] = myfitFunc->GetParameter(4);
  entries_1[2]=myfitFunc->Integral(500+(means_1[2]-2*sigmas_1[2]),500+(means_1[2]+2*sigmas_1[2]))/100000;

  Histo8_1->Fit(myfitFunc,"R");
  sigmas_1[3] = myfitFunc->GetParameter(5);
  means_1[3] = myfitFunc->GetParameter(4);
  entries_1[3]=myfitFunc->Integral(500+(means_1[3]-2*sigmas_1[3]),500+(means_1[3]+2*sigmas_1[3]))/100000;

  Histo10_1->Fit(myfitFunc,"R");
  sigmas_1[4] = myfitFunc->GetParameter(5);
  means_1[4] = myfitFunc->GetParameter(4);
  entries_1[4]=myfitFunc->Integral(500+(means_1[4]-2*sigmas_1[4]),500+(means_1[4]+2*sigmas_1[4]))/100000;
 
  Double_t fpars12_1[6] = {0.,400.,0.02,400.,137.,19.};
  //myfitFunc->SetParameters(fpars12_1);
  Histo12_1->Fit(myfitFunc,"R");
  sigmas_1[5] = myfitFunc->GetParameter(5);
  means_1[5] = myfitFunc->GetParameter(4);
  entries_1[5]=myfitFunc->Integral(500+(means_1[5]-2*sigmas_1[5]),500+(means_1[5]+2*sigmas_1[5]))/100000;

  Double_t fpars14_1[6] = {0.,0.,0.02,606.,189.,13.};
  myfitFunc->SetParameters(fpars14_1);
  Histo14_1->Fit(myfitFunc,"R");
  sigmas_1[6] = myfitFunc->GetParameter(5);
  means_1[6] = myfitFunc->GetParameter(4);
  entries_1[6]=myfitFunc->Integral(500+(means_1[6]-2*sigmas_1[6]),500+(means_1[6]+2*sigmas_1[6]))/100000;

  Double_t fpars16_1[6] = {0.,0.,0.02,876.,250.,10.};
  myfitFunc->SetParameters(fpars16_1);
  Histo16_1->Fit(myfitFunc,"R");
  sigmas_1[7] = myfitFunc->GetParameter(5);
  means_1[7] = myfitFunc->GetParameter(4);
  entries_1[7]=myfitFunc->Integral(500+(means_1[7]-2*sigmas_1[7]),500+(means_1[7]+2*sigmas_1[7]))/100000;
  
  Double_t fpars18_1[6] = {0.,0.,0.022,1461.,317.,7.};
  myfitFunc->SetParameters(fpars18_1);
  Histo18_1->Fit(myfitFunc,"R");
  sigmas_1[8] = myfitFunc->GetParameter(5);
  means_1[8] = myfitFunc->GetParameter(4);
  entries_1[8]=myfitFunc->Integral(500+(means_1[8]-2*sigmas_1[8]),500+(means_1[8]+2*sigmas_1[8]))/100000;

  Double_t fpars19_1[6] = {0.,0.,0.008,2770.,350.,7.};
  myfitFunc->SetParameters(fpars19_1);
  Histo19_1->Fit(myfitFunc,"R");
  sigmas_1[9] = myfitFunc->GetParameter(5);
  means_1[9] = myfitFunc->GetParameter(4);
  entries_1[9]=myfitFunc->Integral(500+(means_1[9]-2*sigmas_1[9]),500+(means_1[9]+2*sigmas_1[9]))/100000;
  
  Double_t fpars20_1[6] = {0.,0.,0.02,1913.,390.,3.};
  myfitFunc->SetParameters(fpars20_1);
  Histo20_1->Fit(myfitFunc,"R");
  sigmas_1[10] = myfitFunc->GetParameter(5);
  means_1[10] = myfitFunc->GetParameter(4);
  entries_1[10]=myfitFunc->Integral(500+(means_1[10]-2*sigmas_1[10]),500+(means_1[10]+2*sigmas_1[10]))/100000;
    
  //auto gr = new TGraph (11,masses,sigmas_1);
  //gr->Draw();

  Int_t x1_1=int(means_1[0]-2*sigmas_1[0]);
  Int_t x2_1=int(means_1[0]+2*sigmas_1[0]);
  entries_1[0]=Histo2_1->Integral(500+x1_1,500+x2_1)/100000;
  std::cout<<Histo2_1->Integral(int(means_1[0]-2*sigmas_1[0]),int(means_1[0]+2*sigmas_1[0]))<<std::endl;
  std::cout<<int(means_1[0]-2*sigmas_1[0])<<" "<<int(means_1[0]+2*sigmas_1[0])<<" "<<entries_1[0]<<std::endl;
  entries_1[1]=Histo4_1->Integral(500+(means_1[1]-2*sigmas_1[1]),500+(means_1[1]+2*sigmas_1[1]))/100000;
  std::cout<<Histo2_1->Integral(int(means_1[1]-2*sigmas_1[1]),int(means_1[1]+2*sigmas_1[1]))<<std::endl;
  std::cout<<int(means_1[1]-2*sigmas_1[1])<<" "<<int(means_1[1]+2*sigmas_1[1])<<" "<<entries_1[1]<<std::endl;
  
  entries_1[2]=Histo6_1->Integral(500+(means_1[2]-2*sigmas_1[2]),500+(means_1[2]+2*sigmas_1[2]))/100000;
  entries_1[3]=Histo8_1->Integral(500+(means_1[3]-2*sigmas_1[3]),500+(means_1[3]+2*sigmas_1[3]))/100000;
  entries_1[4]=Histo10_1->Integral(500+(means_1[4]-2*sigmas_1[4]),500+(means_1[4]+2*sigmas_1[4]))/100000;
  entries_1[5]=Histo12_1->Integral(500+(means_1[5]-2*sigmas_1[5]),500+(means_1[5]+2*sigmas_1[5]))/100000;
  entries_1[6]=Histo14_1->Integral(500+(means_1[6]-2*sigmas_1[6]),500+(means_1[6]+2*sigmas_1[6]))/100000;
  entries_1[7]=Histo16_1->Integral(500+(means_1[7]-2*sigmas_1[7]),500+(means_1[7]+2*sigmas_1[7]))/100000;
  entries_1[8]=Histo18_1->Integral(500+(means_1[8]-2*sigmas_1[8]),500+(means_1[8]+2*sigmas_1[8]))/100000;
  entries_1[9]=Histo19_1->Integral(500+(means_1[9]-2*sigmas_1[9]),500+(means_1[9]+2*sigmas_1[9]))/100000;
  entries_1[10]=Histo20_1->Integral(500+(means_1[10]-2*sigmas_1[10]),500+(means_1[10]+2*sigmas_1[10]))/100000;

  TCanvas *c1 = new TCanvas();
  //  c1->cd();
  //gStyle->SetOptTitle(0);
  TPaveText *t = new TPaveText(0.0, 0.9, 0.3, 1.0, "M^{2}_{miss} for different A' masses (1 e+ per bunch)");
  t->Draw();
  Histo20_1->SetLineColorAlpha(kBlue+3,0.65);
  Histo20_1->SetTitle( "M^{2}_{miss} for different A' masses (1 e+ per bunch)");
  
  //Histo20_1->SetFillColorAlpha(kBlue-6,0.65);
  Histo20_1->SetLineWidth(3);
  Histo20_1->GetYaxis()->SetTitle("N_{events}");
  Histo20_1->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo19_1->SetLineColorAlpha(kBlue,0.65);
  //Histo19_1->SetFillColorAlpha(kBlue-4,0.65);
  Histo19_1->SetLineWidth(3);
  Histo18_1->SetLineColorAlpha(kCyan+3,0.65);
  //Histo18_1->SetFillColorAlpha(kCyan-6,0.65);
  Histo18_1->SetLineWidth(3);
  Histo16_1->SetLineColorAlpha(kCyan+1,0.65);
  //Histo16_1->SetFillColorAlpha(kCyan-9,0.65);
  Histo16_1->SetLineWidth(3);
  Histo14_1->SetLineColorAlpha(kGreen+3,0.65);
  //Histo14_1->SetFillColorAlpha(kGreen-6,0.65);
  Histo14_1->SetLineWidth(3);
  Histo12_1->SetLineColorAlpha(kGreen+1,0.65);
  //Histo12_1->SetFillColorAlpha(kGreen-9,0.65);
  Histo12_1->SetLineWidth(3);
  Histo10_1->SetLineColorAlpha(kYellow+2,0.65);
  //Histo10_1->SetFillColorAlpha(kYellow-6,0.65);
  Histo10_1->SetLineWidth(3);
  Histo8_1->SetLineColorAlpha(kOrange-3,0.65);
  //Histo8_1->SetFillColorAlpha(kOrange-9,0.65);
  Histo8_1->SetLineWidth(3);
  Histo6_1->SetLineColorAlpha(kRed+2,0.65);
  //Histo6_1->SetFillColorAlpha(kRed-6,0.65);
  Histo6_1->SetLineWidth(3);
  Histo4_1->SetLineColorAlpha(kRed,0.65);
  //Histo4_1->SetFillColorAlpha(kRed-9,0.65);
  Histo4_1->SetLineWidth(3);
  Histo2_1->SetLineColorAlpha(kMagenta+1,0.65);
  //Histo2_1->SetFillColorAlpha(kMagenta-9,0.65);
  Histo2_1->SetLineWidth(3);
    
  Histo20_1->Draw();
  Histo19_1->Draw("same");
  Histo18_1->Draw("same");
  Histo4_1->Draw("same");
  Histo6_1->Draw("same");
  Histo8_1->Draw("same");
  Histo10_1->Draw("same");
  Histo12_1->Draw("same");
  Histo14_1->Draw("same");
  Histo16_1->Draw("same");
  Histo2_1->Draw("same");

  auto legend_1 = new TLegend(0.15,0.5,0.3,0.85);
  //legend->SetHeader("The Legend Title","C"); // option "C" allows to center the header
  legend_1->AddEntry(Histo20_1,"M_{A'} = 20 MeV","l");
  legend_1->AddEntry(Histo19_1,"M_{A'} = 19 MeV","l");
  legend_1->AddEntry(Histo18_1,"M_{A'} = 18 MeV","l");
  legend_1->AddEntry(Histo16_1,"M_{A'} = 16 MeV","l");
  legend_1->AddEntry(Histo14_1,"M_{A'} = 14 MeV","l");
  legend_1->AddEntry(Histo12_1,"M_{A'} = 12 MeV","l");
  legend_1->AddEntry(Histo10_1,"M_{A'} = 10 MeV","l");
  legend_1->AddEntry(Histo8_1,"M_{A'} = 8 MeV","l");
  legend_1->AddEntry(Histo6_1,"M_{A'} = 6 MeV","l");
  legend_1->AddEntry(Histo4_1,"M_{A'} = 4 MeV","l");
  legend_1->AddEntry(Histo2_1,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_1->Draw();

  outRoot->WriteObject(c1,"1e_all");
  
  //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

   
  Double_t fpars2_10[6] = {0.,0.,0.02,233.,0.5,28.};
  myfitFunc->SetParameters(fpars2_10);
  Histo2_10->Fit(myfitFunc,"R");
  sigmas_10[0] = myfitFunc->GetParameter(5);
  means_10[0] = myfitFunc->GetParameter(4);
  entries_10[0]=myfitFunc->Integral(500+(means_10[0]-2*sigmas_10[0]),500+(means_10[0]+2*sigmas_10[0]))/100000;

  Histo4_10->Fit(myfitFunc,"R");
  sigmas_10[1] = myfitFunc->GetParameter(5);
  means_10[1] = myfitFunc->GetParameter(4);
  entries_10[1]=myfitFunc->Integral(500+(means_10[1]-2*sigmas_10[1]),500+(means_10[1]+2*sigmas_10[1]))/100000;

  Histo6_10->Fit(myfitFunc,"R");
  sigmas_10[2] = myfitFunc->GetParameter(5);
  means_10[2] = myfitFunc->GetParameter(4);
  entries_10[2]=myfitFunc->Integral(500+(means_10[2]-2*sigmas_10[2]),500+(means_10[2]+2*sigmas_10[2]))/100000;

  Histo8_10->Fit(myfitFunc,"R");
  sigmas_10[3] = myfitFunc->GetParameter(5);
  means_10[3] = myfitFunc->GetParameter(4);
  entries_10[3]=myfitFunc->Integral(500+(means_10[3]-2*sigmas_10[3]),500+(means_10[3]+2*sigmas_10[3]))/100000;

  Double_t fpars10_10[6] = {0.,0.,0.02,350.,92.,19.};
  myfitFunc->SetParameters(fpars10_10);
  Histo10_10->Fit(myfitFunc,"R");
  sigmas_10[4] = myfitFunc->GetParameter(5);
  means_10[4] = myfitFunc->GetParameter(4);
  entries_10[4]=myfitFunc->Integral(500+(means_10[4]-2*sigmas_10[4]),500+(means_10[4]+2*sigmas_10[4]))/100000;
 
  Double_t fpars12_10[6] = {0.,400.,0.02,400.,137.,19.};
  // myfitFunc->SetParameters(fpars12_10);
  Histo12_10->Fit(myfitFunc,"R");
  sigmas_10[5] = myfitFunc->GetParameter(5);
  means_10[5] = myfitFunc->GetParameter(4);
  entries_10[5]=myfitFunc->Integral(500+(means_10[5]-2*sigmas_10[5]),500+(means_10[5]+2*sigmas_10[5]))/100000;
  
  Histo14_10->Fit(myfitFunc,"R");
  sigmas_10[6] = myfitFunc->GetParameter(5);
  means_10[6] = myfitFunc->GetParameter(4);
  entries_10[6]=myfitFunc->Integral(500+(means_10[6]-2*sigmas_10[6]),500+(means_10[6]+2*sigmas_10[6]))/100000;

  Double_t fpars16_10[6] = {0.,0.,0.02,850.,250.,16.};
  myfitFunc->SetParameters(fpars16_10);
  Histo16_10->Fit(myfitFunc,"R");
  sigmas_10[7] = myfitFunc->GetParameter(5);
  means_10[7] = myfitFunc->GetParameter(4);
  entries_10[7]=myfitFunc->Integral(500+(means_10[7]-2*sigmas_10[7]),500+(means_10[7]+2*sigmas_10[7]))/100000;
  
  Double_t fpars18_10[6] = {0.,0.,0.022,1800.,317.,8.};
  myfitFunc->SetParameters(fpars18_10);
  Histo18_10->Fit(myfitFunc,"R");
  sigmas_10[8] = myfitFunc->GetParameter(5);
  means_10[8] = myfitFunc->GetParameter(4);
  entries_10[8]=myfitFunc->Integral(500+(means_10[8]-2*sigmas_10[8]),500+(means_10[8]+2*sigmas_10[8]))/100000;

  Double_t fpars19_10[6] = {0.,0.,0.008,1823.,350.,7.};
  myfitFunc->SetParameters(fpars19_10);
  Histo19_10->Fit(myfitFunc,"R");
  sigmas_10[9] = myfitFunc->GetParameter(5);
  means_10[9] = myfitFunc->GetParameter(4);
  entries_10[9]=myfitFunc->Integral(500+(means_10[9]-2*sigmas_10[9]),500+(means_10[9]+2*sigmas_10[9]))/100000;
  
  Double_t fpars20_10[6] = {0.,0.,0.02,1834.,390.,3.};
  myfitFunc->SetParameters(fpars20_10);
  Histo20_10->Fit(myfitFunc,"R");
  sigmas_10[10] = myfitFunc->GetParameter(5);
  means_10[10] = myfitFunc->GetParameter(4);
  entries_10[10]=myfitFunc->Integral(500+(means_10[10]-2*sigmas_10[10]),500+(means_10[10]+2*sigmas_10[10]))/100000;
    
  //auto gr = new TGraph (11,masses,sigmas_10);
  //gr->Draw();

  Int_t x1_10=int(means_10[0]-2*sigmas_10[0]);
  Int_t x2_10=int(means_10[0]+2*sigmas_10[0]);
  entries_10[0]=Histo2_10->Integral(500+x1_10,500+x2_10)/100000;
  std::cout<<Histo2_10->Integral(int(means_10[0]-2*sigmas_10[0]),int(means_10[0]+2*sigmas_10[0]))<<std::endl;
  std::cout<<int(means_10[0]-2*sigmas_10[0])<<" "<<int(means_10[0]+2*sigmas_10[0])<<" "<<entries_10[0]<<std::endl;
  entries_10[1]=Histo4_10->Integral(500+(means_10[1]-2*sigmas_10[1]),500+(means_10[1]+2*sigmas_10[1]))/100000;
  std::cout<<Histo2_10->Integral(int(means_10[1]-2*sigmas_10[1]),int(means_10[1]+2*sigmas_10[1]))<<std::endl;
  std::cout<<int(means_10[1]-2*sigmas_10[1])<<" "<<int(means_10[1]+2*sigmas_10[1])<<" "<<entries_10[1]<<std::endl;
  
  entries_10[2]=Histo6_10->Integral(500+(means_10[2]-2*sigmas_10[2]),500+(means_10[2]+2*sigmas_10[2]))/100000;
  entries_10[3]=Histo8_10->Integral(500+(means_10[3]-2*sigmas_10[3]),500+(means_10[3]+2*sigmas_10[3]))/100000;
  entries_10[4]=Histo10_10->Integral(500+(means_10[4]-2*sigmas_10[4]),500+(means_10[4]+2*sigmas_10[4]))/100000;
  entries_10[5]=Histo12_10->Integral(500+(means_10[5]-2*sigmas_10[5]),500+(means_10[5]+2*sigmas_10[5]))/100000;
  entries_10[6]=Histo14_10->Integral(500+(means_10[6]-2*sigmas_10[6]),500+(means_10[6]+2*sigmas_10[6]))/100000;
  entries_10[7]=Histo16_10->Integral(500+(means_10[7]-2*sigmas_10[7]),500+(means_10[7]+2*sigmas_10[7]))/100000;
  entries_10[8]=Histo18_10->Integral(500+(means_10[8]-2*sigmas_10[8]),500+(means_10[8]+2*sigmas_10[8]))/100000;
  entries_10[9]=Histo19_10->Integral(500+(means_10[9]-2*sigmas_10[9]),500+(means_10[9]+2*sigmas_10[9]))/100000;
  entries_10[10]=Histo20_10->Integral(500+(means_10[10]-2*sigmas_10[10]),500+(means_10[10]+2*sigmas_10[10]))/100000;


  TCanvas *c10 = new TCanvas();
  c10->cd();
  
  Histo20_10->SetLineColorAlpha(kBlue+3,0.65);
  Histo20_10->SetTitle( "M^{2}_{miss} for different A' masses (10 e+ per bunch)");
  
  //Histo20_10->SetFillColorAlpha(kBlue-6,0.65);
  Histo20_10->SetLineWidth(3);
  Histo20_10->GetYaxis()->SetTitle("N_{events}");
  Histo20_10->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo19_10->SetLineColorAlpha(kBlue,0.65);
  //Histo19_10->SetFillColorAlpha(kBlue-4,0.65);
  Histo19_10->SetLineWidth(3);
  Histo18_10->SetLineColorAlpha(kCyan+3,0.65);
  //Histo18_10->SetFillColorAlpha(kCyan-6,0.65);
  Histo18_10->SetLineWidth(3);
  Histo16_10->SetLineColorAlpha(kCyan+1,0.65);
  //Histo16_10->SetFillColorAlpha(kCyan-9,0.65);
  Histo16_10->SetLineWidth(3);
  Histo14_10->SetLineColorAlpha(kGreen+3,0.65);
  //Histo14_10->SetFillColorAlpha(kGreen-6,0.65);
  Histo14_10->SetLineWidth(3);
  Histo12_10->SetLineColorAlpha(kGreen+1,0.65);
  //Histo12_10->SetFillColorAlpha(kGreen-9,0.65);
  Histo12_10->SetLineWidth(3);
  Histo10_10->SetLineColorAlpha(kYellow+2,0.65);
  //Histo10_10->SetFillColorAlpha(kYellow-6,0.65);
  Histo10_10->SetLineWidth(3);
  Histo8_10->SetLineColorAlpha(kOrange-3,0.65);
  //Histo8_10->SetFillColorAlpha(kOrange-9,0.65);
  Histo8_10->SetLineWidth(3);
  Histo6_10->SetLineColorAlpha(kRed+2,0.65);
  //Histo6_10->SetFillColorAlpha(kRed-6,0.65);
  Histo6_10->SetLineWidth(3);
  Histo4_10->SetLineColorAlpha(kRed,0.65);
  //Histo4_10->SetFillColorAlpha(kRed-9,0.65);
  Histo4_10->SetLineWidth(3);
  Histo2_10->SetLineColorAlpha(kMagenta+1,0.65);
  //Histo2_10->SetFillColorAlpha(kMagenta-9,0.65);
  Histo2_10->SetLineWidth(3);
    
  Histo20_10->Draw();
  Histo19_10->Draw("same");
  Histo18_10->Draw("same");
  Histo4_10->Draw("same");
  Histo6_10->Draw("same");
  Histo8_10->Draw("same");
  Histo10_10->Draw("same");
  Histo12_10->Draw("same");
  Histo14_10->Draw("same");
  Histo16_10->Draw("same");
  Histo2_10->Draw("same");

  auto legend_10 = new TLegend(0.15,0.5,0.3,0.85);
  //legend->SetHeader("The Legend Title","C"); // option "C" allows to center the header
  legend_10->AddEntry(Histo20_10,"M_{A'} = 20 MeV","l");
  legend_10->AddEntry(Histo19_10,"M_{A'} = 19 MeV","l");
  legend_10->AddEntry(Histo18_10,"M_{A'} = 18 MeV","l");
  legend_10->AddEntry(Histo16_10,"M_{A'} = 16 MeV","l");
  legend_10->AddEntry(Histo14_10,"M_{A'} = 14 MeV","l");
  legend_10->AddEntry(Histo12_10,"M_{A'} = 12 MeV","l");
  legend_10->AddEntry(Histo10_10,"M_{A'} = 10 MeV","l");
  legend_10->AddEntry(Histo8_10,"M_{A'} = 8 MeV","l");
  legend_10->AddEntry(Histo6_10,"M_{A'} = 6 MeV","l");
  legend_10->AddEntry(Histo4_10,"M_{A'} = 4 MeV","l");
  legend_10->AddEntry(Histo2_10,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_10->Draw();


  
  outRoot->WriteObject(c10,"10e_all");
  
  //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


   
  Double_t fpars2_100[6] = {0.,0.,0.02,226.,-3.,22.};
  myfitFunc->SetParameters(fpars2_100);
  Histo2_100->Fit(myfitFunc,"R");
  sigmas_100[0] = myfitFunc->GetParameter(5);
  means_100[0] = myfitFunc->GetParameter(4);
  entries_100[0]=myfitFunc->Integral(500+(means_100[0]-2*sigmas_100[0]),500+(means_100[0]+2*sigmas_100[0]))/100000;

  Histo4_100->Fit(myfitFunc,"R");
  sigmas_100[1] = myfitFunc->GetParameter(5);
  means_100[1] = myfitFunc->GetParameter(4);
  entries_100[1]=myfitFunc->Integral(500+(means_100[1]-2*sigmas_100[1]),500+(means_100[1]+2*sigmas_100[1]))/100000;

  Histo6_100->Fit(myfitFunc,"R");
  sigmas_100[2] = myfitFunc->GetParameter(5);
  means_100[2] = myfitFunc->GetParameter(4);
  entries_100[2]=myfitFunc->Integral(500+(means_100[2]-2*sigmas_100[2]),500+(means_100[2]+2*sigmas_100[2]))/100000;

  Histo8_100->Fit(myfitFunc,"R");
  sigmas_100[3] = myfitFunc->GetParameter(5);
  means_100[3] = myfitFunc->GetParameter(4);
  entries_100[3]=myfitFunc->Integral(500+(means_100[3]-2*sigmas_100[3]),500+(means_100[3]+2*sigmas_100[3]))/100000;

  Histo10_100->Fit(myfitFunc,"R");
  sigmas_100[4] = myfitFunc->GetParameter(5);
  means_100[4] = myfitFunc->GetParameter(4);
  entries_100[4]=myfitFunc->Integral(500+(means_100[4]-2*sigmas_100[4]),500+(means_100[4]+2*sigmas_100[4]))/100000;
 
  Double_t fpars12_100[6] = {0.,400.,0.02,400.,137.,19.};
  //myfitFunc->SetParameters(fpars12_100);
  Histo12_100->Fit(myfitFunc,"R");
  sigmas_100[5] = myfitFunc->GetParameter(5);
  means_100[5] = myfitFunc->GetParameter(4);
  entries_100[5]=myfitFunc->Integral(500+(means_100[5]-2*sigmas_100[5]),500+(means_100[5]+2*sigmas_100[5]))/100000;
  
  Histo14_100->Fit(myfitFunc,"R");
  sigmas_100[6] = myfitFunc->GetParameter(5);
  means_100[6] = myfitFunc->GetParameter(4);
  entries_100[6]=myfitFunc->Integral(500+(means_100[6]-2*sigmas_100[6]),500+(means_100[6]+2*sigmas_100[6]))/100000;

  Double_t fpars16_100[6] = {0.,400.,0.02,777.,250.,16.};
  ////myfitFunc->SetParameters(fpars16_100);
  Histo16_100->Fit(myfitFunc,"R");
  sigmas_100[7] = myfitFunc->GetParameter(5);
  means_100[7] = myfitFunc->GetParameter(4);
  entries_100[7]=myfitFunc->Integral(500+(means_100[7]-2*sigmas_100[7]),500+(means_100[7]+2*sigmas_100[7]))/100000;
  
  Double_t fpars18_100[6] = {0.,400.,0.022,1800.,317.,8.};
  ////myfitFunc->SetParameters(fpars18_100);
  Histo18_100->Fit(myfitFunc,"R");
  sigmas_100[8] = myfitFunc->GetParameter(5);
  means_100[8] = myfitFunc->GetParameter(4);
  entries_100[8]=myfitFunc->Integral(500+(means_100[8]-2*sigmas_100[8]),500+(means_100[8]+2*sigmas_100[8]))/100000;

  Double_t fpars19_100[6] = {0.,400.,0.008,2770.,350.,7.};
  // //myfitFunc->SetParameters(fpars19_100);
  Histo19_100->Fit(myfitFunc,"R");
  sigmas_100[9] = myfitFunc->GetParameter(5);
  means_100[9] = myfitFunc->GetParameter(4);
  entries_100[9]=myfitFunc->Integral(500+(means_100[9]-2*sigmas_100[9]),500+(means_100[9]+2*sigmas_100[9]))/100000;
  
  Double_t fpars20_100[6] = {0.,0.,0.02,1792.,390.,3.};
  myfitFunc->SetParameters(fpars20_100);
  Histo20_100->Fit(myfitFunc,"R");
  sigmas_100[10] = myfitFunc->GetParameter(5);
  means_100[10] = myfitFunc->GetParameter(4);
  entries_100[10]=myfitFunc->Integral(500+(means_100[10]-2*sigmas_100[10]),500+(means_100[10]+2*sigmas_100[10]))/100000;
    
  //auto gr = new TGraph (11,masses,sigmas_100);
  //gr->Draw();

  Int_t x1_100=int(means_100[0]-2*sigmas_100[0]);
  Int_t x2_100=int(means_100[0]+2*sigmas_100[0]);
  entries_100[0]=Histo2_100->Integral(500+x1_100,500+x2_100)/100000;
  std::cout<<Histo2_100->Integral(int(means_100[0]-2*sigmas_100[0]),int(means_100[0]+2*sigmas_100[0]))<<std::endl;
  std::cout<<int(means_100[0]-2*sigmas_100[0])<<" "<<int(means_100[0]+2*sigmas_100[0])<<" "<<entries_100[0]<<std::endl;
  entries_100[1]=Histo4_100->Integral(500+(means_100[1]-2*sigmas_100[1]),500+(means_100[1]+2*sigmas_100[1]))/100000;
  std::cout<<Histo2_100->Integral(int(means_100[1]-2*sigmas_100[1]),int(means_100[1]+2*sigmas_100[1]))<<std::endl;
  std::cout<<int(means_100[1]-2*sigmas_100[1])<<" "<<int(means_100[1]+2*sigmas_100[1])<<" "<<entries_100[1]<<std::endl;
  
  entries_100[2]=Histo6_100->Integral(500+(means_100[2]-2*sigmas_100[2]),500+(means_100[2]+2*sigmas_100[2]))/100000;
  entries_100[3]=Histo8_100->Integral(500+(means_100[3]-2*sigmas_100[3]),500+(means_100[3]+2*sigmas_100[3]))/100000;
  entries_100[4]=Histo10_100->Integral(500+(means_100[4]-2*sigmas_100[4]),500+(means_100[4]+2*sigmas_100[4]))/100000;
  entries_100[5]=Histo12_100->Integral(500+(means_100[5]-2*sigmas_100[5]),500+(means_100[5]+2*sigmas_100[5]))/100000;
  entries_100[6]=Histo14_100->Integral(500+(means_100[6]-2*sigmas_100[6]),500+(means_100[6]+2*sigmas_100[6]))/100000;
  entries_100[7]=Histo16_100->Integral(500+(means_100[7]-2*sigmas_100[7]),500+(means_100[7]+2*sigmas_100[7]))/100000;
  entries_100[8]=Histo18_100->Integral(500+(means_100[8]-2*sigmas_100[8]),500+(means_100[8]+2*sigmas_100[8]))/100000;
  entries_100[9]=Histo19_100->Integral(500+(means_100[9]-2*sigmas_100[9]),500+(means_100[9]+2*sigmas_100[9]))/100000;
  entries_100[10]=Histo20_100->Integral(500+(means_100[10]-2*sigmas_100[10]),500+(means_100[10]+2*sigmas_100[10]))/100000;

  TCanvas *c100 = new TCanvas();
  c100->cd();

  Histo20_100->SetLineColorAlpha(kBlue+3,0.65);
  Histo20_100->SetTitle( "M^{2}_{miss} for different A' masses (100 e+ per bunch)");
  
  //Histo20_100->SetFillColorAlpha(kBlue-6,0.65);
  Histo20_100->SetLineWidth(3);
  Histo20_100->GetYaxis()->SetTitle("N_{events}");
  Histo20_100->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo19_100->SetLineColorAlpha(kBlue,0.65);
  //Histo19_100->SetFillColorAlpha(kBlue-4,0.65);
  Histo19_100->SetLineWidth(3);
  Histo18_100->SetLineColorAlpha(kCyan+3,0.65);
  //Histo18_100->SetFillColorAlpha(kCyan-6,0.65);
  Histo18_100->SetLineWidth(3);
  Histo16_100->SetLineColorAlpha(kCyan+1,0.65);
  //Histo16_100->SetFillColorAlpha(kCyan-9,0.65);
  Histo16_100->SetLineWidth(3);
  Histo14_100->SetLineColorAlpha(kGreen+3,0.65);
  //Histo14_100->SetFillColorAlpha(kGreen-6,0.65);
  Histo14_100->SetLineWidth(3);
  Histo12_100->SetLineColorAlpha(kGreen+1,0.65);
  //Histo12_100->SetFillColorAlpha(kGreen-9,0.65);
  Histo12_100->SetLineWidth(3);
  Histo10_100->SetLineColorAlpha(kYellow+2,0.65);
  //Histo10_100->SetFillColorAlpha(kYellow-6,0.65);
  Histo10_100->SetLineWidth(3);
  Histo8_100->SetLineColorAlpha(kOrange-3,0.65);
  //Histo8_100->SetFillColorAlpha(kOrange-9,0.65);
  Histo8_100->SetLineWidth(3);
  Histo6_100->SetLineColorAlpha(kRed+2,0.65);
  //Histo6_100->SetFillColorAlpha(kRed-6,0.65);
  Histo6_100->SetLineWidth(3);
  Histo4_100->SetLineColorAlpha(kRed,0.65);
  //Histo4_100->SetFillColorAlpha(kRed-9,0.65);
  Histo4_100->SetLineWidth(3);
  Histo2_100->SetLineColorAlpha(kMagenta+1,0.65);
  //Histo2_100->SetFillColorAlpha(kMagenta-9,0.65);
  Histo2_100->SetLineWidth(3);
    
  Histo20_100->Draw();
  Histo19_100->Draw("same");
  Histo18_100->Draw("same");
  Histo4_100->Draw("same");
  Histo6_100->Draw("same");
  Histo8_100->Draw("same");
  Histo10_100->Draw("same");
  Histo12_100->Draw("same");
  Histo14_100->Draw("same");
  Histo16_100->Draw("same");
  Histo2_100->Draw("same");

  auto legend_100 = new TLegend(0.15,0.5,0.3,0.85);
  //legend->SetHeader("The Legend Title","C"); // option "C" allows to center the header
  legend_100->AddEntry(Histo20_100,"M_{A'} = 20 MeV","l");
  legend_100->AddEntry(Histo19_100,"M_{A'} = 19 MeV","l");
  legend_100->AddEntry(Histo18_100,"M_{A'} = 18 MeV","l");
  legend_100->AddEntry(Histo16_100,"M_{A'} = 16 MeV","l");
  legend_100->AddEntry(Histo14_100,"M_{A'} = 14 MeV","l");
  legend_100->AddEntry(Histo12_100,"M_{A'} = 12 MeV","l");
  legend_100->AddEntry(Histo10_100,"M_{A'} = 10 MeV","l");
  legend_100->AddEntry(Histo8_100,"M_{A'} = 8 MeV","l");
  legend_100->AddEntry(Histo6_100,"M_{A'} = 6 MeV","l");
  legend_100->AddEntry(Histo4_100,"M_{A'} = 4 MeV","l");
  legend_100->AddEntry(Histo2_100,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_100->Draw();

  
  outRoot->WriteObject(c100,"100e_all");
  
  //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  RooRealVar x("x", "x", -200, 395);
  RooDataHist rHisto2_1000("rHisto2_1000", "rHisto2_1000", x, Import(*Histo2_1000));
  RooDataHist rHisto4_1000("rHisto4_1000", "rHisto4_1000", x, Import(*Histo4_1000));
  RooDataHist rHisto6_1000("rHisto6_1000", "rHisto6_1000", x, Import(*Histo6_1000));
  RooDataHist rHisto8_1000("rHisto8_1000", "rHisto8_1000", x, Import(*Histo8_1000));
  RooDataHist rHisto10_1000("rHisto10_1000", "rHisto10_1000", x, Import(*Histo10_1000));
  RooDataHist rHisto12_1000("rHisto12_1000", "rHisto12_1000", x, Import(*Histo12_1000));
  RooDataHist rHisto14_1000("rHisto14_1000", "rHisto14_1000", x, Import(*Histo14_1000));
  RooDataHist rHisto16_1000("rHisto16_1000", "rHisto16_1000", x, Import(*Histo16_1000));
  RooDataHist rHisto18_1000("rHisto18_1000", "rHisto18_1000", x, Import(*Histo18_1000));
  RooDataHist rHisto19_1000("rHisto19_1000", "rHisto19_1000", x, Import(*Histo19_1000));
  

  RooPlot *frame = x.frame(Title("Test"));

  
  //RooRealVar alpha("alpha1", "alpha1", 0.0095);
  //RooExponential bkg("bkg1", "Background 1", x, alpha);

  RooRealVar alpha1("alpha1", "alpha1", 0.01, 0.005, 0.0015);
  RooExponential bkg1("bkg1", "Background 1", x, alpha1);
  
  RooRealVar alpha2("alpha2", "alpha2", 0.03,0.0025,0.04);
  RooExponential bkg2("bkg2", "Background 2", x, alpha2);

  RooRealVar bkg1frac("bkg1frac","bkg1frac",0.,1.);
  RooAddPdf bkg("bkg","background",{bkg1, bkg2},bkg1frac);

  RooRealVar bkgfrac("bkgfrac", "fraction of background", 0., 1.);
  
  RooRealVar bkgn2_1000("bkgn2_1000","bkgn2_1000",0.,100000.);
  RooRealVar sign2_1000("sign2_1000","sign2_1000",0.,100000.);
  
  RooRealVar mean2_1000("mean2_1000", "mean2_1000", -1., -10., 10.);
  RooRealVar sigma2_1000("sigma2_1000", "sigma2_1000", 25., 10., 40.);
  RooGaussian gauss2_1000("gauss2_1000", "gauss2_1000", x, mean2_1000, sigma2_1000);
  
  RooAddPdf model2_1000("model2_1000", "model2_1000", {bkg, gauss2_1000}, RooArgList(bkgn2_1000,sign2_1000));

  std::unique_ptr<RooFitResult> fitResult2_1000{model2_1000.fitTo(rHisto2_1000, Save(), PrintLevel(-1))};
  fitResult2_1000->Print();
  
  std::cout<<"TUK: "<<mean2_1000.getVal()<<std::endl;
    
  means_1000[0]=mean2_1000.getVal();
  sigmas_1000[0]=sigma2_1000.getVal();

  std::cout<<"1000e_2MeV mean: "<<means_1000[0]<<" 1000e_2MeV sigma: "<<sigmas_1000[0]<<std::endl;

  
  //rHisto2_1000.plotOn(frame, DrawOption( "HIST" ), LineColor(kMagenta+1));
  //model2_1000.plotOn(frame);

  RooArgSet nset(x);
  cout << "model2_1000_Norm[x] = " << model2_1000.getVal(&nset) << endl;
  std::unique_ptr<RooAbsReal> igx{model2_1000.createIntegral(x)};
  cout << "model2_1000_Int[x] = " << igx->getVal() << endl;
 

  x.setRange("signalaaa",-250.,300.);

  rHisto2_1000.plotOn(frame, DrawOption( "HIST" ), LineColor(kMagenta+1), Range("signalaaa"));
  model2_1000.plotOn(frame, Range("signalaaa"));
  frame->Draw();
  
  //std::unique_ptr<RooAbsReal> igx_sig{model2_1000.createIntegral(x, NormSet(x), Range("signalaaa"))};
  //cout << "model2_1000_Int[x|signal]_Norm[x] = " << igx_sig->getVal() << endl;
   
  //RooAbsReal* igx_siga=model2_1000.createIntegral(RooArgSet(x), NormSet(x), Range("signalaaa"));
  //cout << "model2_1000_Int[x|signal] = " << igx_siga->getVal()*(sign.getVal()+bkgn.getVal()) << endl;
  
   //RooAbsReal* igx_sig = model2_1000.createIntegral(x,x,"signal") ;
   //Double_t ntot=model2_1000.expectedEvents(x);
   //cout<<"Integral = "<<igx_sig->getVal()*ntot<<endl ;

  Float_t Histo2_1000_integral = Histo2_1000->Integral(500-150,500+50);
  cout<<"Integral of the histo = "<<Histo2_1000_integral<<endl ;


  // return;
  
  RooRealVar mean4_1000("mean4_1000", "mean4_1000", 10., 0, 20);
  RooRealVar sigma4_1000("sigma4_1000", "sigma4_1000", 22., 10., 40.);
  RooGaussian gauss4_1000("gauss4_1000", "gauss4_1000", x, mean4_1000, sigma4_1000);

   
  RooRealVar bkgn4_1000("bkgn4_1000","bkgn4_1000",0.,100000.);
  RooRealVar sign4_1000("sign4_1000","sign4_1000",0.,100000.);
  
  RooAddPdf model4_1000("model4_1000", "model4_1000", {bkg, gauss4_1000}, RooArgList(bkgn4_1000,sign4_1000));

  std::unique_ptr<RooFitResult> fitResult4_1000{model4_1000.fitTo(rHisto4_1000, Save(), PrintLevel(-1))};
  fitResult4_1000->Print();
  
  means_1000[1]=mean4_1000.getVal();
  sigmas_1000[1]=sigma4_1000.getVal();

  std::cout<<"1000e_4MeV mean: "<<means_1000[1]<<" 1000e_4MeV sigma: "<<sigmas_1000[1]<<std::endl;

  rHisto4_1000.plotOn(frame, DrawOption( "HIST" ), LineColor(kRed));
  model4_1000.plotOn(frame);
  
  RooRealVar mean6_1000("mean6_1000", "mean6_1000", 29., 20., 40.);
  RooRealVar sigma6_1000("sigma6_1000", "sigma6_1000", 21., 10., 40.);
  RooGaussian gauss6_1000("gauss6_1000", "gauss6_1000", x, mean6_1000, sigma6_1000);
  
  RooRealVar bkgn6_1000("bkgn6_1000","bkgn6_1000",0.,100000.);
  RooRealVar sign6_1000("sign6_1000","sign6_1000",0.,100000.);
  
  RooAddPdf model6_1000("model6_1000", "model6_1000", {bkg, gauss6_1000}, RooArgList(bkgn6_1000,sign6_1000));

  std::unique_ptr<RooFitResult> fitResult6_1000{model6_1000.fitTo(rHisto6_1000, Save(), PrintLevel(-1))};
  fitResult6_1000->Print();

  rHisto6_1000.plotOn(frame, DrawOption( "HIST" ), LineColor(kRed+2));
  model6_1000.plotOn(frame);
  

    
  RooRealVar mean8_1000("mean8_1000", "mean8_1000", 57., 40., 65.);
  RooRealVar sigma8_1000("sigma8_1000", "sigma8_1000", 19., 10., 40.);
  RooGaussian gauss8_1000("gauss8_1000", "gauss8_1000", x, mean8_1000, sigma8_1000);
  
  RooRealVar bkgn8_1000("bkgn8_1000","bkgn8_1000",0.,100000.);
  RooRealVar sign8_1000("sign8_1000","sign8_1000",0.,100000.);
  
  RooAddPdf model8_1000("model8_1000", "model8_1000", {bkg, gauss8_1000}, RooArgList(bkgn8_1000,sign8_1000));

  std::unique_ptr<RooFitResult> fitResult8_1000{model8_1000.fitTo(rHisto8_1000, Save(), PrintLevel(-1))};
  fitResult8_1000->Print();

  rHisto8_1000.plotOn(frame, DrawOption( "HIST" ), LineColor(kOrange-3));
  model8_1000.plotOn(frame);
  

    
  RooRealVar mean10_1000("mean10_1000", "mean10_1000", 93., 80., 110.);
  RooRealVar sigma10_1000("sigma10_1000", "sigma10_1000", 19., 10., 40.);
  RooGaussian gauss10_1000("gauss10_1000", "gauss10_1000", x, mean10_1000, sigma10_1000);
  
  RooRealVar bkgn10_1000("bkgn10_1000","bkgn10_1000",0.,100000.);
  RooRealVar sign10_1000("sign10_1000","sign10_1000",0.,100000.);
  
  RooAddPdf model10_1000("model10_1000", "model10_1000", {bkg, gauss10_1000}, RooArgList(bkgn10_1000,sign10_1000));

  std::unique_ptr<RooFitResult> fitResult10_1000{model10_1000.fitTo(rHisto10_1000, Save(), PrintLevel(-1))};
  fitResult10_1000->Print();

  rHisto10_1000.plotOn(frame, DrawOption( "HIST" ), LineColor(kYellow+2));
  model10_1000.plotOn(frame);
  

  //return;  
  

        
  RooRealVar mean12_1000("mean12_1000", "mean12_1000", 136., 120., 150.);
  RooRealVar sigma12_1000("sigma12_1000", "sigma12_1000", 17., 10., 40.);
  RooGaussian gauss12_1000("gauss12_1000", "gauss12_1000", x, mean12_1000, sigma12_1000);
  
  
  RooRealVar bkgn12_1000("bkgn12_1000","bkgn12_1000",0.,100000.);
  RooRealVar sign12_1000("sign12_1000","sign12_1000",0.,100000.);
  
  RooAddPdf model12_1000("model12_1000", "model12_1000", {bkg, gauss12_1000}, RooArgList(bkgn12_1000,sign12_1000));

  std::unique_ptr<RooFitResult> fitResult12_1000{model12_1000.fitTo(rHisto12_1000, Save(), PrintLevel(-1))};
  fitResult12_1000->Print();

  rHisto12_1000.plotOn(frame, DrawOption( "HIST" ), LineColor(kGreen+1));
  model12_1000.plotOn(frame);
  

  RooRealVar mean14_1000("mean14_1000", "mean14_1000", 188., 120., 200.);
  RooRealVar sigma14_1000("sigma14_1000", "sigma14_1000", 14., 10., 40.);
  RooGaussian gauss14_1000("gauss14_1000", "gauss14_1000", x, mean14_1000, sigma14_1000);
  
  
  RooRealVar bkgn14_1000("bkgn14_1000","bkgn14_1000",0.,100000.);
  RooRealVar sign14_1000("sign14_1000","sign14_1000",0.,100000.);
  
  RooAddPdf model14_1000("model14_1000", "model14_1000", {bkg, gauss14_1000}, RooArgList(bkgn14_1000,sign14_1000));

  std::unique_ptr<RooFitResult> fitResult14_1000{model14_1000.fitTo(rHisto14_1000, Save(), PrintLevel(-1))};
  fitResult14_1000->Print();

  rHisto14_1000.plotOn(frame, DrawOption( "HIST" ), LineColor(kGreen+3));
  model14_1000.plotOn(frame);

  RooRealVar mean16_1000("mean16_1000", "mean16_1000", 150., 120., 350.);
  RooRealVar sigma16_1000("sigma16_1000", "sigma16_1000", 11., 10., 40.);
  RooGaussian gauss16_1000("gauss16_1000", "gauss16_1000", x, mean16_1000, sigma16_1000);
  
  
  RooRealVar bkgn16_1000("bkgn16_1000","bkgn16_1000",0.,100000.);
  RooRealVar sign16_1000("sign16_1000","sign16_1000",0.,100000.);
  
  RooAddPdf model16_1000("model16_1000", "model16_1000", {bkg, gauss16_1000}, RooArgList(bkgn16_1000,sign16_1000));

  std::unique_ptr<RooFitResult> fitResult16_1000{model16_1000.fitTo(rHisto16_1000, Save(), PrintLevel(-1))};
  fitResult16_1000->Print();

  rHisto16_1000.plotOn(frame, DrawOption( "HIST" ), LineColor(kCyan+1));
  model16_1000.plotOn(frame);


  RooRealVar mean18_1000("mean18_1000", "mean18_1000", 354., 120., 450.);
  RooRealVar sigma18_1000("sigma18_1000", "sigma18_1000", 6., 0., 40.);
  RooGaussian gauss18_1000("gauss18_1000", "gauss18_1000", x, mean18_1000, sigma18_1000);
  
  
  RooRealVar bkgn18_1000("bkgn18_1000","bkgn18_1000",0.,100000.);
  RooRealVar sign18_1000("sign18_1000","sign18_1000",0.,100000.);
  
  RooAddPdf model18_1000("model18_1000", "model18_1000", {bkg, gauss18_1000}, RooArgList(bkgn18_1000,sign18_1000));

  std::unique_ptr<RooFitResult> fitResult18_1000{model18_1000.fitTo(rHisto18_1000, Save(), PrintLevel(-1))};
  fitResult18_1000->Print();

  rHisto18_1000.plotOn(frame, DrawOption( "HIST" ), LineColor(kCyan+3));
  model18_1000.plotOn(frame);


  RooRealVar mean19_1000("mean19_1000", "mean19_1000", 392., 120., 450.);
  RooRealVar sigma19_1000("sigma19_1000", "sigma19_1000", 3., 0., 40.);
  RooGaussian gauss19_1000("gauss19_1000", "gauss19_1000", x, mean19_1000, sigma19_1000);
  
  
  RooRealVar bkgn19_1000("bkgn19_1000","bkgn19_1000",0.,100000.);
  RooRealVar sign19_1000("sign19_1000","sign19_1000",0.,100000.);
  
  RooAddPdf model19_1000("model19_1000", "model19_1000", {bkg, gauss19_1000}, RooArgList(bkgn19_1000,sign19_1000));

  std::unique_ptr<RooFitResult> fitResult19_1000{model19_1000.fitTo(rHisto19_1000, Save(), PrintLevel(-1))};
  fitResult19_1000->Print();

  rHisto19_1000.plotOn(frame, DrawOption( "HIST" ), LineColor(kBlue));
  model19_1000.plotOn(frame);

  
  // RooRealVar mean20_1000("mean20_1000", "mean20_1000", 136., 120., 150.);
  // RooRealVar sigma20_1000("sigma20_1000", "sigma20_1000", 17., 10., 40.);
  // RooGaussian gauss20_1000("gauss20_1000", "gauss20_1000", x, mean20_1000, sigma20_1000);
  
  
  // RooRealVar bkgn20_1000("bkgn","bkgn",0.,100000.);
  // RooRealVar sign20_1000("sign","sign",0.,100000.);
  
  // RooAddPdf model20_1000("model20_1000", "model20_1000", {bkg, gauss20_1000}, RooArgList(bkgn20_1000,sign20_1000));

  // std::unique_ptr<RooFitResult> fitResult20_1000{model20_1000.fitTo(rHisto20_1000, Save(), PrintLevel(-1))};
  // fitResult20_1000->Print();

  // rHisto20_1000.plotOn(frame, DrawOption( "HIST" ), LineColor(kOrange-3));
  // model20_1000.plotOn(frame);
  




  
  

  //gauss.fitTo(rHisto2_1000, PrintLevel(-1));
  // rHisto2_1000.plotOn(frame);
  
  //rHisto4_1000.plotOn(frame, DrawOption( "HIST" ), LineColor(kRed)); 
  //rHisto6_1000.plotOn(frame, DrawOption( "HIST" ), LineColor(kRed+2));
  //rHisto8_1000.plotOn(frame, DrawOption( "HIST" ), LineColor(kOrange-3));
  //rHisto10_1000.plotOn(frame, DrawOption( "HIST" ), LineColor(kYellow+2));
  //rHisto12_1000.plotOn(frame, DrawOption( "HIST" ), LineColor(kGreen+1));


  //means_1000[0]
  
  //model4_1000.plotOn(frame);
  //model6_1000.plotOn(frame);
  //model8_1000.plotOn(frame);		       
  //model10_1000.plotOn(frame);		       
  //model12_1000.plotOn(frame);

  outRoot->WriteObject(frame,"TestRooFit");
  //return;

  
  Double_t fpars2_1000[6] = {0.,100.,0.01,200.,-1,25.};
  myfitFunc->SetParameters(fpars2_1000);
  Histo2_1000->Fit(myfitFunc,"R");
  sigmas_1000[0] = myfitFunc->GetParameter(5);
  means_1000[0] = myfitFunc->GetParameter(4);
  entries_1000[0]=myfitFunc->Integral(500+(means_1000[0]-2*sigmas_1000[0]),500+(means_1000[0]+2*sigmas_1000[0]))/100000;

  Histo4_1000->Fit(myfitFunc,"R");
  sigmas_1000[1] = myfitFunc->GetParameter(5);
  means_1000[1] = myfitFunc->GetParameter(4);
  entries_1000[1]=myfitFunc->Integral(500+(means_1000[1]-2*sigmas_1000[1]),500+(means_1000[1]+2*sigmas_1000[1]))/100000;

  Histo6_1000->Fit(myfitFunc,"R");
  sigmas_1000[2] = myfitFunc->GetParameter(5);
  means_1000[2] = myfitFunc->GetParameter(4);
  entries_1000[2]=myfitFunc->Integral(500+(means_1000[2]-2*sigmas_1000[2]),500+(means_1000[2]+2*sigmas_1000[2]))/100000;

  Histo8_1000->Fit(myfitFunc,"R");
  sigmas_1000[3] = myfitFunc->GetParameter(5);
  means_1000[3] = myfitFunc->GetParameter(4);
  entries_1000[3]=myfitFunc->Integral(500+(means_1000[3]-2*sigmas_1000[3]),500+(means_1000[3]+2*sigmas_1000[3]))/100000;

  Histo10_1000->Fit(myfitFunc,"R");
  sigmas_1000[4] = myfitFunc->GetParameter(5);
  means_1000[4] = myfitFunc->GetParameter(4);
  entries_1000[4]=myfitFunc->Integral(500+(means_1000[4]-2*sigmas_1000[4]),500+(means_1000[4]+2*sigmas_1000[4]))/100000;
 
  Double_t fpars12_1000[6] = {0.,400.,0.0,400.,137.,19.};
  //myfitFunc->SetParameters(fpars12_1000);
  Histo12_1000->Fit(myfitFunc,"R");
  sigmas_1000[5] = myfitFunc->GetParameter(5);
  means_1000[5] = myfitFunc->GetParameter(4);
  entries_1000[5]=myfitFunc->Integral(500+(means_1000[5]-2*sigmas_1000[5]),500+(means_1000[5]+2*sigmas_1000[5]))/100000;
  
  Histo14_1000->Fit(myfitFunc,"R");
  sigmas_1000[6] = myfitFunc->GetParameter(5);
  means_1000[6] = myfitFunc->GetParameter(4);
  entries_1000[6]=myfitFunc->Integral(500+(means_1000[6]-2*sigmas_1000[6]),500+(means_1000[6]+2*sigmas_1000[6]))/100000;

  Double_t fpars16_1000[6] = {0.,400.,0.02,777.,250.,16.};
  //myfitFunc->SetParameters(fpars16_1000);
  Histo16_1000->Fit(myfitFunc,"R");
  sigmas_1000[7] = myfitFunc->GetParameter(5);
  means_1000[7] = myfitFunc->GetParameter(4);
  entries_1000[7]=myfitFunc->Integral(500+(means_1000[7]-2*sigmas_1000[7]),500+(means_1000[7]+2*sigmas_1000[7]))/100000;
  
  Double_t fpars18_1000[6] = {0.,400.,0.022,1800.,317.,8.};
  //myfitFunc->SetParameters(fpars18_1000);
  Histo18_1000->Fit(myfitFunc,"R");
  sigmas_1000[8] = myfitFunc->GetParameter(5);
  means_1000[8] = myfitFunc->GetParameter(4);
  entries_1000[8]=myfitFunc->Integral(500+(means_1000[8]-2*sigmas_1000[8]),500+(means_1000[8]+2*sigmas_1000[8]))/100000;

  Double_t fpars19_1000[6] = {0.,400.,0.008,2770.,350.,7.};
  //myfitFunc->SetParameters(fpars19_1000);
  Histo19_1000->Fit(myfitFunc,"R");
  sigmas_1000[9] = myfitFunc->GetParameter(5);
  means_1000[9] = myfitFunc->GetParameter(4);
  entries_1000[9]=myfitFunc->Integral(500+(means_1000[9]-2*sigmas_1000[9]),500+(means_1000[9]+2*sigmas_1000[9]))/100000;
  
  Double_t fpars20_1000[6] = {0.,400.,0.02,1500.,390.,3.};
  //myfitFunc->SetParameters(fpars20_1000);
  Histo20_1000->Fit(myfitFunc,"R");
  sigmas_1000[10] = myfitFunc->GetParameter(5);
  means_1000[10] = myfitFunc->GetParameter(4);
  entries_1000[10]=myfitFunc->Integral(500+(means_1000[10]-2*sigmas_1000[10]),500+(means_1000[10]+2*sigmas_1000[10]))/100000;
    
  //auto gr = new TGraph (11,masses,sigmas_1000);
  //gr->Draw();

  Int_t x1_1000=int(means_1000[0]-2*sigmas_1000[0]);
  Int_t x2_1000=int(means_1000[0]+2*sigmas_1000[0]);
  entries_1000[0]=Histo2_1000->Integral(500+x1_1000,500+x2_1000)/100000;
  std::cout<<Histo2_1000->Integral(int(means_1000[0]-2*sigmas_1000[0]),int(means_1000[0]+2*sigmas_1000[0]))<<std::endl;
  std::cout<<int(means_1000[0]-2*sigmas_1000[0])<<" "<<int(means_1000[0]+2*sigmas_1000[0])<<" "<<entries_1000[0]<<std::endl;
  entries_1000[1]=Histo4_1000->Integral(500+(means_1000[1]-2*sigmas_1000[1]),500+(means_1000[1]+2*sigmas_1000[1]))/100000;
  std::cout<<Histo2_1000->Integral(int(means_1000[1]-2*sigmas_1000[1]),int(means_1000[1]+2*sigmas_1000[1]))<<std::endl;
  std::cout<<int(means_1000[1]-2*sigmas_1000[1])<<" "<<int(means_1000[1]+2*sigmas_1000[1])<<" "<<entries_1000[1]<<std::endl;
  
  entries_1000[2]=Histo6_1000->Integral(500+(means_1000[2]-2*sigmas_1000[2]),500+(means_1000[2]+2*sigmas_1000[2]))/100000;
  entries_1000[3]=Histo8_1000->Integral(500+(means_1000[3]-2*sigmas_1000[3]),500+(means_1000[3]+2*sigmas_1000[3]))/100000;
  entries_1000[4]=Histo10_1000->Integral(500+(means_1000[4]-2*sigmas_1000[4]),500+(means_1000[4]+2*sigmas_1000[4]))/100000;
  entries_1000[5]=Histo12_1000->Integral(500+(means_1000[5]-2*sigmas_1000[5]),500+(means_1000[5]+2*sigmas_1000[5]))/100000;
  entries_1000[6]=Histo14_1000->Integral(500+(means_1000[6]-2*sigmas_1000[6]),500+(means_1000[6]+2*sigmas_1000[6]))/100000;
  entries_1000[7]=Histo16_1000->Integral(500+(means_1000[7]-2*sigmas_1000[7]),500+(means_1000[7]+2*sigmas_1000[7]))/100000;
  entries_1000[8]=Histo18_1000->Integral(500+(means_1000[8]-2*sigmas_1000[8]),500+(means_1000[8]+2*sigmas_1000[8]))/100000;
  entries_1000[9]=Histo19_1000->Integral(500+(means_1000[9]-2*sigmas_1000[9]),500+(means_1000[9]+2*sigmas_1000[9]))/100000;
  entries_1000[10]=Histo20_1000->Integral(500+(means_1000[10]-2*sigmas_1000[10]),500+(means_1000[10]+2*sigmas_1000[10]))/100000;

  TCanvas *c1000 = new TCanvas();
  c1000->cd();
  Histo20_1000->Draw();
  Histo19_1000->Draw("same");
  Histo18_1000->Draw("same");
  Histo4_1000->Draw("same");
  Histo6_1000->Draw("same");
  Histo8_1000->Draw("same");
  Histo10_1000->Draw("same");
  Histo12_1000->Draw("same");
  Histo14_1000->Draw("same");
  Histo16_1000->Draw("same");
  Histo2_1000->Draw("same");

  
  outRoot->WriteObject(c1000,"1000e_all");
  
  //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  RooRealVar x_10000("x_10000", "x_10000", -500, 500);

  RooDataHist rHisto2_10000("rHisto2_10000", "rHisto2_10000", x_10000, Import(*Histo2_10000));
  RooDataHist rHisto4_10000("rHisto4_10000", "rHisto4_10000", x_10000, Import(*Histo4_10000));
  RooDataHist rHisto6_10000("rHisto6_10000", "rHisto6_10000", x_10000, Import(*Histo6_10000));
  RooDataHist rHisto8_10000("rHisto8_10000", "rHisto8_10000", x_10000, Import(*Histo8_10000));
  RooDataHist rHisto10_10000("rHisto10_10000", "rHisto10_10000", x_10000, Import(*Histo10_10000));
  RooDataHist rHisto12_10000("rHisto12_10000", "rHisto12_10000", x_10000, Import(*Histo12_10000));
  RooDataHist rHisto14_10000("rHisto14_10000", "rHisto14_10000", x_10000, Import(*Histo14_10000));
  RooDataHist rHisto16_10000("rHisto16_10000", "rHisto16_10000", x_10000, Import(*Histo16_10000));
  RooDataHist rHisto18_10000("rHisto18_10000", "rHisto18_10000", x_10000, Import(*Histo18_10000));
  RooDataHist rHisto19_10000("rHisto19_10000", "rHisto19_10000", x_10000, Import(*Histo19_10000));
  RooDataHist rHisto20_10000("rHisto20_10000", "rHisto20_10000", x_10000, Import(*Histo20_10000));
  

  RooPlot *frame10000 = x_10000.frame(Title("Test"));
  //rHisto2_10000.plotOn(frame10000, DrawOption( "HIST" ), LineColor(kMagenta+1));
  
  
  //RooRealVar alpha("alpha1", "alpha1", 0.0095);
  //RooExponential bkg("bkg1", "Background 1", x, alpha);

  RooRealVar alpha1_10000("alpha1_10000", "alpha1_10000", 0.001, 0.0005, 1.);
  RooExponential bkg1_10000("bkg1_10000", "Background 1_10000", x_10000, alpha1_10000);
  
  RooRealVar alpha2_10000("alpha2_10000", "alpha2_10000", 0.5,0.0025,1.);
  RooExponential bkg2_10000("bkg2_10000", "Background 2_10000", x_10000, alpha2_10000);
  
  RooRealVar meanbkg_10000_2("meanbkg_10000_2", "meanbkg_10000_2", 400, 300., 200000.);
  RooRealVar sigmabkg_10000_2("sigmabkg_10000_2", "sigmabkg_10000_2", 500., 0., 2000.);
  RooGaussian gaussbkg_10000_2("gaussbkg_10000_2", "gaussbkg_10000_2", x_10000, meanbkg_10000_2, sigmabkg_10000_2);

  RooRealVar bkg1frac_10000("bkg1frac_10000","bkg1frac_10000", 0.5,0.,1.);
  RooAddPdf bkg_10000("bkg_10000","background_10000",{bkg1_10000, bkg2_10000},bkg1frac_10000);

  RooRealVar bkgfrac_10000("bkgfrac_10000", "fraction of background_10000", 0., 1.);
  
  RooRealVar bkgn2_10000("bkgn2_10000","bkgn2_10000",0.,1000000.);
  RooRealVar sign2_10000("sign2_10000","sign2_10000",0.,100000.);
  
  RooRealVar mean2_10000("mean2_10000", "mean2_10000", 0.3, -5., 5.);
  RooRealVar sigma2_10000_1("sigma2_10000_1", "sigma2_10000_1", 5., 0., 50.);
  RooGaussian gauss2_10000_1("gauss2_10000_1", "gauss2_10000_1", x_10000, mean2_10000, sigma2_10000_1);

  
  RooRealVar sigma2_10000_2("sigma2_10000_2", "sigma2_10000_2",100., 20., 300.);
  RooGaussian gauss2_10000_2("gauss2_10000_2", "gauss2_10000_2", x_10000, mean2_10000, sigma2_10000_2);
  
  RooRealVar signal1frac_10000_2("signal1frac_10000_2","signal1frac_10000_2",0.,1.);
  RooAddPdf signal_10000_2("signal_10000_2","signal_10000_2",{gauss2_10000_1, gauss2_10000_2},signal1frac_10000_2);
  
  
  RooAddPdf model2_10000("model2_10000", "model2_10000", {gaussbkg_10000_2,signal_10000_2}, RooArgList(bkgn2_10000,sign2_10000));
  x_10000.setRange("signalaaa",-250.,390.);
  std::unique_ptr<RooFitResult> fitResult2_10000{model2_10000.fitTo(rHisto2_10000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult2_10000->Print();
  
  //std::cout<<"TUK: "<<mean2_10000.getVal()<<std::endl;
    
  //means_10000[0]=mean2_10000.getVal();
  // sigmas_10000[0]=sigma2_10000.getVal();

  //std::cout<<"10000e_2MeV mean: "<<means_10000[0]<<" 10000e_2MeV sigma: "<<sigmas_10000[0]<<std::endl;

  
  rHisto2_10000.plotOn(frame10000, DrawOption( "HIST" ), LineColor(kMagenta+1));
  model2_10000.plotOn(frame10000);

  // RooArgSet nset(x);
  // cout << "model2_10000_Norm[x] = " << model2_10000.getVal(&nset) << endl;
  // std::unique_ptr<RooAbsReal> igx{model2_10000.createIntegral(x)};
  // cout << "model2_10000_Int[x] = " << igx->getVal() << endl;
 

  // x_10000.setRange("signalaaa",-250.,300.);

  //rHisto2_10000.plotOn(frame10000, DrawOption( "HIST" ), LineColor(kMagenta+1));
  //model2_10000.plotOn(frame10000);
  
  
  //std::unique_ptr<RooAbsReal> igx_sig{model2_10000.createIntegral(x, NormSet(x), Range("signalaaa"))};
  //cout << "model2_10000_Int[x|signal]_Norm[x] = " << igx_sig->getVal() << endl;
   
  //RooAbsReal* igx_siga=model2_10000.createIntegral(RooArgSet(x), NormSet(x), Range("signalaaa"));
  //cout << "model2_10000_Int[x|signal] = " << igx_siga->getVal()*(sign.getVal()+bkgn.getVal()) << endl;
  
   //RooAbsReal* igx_sig = model2_10000.createIntegral(x,x,"signal") ;
   //Double_t ntot=model2_10000.expectedEvents(x);
   //cout<<"Integral = "<<igx_sig->getVal()*ntot<<endl ;

  Float_t Histo2_10000_integral = Histo2_10000->Integral(500-150,500+50);
  cout<<"Integral of the histo = "<<Histo2_10000_integral<<endl ;

  //return;

  RooRealVar meanbkg_10000_4("meanbkg_10000_4", "meanbkg_10000_4", 400, 300., 200000.);
  RooRealVar sigmabkg_10000_4("sigmabkg_10000_4", "sigmabkg_10000_4", 500., 0., 2000.);
  RooGaussian gaussbkg_10000_4("gaussbkg_10000_4", "gaussbkg_10000_4", x_10000, meanbkg_10000_4, sigmabkg_10000_4);

  
  RooRealVar mean4_10000("mean4_10000", "mean4_10000", 10., 0, 20);
  RooRealVar sigma4_10000_1("sigma4_10000_1", "sigma4_10000_1", 5., 0., 50.);
  RooGaussian gauss4_10000_1("gauss4_10000_1", "gauss4_10000_1", x_10000, mean4_10000, sigma4_10000_1);
  RooRealVar sigma4_10000_2("sigma4_10000_2", "sigma4_10000_2", 100., 20., 300.);
  RooGaussian gauss4_10000_2("gauss4_10000_2", "gauss4_10000_2", x_10000, mean4_10000, sigma4_10000_2);
  
  RooRealVar signal1frac_10000_4("signal1frac_10000_4","signal1frac_10000_4",0.,1.);
  RooAddPdf signal_10000_4("signal_10000_4","signal_10000_4",{gauss4_10000_1, gauss4_10000_2},signal1frac_10000_4);
  
   
  RooRealVar bkgn4_10000("bkgn4_10000","bkgn4_10000",0.,1000000.);
  RooRealVar sign4_10000("sign4_10000","sign4_10000",0.,100000.);
  
  RooAddPdf model4_10000("model4_10000", "model4_10000", {gaussbkg_10000_4, signal_10000_4}, RooArgList(bkgn4_10000,sign4_10000));

  std::unique_ptr<RooFitResult> fitResult4_10000{model4_10000.fitTo(rHisto4_10000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult4_10000->Print();
  
  //means_10000[1]=mean4_10000.getVal();
  //sigmas_10000[1]=sigma4_10000.getVal();

  //std::cout<<"1000e_4MeV mean: "<<means_10000[1]<<" 1000e_4MeV sigma: "<<sigmas_10000[1]<<std::endl;

  rHisto4_10000.plotOn(frame10000, DrawOption( "HIST" ), LineColor(kRed));
  model4_10000.plotOn(frame10000);
  //frame10000->Draw();
  //outRoot->WriteObject(frame10000,"TestRooFit2");
  //return;



  RooRealVar meanbkg_10000_6("meanbkg_10000_6", "meanbkg_10000_6", 400, 300., 200000.);
  RooRealVar sigmabkg_10000_6("sigmabkg_10000_6", "sigmabkg_10000_6", 500., 0., 2000.);
  RooGaussian gaussbkg_10000_6("gaussbkg_10000_6", "gaussbkg_10000_6", x_10000, meanbkg_10000_6, sigmabkg_10000_6);
  
  RooRealVar mean6_10000("mean6_10000", "mean6_10000", 29., 20., 40.);
  RooRealVar sigma6_10000_1("sigma6_10000_1", "sigma6_10000_1", 5., 0., 50.);
  RooGaussian gauss6_10000_1("gauss6_10000_1", "gauss6_10000_1", x_10000, mean6_10000, sigma6_10000_1);
  RooRealVar sigma6_10000_2("sigma6_10000_2", "sigma6_10000_2", 100., 20., 300.);
  RooGaussian gauss6_10000_2("gauss6_10000_2", "gauss6_10000_2", x_10000, mean6_10000, sigma6_10000_2);

  RooRealVar signal1frac_10000_6("signal1frac_10000_6","signal1frac_10000_6",0.,1.);
  RooAddPdf signal_10000_6("signal_10000_6","signal_10000_6",{gauss6_10000_1, gauss6_10000_2},signal1frac_10000_6);
  
   
  RooRealVar bkgn6_10000("bkgn6_10000","bkgn6_10000",0.,1000000.);
  RooRealVar sign6_10000("sign6_10000","sign6_10000",0.,100000.);
  
  RooAddPdf model6_10000("model6_10000", "model6_10000", {gaussbkg_10000_6, signal_10000_6}, RooArgList(bkgn6_10000,sign6_10000));

  std::unique_ptr<RooFitResult> fitResult6_10000{model6_10000.fitTo(rHisto6_10000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult6_10000->Print();

  rHisto6_10000.plotOn(frame10000, DrawOption( "HIST" ), LineColor(kRed+2));
  model6_10000.plotOn(frame10000);
  //frame10000->Draw();
  //outRoot->WriteObject(frame10000,"TestRooFit2");
  //return;

  RooRealVar meanbkg_10000_8("meanbkg_10000_8", "meanbkg_10000_8", 400, 300., 200000.);
  RooRealVar sigmabkg_10000_8("sigmabkg_10000_8", "sigmabkg_10000_8", 500., 0., 2000.);
  RooGaussian gaussbkg_10000_8("gaussbkg_10000_8", "gaussbkg_10000_8", x_10000, meanbkg_10000_8, sigmabkg_10000_8);
  

  RooRealVar mean8_10000("mean8_10000", "mean8_10000", 57., 40., 65.);
  RooRealVar sigma8_10000_1("sigma8_10000_1", "sigma8_10000_1", 5., 0., 50.);
  RooGaussian gauss8_10000_1("gauss8_10000_1", "gauss8_10000_1", x_10000, mean8_10000, sigma8_10000_1);
  RooRealVar sigma8_10000_2("sigma8_10000_2", "sigma8_10000_2", 100., 20., 300.);
  RooGaussian gauss8_10000_2("gauss8_10000_2", "gauss8_10000_2", x_10000, mean8_10000, sigma8_10000_2);

  RooRealVar signal1frac_10000_8("signal1frac_10000_8","signal1frac_10000_8",0.,1.);
  RooAddPdf signal_10000_8("signal_10000_8","signal_10000_8",{gauss8_10000_1, gauss8_10000_2},signal1frac_10000_8);
  
  RooRealVar bkgn8_10000("bkgn8_10000","bkgn8_10000",0.,1000000.);
  RooRealVar sign8_10000("sign8_10000","sign8_10000",0.,100000.);
  
  RooAddPdf model8_10000("model8_10000", "model8_10000", {gaussbkg_10000_8, signal_10000_8}, RooArgList(bkgn8_10000,sign8_10000));

  std::unique_ptr<RooFitResult> fitResult8_10000{model8_10000.fitTo(rHisto8_10000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult8_10000->Print();

  rHisto8_10000.plotOn(frame10000, DrawOption( "HIST" ), LineColor(kOrange-3));
  model8_10000.plotOn(frame10000);
  // frame10000->Draw();
  // outRoot->WriteObject(frame10000,"TestRooFit2");
  // return;
  
  RooRealVar meanbkg_10000_10("meanbkg_10000_10", "meanbkg_10000_10", 400, 300., 200000.);
  RooRealVar sigmabkg_10000_10("sigmabkg_10000_10", "sigmabkg_10000_10", 500., 0., 2000.);
  RooGaussian gaussbkg_10000_10("gaussbkg_10000_10", "gaussbkg_10000_10", x_10000, meanbkg_10000_10, sigmabkg_10000_10);
    
  RooRealVar mean10_10000("mean10_10000", "mean10_10000", 93., 80., 110.);
  RooRealVar sigma10_10000_1("sigma10_10000_1", "sigma10_10000_1", 5., 0., 50.);
  RooGaussian gauss10_10000_1("gauss10_10000_1", "gauss10_10000_1", x_10000, mean10_10000, sigma10_10000_1);
  RooRealVar sigma10_10000_2("sigma10_10000_2", "sigma10_10000_2", 100., 20., 300.);
  RooGaussian gauss10_10000_2("gauss10_10000_2", "gauss10_10000_2", x_10000, mean10_10000, sigma10_10000_2);
  
  RooRealVar signal1frac_10000_10("signal1frac_10000_10","signal1frac_10000_10",0.,1.);
  RooAddPdf signal_10000_10("signal_10000_10","signal_10000_10",{gauss10_10000_1, gauss10_10000_2},signal1frac_10000_10);
  
  RooRealVar bkgn10_10000("bkgn10_10000","bkgn10_10000",0.,1000000.);
  RooRealVar sign10_10000("sign10_10000","sign10_10000",0.,100000.);
  
  RooAddPdf model10_10000("model10_10000", "model10_10000", {gaussbkg_10000_10, signal_10000_10}, RooArgList(bkgn10_10000,sign10_10000));

  std::unique_ptr<RooFitResult> fitResult10_10000{model10_10000.fitTo(rHisto10_10000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult10_10000->Print();

  rHisto10_10000.plotOn(frame10000, DrawOption( "HIST" ), LineColor(kYellow+2));
  model10_10000.plotOn(frame10000);
  // frame10000->Draw();
  // outRoot->WriteObject(frame10000,"TestRooFit2");

  // return;  
  
  RooRealVar meanbkg_10000_12("meanbkg_10000_12", "meanbkg_10000_12", 400, 300., 200000.);
  RooRealVar sigmabkg_10000_12("sigmabkg_10000_12", "sigmabkg_10000_12", 500., 0., 2000.);
  RooGaussian gaussbkg_10000_12("gaussbkg_10000_12", "gaussbkg_10000_12", x_10000, meanbkg_10000_12, sigmabkg_10000_12);
       
  RooRealVar mean12_10000("mean12_10000", "mean12_10000", 136., 120., 150.);
  RooRealVar sigma12_10000_1("sigma12_10000_1", "sigma12_10000_1", 5., 0., 50.);
  RooGaussian gauss12_10000_1("gauss12_10000_1", "gauss12_10000_1", x_10000, mean12_10000, sigma12_10000_1);
  RooRealVar sigma12_10000_2("sigma12_10000_2", "sigma12_10000_2", 100., 20., 300.);
  RooGaussian gauss12_10000_2("gauss12_10000_2", "gauss12_10000_2", x_10000, mean12_10000, sigma12_10000_2);

  RooRealVar signal1frac_10000_12("signal1frac_10000_12","signal1frac_10000_12",0.,1.);
  RooAddPdf signal_10000_12("signal_10000_12","signal_10000_12",{gauss12_10000_1, gauss12_10000_2},signal1frac_10000_12);
 
  RooRealVar bkgn12_10000("bkgn12_10000","bkgn12_10000",0.,1000000.);
  RooRealVar sign12_10000("sign12_10000","sign12_10000",0.,100000.);
  
  RooAddPdf model12_10000("model12_10000", "model12_10000", {gaussbkg_10000_12, signal_10000_12}, RooArgList(bkgn12_10000,sign12_10000));

  std::unique_ptr<RooFitResult> fitResult12_10000{model12_10000.fitTo(rHisto12_10000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult12_10000->Print();

  rHisto12_10000.plotOn(frame10000, DrawOption( "HIST" ), LineColor(kGreen+1));
  model12_10000.plotOn(frame10000);
  // frame10000->Draw();
  // outRoot->WriteObject(frame10000,"TestRooFit2");
  // return;
  
  
  RooRealVar meanbkg_10000_14("meanbkg_10000_14", "meanbkg_10000_14", 400, 300., 200000.);
  RooRealVar sigmabkg_10000_14("sigmabkg_10000_14", "sigmabkg_10000_14", 500., 0., 2000.);
  RooGaussian gaussbkg_10000_14("gaussbkg_10000_14", "gaussbkg_10000_14", x_10000, meanbkg_10000_14, sigmabkg_10000_14);
    
  RooRealVar mean14_10000("mean14_10000", "mean14_10000", 188., 120., 200.);
  RooRealVar sigma14_10000_1("sigma14_10000_1", "sigma14_10000_1", 5., 0., 50.);
  RooGaussian gauss14_10000_1("gauss14_10000_1", "gauss14_10000_1", x_10000, mean14_10000, sigma14_10000_1);
  RooRealVar sigma14_10000_2("sigma14_10000_2", "sigma14_10000_2", 100., 20., 300.);
  RooGaussian gauss14_10000_2("gauss14_10000_2", "gauss14_10000_2", x_10000, mean14_10000, sigma14_10000_2);

  RooRealVar signal1frac_10000_14("signal1frac_10000_14","signal1frac_10000_14",0.,1.);
  RooAddPdf signal_10000_14("signal_10000_14","signal_10000_14",{gauss14_10000_1, gauss14_10000_2},signal1frac_10000_14);
 
  
  RooRealVar bkgn14_10000("bkgn14_10000","bkgn14_10000",0.,1000000.);
  RooRealVar sign14_10000("sign14_10000","sign14_10000",0.,100000.);
  
  RooAddPdf model14_10000("model14_10000", "model14_10000", {gaussbkg_10000_14, signal_10000_14}, RooArgList(bkgn14_10000,sign14_10000));

  std::unique_ptr<RooFitResult> fitResult14_10000{model14_10000.fitTo(rHisto14_10000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult14_10000->Print();

  rHisto14_10000.plotOn(frame10000, DrawOption( "HIST" ), LineColor(kGreen+3));
  model14_10000.plotOn(frame10000);


  x_10000.setRange("signala",140.,320.);
  
  RooRealVar meanbkg_10000_16("meanbkg_10000_16", "meanbkg_10000_16", 400, 400., 200000.);
  RooRealVar sigmabkg_10000_16("sigmabkg_10000_16", "sigmabkg_10000_16", 100., 0., 2000.);
  RooGaussian gaussbkg_10000_16("gaussbkg_10000_16", "gaussbkg_10000_16", x_10000, meanbkg_10000_16, sigmabkg_10000_16);
  


  RooRealVar mean16_10000("mean16_10000", "mean16_10000", 240., 120., 350.);
  RooRealVar sigma16_10000_1("sigma16_10000_1", "sigma16_10000_1", 5., 0., 50.);
  RooGaussian gauss16_10000_1("gauss16_10000_1", "gauss16_10000_1", x_10000, mean16_10000, sigma16_10000_1);
  RooRealVar sigma16_10000_2("sigma16_10000_2", "sigma16_10000_2", 100., 20., 500.);
  RooGaussian gauss16_10000_2("gauss16_10000_2", "gauss16_10000_2", x_10000, mean16_10000, sigma16_10000_2);
  

  RooRealVar signal1frac_10000_16("signal1frac_10000_16","signal1frac_10000_16",0.,1.);
  RooAddPdf signal_10000_16("signal_10000_16","signal_10000_16",{gauss16_10000_1, gauss16_10000_2},signal1frac_10000_16);
  
  
  RooRealVar bkgn16_10000("bkgn16_10000","bkgn16_10000",0.,1000000.);
  RooRealVar sign16_10000("sign16_10000","sign16_10000",0.,100000.);
  
  RooAddPdf model16_10000("model16_10000", "model16_10000", {gaussbkg_10000_16, signal_10000_16}, RooArgList(bkgn16_10000,sign16_10000));

  std::unique_ptr<RooFitResult> fitResult16_10000{model16_10000.fitTo(rHisto16_10000, Save(), PrintLevel(-1), Range("signala"))};
  
  fitResult16_10000->Print();
  RooPlot *frame10000_2 = x_10000.frame(Title("Tes222t"));

  rHisto16_10000.plotOn(frame10000_2, DrawOption( "HIST" ), LineColor(kCyan+1));
  model16_10000.plotOn(frame10000_2, Range("signala"));
  frame10000_2->Draw();
  // outRoot->WriteObject(frame10000,"TestRooFit2");
  //return;

  
  x_10000.setRange("signalaa",240.,395.);
  
  RooRealVar meanbkg_10000_18("meanbkg_10000_18", "meanbkg_10000_18", 400, 400., 200000.);
  RooRealVar sigmabkg_10000_18("sigmabkg_10000_18", "sigmabkg_10000_18", 500., 0., 2000.);
  RooGaussian gaussbkg_10000_18("gaussbkg_10000_18", "gaussbkg_10000_18", x_10000, meanbkg_10000_18, sigmabkg_10000_18);
  

  RooRealVar mean18_10000("mean18_10000", "mean18_10000", 300., 120., 350.);
  RooRealVar sigma18_10000_1("sigma18_10000_1", "sigma18_10000_1", 5., 0., 50.);
  RooGaussian gauss18_10000_1("gauss18_10000_1", "gauss18_10000_1", x_10000, mean18_10000, sigma18_10000_1);
  RooRealVar sigma18_10000_2("sigma18_10000_2", "sigma18_10000_2", 100., 10., 300.);
  RooGaussian gauss18_10000_2("gauss18_10000_2", "gauss18_10000_2", x_10000, mean18_10000, sigma18_10000_2);

  RooRealVar signal1frac_10000_18("signal1frac_10000_18","signal1frac_10000_18",0.,1.);
  RooAddPdf signal_10000_18("signal_10000_18","signal_10000_18",{gauss18_10000_1, gauss18_10000_2},signal1frac_10000_18);
  
  RooRealVar bkgn18_10000("bkgn18_10000","bkgn18_10000",0.,10000000.);
  RooRealVar sign18_10000("sign18_10000","sign18_10000",0.,1000000.);
  
  RooAddPdf model18_10000("model18_10000", "model18_10000", {gaussbkg_10000_18, signal_10000_18}, RooArgList(bkgn18_10000,sign18_10000));

  std::unique_ptr<RooFitResult> fitResult18_10000{model18_10000.fitTo(rHisto18_10000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult18_10000->Print();

  rHisto18_10000.plotOn(frame10000, DrawOption( "HIST" ), LineColor(kCyan+3));
  model18_10000.plotOn(frame10000);
  //return;


  RooRealVar meanbkg_10000_19("meanbkg_10000_19", "meanbkg_10000_19", 400, 400., 200000.);
  RooRealVar sigmabkg_10000_19("sigmabkg_10000_19", "sigmabkg_10000_19", 500., 0., 2000.);
  RooGaussian gaussbkg_10000_19("gaussbkg_10000_19", "gaussbkg_10000_19", x_10000, meanbkg_10000_19, sigmabkg_10000_19);
  

  RooRealVar mean19_10000("mean19_10000", "mean19_10000", 350., 120., 450.);
  RooRealVar sigma19_10000_1("sigma19_10000_1", "sigma19_10000_1", 5., 0., 50.);
  RooGaussian gauss19_10000_1("gauss19_10000_1", "gauss19_10000_1", x_10000, mean19_10000, sigma19_10000_1);
  RooRealVar sigma19_10000_2("sigma19_10000_2", "sigma19_10000_2", 100., 5., 300.);
  RooGaussian gauss19_10000_2("gauss19_10000_2", "gauss19_10000_2", x_10000, mean19_10000, sigma19_10000_2);

  RooRealVar signal1frac_10000_19("signal1frac_10000_19","signal1frac_10000_19",0.,1.);
  RooAddPdf signal_10000_19("signal_10000_19","signal_10000_19",{gauss19_10000_1, gauss19_10000_2},signal1frac_10000_19);

  
  
  RooRealVar bkgn19_10000("bkgn19_10000","bkgn19_10000",0.,10000000.);
  RooRealVar sign19_10000("sign19_10000","sign19_10000",0.,1000000.);
  
  RooAddPdf model19_10000("model19_10000", "model19_10000", {gaussbkg_10000_19, signal_10000_19}, RooArgList(bkgn19_10000,sign19_10000));

  std::unique_ptr<RooFitResult> fitResult19_10000{model19_10000.fitTo(rHisto19_10000, Save(), PrintLevel(-1), Range("signalaa"))};
  fitResult19_10000->Print();


  return;
  rHisto19_10000.plotOn(frame10000, DrawOption( "HIST" ), LineColor(kBlue));
  model19_10000.plotOn(frame10000);


  // RooRealVar meanbkg_10000_20("meanbkg_10000_20", "meanbkg_10000_20", 400, 300., 200000.);
  // RooRealVar sigmabkg_10000_20("sigmabkg_10000_20", "sigmabkg_10000_20", 500., 0., 2000.);
  // RooGaussian gaussbkg_10000_20("gaussbkg_10000_20", "gaussbkg_10000_20", x_10000, meanbkg_10000_20, sigmabkg_10000_20);
  

  // RooRealVar mean20_10000("mean20_10000", "mean20_10000", 400., 120., 550.);
  // RooRealVar sigma20_10000_1("sigma20_10000_1", "sigma20_10000_1", 5., 0., 50.);
  // RooGaussian gauss20_10000_1("gauss20_10000_1", "gauss20_10000_1", x_10000, mean20_10000, sigma20_10000_1);
  // RooRealVar sigma20_10000_2("sigma20_10000_2", "sigma20_10000_2", 100., 20., 300.);
  // RooGaussian gauss20_10000_2("gauss20_10000_2", "gauss20_10000_2", x_10000, mean20_10000, sigma20_10000_2);

  // RooRealVar signal1frac_10000_20("signal1frac_10000_20","signal1frac_10000_20",0.,1.);
  // RooAddPdf signal_10000_20("signal_10000_20","signal_10000_20",{gauss20_10000_1, gauss20_10000_2},signal1frac_10000_20);

  
  
  // RooRealVar bkgn20_10000("bkgn20_10000","bkgn20_10000",0.,10000000.);
  // RooRealVar sign20_10000("sign20_10000","sign20_100000",0.,1000000.);
  
  // RooAddPdf model20_10000("model20_10000", "model20_10000", {gaussbkg_10000_20, signal_10000_20}, RooArgList(bkgn20_10000,sign20_10000));

  // std::unique_ptr<RooFitResult> fitResult20_10000{model20_10000.fitTo(rHisto20_10000, Save(), PrintLevel(-1), Range("signalaaa"))};
  // fitResult20_10000->Print();

  // rHisto20_10000.plotOn(frame10000, DrawOption( "HIST" ), LineColor(kBlue));
  // model20_10000.plotOn(frame10000);
  frame10000->Draw();
  outRoot->WriteObject(frame10000,"TestRooFit2");
  return;

  outRoot->WriteObject(frame10000,"TestRooFit2");
  
  
  Double_t fpars2_10000[6] = {0.,400.,0.02,200.,0.5,28.};
  myfitFunc->SetParameters(fpars2_10000);
  Histo2_10000->Fit(myfitFunc,"R");
  sigmas_10000[0] = myfitFunc->GetParameter(5);
  means_10000[0] = myfitFunc->GetParameter(4);
  entries_10000[0]=myfitFunc->Integral(500+(means_10000[0]-2*sigmas_10000[0]),500+(means_10000[0]+2*sigmas_10000[0]))/100000;

  Histo4_10000->Fit(myfitFunc,"R");
  sigmas_10000[1] = myfitFunc->GetParameter(5);
  means_10000[1] = myfitFunc->GetParameter(4);
  entries_10000[1]=myfitFunc->Integral(500+(means_10000[1]-2*sigmas_10000[1]),500+(means_10000[1]+2*sigmas_10000[1]))/100000;

  Histo6_10000->Fit(myfitFunc,"R");
  sigmas_10000[2] = myfitFunc->GetParameter(5);
  means_10000[2] = myfitFunc->GetParameter(4);
  entries_10000[2]=myfitFunc->Integral(500+(means_10000[2]-2*sigmas_10000[2]),500+(means_10000[2]+2*sigmas_10000[2]))/100000;

  Histo8_10000->Fit(myfitFunc,"R");
  sigmas_10000[3] = myfitFunc->GetParameter(5);
  means_10000[3] = myfitFunc->GetParameter(4);
  entries_10000[3]=myfitFunc->Integral(500+(means_10000[3]-2*sigmas_10000[3]),500+(means_10000[3]+2*sigmas_10000[3]))/100000;

  Histo10_10000->Fit(myfitFunc,"R");
  sigmas_10000[4] = myfitFunc->GetParameter(5);
  means_10000[4] = myfitFunc->GetParameter(4);
  entries_10000[4]=myfitFunc->Integral(500+(means_10000[4]-2*sigmas_10000[4]),500+(means_10000[4]+2*sigmas_10000[4]))/100000;
 
  Double_t fpars12_10000[6] = {0.,400.,0.02,400.,137.,19.};
  myfitFunc->SetParameters(fpars12_10000);
  Histo12_10000->Fit(myfitFunc,"R");
  sigmas_10000[5] = myfitFunc->GetParameter(5);
  means_10000[5] = myfitFunc->GetParameter(4);
  entries_10000[5]=myfitFunc->Integral(500+(means_10000[5]-2*sigmas_10000[5]),500+(means_10000[5]+2*sigmas_10000[5]))/100000;
  
  Histo14_10000->Fit(myfitFunc,"R");
  sigmas_10000[6] = myfitFunc->GetParameter(5);
  means_10000[6] = myfitFunc->GetParameter(4);
  entries_10000[6]=myfitFunc->Integral(500+(means_10000[6]-2*sigmas_10000[6]),500+(means_10000[6]+2*sigmas_10000[6]))/100000;

  Double_t fpars16_10000[6] = {0.,400.,0.02,777.,250.,16.};
  myfitFunc->SetParameters(fpars16_10000);
  Histo16_10000->Fit(myfitFunc,"R");
  sigmas_10000[7] = myfitFunc->GetParameter(5);
  means_10000[7] = myfitFunc->GetParameter(4);
  entries_10000[7]=myfitFunc->Integral(500+(means_10000[7]-2*sigmas_10000[7]),500+(means_10000[7]+2*sigmas_10000[7]))/100000;
  
  Double_t fpars18_10000[6] = {0.,400.,0.022,1800.,317.,8.};
  myfitFunc->SetParameters(fpars18_10000);
  Histo18_10000->Fit(myfitFunc,"R");
  sigmas_10000[8] = myfitFunc->GetParameter(5);
  means_10000[8] = myfitFunc->GetParameter(4);
  entries_10000[8]=myfitFunc->Integral(500+(means_10000[8]-2*sigmas_10000[8]),500+(means_10000[8]+2*sigmas_10000[8]))/100000;

  Double_t fpars19_10000[6] = {0.,400.,0.008,2770.,350.,7.};
  myfitFunc->SetParameters(fpars19_10000);
  Histo19_10000->Fit(myfitFunc,"R");
  sigmas_10000[9] = myfitFunc->GetParameter(5);
  means_10000[9] = myfitFunc->GetParameter(4);
  entries_10000[9]=myfitFunc->Integral(500+(means_10000[9]-2*sigmas_10000[9]),500+(means_10000[9]+2*sigmas_10000[9]))/100000;
  
  Double_t fpars20_10000[6] = {0.,400.,0.02,1500.,390.,3.};
  myfitFunc->SetParameters(fpars20_10000);
  Histo20_10000->Fit(myfitFunc,"R");
  sigmas_10000[10] = myfitFunc->GetParameter(5);
  means_10000[10] = myfitFunc->GetParameter(4);
  entries_10000[10]=myfitFunc->Integral(500+(means_10000[10]-2*sigmas_10000[10]),500+(means_10000[10]+2*sigmas_10000[10]))/100000;
    
  //auto gr = new TGraph (11,masses,sigmas_10000);
  //gr->Draw();

  Int_t x1_10000=int(means_10000[0]-2*sigmas_10000[0]);
  Int_t x2_10000=int(means_10000[0]+2*sigmas_10000[0]);
  entries_10000[0]=Histo2_10000->Integral(500+x1_10000,500+x2_10000)/100000;
  std::cout<<Histo2_10000->Integral(int(means_10000[0]-2*sigmas_10000[0]),int(means_10000[0]+2*sigmas_10000[0]))<<std::endl;
  std::cout<<int(means_10000[0]-2*sigmas_10000[0])<<" "<<int(means_10000[0]+2*sigmas_10000[0])<<" "<<entries_10000[0]<<std::endl;
  entries_10000[1]=Histo4_10000->Integral(500+(means_10000[1]-2*sigmas_10000[1]),500+(means_10000[1]+2*sigmas_10000[1]))/100000;
  std::cout<<Histo2_10000->Integral(int(means_10000[1]-2*sigmas_10000[1]),int(means_10000[1]+2*sigmas_10000[1]))<<std::endl;
  std::cout<<int(means_10000[1]-2*sigmas_10000[1])<<" "<<int(means_10000[1]+2*sigmas_10000[1])<<" "<<entries_10000[1]<<std::endl;
  
  entries_10000[2]=Histo6_10000->Integral(500+(means_10000[2]-2*sigmas_10000[2]),500+(means_10000[2]+2*sigmas_10000[2]))/100000;
  entries_10000[3]=Histo8_10000->Integral(500+(means_10000[3]-2*sigmas_10000[3]),500+(means_10000[3]+2*sigmas_10000[3]))/100000;
  entries_10000[4]=Histo10_10000->Integral(500+(means_10000[4]-2*sigmas_10000[4]),500+(means_10000[4]+2*sigmas_10000[4]))/100000;
  entries_10000[5]=Histo12_10000->Integral(500+(means_10000[5]-2*sigmas_10000[5]),500+(means_10000[5]+2*sigmas_10000[5]))/100000;
  entries_10000[6]=Histo14_10000->Integral(500+(means_10000[6]-2*sigmas_10000[6]),500+(means_10000[6]+2*sigmas_10000[6]))/100000;
  entries_10000[7]=Histo16_10000->Integral(500+(means_10000[7]-2*sigmas_10000[7]),500+(means_10000[7]+2*sigmas_10000[7]))/100000;
  entries_10000[8]=Histo18_10000->Integral(500+(means_10000[8]-2*sigmas_10000[8]),500+(means_10000[8]+2*sigmas_10000[8]))/100000;
  entries_10000[9]=Histo19_10000->Integral(500+(means_10000[9]-2*sigmas_10000[9]),500+(means_10000[9]+2*sigmas_10000[9]))/100000;
  entries_10000[10]=Histo20_10000->Integral(500+(means_10000[10]-2*sigmas_10000[10]),500+(means_10000[10]+2*sigmas_10000[10]))/100000;

  TCanvas *c10000= new TCanvas();
  c10000->cd();
  Histo20_10000->Draw();
  Histo19_10000->Draw("same");
  Histo18_10000->Draw("same");
  Histo4_10000->Draw("same");
  Histo6_10000->Draw("same");
  Histo8_10000->Draw("same");
  Histo10_10000->Draw("same");
  Histo12_10000->Draw("same");
  Histo14_10000->Draw("same");
  Histo16_10000->Draw("same");
  Histo2_10000->Draw("same");

  
  outRoot->WriteObject(c10000,"10000e_all");
  

  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  
   
  Double_t fpars2_25000[6] = {0.,400.,0.02,200.,0.5,28.};
  //myfitFunc->SetParameters(fpars2_25000);
  Histo2_25000->Fit(myfitFunc,"R");
  sigmas_25000[0] = myfitFunc->GetParameter(5);
  means_25000[0] = myfitFunc->GetParameter(4);
  entries_25000[0]=myfitFunc->Integral(500+(means_25000[0]-2*sigmas_25000[0]),500+(means_25000[0]+2*sigmas_25000[0]))/100000;

  Histo4_25000->Fit(myfitFunc,"R");
  sigmas_25000[1] = myfitFunc->GetParameter(5);
  means_25000[1] = myfitFunc->GetParameter(4);
  entries_25000[1]=myfitFunc->Integral(500+(means_25000[1]-2*sigmas_25000[1]),500+(means_25000[1]+2*sigmas_25000[1]))/100000;

  Histo6_25000->Fit(myfitFunc,"R");
  sigmas_25000[2] = myfitFunc->GetParameter(5);
  means_25000[2] = myfitFunc->GetParameter(4);
  entries_25000[2]=myfitFunc->Integral(500+(means_25000[2]-2*sigmas_25000[2]),500+(means_25000[2]+2*sigmas_25000[2]))/100000;

  Histo8_25000->Fit(myfitFunc,"R");
  sigmas_25000[3] = myfitFunc->GetParameter(5);
  means_25000[3] = myfitFunc->GetParameter(4);
  entries_25000[3]=myfitFunc->Integral(500+(means_25000[3]-2*sigmas_25000[3]),500+(means_25000[3]+2*sigmas_25000[3]))/100000;

  Histo10_25000->Fit(myfitFunc,"R");
  sigmas_25000[4] = myfitFunc->GetParameter(5);
  means_25000[4] = myfitFunc->GetParameter(4);
  entries_25000[4]=myfitFunc->Integral(500+(means_25000[4]-2*sigmas_25000[4]),500+(means_25000[4]+2*sigmas_25000[4]))/100000;
 
  Double_t fpars12_25000[6] = {0.,400.,0.02,400.,137.,19.};
  //myfitFunc->SetParameters(fpars12_25000);
  Histo12_25000->Fit(myfitFunc,"R");
  sigmas_25000[5] = myfitFunc->GetParameter(5);
  means_25000[5] = myfitFunc->GetParameter(4);
  entries_25000[5]=myfitFunc->Integral(500+(means_25000[5]-2*sigmas_25000[5]),500+(means_25000[5]+2*sigmas_25000[5]))/100000;
  
  Histo14_25000->Fit(myfitFunc,"R");
  sigmas_25000[6] = myfitFunc->GetParameter(5);
  means_25000[6] = myfitFunc->GetParameter(4);
  entries_25000[6]=myfitFunc->Integral(500+(means_25000[6]-2*sigmas_25000[6]),500+(means_25000[6]+2*sigmas_25000[6]))/100000;

  Double_t fpars16_25000[6] = {0.,400.,0.02,777.,250.,16.};
  //myfitFunc->SetParameters(fpars16_25000);
  Histo16_25000->Fit(myfitFunc,"R");
  sigmas_25000[7] = myfitFunc->GetParameter(5);
  means_25000[7] = myfitFunc->GetParameter(4);
  entries_25000[7]=myfitFunc->Integral(500+(means_25000[7]-2*sigmas_25000[7]),500+(means_25000[7]+2*sigmas_25000[7]))/100000;
  
  Double_t fpars18_25000[6] = {0.,400.,0.022,1800.,317.,8.};
  //myfitFunc->SetParameters(fpars18_25000);
  Histo18_25000->Fit(myfitFunc,"R");
  sigmas_25000[8] = myfitFunc->GetParameter(5);
  means_25000[8] = myfitFunc->GetParameter(4);
  entries_25000[8]=myfitFunc->Integral(500+(means_25000[8]-2*sigmas_25000[8]),500+(means_25000[8]+2*sigmas_25000[8]))/100000;

  Double_t fpars19_25000[6] = {0.,400.,0.008,2770.,350.,7.};
  //myfitFunc->SetParameters(fpars19_25000);
  Histo19_25000->Fit(myfitFunc,"R");
  sigmas_25000[9] = myfitFunc->GetParameter(5);
  means_25000[9] = myfitFunc->GetParameter(4);
  entries_25000[9]=myfitFunc->Integral(500+(means_25000[9]-2*sigmas_25000[9]),500+(means_25000[9]+2*sigmas_25000[9]))/100000;
  
  Double_t fpars20_25000[6] = {0.,400.,0.02,1500.,390.,3.};
  //myfitFunc->SetParameters(fpars20_25000);
  Histo20_25000->Fit(myfitFunc,"R");
  sigmas_25000[10] = myfitFunc->GetParameter(5);
  means_25000[10] = myfitFunc->GetParameter(4);
  entries_25000[10]=myfitFunc->Integral(500+(means_25000[10]-2*sigmas_25000[10]),500+(means_25000[10]+2*sigmas_25000[10]))/100000;
    
  //auto gr = new TGraph (11,masses,sigmas_25000);
  //gr->Draw();

  Int_t x1_25000=int(means_25000[0]-2*sigmas_25000[0]);
  Int_t x2_25000=int(means_25000[0]+2*sigmas_25000[0]);
  entries_25000[0]=Histo2_25000->Integral(500+x1_25000,500+x2_25000)/100000;
  std::cout<<Histo2_25000->Integral(int(means_25000[0]-2*sigmas_25000[0]),int(means_25000[0]+2*sigmas_25000[0]))<<std::endl;
  std::cout<<int(means_25000[0]-2*sigmas_25000[0])<<" "<<int(means_25000[0]+2*sigmas_25000[0])<<" "<<entries_25000[0]<<std::endl;
  entries_25000[1]=Histo4_25000->Integral(500+(means_25000[1]-2*sigmas_25000[1]),500+(means_25000[1]+2*sigmas_25000[1]))/100000;
  std::cout<<Histo2_25000->Integral(int(means_25000[1]-2*sigmas_25000[1]),int(means_25000[1]+2*sigmas_25000[1]))<<std::endl;
  std::cout<<int(means_25000[1]-2*sigmas_25000[1])<<" "<<int(means_25000[1]+2*sigmas_25000[1])<<" "<<entries_25000[1]<<std::endl;
  
  entries_25000[2]=Histo6_25000->Integral(500+(means_25000[2]-2*sigmas_25000[2]),500+(means_25000[2]+2*sigmas_25000[2]))/100000;
  entries_25000[3]=Histo8_25000->Integral(500+(means_25000[3]-2*sigmas_25000[3]),500+(means_25000[3]+2*sigmas_25000[3]))/100000;
  entries_25000[4]=Histo10_25000->Integral(500+(means_25000[4]-2*sigmas_25000[4]),500+(means_25000[4]+2*sigmas_25000[4]))/100000;
  entries_25000[5]=Histo12_25000->Integral(500+(means_25000[5]-2*sigmas_25000[5]),500+(means_25000[5]+2*sigmas_25000[5]))/100000;
  entries_25000[6]=Histo14_25000->Integral(500+(means_25000[6]-2*sigmas_25000[6]),500+(means_25000[6]+2*sigmas_25000[6]))/100000;
  entries_25000[7]=Histo16_25000->Integral(500+(means_25000[7]-2*sigmas_25000[7]),500+(means_25000[7]+2*sigmas_25000[7]))/100000;
  entries_25000[8]=Histo18_25000->Integral(500+(means_25000[8]-2*sigmas_25000[8]),500+(means_25000[8]+2*sigmas_25000[8]))/100000;
  entries_25000[9]=Histo19_25000->Integral(500+(means_25000[9]-2*sigmas_25000[9]),500+(means_25000[9]+2*sigmas_25000[9]))/100000;
  entries_25000[10]=Histo20_25000->Integral(500+(means_25000[10]-2*sigmas_25000[10]),500+(means_25000[10]+2*sigmas_25000[10]))/100000;

  TCanvas *c25000= new TCanvas();
  c25000->cd();
  Histo20_25000->Draw();
  Histo19_25000->Draw("same");
  Histo18_25000->Draw("same");
  Histo4_25000->Draw("same");
  Histo6_25000->Draw("same");
  Histo8_25000->Draw("same");
  Histo10_25000->Draw("same");
  Histo12_25000->Draw("same");
  Histo14_25000->Draw("same");
  Histo16_25000->Draw("same");
  Histo2_25000->Draw("same");

  outRoot->WriteObject(c25000,"25000e_all");
  
  
  //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


  //auto gr2 = new TGraph (11,masses,entries);
  //gr2->Draw();



  outRoot->Write();
  outRoot->Close();
  
}
