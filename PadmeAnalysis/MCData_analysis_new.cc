#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooGaussian.h"
#include "RooExponential.h"
#include "TCanvas.h"
#include "RooPlot.h"
#include "TTree.h"
#include "TH1D.h"
#include "TRandom.h"

using namespace RooFit;

void MCData_analysis(){
  
  //---------------------------------------------- Open Files --------------------------------------------------
  
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

   
  //---------------------------------------------- Build Directories --------------------------------------------------
  
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

  TFile *outRoot = new TFile("MCDataanalysis_new.root","RECREATE");

  Float_t sigmas_1[10];
  Float_t means_1[10];
  Float_t entries_1[10];

  Float_t sigmas_10[10];
  Float_t means_10[10];
  Float_t entries_10[10];

  Float_t sigmas_100[10];
  Float_t means_100[10];
  Float_t entries_100[10];

  Float_t sigmas_1000[10];
  Float_t means_1000[10];
  Float_t entries_1000[10];

  Float_t sigmas_10000[10];
  Float_t means_10000[10];
  Float_t entries_10000[10];
  
  Float_t sigmas_25000[10];
  Float_t means_25000[10];
  Float_t entries_25000[10];
   
  //---------------------------------------------- Open Histograms --------------------------------------------------

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

  Float_t masses[10];
  
  for(int i=0;i<9;i++){
    if(i==0) masses[i]=2;
    else masses[i]=masses[i-1]+2;
    std::cout<<"Masses["<<i<<"] = "<<masses[i]<<std::endl;
  }

  masses[9] = 19;
  std::cout<<"Masses[9] = "<<masses[9]<<std::endl;
  
  Int_t n=10;
  
  //------------------------------------------------------------------------------------------------------------
  //Calculate for 1 e+

  RooRealVar x_1("x_1", "x_1", -200, 400);
  RooDataHist rHisto2_1("rHisto2_1", "rHisto2_1", x_1, Import(*Histo2_1));
  RooDataHist rHisto4_1("rHisto4_1", "rHisto4_1", x_1, Import(*Histo4_1));
  RooDataHist rHisto6_1("rHisto6_1", "rHisto6_1", x_1, Import(*Histo6_1));
  RooDataHist rHisto8_1("rHisto8_1", "rHisto8_1", x_1, Import(*Histo8_1));
  RooDataHist rHisto10_1("rHisto10_1", "rHisto10_1", x_1, Import(*Histo10_1));
  RooDataHist rHisto12_1("rHisto12_1", "rHisto12_1", x_1, Import(*Histo12_1));
  RooDataHist rHisto14_1("rHisto14_1", "rHisto14_1", x_1, Import(*Histo14_1));
  RooDataHist rHisto16_1("rHisto16_1", "rHisto16_1", x_1, Import(*Histo16_1));
  RooDataHist rHisto18_1("rHisto18_1", "rHisto18_1", x_1, Import(*Histo18_1));
  RooDataHist rHisto19_1("rHisto19_1", "rHisto19_1", x_1, Import(*Histo19_1));

  RooPlot *frame_1 = x_1.frame(Title("Test"));
  x_1.setRange("signalaaa",-250.,390.);
  x_1.setRange("signal",-100.,100.);
  x_1.setRange("signala",140.,320.);
  x_1.setRange("signalaa",240.,395.);
  
  //--------------------------------------------- 1e+---2 MeV----------------------------------------------
  RooRealVar mean2_1("mean2_1", "mean2_1", 0.3, -5., 5.);
  RooRealVar sigma2_1_1("sigma2_1_1", "sigma2_1_1", 5., 0., 50.);
  RooGaussian gauss2_1_1("gauss2_1_1", "gauss2_1_1", x_1, mean2_1, sigma2_1_1); 
  RooRealVar sigma2_1_2("sigma2_1_2", "sigma2_1_2",100., 20., 1000.);
  RooGaussian gauss2_1_2("gauss2_1_2", "gauss2_1_2", x_1, mean2_1, sigma2_1_2);

  RooRealVar signal1frac_1_2("signal1frac_1_2","signal1frac_1_2",0.,1.);
  RooAddPdf signal_1_2("signal_1_2","signal_1_2",{gauss2_1_1, gauss2_1_2},signal1frac_1_2);
  
  RooRealVar sign2_1("sign2_1","sign2_1",0.,100000.);

  RooAddPdf model2_1("model2_1", "model2_1", {signal_1_2}, RooArgList(sign2_1));

  // std::unique_ptr<RooFitResult> fitResult2_1_tmp{model2_1.fitTo(rHisto2_1, Save(), PrintLevel(-1), Range("signal"))};
  // fitResult2_1_tmp->Print();

  std::unique_ptr<RooFitResult> fitResult2_1{model2_1.fitTo(rHisto2_1, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult2_1->Print();

  Float_t min2_1 = mean2_1.getVal()-n*sigma2_1_1.getVal();
  Float_t max2_1 = mean2_1.getVal()+n*sigma2_1_1.getVal();
  
  x_1.setRange("gaus2_1",min2_1,max2_1);

  // RooAbsReal* integral2_1_1 = gauss2_1_1.createIntegral(x_1,NormSet(x_1),Range("gaus2_1"));
  // double int2_1_1 = integral2_1_1->getVal()*sign2_1.getVal()*signal1frac_1_2.getVal();
  // std::cout<<"Integral2_1_1: "<<int2_1_1<<std::endl;
  
  RooAbsReal* integral2_1 = signal_1_2.createIntegral(x_1,NormSet(x_1),Range("gaus2_1"));
  std::cout<<"Integral2_1: "<<integral2_1->getVal()*sign2_1.getVal()<<std::endl;

  rHisto2_1.plotOn(frame_1, DrawOption( "HIST" ), LineColor(kMagenta+1));
  model2_1.plotOn(frame_1);

  sigmas_1[0] = sqrt(sigma2_1_1.getVal()*sigma2_1_1.getVal()+sigma2_1_2.getVal()*sigma2_1_2.getVal());
  means_1[0] = mean2_1.getVal();
  entries_1[0] = integral2_1->getVal()*sign2_1.getVal();
  //return;
  //--------------------------------------------- 1e+---4 MeV----------------------------------------------

  RooRealVar mean4_1("mean4_1", "mean4_1", 10., 0, 20);
  RooRealVar mean4_1_2("mean4_1_2", "mean4_1_2", 15., 0, 20);
  RooRealVar sigma4_1_1("sigma4_1_1", "sigma4_1_1", 5., 0., 50.);
  RooGaussian gauss4_1_1("gauss4_1_1", "gauss4_1_1", x_1, mean4_1, sigma4_1_1);
  RooRealVar sigma4_1_2("sigma4_1_2", "sigma4_1_2", 10., 5., 300.);
  RooGaussian gauss4_1_2("gauss4_1_2", "gauss4_1_2", x_1, mean4_1, sigma4_1_2);
  
  RooRealVar signal1frac_1_4("signal1frac_1_4","signal1frac_1_4",0.1,0.,1.);
  RooAddPdf signal_1_4("signal_1_4","signal_1_4",{gauss4_1_1, gauss4_1_2},signal1frac_1_4);

  RooRealVar sign4_1("sign4_1","sign4_1",0.,100000.);

  RooAddPdf model4_1("model4_1", "model4_1", {signal_1_4}, RooArgList(sign4_1));

  // std::unique_ptr<RooFitResult> fitResult4_1_tmp{model4_1.fitTo(rHisto4_1, Save(), PrintLevel(-1), Range("signal"))};
  // fitResult4_1_tmp->Print();

  std::unique_ptr<RooFitResult> fitResult4_1{model4_1.fitTo(rHisto4_1, Save(), PrintLevel(-1), Range("signal"))};
  fitResult4_1->Print();

  Float_t min4_1 = mean4_1.getVal()-n*sigma4_1_1.getVal();
  Float_t max4_1 = mean4_1.getVal()+n*sigma4_1_1.getVal();
  
  x_1.setRange("gaus4_1",min4_1,max4_1);

  // RooAbsReal* integral4_1_1 = gauss4_1_1.createIntegral(x_1,NormSet(x_1),Range("gaus4_1"));
  // double int4_1_1 = integral4_1_1->getVal()*sign4_1.getVal()*signal1frac_1_2.getVal();
  // std::cout<<"Integral4_1_1: "<<int4_1_1<<std::endl;
  
  
  RooAbsReal* integral4_1 = signal_1_4.createIntegral(x_1,NormSet(x_1),Range("gaus4_1"));
  std::cout<<"Integral4_1: "<<integral4_1->getVal()*sign4_1.getVal()<<std::endl;

  rHisto4_1.plotOn(frame_1, DrawOption( "HIST" ), LineColor(kRed));
  model4_1.plotOn(frame_1);
  // model4_1.plotOn(frame_1,Components("gauss4_1_1"),NormSet(x_1));
  // model4_1.plotOn(frame_1,Components("gauss4_1_2"),NormSet(x_1));
  // model4_1.plotOn(frame_1,Components("signal_1_4"),LineColor(kGreen),NormSet(x_1));
  // frame_1->Draw();
  sigmas_1[1] = sqrt(sigma4_1_1.getVal()*sigma4_1_1.getVal()+sigma4_1_2.getVal()*sigma4_1_2.getVal());
  means_1[1] = mean4_1.getVal();
  entries_1[1] = integral4_1->getVal()*sign4_1.getVal();
  //return;
  //--------------------------------------------- 1e+---6 MeV----------------------------------------------

  RooRealVar mean6_1("mean6_1", "mean6_1", 29., 20., 40.);
  RooRealVar sigma6_1_1("sigma6_1_1", "sigma6_1_1", 5., 0., 50.);
  RooGaussian gauss6_1_1("gauss6_1_1", "gauss6_1_1", x_1, mean6_1, sigma6_1_1);
  RooRealVar sigma6_1_2("sigma6_1_2", "sigma6_1_2", 100., 20., 300.);
  RooGaussian gauss6_1_2("gauss6_1_2", "gauss6_1_2", x_1, mean6_1, sigma6_1_2);

  RooRealVar signal1frac_1_6("signal1frac_1_6","signal1frac_1_6",0.,1.);
  RooAddPdf signal_1_6("signal_1_6","signal_1_6",{gauss6_1_1, gauss6_1_2},signal1frac_1_6);
  
  RooRealVar sign6_1("sign6_1","sign6_1",0.,100000.);
  
  RooAddPdf model6_1("model6_1", "model6_1", {signal_1_6}, RooArgList(sign6_1));

  std::unique_ptr<RooFitResult> fitResult6_1{model6_1.fitTo(rHisto6_1, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult6_1->Print();

  Float_t min6_1 = mean6_1.getVal()-n*sigma6_1_1.getVal();
  Float_t max6_1 = mean6_1.getVal()+n*sigma6_1_1.getVal();
  
  x_1.setRange("gaus6_1",min6_1,max6_1);

  RooAbsReal* integral6_1 = signal_1_6.createIntegral(x_1,NormSet(x_1),Range("gaus6_1"));
  std::cout<<"Integral6_1: "<<integral6_1->getVal()*sign6_1.getVal()<<std::endl;

  rHisto6_1.plotOn(frame_1, DrawOption( "HIST" ), LineColor(kRed+2));
  model6_1.plotOn(frame_1);
  
  sigmas_1[2] = sqrt(sigma6_1_1.getVal()*sigma6_1_1.getVal()+sigma6_1_2.getVal()*sigma6_1_2.getVal());
  means_1[2] = mean6_1.getVal();
  entries_1[2] = integral6_1->getVal()*sign6_1.getVal();
 
//--------------------------------------------- 1e+---8 MeV----------------------------------------------
  
  RooRealVar mean8_1("mean8_1", "mean8_1", 57., 40., 65.);
  RooRealVar sigma8_1_1("sigma8_1_1", "sigma8_1_1", 5., 0., 50.);
  RooGaussian gauss8_1_1("gauss8_1_1", "gauss8_1_1", x_1, mean8_1, sigma8_1_1);
  RooRealVar sigma8_1_2("sigma8_1_2", "sigma8_1_2", 100., 20., 300.);
  RooGaussian gauss8_1_2("gauss8_1_2", "gauss8_1_2", x_1, mean8_1, sigma8_1_2);

  RooRealVar signal1frac_1_8("signal1frac_1_8","signal1frac_1_8",0.,1.);
  RooAddPdf signal_1_8("signal_1_8","signal_1_8",{gauss8_1_1, gauss8_1_2},signal1frac_1_8);
  
  RooRealVar sign8_1("sign8_1","sign8_1",0.,100000.);
  
  RooAddPdf model8_1("model8_1", "model8_1", {signal_1_8}, RooArgList(sign8_1));

  std::unique_ptr<RooFitResult> fitResult8_1{model8_1.fitTo(rHisto8_1, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult8_1->Print();

  Float_t min8_1 = mean8_1.getVal()-n*sigma8_1_1.getVal();
  Float_t max8_1 = mean8_1.getVal()+n*sigma8_1_1.getVal();
  
  x_1.setRange("gaus8_1",min8_1,max8_1);

  RooAbsReal* integral8_1 = signal_1_8.createIntegral(x_1,NormSet(x_1),Range("gaus8_1"));
  std::cout<<"Integral8_1: "<<integral8_1->getVal()*sign8_1.getVal()<<std::endl;
  
  rHisto8_1.plotOn(frame_1, DrawOption( "HIST" ), LineColor(kOrange-3));
  model8_1.plotOn(frame_1);
  
  sigmas_1[3] = sqrt(sigma8_1_1.getVal()*sigma8_1_1.getVal()+sigma8_1_2.getVal()*sigma8_1_2.getVal());
  means_1[3] = mean8_1.getVal();
  entries_1[3] = integral8_1->getVal()*sign8_1.getVal();
 
//--------------------------------------------- 1e+--10 MeV----------------------------------------------
  
  RooRealVar mean10_1("mean10_1", "mean10_1", 93., 80., 110.);
  RooRealVar sigma10_1_1("sigma10_1_1", "sigma10_1_1", 5., 0., 50.);
  RooGaussian gauss10_1_1("gauss10_1_1", "gauss10_1_1", x_1, mean10_1, sigma10_1_1);
  RooRealVar sigma10_1_2("sigma10_1_2", "sigma10_1_2", 100., 20., 300.);
  RooGaussian gauss10_1_2("gauss10_1_2", "gauss10_1_2", x_1, mean10_1, sigma10_1_2);
  
  RooRealVar signal1frac_1_10("signal1frac_1_10","signal1frac_1_10",0.,1.);
  RooAddPdf signal_1_10("signal_1_10","signal_1_10",{gauss10_1_1, gauss10_1_2},signal1frac_1_10);
  
  RooRealVar sign10_1("sign10_1","sign10_1",0.,100000.);
  
  RooAddPdf model10_1("model10_1", "model10_1", {signal_1_10}, RooArgList(sign10_1));

  std::unique_ptr<RooFitResult> fitResult10_1{model10_1.fitTo(rHisto10_1, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult10_1->Print();

  Float_t min10_1 = mean10_1.getVal()-n*sigma10_1_1.getVal();
  Float_t max10_1 = mean10_1.getVal()+n*sigma10_1_1.getVal();
  
  x_1.setRange("gaus10_1",min10_1,max10_1);

  RooAbsReal* integral10_1 = signal_1_10.createIntegral(x_1,NormSet(x_1),Range("gaus10_1"));
  std::cout<<"Integral10_1: "<<integral10_1->getVal()*sign10_1.getVal()<<std::endl;

  rHisto10_1.plotOn(frame_1, DrawOption( "HIST" ), LineColor(kYellow+2));
  model10_1.plotOn(frame_1);
  
  sigmas_1[4] = sqrt(sigma10_1_1.getVal()*sigma10_1_1.getVal()+sigma10_1_2.getVal()*sigma10_1_2.getVal());
  means_1[4] = mean10_1.getVal();
  entries_1[4] = integral10_1->getVal()*sign10_1.getVal();
 
//--------------------------------------------- 1e+--12 MeV----------------------------------------------
  
  RooRealVar mean12_1("mean12_1", "mean12_1", 136., 120., 150.);
  RooRealVar sigma12_1_1("sigma12_1_1", "sigma12_1_1", 5., 0., 50.);
  RooGaussian gauss12_1_1("gauss12_1_1", "gauss12_1_1", x_1, mean12_1, sigma12_1_1);
  RooRealVar sigma12_1_2("sigma12_1_2", "sigma12_1_2", 100., 20., 300.);
  RooGaussian gauss12_1_2("gauss12_1_2", "gauss12_1_2", x_1, mean12_1, sigma12_1_2);

  RooRealVar signal1frac_1_12("signal1frac_1_12","signal1frac_1_12",0.,1.);
  RooAddPdf signal_1_12("signal_1_12","signal_1_12",{gauss12_1_1, gauss12_1_2},signal1frac_1_12);
 
  RooRealVar sign12_1("sign12_1","sign12_1",0.,100000.);
  
  RooAddPdf model12_1("model12_1", "model12_1", {signal_1_12}, RooArgList(sign12_1));

  std::unique_ptr<RooFitResult> fitResult12_1{model12_1.fitTo(rHisto12_1, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult12_1->Print();

  Float_t min12_1 = mean12_1.getVal()-n*sigma12_1_1.getVal();
  Float_t max12_1 = mean12_1.getVal()+n*sigma12_1_1.getVal();
  
  x_1.setRange("gaus12_1",min12_1,max12_1);

  RooAbsReal* integral12_1 = signal_1_12.createIntegral(x_1,NormSet(x_1),Range("gaus12_1"));
  std::cout<<"Integral12_1: "<<integral12_1->getVal()*sign12_1.getVal()<<std::endl;

  rHisto12_1.plotOn(frame_1, DrawOption( "HIST" ), LineColor(kGreen+1));
  model12_1.plotOn(frame_1);
  
  sigmas_1[5] = sqrt(sigma12_1_1.getVal()*sigma12_1_1.getVal()+sigma12_1_2.getVal()*sigma12_1_2.getVal());
  means_1[5] = mean12_1.getVal();
  entries_1[5] = integral12_1->getVal()*sign12_1.getVal();
 
//--------------------------------------------- 1e+--14 MeV----------------------------------------------
  
  RooRealVar mean14_1("mean14_1", "mean14_1", 188., 120., 200.);
  RooRealVar sigma14_1_1("sigma14_1_1", "sigma14_1_1", 5., 0., 50.);
  RooGaussian gauss14_1_1("gauss14_1_1", "gauss14_1_1", x_1, mean14_1, sigma14_1_1);
  RooRealVar sigma14_1_2("sigma14_1_2", "sigma14_1_2", 100., 20., 300.);
  RooGaussian gauss14_1_2("gauss14_1_2", "gauss14_1_2", x_1, mean14_1, sigma14_1_2);

  RooRealVar signal1frac_1_14("signal1frac_1_14","signal1frac_1_14",0.,1.);
  RooAddPdf signal_1_14("signal_1_14","signal_1_14",{gauss14_1_1, gauss14_1_2},signal1frac_1_14);
 
  RooRealVar sign14_1("sign14_1","sign14_1",0.,100000.);
  
  RooAddPdf model14_1("model14_1", "model14_1", {signal_1_14}, RooArgList(sign14_1));

  std::unique_ptr<RooFitResult> fitResult14_1{model14_1.fitTo(rHisto14_1, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult14_1->Print();

  Float_t min14_1 = mean14_1.getVal()-n*sigma14_1_1.getVal();
  Float_t max14_1 = mean14_1.getVal()+n*sigma14_1_1.getVal();
  
  x_1.setRange("gaus14_1",min14_1,max14_1);

  RooAbsReal* integral14_1 = signal_1_14.createIntegral(x_1,NormSet(x_1),Range("gaus14_1"));
  std::cout<<"Integral14_1: "<<integral14_1->getVal()*sign14_1.getVal()<<std::endl;

  rHisto14_1.plotOn(frame_1, DrawOption( "HIST" ), LineColor(kGreen+3));
  model14_1.plotOn(frame_1);
    
  sigmas_1[6] = sqrt(sigma14_1_1.getVal()*sigma14_1_1.getVal()+sigma14_1_2.getVal()*sigma14_1_2.getVal());
  means_1[6] = mean14_1.getVal();
  entries_1[6] = integral14_1->getVal()*sign14_1.getVal();
 
//--------------------------------------------- 1e+--16 MeV----------------------------------------------
  
  RooRealVar mean16_1("mean16_1", "mean16_1", 240., 120., 350.);
  RooRealVar sigma16_1_1("sigma16_1_1", "sigma16_1_1", 5., 0., 50.);
  RooGaussian gauss16_1_1("gauss16_1_1", "gauss16_1_1", x_1, mean16_1, sigma16_1_1);
  RooRealVar sigma16_1_2("sigma16_1_2", "sigma16_1_2", 100., 20., 500.);
  RooGaussian gauss16_1_2("gauss16_1_2", "gauss16_1_2", x_1, mean16_1, sigma16_1_2);
  

  RooRealVar signal1frac_1_16("signal1frac_1_16","signal1frac_1_16",0.,1.);
  RooAddPdf signal_1_16("signal_1_16","signal_1_16",{gauss16_1_1, gauss16_1_2},signal1frac_1_16);
  
  RooRealVar sign16_1("sign16_1","sign16_1",0.,100000.);
  
  RooAddPdf model16_1("model16_1", "model16_1", {signal_1_16}, RooArgList(sign16_1));

  std::unique_ptr<RooFitResult> fitResult16_1{model16_1.fitTo(rHisto16_1, Save(), PrintLevel(-1), Range("signala"))};
  
  fitResult16_1->Print();

  Float_t min16_1 = mean16_1.getVal()-n*sigma16_1_1.getVal();
  Float_t max16_1 = mean16_1.getVal()+n*sigma16_1_1.getVal();
  
  x_1.setRange("gaus16_1",min16_1,max16_1);

  RooAbsReal* integral16_1 = signal_1_16.createIntegral(x_1,NormSet(x_1),Range("gaus16_1"));
  std::cout<<"Integral16_1: "<<integral16_1->getVal()*sign16_1.getVal()<<std::endl;
 
  rHisto16_1.plotOn(frame_1, DrawOption( "HIST" ), LineColor(kCyan+1));
  model16_1.plotOn(frame_1, Range("signala"));
    
  sigmas_1[7] = sqrt(sigma16_1_1.getVal()*sigma16_1_1.getVal()+sigma16_1_2.getVal()*sigma16_1_2.getVal());
  means_1[7] = mean16_1.getVal();
  entries_1[7] = integral16_1->getVal()*sign16_1.getVal();
 
//--------------------------------------------- 1e+--18 MeV----------------------------------------------
  
  RooRealVar mean18_1("mean18_1", "mean18_1", 300., 120., 350.);
  RooRealVar sigma18_1_1("sigma18_1_1", "sigma18_1_1", 5., 0., 50.);
  RooGaussian gauss18_1_1("gauss18_1_1", "gauss18_1_1", x_1, mean18_1, sigma18_1_1);
  RooRealVar sigma18_1_2("sigma18_1_2", "sigma18_1_2", 100., 10., 300.);
  RooGaussian gauss18_1_2("gauss18_1_2", "gauss18_1_2", x_1, mean18_1, sigma18_1_2);

  RooRealVar signal1frac_1_18("signal1frac_1_18","signal1frac_1_18",0.,1.);
  RooAddPdf signal_1_18("signal_1_18","signal_1_18",{gauss18_1_1, gauss18_1_2},signal1frac_1_18);
  
  RooRealVar sign18_1("sign18_1","sign18_1",0.,1000000.);
  
  RooAddPdf model18_1("model18_1", "model18_1", {signal_1_18}, RooArgList(sign18_1));

  std::unique_ptr<RooFitResult> fitResult18_1{model18_1.fitTo(rHisto18_1, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult18_1->Print();

  Float_t min18_1 = mean18_1.getVal()-n*sigma18_1_1.getVal();
  Float_t max18_1 = mean18_1.getVal()+n*sigma18_1_1.getVal();
  
  x_1.setRange("gaus18_1",min18_1,max18_1);

  RooAbsReal* integral18_1 = signal_1_18.createIntegral(x_1,NormSet(x_1),Range("gaus18_1"));
  std::cout<<"Integral18_1: "<<integral18_1->getVal()*sign18_1.getVal()<<std::endl;

  rHisto18_1.plotOn(frame_1, DrawOption( "HIST" ), LineColor(kCyan+3));
  model18_1.plotOn(frame_1);
    
  sigmas_1[8] = sqrt(sigma18_1_1.getVal()*sigma18_1_1.getVal()+sigma18_1_2.getVal()*sigma18_1_2.getVal());
  means_1[8] = mean18_1.getVal();
  entries_1[8] = integral18_1->getVal()*sign18_1.getVal();
  
//--------------------------------------------- 1e+--19 MeV----------------------------------------------
  
  RooRealVar mean19_1("mean19_1", "mean19_1", 350., 120., 450.);
  RooRealVar sigma19_1_1("sigma19_1_1", "sigma19_1_1", 5., 0., 50.);
  RooGaussian gauss19_1_1("gauss19_1_1", "gauss19_1_1", x_1, mean19_1, sigma19_1_1);
  RooRealVar sigma19_1_2("sigma19_1_2", "sigma19_1_2", 100., 5., 300.);
  RooGaussian gauss19_1_2("gauss19_1_2", "gauss19_1_2", x_1, mean19_1, sigma19_1_2);

  RooRealVar signal1frac_1_19("signal1frac_1_19","signal1frac_1_19",0.,1.);
  RooAddPdf signal_1_19("signal_1_19","signal_1_19",{gauss19_1_1, gauss19_1_2},signal1frac_1_19);

  RooRealVar sign19_1("sign19_1","sign19_1",0.,1000000.);
  
  RooAddPdf model19_1("model19_1", "model19_1", {signal_1_19}, RooArgList(sign19_1));

  std::unique_ptr<RooFitResult> fitResult19_1{model19_1.fitTo(rHisto19_1, Save(), PrintLevel(-1), Range("signalaa"))};
  fitResult19_1->Print();

  Float_t min19_1 = mean19_1.getVal()-n*sigma19_1_1.getVal();
  Float_t max19_1 = mean19_1.getVal()+n*sigma19_1_1.getVal();
  
  x_1.setRange("gaus19_1",min19_1,max19_1);

  RooAbsReal* integral19_1 = signal_1_19.createIntegral(x_1,NormSet(x_1),Range("gaus19_1"));
  std::cout<<"Integral19_1: "<<integral19_1->getVal()*sign19_1.getVal()<<std::endl;


  rHisto19_1.plotOn(frame_1, DrawOption( "HIST" ), LineColor(kBlue));
  model19_1.plotOn(frame_1);
  
  sigmas_1[9] = sqrt(sigma19_1_1.getVal()*sigma19_1_1.getVal()+sigma19_1_2.getVal()*sigma19_1_2.getVal());
  means_1[9] = mean19_1.getVal();
  entries_1[9] = integral19_1->getVal()*sign19_1.getVal();

  frame_1->Draw();
  //return;
  outRoot->WriteObject(frame_1,"M^{2}_{miss} for different A' masses (1 e+ per bunch)");


  //------------------------------------------------------------------------------------------------------------
  //Calculate for 10 e+

  RooRealVar x_10("x_10", "x_10", -200, 400);
  RooDataHist rHisto2_10("rHisto2_10", "rHisto2_10", x_10, Import(*Histo2_10));
  RooDataHist rHisto4_10("rHisto4_10", "rHisto4_10", x_10, Import(*Histo4_10));
  RooDataHist rHisto6_10("rHisto6_10", "rHisto6_10", x_10, Import(*Histo6_10));
  RooDataHist rHisto8_10("rHisto8_10", "rHisto8_10", x_10, Import(*Histo8_10));
  RooDataHist rHisto10_10("rHisto10_10", "rHisto10_10", x_10, Import(*Histo10_10));
  RooDataHist rHisto12_10("rHisto12_10", "rHisto12_10", x_10, Import(*Histo12_10));
  RooDataHist rHisto14_10("rHisto14_10", "rHisto14_10", x_10, Import(*Histo14_10));
  RooDataHist rHisto16_10("rHisto16_10", "rHisto16_10", x_10, Import(*Histo16_10));
  RooDataHist rHisto18_10("rHisto18_10", "rHisto18_10", x_10, Import(*Histo18_10));
  RooDataHist rHisto19_10("rHisto19_10", "rHisto19_10", x_10, Import(*Histo19_10));

  RooPlot *frame_10 = x_10.frame(Title("Test"));
  x_10.setRange("signalaaa",-250.,390.);
  x_10.setRange("signala",140.,320.);
  x_10.setRange("signalaa",240.,395.);
  
  //--------------------------------------------- 10e+---2 MeV----------------------------------------------

  RooRealVar meanbkg_10_2("meanbkg_10_2", "meanbkg_10_2", 400, 300., 200000.);
  RooRealVar sigmabkg_10_2("sigmabkg_10_2", "sigmabkg_10_2", 500., 0., 2000.);
  RooGaussian gaussbkg_10_2("gaussbkg_10_2", "gaussbkg_10_2", x_10, meanbkg_10_2, sigmabkg_10_2);

  RooRealVar mean2_10("mean2_10", "mean2_10", 0.3, -5., 5.);
  RooRealVar sigma2_10_1("sigma2_10_1", "sigma2_10_1", 5., 0., 50.);
  RooGaussian gauss2_10_1("gauss2_10_1", "gauss2_10_1", x_10, mean2_10, sigma2_10_1); 
  RooRealVar sigma2_10_2("sigma2_10_2", "sigma2_10_2",100., 20., 1000.);
  RooGaussian gauss2_10_2("gauss2_10_2", "gauss2_10_2", x_10, mean2_10, sigma2_10_2);

  RooRealVar signal1frac_10_2("signal1frac_10_2","signal1frac_10_2",0.,1.);
  RooAddPdf signal_10_2("signal_10_2","signal_10_2",{gauss2_10_1, gauss2_10_2},signal1frac_10_2);  
   
  RooRealVar bkgn2_10("bkgn2_10","bkgn2_10",0.,1000000.);
  RooRealVar sign2_10("sign2_10","sign2_10",0.,100000.);
  
  RooAddPdf model2_10("model2_10", "model2_10", {gaussbkg_10_2,signal_10_2}, RooArgList(bkgn2_10,sign2_10));

  std::unique_ptr<RooFitResult> fitResult2_10{model2_10.fitTo(rHisto2_10, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult2_10->Print();

  Float_t min2_10 = mean2_10.getVal()-n*sigma2_10_1.getVal();
  Float_t max2_10 = mean2_10.getVal()+n*sigma2_10_1.getVal();
  
  x_10.setRange("gaus2_10",min2_10,max2_10);

  RooAbsReal* integral2_10 = signal_10_2.createIntegral(x_10,NormSet(x_10),Range("gaus2_10"));
  std::cout<<"Integral2_10: "<<integral2_10->getVal()*sign2_10.getVal()<<std::endl;

  rHisto2_10.plotOn(frame_10, DrawOption( "HIST" ), LineColor(kMagenta+1));
  model2_10.plotOn(frame_10);

  sigmas_10[0] = sqrt(sigma2_10_1.getVal()*sigma2_10_1.getVal()+sigma2_10_2.getVal()*sigma2_10_2.getVal());
  means_10[0] = mean2_10.getVal();
  entries_10[0] = integral2_10->getVal()*sign2_10.getVal();
  
  //--------------------------------------------- 10e+---4 MeV----------------------------------------------

  RooRealVar meanbkg_10_4("meanbkg_10_4", "meanbkg_10_4", 400, 300., 200000.);
  RooRealVar sigmabkg_10_4("sigmabkg_10_4", "sigmabkg_10_4", 100., 0., 5000.);
  RooGaussian gaussbkg_10_4("gaussbkg_10_4", "gaussbkg_10_4", x_10, meanbkg_10_4, sigmabkg_10_4);
  
  RooRealVar mean4_10("mean4_10", "mean4_10", 10., 0, 20);
  RooRealVar sigma4_10_1("sigma4_10_1", "sigma4_10_1", 5., 0., 50.);
  RooGaussian gauss4_10_1("gauss4_10_1", "gauss4_10_1", x_10, mean4_10, sigma4_10_1);
  RooRealVar sigma4_10_2("sigma4_10_2", "sigma4_10_2", 100., 20., 300.);
  RooGaussian gauss4_10_2("gauss4_10_2", "gauss4_10_2", x_10, mean4_10, sigma4_10_2);
  
  RooRealVar signal1frac_10_4("signal1frac_10_4","signal1frac_10_4",0.,1.);
  RooAddPdf signal_10_4("signal_10_4","signal_10_4",{gauss4_10_1, gauss4_10_2},signal1frac_10_4);

  RooRealVar bkgn4_10("bkgn4_10","bkgn4_10",0.,1000000.);
  RooRealVar sign4_10("sign4_10","sign4_10",0.,100000.);

  RooAddPdf model4_10("model4_10", "model4_10", {gaussbkg_10_4,signal_10_4}, RooArgList(bkgn4_10,sign4_10));

  std::unique_ptr<RooFitResult> fitResult4_10{model4_10.fitTo(rHisto4_10, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult4_10->Print();

  Float_t min4_10 = mean4_10.getVal()-n*sigma4_10_1.getVal();
  Float_t max4_10 = mean4_10.getVal()+n*sigma4_10_1.getVal();
  
  x_10.setRange("gaus4_10",min4_10,max4_10);

  RooAbsReal* integral4_10 = signal_10_4.createIntegral(x_10,NormSet(x_10),Range("gaus4_10"));
  std::cout<<"Integral4_10: "<<integral4_10->getVal()*sign4_10.getVal()<<std::endl;

  rHisto4_10.plotOn(frame_10, DrawOption( "HIST" ), LineColor(kRed));
  model4_10.plotOn(frame_10);

  sigmas_10[1] = sqrt(sigma4_10_1.getVal()*sigma4_10_1.getVal()+sigma4_10_2.getVal()*sigma4_10_2.getVal());
  means_10[1] = mean4_10.getVal();
  entries_10[1] = integral4_10->getVal()*sign4_10.getVal();
  
  //--------------------------------------------- 10e+---6 MeV----------------------------------------------

  RooRealVar meanbkg_10_6("meanbkg_10_6", "meanbkg_10_6", 400, 300., 200000.);
  RooRealVar sigmabkg_10_6("sigmabkg_10_6", "sigmabkg_10_6", 100., 0., 5000.);
  RooGaussian gaussbkg_10_6("gaussbkg_10_6", "gaussbkg_10_6", x_10, meanbkg_10_6, sigmabkg_10_6);
  
  RooRealVar mean6_10("mean6_10", "mean6_10", 29., 20., 40.);
  RooRealVar sigma6_10_1("sigma6_10_1", "sigma6_10_1", 5., 0., 50.);
  RooGaussian gauss6_10_1("gauss6_10_1", "gauss6_10_1", x_10, mean6_10, sigma6_10_1);
  RooRealVar sigma6_10_2("sigma6_10_2", "sigma6_10_2", 100., 20., 300.);
  RooGaussian gauss6_10_2("gauss6_10_2", "gauss6_10_2", x_10, mean6_10, sigma6_10_2);

  RooRealVar signal1frac_10_6("signal1frac_10_6","signal1frac_10_6",0.,1.);
  RooAddPdf signal_10_6("signal_10_6","signal_10_6",{gauss6_10_1, gauss6_10_2},signal1frac_10_6);

  RooRealVar bkgn6_10("bkgn6_10","bkgn6_10",0.,1000000.);
  RooRealVar sign6_10("sign6_10","sign6_10",0.,100000.);
  
  RooAddPdf model6_10("model6_10", "model6_10", {gaussbkg_10_6,signal_10_6}, RooArgList(bkgn6_10,sign6_10));

  std::unique_ptr<RooFitResult> fitResult6_10{model6_10.fitTo(rHisto6_10, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult6_10->Print();

  Float_t min6_10 = mean6_10.getVal()-n*sigma6_10_1.getVal();
  Float_t max6_10 = mean6_10.getVal()+n*sigma6_10_1.getVal();
  
  x_10.setRange("gaus6_10",min6_10,max6_10);

  RooAbsReal* integral6_10 = signal_10_6.createIntegral(x_10,NormSet(x_10),Range("gaus6_10"));
  std::cout<<"Integral6_10: "<<integral6_10->getVal()*sign6_10.getVal()<<std::endl;

  rHisto6_10.plotOn(frame_10, DrawOption( "HIST" ), LineColor(kRed+2));
  model6_10.plotOn(frame_10);
  
  sigmas_10[2] = sqrt(sigma6_10_1.getVal()*sigma6_10_1.getVal()+sigma6_10_2.getVal()*sigma6_10_2.getVal());
  means_10[2] = mean6_10.getVal();
  entries_10[2] = integral6_10->getVal()*sign6_10.getVal();
 
//--------------------------------------------- 10e+---8 MeV----------------------------------------------

  RooRealVar meanbkg_10_8("meanbkg_10_8", "meanbkg_10_8", 400, 300., 200000.);
  RooRealVar sigmabkg_10_8("sigmabkg_10_8", "sigmabkg_10_8", 100., 0., 5000.);
  RooGaussian gaussbkg_10_8("gaussbkg_10_8", "gaussbkg_10_8", x_10, meanbkg_10_8, sigmabkg_10_8);
  
  RooRealVar mean8_10("mean8_10", "mean8_10", 57., 40., 70.);
  RooRealVar sigma8_10_1("sigma8_10_1", "sigma8_10_1", 5., 0., 50.);
  RooGaussian gauss8_10_1("gauss8_10_1", "gauss8_10_1", x_10, mean8_10, sigma8_10_1);
  RooRealVar sigma8_10_2("sigma8_10_2", "sigma8_10_2", 100., 20., 300.);
  RooGaussian gauss8_10_2("gauss8_10_2", "gauss8_10_2", x_10, mean8_10, sigma8_10_2);

  RooRealVar signal1frac_10_8("signal1frac_10_8","signal1frac_10_8",0.,1.);
  RooAddPdf signal_10_8("signal_10_8","signal_10_8",{gauss8_10_1, gauss8_10_2},signal1frac_10_8);

  RooRealVar bkgn8_10("bkgn8_10","bkgn8_10",0.,1000000.);
  RooRealVar sign8_10("sign8_10","sign8_10",0.,100000.);
  
  RooAddPdf model8_10("model8_10", "model8_10", {gaussbkg_10_8,signal_10_8}, RooArgList(bkgn8_10,sign8_10));

  std::unique_ptr<RooFitResult> fitResult8_10{model8_10.fitTo(rHisto8_10, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult8_10->Print();

  Float_t min8_10 = mean8_10.getVal()-n*sigma8_10_1.getVal();
  Float_t max8_10 = mean8_10.getVal()+n*sigma8_10_1.getVal();
  
  x_10.setRange("gaus8_10",min8_10,max8_10);

  RooAbsReal* integral8_10 = signal_10_8.createIntegral(x_10,NormSet(x_10),Range("gaus8_10"));
  std::cout<<"Integral8_10: "<<integral8_10->getVal()*sign8_10.getVal()<<std::endl;

  rHisto8_10.plotOn(frame_10, DrawOption( "HIST" ), LineColor(kOrange-3));
  model8_10.plotOn(frame_10);
  
  sigmas_10[3] = sqrt(sigma8_10_1.getVal()*sigma8_10_1.getVal()+sigma8_10_2.getVal()*sigma8_10_2.getVal());
  means_10[3] = mean8_10.getVal();
  entries_10[3] = integral8_10->getVal()*sign8_10.getVal();
 
//--------------------------------------------- 10e+--10 MeV----------------------------------------------
  
  RooRealVar meanbkg_10_10("meanbkg_10_10", "meanbkg_10_10", 400, 300., 200000.);
  RooRealVar sigmabkg_10_10("sigmabkg_10_10", "sigmabkg_10_10", 100., 0., 5000.);
  RooGaussian gaussbkg_10_10("gaussbkg_10_10", "gaussbkg_10_10", x_10, meanbkg_10_10, sigmabkg_10_10);
 
  RooRealVar mean10_10("mean10_10", "mean10_10", 93., 80., 110.);
  RooRealVar sigma10_10_1("sigma10_10_1", "sigma10_10_1", 5., 0., 50.);
  RooGaussian gauss10_10_1("gauss10_10_1", "gauss10_10_1", x_10, mean10_10, sigma10_10_1);
  RooRealVar sigma10_10_2("sigma10_10_2", "sigma10_10_2", 100., 20., 300.);
  RooGaussian gauss10_10_2("gauss10_10_2", "gauss10_10_2", x_10, mean10_10, sigma10_10_2);
  
  RooRealVar signal1frac_10_10("signal1frac_10_10","signal1frac_10_10",0.,1.);
  RooAddPdf signal_10_10("signal_10_10","signal_10_10",{gauss10_10_1, gauss10_10_2},signal1frac_10_10);
 
  RooRealVar bkgn10_10("bkgn10_10","bkgn10_10",0.,1000000.); 
  RooRealVar sign10_10("sign10_10","sign10_10",0.,100000.);
  
  RooAddPdf model10_10("model10_10", "model10_10", {gaussbkg_10_10,signal_10_10}, RooArgList(bkgn10_10,sign10_10));

  std::unique_ptr<RooFitResult> fitResult10_10{model10_10.fitTo(rHisto10_10, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult10_10->Print();

  Float_t min10_10 = mean10_10.getVal()-n*sigma10_10_1.getVal();
  Float_t max10_10 = mean10_10.getVal()+n*sigma10_10_1.getVal();
  
  x_10.setRange("gaus10_10",min10_10,max10_10);

  RooAbsReal* integral10_10 = signal_10_10.createIntegral(x_10,NormSet(x_10),Range("gaus10_10"));
  std::cout<<"Integral10_10: "<<integral10_10->getVal()*sign10_10.getVal()<<std::endl;

  rHisto10_10.plotOn(frame_10, DrawOption( "HIST" ), LineColor(kYellow+2));
  model10_10.plotOn(frame_10);
  
  sigmas_10[4] = sqrt(sigma10_10_1.getVal()*sigma10_10_1.getVal()+sigma10_10_2.getVal()*sigma10_10_2.getVal());
  means_10[4] = mean10_10.getVal();
  entries_10[4] = integral10_10->getVal()*sign10_10.getVal();
 
//--------------------------------------------- 10e+--12 MeV----------------------------------------------
  
  RooRealVar meanbkg_10_12("meanbkg_10_12", "meanbkg_10_12", 400, 300., 200000.);
  RooRealVar sigmabkg_10_12("sigmabkg_10_12", "sigmabkg_10_12", 100., 0., 5000.);
  RooGaussian gaussbkg_10_12("gaussbkg_10_12", "gaussbkg_10_12", x_10, meanbkg_10_12, sigmabkg_10_12);
  
  RooRealVar mean12_10("mean12_10", "mean12_10", 136., 120., 150.);
  RooRealVar sigma12_10_1("sigma12_10_1", "sigma12_10_1", 5., 0., 50.);
  RooGaussian gauss12_10_1("gauss12_10_1", "gauss12_10_1", x_10, mean12_10, sigma12_10_1);
  RooRealVar sigma12_10_2("sigma12_10_2", "sigma12_10_2", 100., 20., 300.);
  RooGaussian gauss12_10_2("gauss12_10_2", "gauss12_10_2", x_10, mean12_10, sigma12_10_2);

  RooRealVar signal1frac_10_12("signal1frac_10_12","signal1frac_10_12",0.,1.);
  RooAddPdf signal_10_12("signal_10_12","signal_10_12",{gauss12_10_1, gauss12_10_2},signal1frac_10_12);
  
  RooRealVar bkgn12_10("bkgn12_10","bkgn12_10",0.,1000000.);
  RooRealVar sign12_10("sign12_10","sign12_10",0.,100000.);
  
  RooAddPdf model12_10("model12_10", "model12_10", {gaussbkg_10_12,signal_10_12}, RooArgList(bkgn12_10,sign12_10));

  std::unique_ptr<RooFitResult> fitResult12_10{model12_10.fitTo(rHisto12_10, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult12_10->Print();

  Float_t min12_10 = mean12_10.getVal()-n*sigma12_10_1.getVal();
  Float_t max12_10 = mean12_10.getVal()+n*sigma12_10_1.getVal();
  
  x_10.setRange("gaus12_10",min12_10,max12_10);

  RooAbsReal* integral12_10 = signal_10_12.createIntegral(x_10,NormSet(x_10),Range("gaus12_10"));
  std::cout<<"Integral12_10: "<<integral12_10->getVal()*sign12_10.getVal()<<std::endl;

  rHisto12_10.plotOn(frame_10, DrawOption( "HIST" ), LineColor(kGreen+1));
  model12_10.plotOn(frame_10);
  
  sigmas_10[5] = sqrt(sigma12_10_1.getVal()*sigma12_10_1.getVal()+sigma12_10_2.getVal()*sigma12_10_2.getVal());
  means_10[5] = mean12_10.getVal();
  entries_10[5] = integral12_10->getVal()*sign12_10.getVal();
 
//--------------------------------------------- 10e+--14 MeV----------------------------------------------
    
  RooRealVar meanbkg_10_14("meanbkg_10_14", "meanbkg_10_14", 400, 300., 200000.);
  RooRealVar sigmabkg_10_14("sigmabkg_10_14", "sigmabkg_10_14", 100., 0., 5000.);
  RooGaussian gaussbkg_10_14("gaussbkg_10_14", "gaussbkg_10_14", x_10, meanbkg_10_14, sigmabkg_10_14);
 
  RooRealVar mean14_10("mean14_10", "mean14_10", 188., 120., 200.);
  RooRealVar sigma14_10_1("sigma14_10_1", "sigma14_10_1", 5., 0., 50.);
  RooGaussian gauss14_10_1("gauss14_10_1", "gauss14_10_1", x_10, mean14_10, sigma14_10_1);
  RooRealVar sigma14_10_2("sigma14_10_2", "sigma14_10_2", 100., 20., 300.);
  RooGaussian gauss14_10_2("gauss14_10_2", "gauss14_10_2", x_10, mean14_10, sigma14_10_2);

  RooRealVar signal1frac_10_14("signal1frac_10_104","signal1frac_10_104",0.,1.);
  RooAddPdf signal_10_14("signal_10_104","signal_10_14",{gauss14_10_1, gauss14_10_2},signal1frac_10_14);
   
  RooRealVar bkgn14_10("bkgn14_10","bkgn14_10",0.,1000000.);
  RooRealVar sign14_10("sign14_10","sign14_10",0.,100000.);
  
  RooAddPdf model14_10("model14_10", "model14_10", {gaussbkg_10_14,signal_10_14}, RooArgList(bkgn14_10,sign14_10));

  std::unique_ptr<RooFitResult> fitResult14_10{model14_10.fitTo(rHisto14_10, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult14_10->Print();

  Float_t min14_10 = mean14_10.getVal()-n*sigma14_10_1.getVal();
  Float_t max14_10 = mean14_10.getVal()+n*sigma14_10_1.getVal();
  
  x_10.setRange("gaus14_10",min14_10,max14_10);

  RooAbsReal* integral14_10 = signal_10_14.createIntegral(x_10,NormSet(x_10),Range("gaus14_10"));
  std::cout<<"Integral14_10: "<<integral14_10->getVal()*sign14_10.getVal()<<std::endl;

  rHisto14_10.plotOn(frame_10, DrawOption( "HIST" ), LineColor(kGreen+3));
  model14_10.plotOn(frame_10);
    
  sigmas_10[6] = sqrt(sigma14_10_1.getVal()*sigma14_10_1.getVal()+sigma14_10_2.getVal()*sigma14_10_2.getVal());
  means_10[6] = mean14_10.getVal();
  entries_10[6] = integral14_10->getVal()*sign14_10.getVal();
 
//--------------------------------------------- 10e+--16 MeV----------------------------------------------
      
  RooRealVar meanbkg_10_16("meanbkg_10_16", "meanbkg_10_16", 400, 300., 200000.);
  RooRealVar sigmabkg_10_16("sigmabkg_10_16", "sigmabkg_10_16", 100., 0., 5000.);
  RooGaussian gaussbkg_10_16("gaussbkg_10_16", "gaussbkg_10_16", x_10, meanbkg_10_16, sigmabkg_10_16);
 
  RooRealVar mean16_10("mean16_10", "mean16_10", 240., 120., 350.);
  RooRealVar sigma16_10_1("sigma16_10_1", "sigma16_10_1", 5., 0., 50.);
  RooGaussian gauss16_10_1("gauss16_10_1", "gauss16_10_1", x_10, mean16_10, sigma16_10_1);
  RooRealVar sigma16_10_2("sigma16_10_2", "sigma16_10_2", 100., 20., 500.);
  RooGaussian gauss16_10_2("gauss16_10_2", "gauss16_10_2", x_10, mean16_10, sigma16_10_2);
  

  RooRealVar signal1frac_10_16("signal1frac_10_16","signal1frac_10_16",0.,1.);
  RooAddPdf signal_10_16("signal_10_16","signal_10_16",{gauss16_10_1, gauss16_10_2},signal1frac_10_16);
     
  RooRealVar bkgn16_10("bkgn16_10","bkgn16_10",0.,1000000.);
  RooRealVar sign16_10("sign16_10","sign16_10",0.,100000.);
  
  RooAddPdf model16_10("model16_10", "model16_10", {gaussbkg_10_16,signal_10_16}, RooArgList(bkgn16_10,sign16_10));

  std::unique_ptr<RooFitResult> fitResult16_10{model16_10.fitTo(rHisto16_10, Save(), PrintLevel(-1), Range("signala"))};
  
  fitResult16_10->Print();

  Float_t min16_10 = mean16_10.getVal()-n*sigma16_10_1.getVal();
  Float_t max16_10 = mean16_10.getVal()+n*sigma16_10_1.getVal();
  
  x_10.setRange("gaus16_10",min16_10,max16_10);

  RooAbsReal* integral16_10 = signal_10_16.createIntegral(x_10,NormSet(x_10),Range("gaus16_10"));
  std::cout<<"Integral16_10: "<<integral16_10->getVal()*sign16_10.getVal()<<std::endl;
 
  rHisto16_10.plotOn(frame_10, DrawOption( "HIST" ), LineColor(kCyan+1));
  model16_10.plotOn(frame_10, Range("signala"));
    
  sigmas_10[7] = sqrt(sigma16_10_1.getVal()*sigma16_10_1.getVal()+sigma16_10_2.getVal()*sigma16_10_2.getVal());
  means_10[7] = mean16_10.getVal();
  entries_10[7] = integral16_10->getVal()*sign16_10.getVal();
 
//--------------------------------------------- 10e+--18 MeV----------------------------------------------
      
  RooRealVar meanbkg_10_18("meanbkg_10_18", "meanbkg_10_18", 400, 300., 200000.);
  RooRealVar sigmabkg_10_18("sigmabkg_10_18", "sigmabkg_10_18", 100., 0., 5000.);
  RooGaussian gaussbkg_10_18("gaussbkg_10_18", "gaussbkg_10_18", x_10, meanbkg_10_18, sigmabkg_10_18);
   
  RooRealVar mean18_10("mean18_10", "mean18_10", 300., 120., 350.);
  RooRealVar sigma18_10_1("sigma18_10_1", "sigma18_10_1", 5., 0., 50.);
  RooGaussian gauss18_10_1("gauss18_10_1", "gauss18_10_1", x_10, mean18_10, sigma18_10_1);
  RooRealVar sigma18_10_2("sigma18_10_2", "sigma18_10_2", 100., 10., 300.);
  RooGaussian gauss18_10_2("gauss18_10_2", "gauss18_10_2", x_10, mean18_10, sigma18_10_2);

  RooRealVar signal1frac_10_18("signal1frac_10_18","signal1frac_10_18",0.,1.);
  RooAddPdf signal_10_18("signal_10_18","signal_10_18",{gauss18_10_1, gauss18_10_2},signal1frac_10_18);
       
  RooRealVar bkgn18_10("bkgn18_10","bkgn18_10",0.,1000000.);
  RooRealVar sign18_10("sign18_10","sign18_10",0.,1000000.);
  
  RooAddPdf model18_10("model18_10", "model18_10", {gaussbkg_10_18,signal_10_18}, RooArgList(bkgn18_10,sign18_10));

  std::unique_ptr<RooFitResult> fitResult18_10{model18_10.fitTo(rHisto18_10, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult18_10->Print();

  Float_t min18_10 = mean18_10.getVal()-n*sigma18_10_1.getVal();
  Float_t max18_10 = mean18_10.getVal()+n*sigma18_10_1.getVal();
  
  x_10.setRange("gaus18_10",min18_10,max18_10);

  RooAbsReal* integral18_10 = signal_10_18.createIntegral(x_10,NormSet(x_10),Range("gaus18_10"));
  std::cout<<"Integral18_10: "<<integral18_10->getVal()*sign18_10.getVal()<<std::endl;

  rHisto18_10.plotOn(frame_10, DrawOption( "HIST" ), LineColor(kCyan+3));
  model18_10.plotOn(frame_10);
    
  sigmas_10[8] = sqrt(sigma18_10_1.getVal()*sigma18_10_1.getVal()+sigma18_10_2.getVal()*sigma18_10_2.getVal());
  means_10[8] = mean18_10.getVal();
  entries_10[8] = integral18_10->getVal()*sign18_10.getVal();
 
//--------------------------------------------- 10e+--19 MeV----------------------------------------------
        
  RooRealVar meanbkg_10_19("meanbkg_10_19", "meanbkg_10_19", 400, 400., 200000.);
  RooRealVar sigmabkg_10_19("sigmabkg_10_19", "sigmabkg_10_19", 500., 0., 2000.);
  RooGaussian gaussbkg_10_19("gaussbkg_10_19", "gaussbkg_10_19", x_10, meanbkg_10_19, sigmabkg_10_19);
  
  RooRealVar mean19_10("mean19_10", "mean19_10", 350., 120., 450.);
  RooRealVar sigma19_10_1("sigma19_10_1", "sigma19_10_1", 5., 0., 50.);
  RooGaussian gauss19_10_1("gauss19_10_1", "gauss19_10_1", x_10, mean19_10, sigma19_10_1);
  RooRealVar sigma19_10_2("sigma19_10_2", "sigma19_10_2", 100., 5., 300.);
  RooGaussian gauss19_10_2("gauss19_10_2", "gauss19_10_2", x_10, mean19_10, sigma19_10_2);

  RooRealVar signal1frac_10_19("signal1frac_10_19","signal1frac_10_19",0.,1.);
  RooAddPdf signal_10_19("signal_10_19","signal_10_19",{gauss19_10_1, gauss19_10_2},signal1frac_10_19);
       
  RooRealVar bkgn19_10("bkgn19_10","bkgn19_10",0.,1000000.);
  RooRealVar sign19_10("sign19_10","sign19_10",0.,1000000.);
  
  RooAddPdf model19_10("model19_10", "model19_10", {gaussbkg_10_19,signal_10_19}, RooArgList(bkgn19_10,sign19_10));

  std::unique_ptr<RooFitResult> fitResult19_10{model19_10.fitTo(rHisto19_10, Save(), PrintLevel(-1), Range("signalaa"))};
  fitResult19_10->Print();

  Float_t min19_10 = mean19_10.getVal()-n*sigma19_10_1.getVal();
  Float_t max19_10 = mean19_10.getVal()+n*sigma19_10_1.getVal();
  
  x_10.setRange("gaus19_10",min19_10,max19_10);

  RooAbsReal* integral19_10 = signal_10_19.createIntegral(x_10,NormSet(x_10),Range("gaus19_10"));
  std::cout<<"Integral19_10: "<<integral19_10->getVal()*sign19_10.getVal()<<std::endl;

  rHisto19_10.plotOn(frame_10, DrawOption( "HIST" ), LineColor(kBlue));
  model19_10.plotOn(frame_10);
   
  sigmas_10[9] = sqrt(sigma19_10_1.getVal()*sigma19_10_1.getVal()+sigma19_10_2.getVal()*sigma19_10_2.getVal());
  means_10[9] = mean19_10.getVal();
  entries_10[9] = integral19_10->getVal()*sign19_10.getVal();

 
  outRoot->WriteObject(frame_10,"M^{2}_{miss} for different A' masses (10 e+ per bunch)");


  //------------------------------------------------------------------------------------------------------------
  //Calculate for 100 e+

  RooRealVar x_100("x_100", "x_100", -200, 400);
  RooDataHist rHisto2_100("rHisto2_100", "rHisto2_100", x_100, Import(*Histo2_100));
  RooDataHist rHisto4_100("rHisto4_100", "rHisto4_100", x_100, Import(*Histo4_100));
  RooDataHist rHisto6_100("rHisto6_100", "rHisto6_100", x_100, Import(*Histo6_100));
  RooDataHist rHisto8_100("rHisto8_100", "rHisto8_100", x_100, Import(*Histo8_100));
  RooDataHist rHisto10_100("rHisto10_100", "rHisto10_100", x_100, Import(*Histo10_100));
  RooDataHist rHisto12_100("rHisto12_100", "rHisto12_100", x_100, Import(*Histo12_100));
  RooDataHist rHisto14_100("rHisto14_100", "rHisto14_100", x_100, Import(*Histo14_100));
  RooDataHist rHisto16_100("rHisto16_100", "rHisto16_100", x_100, Import(*Histo16_100));
  RooDataHist rHisto18_100("rHisto18_100", "rHisto18_100", x_100, Import(*Histo18_100));
  RooDataHist rHisto19_100("rHisto19_100", "rHisto19_100", x_100, Import(*Histo19_100));

  RooPlot *frame_100 = x_100.frame(Title("Test"));
  x_100.setRange("signalaaa",-250.,390.);
  x_100.setRange("signala",140.,320.);
  x_100.setRange("signalaa",240.,395.);
  
  //--------------------------------------------- 100e+---2 MeV----------------------------------------------

  RooRealVar meanbkg_100_2("meanbkg_100_2", "meanbkg_100_2", 400, 300., 200000.);
  RooRealVar sigmabkg_100_2("sigmabkg_100_2", "sigmabkg_100_2", 500., 0., 2000.);
  RooGaussian gaussbkg_100_2("gaussbkg_100_2", "gaussbkg_100_2", x_100, meanbkg_100_2, sigmabkg_100_2);

  RooRealVar mean2_100("mean2_100", "mean2_100", 0.3, -5., 5.);
  RooRealVar sigma2_100_1("sigma2_100_1", "sigma2_100_1", 5., 0., 50.);
  RooGaussian gauss2_100_1("gauss2_100_1", "gauss2_100_1", x_100, mean2_100, sigma2_100_1); 
  RooRealVar sigma2_100_2("sigma2_100_2", "sigma2_100_2",100., 20., 500.);
  RooGaussian gauss2_100_2("gauss2_100_2", "gauss2_100_2", x_100, mean2_100, sigma2_100_2);

  RooRealVar signal1frac_100_2("signal1frac_100_2","signal1frac_100_2",0.,1.);
  RooAddPdf signal_100_2("signal_100_2","signal_100_2",{gauss2_100_1, gauss2_100_2},signal1frac_100_2);  
   
  RooRealVar bkgn2_100("bkgn2_100","bkgn2_100",0.,1000000.);
  RooRealVar sign2_100("sign2_100","sign2_100",0.,100000.);
  
  RooAddPdf model2_100("model2_100", "model2_100", {gaussbkg_100_2,signal_100_2}, RooArgList(bkgn2_100,sign2_100));

  std::unique_ptr<RooFitResult> fitResult2_100{model2_100.fitTo(rHisto2_100, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult2_100->Print();

  Float_t min2_100 = mean2_100.getVal()-n*sigma2_100_1.getVal();
  Float_t max2_100 = mean2_100.getVal()+n*sigma2_100_1.getVal();
  
  x_100.setRange("gaus2_100",min2_100,max2_100);

  RooAbsReal* integral2_100 = signal_100_2.createIntegral(x_100,NormSet(x_100),Range("gaus2_100"));
  std::cout<<"Integral2_100: "<<integral2_100->getVal()*sign2_100.getVal()<<std::endl;

  rHisto2_100.plotOn(frame_100, DrawOption( "HIST" ), LineColor(kMagenta+1));
  model2_100.plotOn(frame_100);
  
  sigmas_100[0] = sqrt(sigma2_100_1.getVal()*sigma2_100_1.getVal()+sigma2_100_2.getVal()*sigma2_100_2.getVal());
  means_100[0] = mean2_100.getVal();
  entries_100[0] = integral2_100->getVal()*sign2_100.getVal();
  
  //--------------------------------------------- 100e+---4 MeV----------------------------------------------

  RooRealVar meanbkg_100_4("meanbkg_100_4", "meanbkg_100_4", 400, 300., 200000.);
  RooRealVar sigmabkg_100_4("sigmabkg_100_4", "sigmabkg_100_4", 100., 0., 5000.);
  RooGaussian gaussbkg_100_4("gaussbkg_100_4", "gaussbkg_100_4", x_100, meanbkg_100_4, sigmabkg_100_4);
  
  RooRealVar mean4_100("mean4_100", "mean4_100", 10., 0, 20);
  RooRealVar sigma4_100_1("sigma4_100_1", "sigma4_100_1", 5., 0., 50.);
  RooGaussian gauss4_100_1("gauss4_100_1", "gauss4_100_1", x_100, mean4_100, sigma4_100_1);
  RooRealVar sigma4_100_2("sigma4_100_2", "sigma4_100_2", 100., 20., 300.);
  RooGaussian gauss4_100_2("gauss4_100_2", "gauss4_100_2", x_100, mean4_100, sigma4_100_2);
  
  RooRealVar signal1frac_100_4("signal1frac_100_4","signal1frac_100_4",0.,1.);
  RooAddPdf signal_100_4("signal_100_4","signal_100_4",{gauss4_100_1, gauss4_100_2},signal1frac_100_4);

  RooRealVar bkgn4_100("bkgn4_100","bkgn4_100",0.,1000000.);
  RooRealVar sign4_100("sign4_100","sign4_100",0.,100000.);

  RooAddPdf model4_100("model4_100", "model4_100", {gaussbkg_100_4,signal_100_4}, RooArgList(bkgn4_100,sign4_100));

  std::unique_ptr<RooFitResult> fitResult4_100{model4_100.fitTo(rHisto4_100, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult4_100->Print();

  Float_t min4_100 = mean4_100.getVal()-n*sigma4_100_1.getVal();
  Float_t max4_100 = mean4_100.getVal()+n*sigma4_100_1.getVal();
  
  x_100.setRange("gaus4_100",min4_100,max4_100);

  RooAbsReal* integral4_100 = signal_100_4.createIntegral(x_100,NormSet(x_100),Range("gaus4_100"));
  std::cout<<"Integral4_100: "<<integral4_100->getVal()*sign4_100.getVal()<<std::endl;

  rHisto4_100.plotOn(frame_100, DrawOption( "HIST" ), LineColor(kRed));
  model4_100.plotOn(frame_100);

  sigmas_100[1] = sqrt(sigma4_100_1.getVal()*sigma4_100_1.getVal()+sigma4_100_2.getVal()*sigma4_100_2.getVal());
  means_100[1] = mean4_100.getVal();
  entries_100[1] = integral4_100->getVal()*sign4_100.getVal();
  
  //--------------------------------------------- 100e+---6 MeV----------------------------------------------

  RooRealVar meanbkg_100_6("meanbkg_100_6", "meanbkg_100_6", 400, 300., 200000.);
  RooRealVar sigmabkg_100_6("sigmabkg_100_6", "sigmabkg_100_6", 100., 0., 5000.);
  RooGaussian gaussbkg_100_6("gaussbkg_100_6", "gaussbkg_100_6", x_100, meanbkg_100_6, sigmabkg_100_6);
  
  RooRealVar mean6_100("mean6_100", "mean6_100", 29., 20., 40.);
  RooRealVar sigma6_100_1("sigma6_100_1", "sigma6_100_1", 5., 0., 50.);
  RooGaussian gauss6_100_1("gauss6_100_1", "gauss6_100_1", x_100, mean6_100, sigma6_100_1);
  RooRealVar sigma6_100_2("sigma6_100_2", "sigma6_100_2", 100., 20., 300.);
  RooGaussian gauss6_100_2("gauss6_100_2", "gauss6_100_2", x_100, mean6_100, sigma6_100_2);

  RooRealVar signal1frac_100_6("signal1frac_100_6","signal1frac_100_6",0.,1.);
  RooAddPdf signal_100_6("signal_100_6","signal_100_6",{gauss6_100_1, gauss6_100_2},signal1frac_100_6);

  RooRealVar bkgn6_100("bkgn6_100","bkgn6_100",0.,1000000.);
  RooRealVar sign6_100("sign6_100","sign6_100",0.,100000.);
  
  RooAddPdf model6_100("model6_100", "model6_100", {gaussbkg_100_6,signal_100_6}, RooArgList(bkgn6_100,sign6_100));

  std::unique_ptr<RooFitResult> fitResult6_100{model6_100.fitTo(rHisto6_100, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult6_100->Print();

  Float_t min6_100 = mean6_100.getVal()-n*sigma6_100_1.getVal();
  Float_t max6_100 = mean6_100.getVal()+n*sigma6_100_1.getVal();
  
  x_100.setRange("gaus6_100",min6_100,max6_100);

  RooAbsReal* integral6_100 = signal_100_6.createIntegral(x_100,NormSet(x_100),Range("gaus6_100"));
  std::cout<<"Integral6_100: "<<integral6_100->getVal()*sign6_100.getVal()<<std::endl;

  rHisto6_100.plotOn(frame_100, DrawOption( "HIST" ), LineColor(kRed+2));
  model6_100.plotOn(frame_100);
  
  sigmas_100[2] = sqrt(sigma6_100_1.getVal()*sigma6_100_1.getVal()+sigma6_100_2.getVal()*sigma6_100_2.getVal());
  means_100[2] = mean6_100.getVal();
  entries_100[2] = integral6_100->getVal()*sign6_100.getVal();
 
//--------------------------------------------- 100e+---8 MeV----------------------------------------------

  RooRealVar meanbkg_100_8("meanbkg_100_8", "meanbkg_100_8", 400, 300., 200000.);
  RooRealVar sigmabkg_100_8("sigmabkg_100_8", "sigmabkg_100_8", 100., 0., 5000.);
  RooGaussian gaussbkg_100_8("gaussbkg_100_8", "gaussbkg_100_8", x_100, meanbkg_100_8, sigmabkg_100_8);
  
  RooRealVar mean8_100("mean8_100", "mean8_100", 57., 40., 70.);
  RooRealVar sigma8_100_1("sigma8_100_1", "sigma8_100_1", 5., 0., 50.);
  RooGaussian gauss8_100_1("gauss8_100_1", "gauss8_100_1", x_100, mean8_100, sigma8_100_1);
  RooRealVar sigma8_100_2("sigma8_100_2", "sigma8_100_2", 100., 20., 300.);
  RooGaussian gauss8_100_2("gauss8_100_2", "gauss8_100_2", x_100, mean8_100, sigma8_100_2);

  RooRealVar signal1frac_100_8("signal1frac_100_8","signal1frac_100_8",0.,1.);
  RooAddPdf signal_100_8("signal_100_8","signal_100_8",{gauss8_100_1, gauss8_100_2},signal1frac_100_8);

  RooRealVar bkgn8_100("bkgn8_100","bkgn8_100",0.,1000000.);
  RooRealVar sign8_100("sign8_100","sign8_100",0.,100000.);
  
  RooAddPdf model8_100("model8_100", "model8_100", {gaussbkg_100_8,signal_100_8}, RooArgList(bkgn8_100,sign8_100));

  std::unique_ptr<RooFitResult> fitResult8_100{model8_100.fitTo(rHisto8_100, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult8_100->Print();

  Float_t min8_100 = mean8_100.getVal()-n*sigma8_100_1.getVal();
  Float_t max8_100 = mean8_100.getVal()+n*sigma8_100_1.getVal();
  
  x_100.setRange("gaus8_100",min8_100,max8_100);

  RooAbsReal* integral8_100 = signal_100_8.createIntegral(x_100,NormSet(x_100),Range("gaus8_100"));
  std::cout<<"Integral8_100: "<<integral8_100->getVal()*sign8_100.getVal()<<std::endl;

  rHisto8_100.plotOn(frame_100, DrawOption( "HIST" ), LineColor(kOrange-3));
  model8_100.plotOn(frame_100);
  
  sigmas_100[3] = sqrt(sigma8_100_1.getVal()*sigma8_100_1.getVal()+sigma8_100_2.getVal()*sigma8_100_2.getVal());
  means_100[3] = mean8_100.getVal();
  entries_100[3] = integral8_100->getVal()*sign8_100.getVal();
 
//--------------------------------------------- 100e+--10 MeV----------------------------------------------
  
  RooRealVar meanbkg_100_10("meanbkg_100_10", "meanbkg_100_10", 400, 300., 200000.);
  RooRealVar sigmabkg_100_10("sigmabkg_100_10", "sigmabkg_100_10", 100., 0., 5000.);
  RooGaussian gaussbkg_100_10("gaussbkg_100_10", "gaussbkg_100_10", x_100, meanbkg_100_10, sigmabkg_100_10);
 
  RooRealVar mean10_100("mean10_100", "mean10_100", 93., 80., 110.);
  RooRealVar sigma10_100_1("sigma10_100_1", "sigma10_100_1", 5., 0., 50.);
  RooGaussian gauss10_100_1("gauss10_100_1", "gauss10_100_1", x_100, mean10_100, sigma10_100_1);
  RooRealVar sigma10_100_2("sigma10_100_2", "sigma10_100_2", 100., 20., 300.);
  RooGaussian gauss10_100_2("gauss10_100_2", "gauss10_100_2", x_100, mean10_100, sigma10_100_2);
  
  RooRealVar signal1frac_100_10("signal1frac_100_10","signal1frac_100_10",0.,1.);
  RooAddPdf signal_100_10("signal_100_10","signal_100_10",{gauss10_100_1, gauss10_100_2},signal1frac_100_10);
 
  RooRealVar bkgn10_100("bkgn10_100","bkgn10_100",0.,1000000.); 
  RooRealVar sign10_100("sign10_100","sign10_100",0.,100000.);
  
  RooAddPdf model10_100("model10_100", "model10_100", {gaussbkg_100_10,signal_100_10}, RooArgList(bkgn10_100,sign10_100));

  std::unique_ptr<RooFitResult> fitResult10_100{model10_100.fitTo(rHisto10_100, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult10_100->Print();

  Float_t min10_100 = mean10_100.getVal()-n*sigma10_100_1.getVal();
  Float_t max10_100 = mean10_100.getVal()+n*sigma10_100_1.getVal();
  
  x_100.setRange("gaus10_100",min10_100,max10_100);

  RooAbsReal* integral10_100 = signal_100_10.createIntegral(x_100,NormSet(x_100),Range("gaus10_100"));
  std::cout<<"Integral10_100: "<<integral10_100->getVal()*sign10_100.getVal()<<std::endl;

  rHisto10_100.plotOn(frame_100, DrawOption( "HIST" ), LineColor(kYellow+2));
  model10_100.plotOn(frame_100);
  
  sigmas_100[4] = sqrt(sigma10_100_1.getVal()*sigma10_100_1.getVal()+sigma10_100_2.getVal()*sigma10_100_2.getVal());
  means_100[4] = mean10_100.getVal();
  entries_100[4] = integral10_100->getVal()*sign10_100.getVal();
 
//--------------------------------------------- 100e+--12 MeV----------------------------------------------
  
  RooRealVar meanbkg_100_12("meanbkg_100_12", "meanbkg_100_12", 400, 300., 200000.);
  RooRealVar sigmabkg_100_12("sigmabkg_100_12", "sigmabkg_100_12", 100., 0., 5000.);
  RooGaussian gaussbkg_100_12("gaussbkg_100_12", "gaussbkg_100_12", x_100, meanbkg_100_12, sigmabkg_100_12);
  
  RooRealVar mean12_100("mean12_100", "mean12_100", 136., 120., 150.);
  RooRealVar sigma12_100_1("sigma12_100_1", "sigma12_100_1", 5., 0., 50.);
  RooGaussian gauss12_100_1("gauss12_100_1", "gauss12_100_1", x_100, mean12_100, sigma12_100_1);
  RooRealVar sigma12_100_2("sigma12_100_2", "sigma12_100_2", 100., 20., 300.);
  RooGaussian gauss12_100_2("gauss12_100_2", "gauss12_100_2", x_100, mean12_100, sigma12_100_2);

  RooRealVar signal1frac_100_12("signal1frac_100_12","signal1frac_100_12",0.,1.);
  RooAddPdf signal_100_12("signal_100_12","signal_100_12",{gauss12_100_1, gauss12_100_2},signal1frac_100_12);
  
  RooRealVar bkgn12_100("bkgn12_100","bkgn12_100",0.,1000000.);
  RooRealVar sign12_100("sign12_100","sign12_100",0.,100000.);
  
  RooAddPdf model12_100("model12_100", "model12_100", {gaussbkg_100_12,signal_100_12}, RooArgList(bkgn12_100,sign12_100));

  std::unique_ptr<RooFitResult> fitResult12_100{model12_100.fitTo(rHisto12_100, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult12_100->Print();

  Float_t min12_100 = mean12_100.getVal()-n*sigma12_100_1.getVal();
  Float_t max12_100 = mean12_100.getVal()+n*sigma12_100_1.getVal();
  
  x_100.setRange("gaus12_100",min12_100,max12_100);

  RooAbsReal* integral12_100 = signal_100_12.createIntegral(x_100,NormSet(x_100),Range("gaus12_100"));
  std::cout<<"Integral12_100: "<<integral12_100->getVal()*sign12_100.getVal()<<std::endl;

  rHisto12_100.plotOn(frame_100, DrawOption( "HIST" ), LineColor(kGreen+1));
  model12_100.plotOn(frame_100);
  
  sigmas_100[5] = sqrt(sigma12_100_1.getVal()*sigma12_100_1.getVal()+sigma12_100_2.getVal()*sigma12_100_2.getVal());
  means_100[5] = mean12_100.getVal();
  entries_100[5] = integral12_100->getVal()*sign12_100.getVal();
 
//--------------------------------------------- 100e+--14 MeV----------------------------------------------
    
  RooRealVar meanbkg_100_14("meanbkg_100_14", "meanbkg_100_14", 400, 300., 200000.);
  RooRealVar sigmabkg_100_14("sigmabkg_100_14", "sigmabkg_100_14", 100., 0., 5000.);
  RooGaussian gaussbkg_100_14("gaussbkg_100_14", "gaussbkg_100_14", x_100, meanbkg_100_14, sigmabkg_100_14);
 
  RooRealVar mean14_100("mean14_100", "mean14_100", 188., 120., 200.);
  RooRealVar sigma14_100_1("sigma14_100_1", "sigma14_100_1", 5., 0., 50.);
  RooGaussian gauss14_100_1("gauss14_100_1", "gauss14_100_1", x_100, mean14_100, sigma14_100_1);
  RooRealVar sigma14_100_2("sigma14_100_2", "sigma14_100_2", 100., 20., 300.);
  RooGaussian gauss14_100_2("gauss14_100_2", "gauss14_100_2", x_100, mean14_100, sigma14_100_2);

  RooRealVar signal1frac_100_14("signal1frac_100_1004","signal1frac_100_1004",0.,1.);
  RooAddPdf signal_100_14("signal_100_1004","signal_100_14",{gauss14_100_1, gauss14_100_2},signal1frac_100_14);
   
  RooRealVar bkgn14_100("bkgn14_100","bkgn14_100",0.,1000000.);
  RooRealVar sign14_100("sign14_100","sign14_100",0.,100000.);
  
  RooAddPdf model14_100("model14_100", "model14_100", {gaussbkg_100_14,signal_100_14}, RooArgList(bkgn14_100,sign14_100));

  std::unique_ptr<RooFitResult> fitResult14_100{model14_100.fitTo(rHisto14_100, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult14_100->Print();

  Float_t min14_100 = mean14_100.getVal()-n*sigma14_100_1.getVal();
  Float_t max14_100 = mean14_100.getVal()+n*sigma14_100_1.getVal();
  
  x_100.setRange("gaus14_100",min14_100,max14_100);

  RooAbsReal* integral14_100 = signal_100_14.createIntegral(x_100,NormSet(x_100),Range("gaus14_100"));
  std::cout<<"Integral14_100: "<<integral14_100->getVal()*sign14_100.getVal()<<std::endl;

  rHisto14_100.plotOn(frame_100, DrawOption( "HIST" ), LineColor(kGreen+3));
  model14_100.plotOn(frame_100);
    
  sigmas_100[6] = sqrt(sigma14_100_1.getVal()*sigma14_100_1.getVal()+sigma14_100_2.getVal()*sigma14_100_2.getVal());
  means_100[6] = mean14_100.getVal();
  entries_100[6] = integral14_100->getVal()*sign14_100.getVal();
 
//--------------------------------------------- 100e+--16 MeV----------------------------------------------
      
  RooRealVar meanbkg_100_16("meanbkg_100_16", "meanbkg_100_16", 400, 300., 200000.);
  RooRealVar sigmabkg_100_16("sigmabkg_100_16", "sigmabkg_100_16", 100., 0., 5000.);
  RooGaussian gaussbkg_100_16("gaussbkg_100_16", "gaussbkg_100_16", x_100, meanbkg_100_16, sigmabkg_100_16);
 
  RooRealVar mean16_100("mean16_100", "mean16_100", 240., 120., 350.);
  RooRealVar sigma16_100_1("sigma16_100_1", "sigma16_100_1", 5., 0., 50.);
  RooGaussian gauss16_100_1("gauss16_100_1", "gauss16_100_1", x_100, mean16_100, sigma16_100_1);
  RooRealVar sigma16_100_2("sigma16_100_2", "sigma16_100_2", 100., 20., 500.);
  RooGaussian gauss16_100_2("gauss16_100_2", "gauss16_100_2", x_100, mean16_100, sigma16_100_2);
  

  RooRealVar signal1frac_100_16("signal1frac_100_16","signal1frac_100_16",0.,1.);
  RooAddPdf signal_100_16("signal_100_16","signal_100_16",{gauss16_100_1, gauss16_100_2},signal1frac_100_16);
     
  RooRealVar bkgn16_100("bkgn16_100","bkgn16_100",0.,1000000.);
  RooRealVar sign16_100("sign16_100","sign16_100",0.,100000.);
  
  RooAddPdf model16_100("model16_100", "model16_100", {gaussbkg_100_16,signal_100_16}, RooArgList(bkgn16_100,sign16_100));

  std::unique_ptr<RooFitResult> fitResult16_100{model16_100.fitTo(rHisto16_100, Save(), PrintLevel(-1), Range("signala"))};
  
  fitResult16_100->Print();

  Float_t min16_100 = mean16_100.getVal()-n*sigma16_100_1.getVal();
  Float_t max16_100 = mean16_100.getVal()+n*sigma16_100_1.getVal();
  
  x_100.setRange("gaus16_100",min16_100,max16_100);

  RooAbsReal* integral16_100 = signal_100_16.createIntegral(x_100,NormSet(x_100),Range("gaus16_100"));
  std::cout<<"Integral16_100: "<<integral16_100->getVal()*sign16_100.getVal()<<std::endl;
 
  rHisto16_100.plotOn(frame_100, DrawOption( "HIST" ), LineColor(kCyan+1));
  model16_100.plotOn(frame_100, Range("signala"));
    
  sigmas_100[7] = sqrt(sigma16_100_1.getVal()*sigma16_100_1.getVal()+sigma16_100_2.getVal()*sigma16_100_2.getVal());
  means_100[7] = mean16_100.getVal();
  entries_100[7] = integral16_100->getVal()*sign16_100.getVal();
 
//--------------------------------------------- 100e+--18 MeV----------------------------------------------
      
  RooRealVar meanbkg_100_18("meanbkg_100_18", "meanbkg_100_18", 400, 300., 200000.);
  RooRealVar sigmabkg_100_18("sigmabkg_100_18", "sigmabkg_100_18", 100., 0., 5000.);
  RooGaussian gaussbkg_100_18("gaussbkg_100_18", "gaussbkg_100_18", x_100, meanbkg_100_18, sigmabkg_100_18);
   
  RooRealVar mean18_100("mean18_100", "mean18_100", 300., 120., 350.);
  RooRealVar sigma18_100_1("sigma18_100_1", "sigma18_100_1", 5., 0., 50.);
  RooGaussian gauss18_100_1("gauss18_100_1", "gauss18_100_1", x_100, mean18_100, sigma18_100_1);
  RooRealVar sigma18_100_2("sigma18_100_2", "sigma18_100_2", 100., 10., 300.);
  RooGaussian gauss18_100_2("gauss18_100_2", "gauss18_100_2", x_100, mean18_100, sigma18_100_2);

  RooRealVar signal1frac_100_18("signal1frac_100_18","signal1frac_100_18",0.,1.);
  RooAddPdf signal_100_18("signal_100_18","signal_100_18",{gauss18_100_1, gauss18_100_2},signal1frac_100_18);
       
  RooRealVar bkgn18_100("bkgn18_100","bkgn18_100",0.,1000000.);
  RooRealVar sign18_100("sign18_100","sign18_100",0.,1000000.);
  
  RooAddPdf model18_100("model18_100", "model18_100", {gaussbkg_100_18,signal_100_18}, RooArgList(bkgn18_100,sign18_100));

  std::unique_ptr<RooFitResult> fitResult18_100{model18_100.fitTo(rHisto18_100, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult18_100->Print();

  Float_t min18_100 = mean18_100.getVal()-n*sigma18_100_1.getVal();
  Float_t max18_100 = mean18_100.getVal()+n*sigma18_100_1.getVal();
  
  x_100.setRange("gaus18_100",min18_100,max18_100);

  RooAbsReal* integral18_100 = signal_100_18.createIntegral(x_100,NormSet(x_100),Range("gaus18_100"));
  std::cout<<"Integral18_100: "<<integral18_100->getVal()*sign18_100.getVal()<<std::endl;

  rHisto18_100.plotOn(frame_100, DrawOption( "HIST" ), LineColor(kCyan+3));
  model18_100.plotOn(frame_100);
    
  sigmas_100[8] = sqrt(sigma18_100_1.getVal()*sigma18_100_1.getVal()+sigma18_100_2.getVal()*sigma18_100_2.getVal());
  means_100[8] = mean18_100.getVal();
  entries_100[8] = integral18_100->getVal()*sign18_100.getVal();
 
//--------------------------------------------- 100e+--19 MeV----------------------------------------------
        
  RooRealVar meanbkg_100_19("meanbkg_100_19", "meanbkg_100_19", 400, 400., 200000.);
  RooRealVar sigmabkg_100_19("sigmabkg_100_19", "sigmabkg_100_19", 500., 0., 2000.);
  RooGaussian gaussbkg_100_19("gaussbkg_100_19", "gaussbkg_100_19", x_100, meanbkg_100_19, sigmabkg_100_19);
  
  RooRealVar mean19_100("mean19_100", "mean19_100", 350., 120., 450.);
  RooRealVar sigma19_100_1("sigma19_100_1", "sigma19_100_1", 5., 0., 50.);
  RooGaussian gauss19_100_1("gauss19_100_1", "gauss19_100_1", x_100, mean19_100, sigma19_100_1);
  RooRealVar sigma19_100_2("sigma19_100_2", "sigma19_100_2", 100., 5., 300.);
  RooGaussian gauss19_100_2("gauss19_100_2", "gauss19_100_2", x_100, mean19_100, sigma19_100_2);

  RooRealVar signal1frac_100_19("signal1frac_100_19","signal1frac_100_19",0.,1.);
  RooAddPdf signal_100_19("signal_100_19","signal_100_19",{gauss19_100_1, gauss19_100_2},signal1frac_100_19);
       
  RooRealVar bkgn19_100("bkgn19_100","bkgn19_100",0.,1000000.);
  RooRealVar sign19_100("sign19_100","sign19_100",0.,1000000.);
  
  RooAddPdf model19_100("model19_100", "model19_100", {gaussbkg_100_19,signal_100_19}, RooArgList(bkgn19_100,sign19_100));

  std::unique_ptr<RooFitResult> fitResult19_100{model19_100.fitTo(rHisto19_100, Save(), PrintLevel(-1), Range("signalaa"))};
  fitResult19_100->Print();

  Float_t min19_100 = mean19_100.getVal()-n*sigma19_100_1.getVal();
  Float_t max19_100 = mean19_100.getVal()+n*sigma19_100_1.getVal();
  
  x_100.setRange("gaus19_100",min19_100,max19_100);

  RooAbsReal* integral19_100 = signal_100_19.createIntegral(x_100,NormSet(x_100),Range("gaus19_100"));
  std::cout<<"Integral19_100: "<<integral19_100->getVal()*sign19_100.getVal()<<std::endl;

  rHisto19_100.plotOn(frame_100, DrawOption( "HIST" ), LineColor(kBlue));
  model19_100.plotOn(frame_100);
  
  sigmas_100[9] = sqrt(sigma19_100_1.getVal()*sigma19_100_1.getVal()+sigma19_100_2.getVal()*sigma19_100_2.getVal());
  means_100[9] = mean19_100.getVal();
  entries_100[9] = integral19_100->getVal()*sign19_100.getVal();

 
  outRoot->WriteObject(frame_100,"M^{2}_{miss} for different A' masses (100 e+ per bunch)");


  //------------------------------------------------------------------------------------------------------------
  //Calculate for 1000 e+

  RooRealVar x_1000("x_1000", "x_1000", -200, 400);
  RooDataHist rHisto2_1000("rHisto2_1000", "rHisto2_1000", x_1000, Import(*Histo2_1000));
  RooDataHist rHisto4_1000("rHisto4_1000", "rHisto4_1000", x_1000, Import(*Histo4_1000));
  RooDataHist rHisto6_1000("rHisto6_1000", "rHisto6_1000", x_1000, Import(*Histo6_1000));
  RooDataHist rHisto8_1000("rHisto8_1000", "rHisto8_1000", x_1000, Import(*Histo8_1000));
  RooDataHist rHisto10_1000("rHisto10_1000", "rHisto10_1000", x_1000, Import(*Histo10_1000));
  RooDataHist rHisto12_1000("rHisto12_1000", "rHisto12_1000", x_1000, Import(*Histo12_1000));
  RooDataHist rHisto14_1000("rHisto14_1000", "rHisto14_1000", x_1000, Import(*Histo14_1000));
  RooDataHist rHisto16_1000("rHisto16_1000", "rHisto16_1000", x_1000, Import(*Histo16_1000));
  RooDataHist rHisto18_1000("rHisto18_1000", "rHisto18_1000", x_1000, Import(*Histo18_1000));
  RooDataHist rHisto19_1000("rHisto19_1000", "rHisto19_1000", x_1000, Import(*Histo19_1000));

  RooPlot *frame_1000 = x_1000.frame(Title("Test"));
  x_1000.setRange("signalaaa",-250.,390.);
  x_1000.setRange("signala",140.,320.);
  x_1000.setRange("signalaa",240.,395.);
  
  //--------------------------------------------- 1000e+---2 MeV----------------------------------------------

  RooRealVar meanbkg_1000_2("meanbkg_1000_2", "meanbkg_1000_2", 400, 400., 200000.);
  RooRealVar sigmabkg_1000_2("sigmabkg_1000_2", "sigmabkg_1000_2", 500., 0., 2000.);
  RooGaussian gaussbkg_1000_2("gaussbkg_1000_2", "gaussbkg_1000_2", x_1000, meanbkg_1000_2, sigmabkg_1000_2);

  RooRealVar mean2_1000("mean2_1000", "mean2_1000", 0.3, -5., 5.);
  RooRealVar sigma2_1000_1("sigma2_1000_1", "sigma2_1000_1", 5., 0., 50.);
  RooGaussian gauss2_1000_1("gauss2_1000_1", "gauss2_1000_1", x_1000, mean2_1000, sigma2_1000_1); 
  RooRealVar sigma2_1000_2("sigma2_1000_2", "sigma2_1000_2",100., 20., 500.);
  RooGaussian gauss2_1000_2("gauss2_1000_2", "gauss2_1000_2", x_1000, mean2_1000, sigma2_1000_2);

  RooRealVar signal1frac_1000_2("signal1frac_1000_2","signal1frac_1000_2",0.,1.);
  RooAddPdf signal_1000_2("signal_1000_2","signal_1000_2",{gauss2_1000_1, gauss2_1000_2},signal1frac_1000_2);  
   
  RooRealVar bkgn2_1000("bkgn2_1000","bkgn2_1000",0.,1000000.);
  RooRealVar sign2_1000("sign2_1000","sign2_1000",0.,100000.);
  
  RooAddPdf model2_1000("model2_1000", "model2_1000", {gaussbkg_1000_2,signal_1000_2}, RooArgList(bkgn2_1000,sign2_1000));

  std::unique_ptr<RooFitResult> fitResult2_1000{model2_1000.fitTo(rHisto2_1000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult2_1000->Print();

  Float_t min2_1000 = mean2_1000.getVal()-n*sigma2_1000_1.getVal();
  Float_t max2_1000 = mean2_1000.getVal()+n*sigma2_1000_1.getVal();
  
  x_1000.setRange("gaus2_1000",min2_1000,max2_1000);

  RooAbsReal* integral2_1000 = signal_1000_2.createIntegral(x_1000,NormSet(x_1000),Range("gaus2_1000"));
  std::cout<<"Integral2_1000: "<<integral2_1000->getVal()*sign2_1000.getVal()<<std::endl;

  rHisto2_1000.plotOn(frame_1000, DrawOption( "HIST" ), LineColor(kMagenta+1));
  model2_1000.plotOn(frame_1000);

  sigmas_1000[0] = sqrt(sigma2_1000_1.getVal()*sigma2_1000_1.getVal()+sigma2_1000_2.getVal()*sigma2_1000_2.getVal());
  means_1000[0] = mean2_1000.getVal();
  entries_1000[0] = integral2_1000->getVal()*sign2_1000.getVal();
  
  //--------------------------------------------- 1000e+---4 MeV----------------------------------------------

  RooRealVar meanbkg_1000_4("meanbkg_1000_4", "meanbkg_1000_4", 400, 400., 200000.);
  RooRealVar sigmabkg_1000_4("sigmabkg_1000_4", "sigmabkg_1000_4", 100., 0., 5000.);
  RooGaussian gaussbkg_1000_4("gaussbkg_1000_4", "gaussbkg_1000_4", x_1000, meanbkg_1000_4, sigmabkg_1000_4);
  
  RooRealVar mean4_1000("mean4_1000", "mean4_1000", 10., 0, 20);
  RooRealVar sigma4_1000_1("sigma4_1000_1", "sigma4_1000_1", 5., 0., 50.);
  RooGaussian gauss4_1000_1("gauss4_1000_1", "gauss4_1000_1", x_1000, mean4_1000, sigma4_1000_1);
  RooRealVar sigma4_1000_2("sigma4_1000_2", "sigma4_1000_2", 100., 20., 300.);
  RooGaussian gauss4_1000_2("gauss4_1000_2", "gauss4_1000_2", x_1000, mean4_1000, sigma4_1000_2);
  
  RooRealVar signal1frac_1000_4("signal1frac_1000_4","signal1frac_1000_4",0.,1.);
  RooAddPdf signal_1000_4("signal_1000_4","signal_1000_4",{gauss4_1000_1, gauss4_1000_2},signal1frac_1000_4);

  RooRealVar bkgn4_1000("bkgn4_1000","bkgn4_1000",0.,1000000.);
  RooRealVar sign4_1000("sign4_1000","sign4_1000",0.,100000.);

  RooAddPdf model4_1000("model4_1000", "model4_1000", {gaussbkg_1000_4,signal_1000_4}, RooArgList(bkgn4_1000,sign4_1000));

  std::unique_ptr<RooFitResult> fitResult4_1000{model4_1000.fitTo(rHisto4_1000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult4_1000->Print();

  Float_t min4_1000 = mean4_1000.getVal()-n*sigma4_1000_1.getVal();
  Float_t max4_1000 = mean4_1000.getVal()+n*sigma4_1000_1.getVal();
  
  x_1000.setRange("gaus4_1000",min4_1000,max4_1000);

  RooAbsReal* integral4_1000 = signal_1000_4.createIntegral(x_1000,NormSet(x_1000),Range("gaus4_1000"));
  std::cout<<"Integral4_1000: "<<integral4_1000->getVal()*sign4_1000.getVal()<<std::endl;


  rHisto4_1000.plotOn(frame_1000, DrawOption( "HIST" ), LineColor(kRed));
  model4_1000.plotOn(frame_1000);

  sigmas_1000[1] = sqrt(sigma4_1000_1.getVal()*sigma4_1000_1.getVal()+sigma4_1000_2.getVal()*sigma4_1000_2.getVal());
  means_1000[1] = mean4_1000.getVal();
  entries_1000[1] = integral4_1000->getVal()*sign4_1000.getVal();
  
  //--------------------------------------------- 1000e+---6 MeV----------------------------------------------

  RooRealVar meanbkg_1000_6("meanbkg_1000_6", "meanbkg_1000_6", 400., 400., 200000.);
  RooRealVar sigmabkg_1000_6("sigmabkg_1000_6", "sigmabkg_1000_6", 100., 0., 5000.);
  RooGaussian gaussbkg_1000_6("gaussbkg_1000_6", "gaussbkg_1000_6", x_1000, meanbkg_1000_6, sigmabkg_1000_6);
  
  RooRealVar mean6_1000("mean6_1000", "mean6_1000", 29., 20., 40.);
  RooRealVar sigma6_1000_1("sigma6_1000_1", "sigma6_1000_1", 5., 0., 50.);
  RooGaussian gauss6_1000_1("gauss6_1000_1", "gauss6_1000_1", x_1000, mean6_1000, sigma6_1000_1);
  RooRealVar sigma6_1000_2("sigma6_1000_2", "sigma6_1000_2", 100., 20., 300.);
  RooGaussian gauss6_1000_2("gauss6_1000_2", "gauss6_1000_2", x_1000, mean6_1000, sigma6_1000_2);

  RooRealVar signal1frac_1000_6("signal1frac_1000_6","signal1frac_1000_6",0.,1.);
  RooAddPdf signal_1000_6("signal_1000_6","signal_1000_6",{gauss6_1000_1, gauss6_1000_2},signal1frac_1000_6);

  RooRealVar bkgn6_1000("bkgn6_1000","bkgn6_1000",0.,1000000.);
  RooRealVar sign6_1000("sign6_1000","sign6_1000",0.,100000.);
  
  RooAddPdf model6_1000("model6_1000", "model6_1000", {gaussbkg_1000_6,signal_1000_6}, RooArgList(bkgn6_1000,sign6_1000));

  std::unique_ptr<RooFitResult> fitResult6_1000{model6_1000.fitTo(rHisto6_1000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult6_1000->Print();

  Float_t min6_1000 = mean6_1000.getVal()-n*sigma6_1000_1.getVal();
  Float_t max6_1000 = mean6_1000.getVal()+n*sigma6_1000_1.getVal();
  
  x_1000.setRange("gaus6_1000",min6_1000,max6_1000);

  RooAbsReal* integral6_1000 = signal_1000_6.createIntegral(x_1000,NormSet(x_1000),Range("gaus6_1000"));
  std::cout<<"Integral6_1000: "<<integral6_1000->getVal()*sign6_1000.getVal()<<std::endl;

  rHisto6_1000.plotOn(frame_1000, DrawOption( "HIST" ), LineColor(kRed+2));
  model6_1000.plotOn(frame_1000);
  
  sigmas_1000[2] = sqrt(sigma6_1000_1.getVal()*sigma6_1000_1.getVal()+sigma6_1000_2.getVal()*sigma6_1000_2.getVal());
  means_1000[2] = mean6_1000.getVal();
  entries_1000[2] = integral6_1000->getVal()*sign6_1000.getVal();
 
//--------------------------------------------- 1000e+---8 MeV----------------------------------------------

  RooRealVar meanbkg_1000_8("meanbkg_1000_8", "meanbkg_1000_8", 400., 400., 200000.);
  RooRealVar sigmabkg_1000_8("sigmabkg_1000_8", "sigmabkg_1000_8", 100., 0., 5000.);
  RooGaussian gaussbkg_1000_8("gaussbkg_1000_8", "gaussbkg_1000_8", x_1000, meanbkg_1000_8, sigmabkg_1000_8);
  
  RooRealVar mean8_1000("mean8_1000", "mean8_1000", 57., 40., 70.);
  RooRealVar sigma8_1000_1("sigma8_1000_1", "sigma8_1000_1", 5., 0., 50.);
  RooGaussian gauss8_1000_1("gauss8_1000_1", "gauss8_1000_1", x_1000, mean8_1000, sigma8_1000_1);
  RooRealVar sigma8_1000_2("sigma8_1000_2", "sigma8_1000_2", 100., 20., 300.);
  RooGaussian gauss8_1000_2("gauss8_1000_2", "gauss8_1000_2", x_1000, mean8_1000, sigma8_1000_2);

  RooRealVar signal1frac_1000_8("signal1frac_1000_8","signal1frac_1000_8",0.,1.);
  RooAddPdf signal_1000_8("signal_1000_8","signal_1000_8",{gauss8_1000_1, gauss8_1000_2},signal1frac_1000_8);

  RooRealVar bkgn8_1000("bkgn8_1000","bkgn8_1000",0.,1000000.);
  RooRealVar sign8_1000("sign8_1000","sign8_1000",0.,100000.);
  
  RooAddPdf model8_1000("model8_1000", "model8_1000", {gaussbkg_1000_8,signal_1000_8}, RooArgList(bkgn8_1000,sign8_1000));

  std::unique_ptr<RooFitResult> fitResult8_1000{model8_1000.fitTo(rHisto8_1000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult8_1000->Print();

  Float_t min8_1000 = mean8_1000.getVal()-n*sigma8_1000_1.getVal();
  Float_t max8_1000 = mean8_1000.getVal()+n*sigma8_1000_1.getVal();
  
  x_1000.setRange("gaus8_1000",min8_1000,max8_1000);

  RooAbsReal* integral8_1000 = signal_1000_8.createIntegral(x_1000,NormSet(x_1000),Range("gaus8_1000"));
  std::cout<<"Integral8_1000: "<<integral8_1000->getVal()*sign8_1000.getVal()<<std::endl;

  rHisto8_1000.plotOn(frame_1000, DrawOption( "HIST" ), LineColor(kOrange-3));
  model8_1000.plotOn(frame_1000);
  
  sigmas_1000[3] = sqrt(sigma8_1000_1.getVal()*sigma8_1000_1.getVal()+sigma8_1000_2.getVal()*sigma8_1000_2.getVal());
  means_1000[3] = mean8_1000.getVal();
  entries_1000[3] = integral8_1000->getVal()*sign8_1000.getVal();
 
//--------------------------------------------- 1000e+--10 MeV----------------------------------------------
  
  RooRealVar meanbkg_1000_10("meanbkg_1000_10", "meanbkg_1000_10", 400., 400., 200000.);
  RooRealVar sigmabkg_1000_10("sigmabkg_1000_10", "sigmabkg_1000_10", 100., 0., 5000.);
  RooGaussian gaussbkg_1000_10("gaussbkg_1000_10", "gaussbkg_1000_10", x_1000, meanbkg_1000_10, sigmabkg_1000_10);
 
  RooRealVar mean10_1000("mean10_1000", "mean10_1000", 93., 80., 110.);
  RooRealVar sigma10_1000_1("sigma10_1000_1", "sigma10_1000_1", 5., 0., 50.);
  RooGaussian gauss10_1000_1("gauss10_1000_1", "gauss10_1000_1", x_1000, mean10_1000, sigma10_1000_1);
  RooRealVar sigma10_1000_2("sigma10_1000_2", "sigma10_1000_2", 100., 20., 300.);
  RooGaussian gauss10_1000_2("gauss10_1000_2", "gauss10_1000_2", x_1000, mean10_1000, sigma10_1000_2);
  
  RooRealVar signal1frac_1000_10("signal1frac_1000_10","signal1frac_1000_10",0.,1.);
  RooAddPdf signal_1000_10("signal_1000_10","signal_1000_10",{gauss10_1000_1, gauss10_1000_2},signal1frac_1000_10);
 
  RooRealVar bkgn10_1000("bkgn10_1000","bkgn10_1000",0.,1000000.); 
  RooRealVar sign10_1000("sign10_1000","sign10_1000",0.,100000.);
  
  RooAddPdf model10_1000("model10_1000", "model10_1000", {gaussbkg_1000_10,signal_1000_10}, RooArgList(bkgn10_1000,sign10_1000));

  std::unique_ptr<RooFitResult> fitResult10_1000{model10_1000.fitTo(rHisto10_1000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult10_1000->Print();

  Float_t min10_1000 = mean10_1000.getVal()-n*sigma10_1000_1.getVal();
  Float_t max10_1000 = mean10_1000.getVal()+n*sigma10_1000_1.getVal();
  
  x_1000.setRange("gaus10_1000",min10_1000,max10_1000);

  RooAbsReal* integral10_1000 = signal_1000_10.createIntegral(x_1000,NormSet(x_1000),Range("gaus10_1000"));
  std::cout<<"Integral10_1000: "<<integral10_1000->getVal()*sign10_1000.getVal()<<std::endl;

  rHisto10_1000.plotOn(frame_1000, DrawOption( "HIST" ), LineColor(kYellow+2));
  model10_1000.plotOn(frame_1000);
  
  sigmas_1000[4] = sqrt(sigma10_1000_1.getVal()*sigma10_1000_1.getVal()+sigma10_1000_2.getVal()*sigma10_1000_2.getVal());
  means_1000[4] = mean10_1000.getVal();
  entries_1000[4] = integral10_1000->getVal()*sign10_1000.getVal();
 
//--------------------------------------------- 1000e+--12 MeV----------------------------------------------
  
  RooRealVar meanbkg_1000_12("meanbkg_1000_12", "meanbkg_1000_12", 400., 400., 200000.);
  RooRealVar sigmabkg_1000_12("sigmabkg_1000_12", "sigmabkg_1000_12", 100., 0., 5000.);
  RooGaussian gaussbkg_1000_12("gaussbkg_1000_12", "gaussbkg_1000_12", x_1000, meanbkg_1000_12, sigmabkg_1000_12);
  
  RooRealVar mean12_1000("mean12_1000", "mean12_1000", 136., 100., 150.);
  RooRealVar sigma12_1000_1("sigma12_1000_1", "sigma12_1000_1", 5., 0., 50.);
  RooGaussian gauss12_1000_1("gauss12_1000_1", "gauss12_1000_1", x_1000, mean12_1000, sigma12_1000_1);
  RooRealVar sigma12_1000_2("sigma12_1000_2", "sigma12_1000_2", 100., 10., 300.);
  RooGaussian gauss12_1000_2("gauss12_1000_2", "gauss12_1000_2", x_1000, mean12_1000, sigma12_1000_2);

  RooRealVar signal1frac_1000_12("signal1frac_1000_12","signal1frac_1000_12",0.,1.);
  RooAddPdf signal_1000_12("signal_1000_12","signal_1000_12",{gauss12_1000_1, gauss12_1000_2},signal1frac_1000_12);
  
  RooRealVar bkgn12_1000("bkgn12_1000","bkgn12_1000",0.,1000000.);
  RooRealVar sign12_1000("sign12_1000","sign12_1000",0.,100000.);
  
  RooAddPdf model12_1000("model12_1000", "model12_1000", {gaussbkg_1000_12,signal_1000_12}, RooArgList(bkgn12_1000,sign12_1000));

  std::unique_ptr<RooFitResult> fitResult12_1000{model12_1000.fitTo(rHisto12_1000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult12_1000->Print();

  Float_t min12_1000 = mean12_1000.getVal()-n*sigma12_1000_1.getVal();
  Float_t max12_1000 = mean12_1000.getVal()+n*sigma12_1000_1.getVal();
  
  x_1000.setRange("gaus12_1000",min12_1000,max12_1000);

  RooAbsReal* integral12_1000 = signal_1000_12.createIntegral(x_1000,NormSet(x_1000),Range("gaus12_1000"));
  std::cout<<"Integral12_1000: "<<integral12_1000->getVal()*sign12_1000.getVal()<<std::endl;
 
  rHisto12_1000.plotOn(frame_1000, DrawOption( "HIST" ), LineColor(kGreen+1));
  model12_1000.plotOn(frame_1000);
  
  sigmas_1000[5] = sqrt(sigma12_1000_1.getVal()*sigma12_1000_1.getVal()+sigma12_1000_2.getVal()*sigma12_1000_2.getVal());
  means_1000[5] = mean12_1000.getVal();
  entries_1000[5] = integral12_1000->getVal()*sign12_1000.getVal();
 
//--------------------------------------------- 1000e+--14 MeV----------------------------------------------
    
  RooRealVar meanbkg_1000_14("meanbkg_1000_14", "meanbkg_1000_14", 400., 400., 200000.);
  RooRealVar sigmabkg_1000_14("sigmabkg_1000_14", "sigmabkg_1000_14", 500., 0., 5000.);
  RooGaussian gaussbkg_1000_14("gaussbkg_1000_14", "gaussbkg_1000_14", x_1000, meanbkg_1000_14, sigmabkg_1000_14);
 
  RooRealVar mean14_1000("mean14_1000", "mean14_1000", 188., 120., 200.);
  RooRealVar sigma14_1000_1("sigma14_1000_1", "sigma14_1000_1", 5., 0., 50.);
  RooGaussian gauss14_1000_1("gauss14_1000_1", "gauss14_1000_1", x_1000, mean14_1000, sigma14_1000_1);
  RooRealVar sigma14_1000_2("sigma14_1000_2", "sigma14_1000_2", 100., 0., 300.);
  RooGaussian gauss14_1000_2("gauss14_1000_2", "gauss14_1000_2", x_1000, mean14_1000, sigma14_1000_2);

  RooRealVar signal1frac_1000_14("signal1frac_1000_10004","signal1frac_1000_10004",0.,1.);
  RooAddPdf signal_1000_14("signal_1000_10004","signal_1000_14",{gauss14_1000_1, gauss14_1000_2},signal1frac_1000_14);
   
  RooRealVar bkgn14_1000("bkgn14_1000","bkgn14_1000",0.,1000000.);
  RooRealVar sign14_1000("sign14_1000","sign14_1000",0.,100000.);
  
  RooAddPdf model14_1000("model14_1000", "model14_1000", {gaussbkg_1000_14,signal_1000_14}, RooArgList(bkgn14_1000,sign14_1000));

  std::unique_ptr<RooFitResult> fitResult14_1000{model14_1000.fitTo(rHisto14_1000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult14_1000->Print();

  Float_t min14_1000 = mean14_1000.getVal()-n*sigma14_1000_1.getVal();
  Float_t max14_1000 = mean14_1000.getVal()+n*sigma14_1000_1.getVal();
  
  x_1000.setRange("gaus14_1000",min14_1000,max14_1000);

  RooAbsReal* integral14_1000 = signal_1000_14.createIntegral(x_1000,NormSet(x_1000),Range("gaus14_1000"));
  std::cout<<"Integral14_1000: "<<integral14_1000->getVal()*sign14_1000.getVal()<<std::endl;

  rHisto14_1000.plotOn(frame_1000, DrawOption( "HIST" ), LineColor(kGreen+3));
  model14_1000.plotOn(frame_1000);
  
  sigmas_1000[6] = sqrt(sigma14_1000_1.getVal()*sigma14_1000_1.getVal()+sigma14_1000_2.getVal()*sigma14_1000_2.getVal());
  means_1000[6] = mean14_1000.getVal();
  entries_1000[6] = integral14_1000->getVal()*sign14_1000.getVal();
 
//--------------------------------------------- 1000e+--16 MeV----------------------------------------------
      
  RooRealVar meanbkg_1000_16("meanbkg_1000_16", "meanbkg_1000_16", 400., 400., 200000.);
  RooRealVar sigmabkg_1000_16("sigmabkg_1000_16", "sigmabkg_1000_16", 100., 0., 5000.);
  RooGaussian gaussbkg_1000_16("gaussbkg_1000_16", "gaussbkg_1000_16", x_1000, meanbkg_1000_16, sigmabkg_1000_16);
 
  RooRealVar mean16_1000("mean16_1000", "mean16_1000", 240., 120., 350.);
  RooRealVar sigma16_1000_1("sigma16_1000_1", "sigma16_1000_1", 5., 0., 50.);
  RooGaussian gauss16_1000_1("gauss16_1000_1", "gauss16_1000_1", x_1000, mean16_1000, sigma16_1000_1);
  RooRealVar sigma16_1000_2("sigma16_1000_2", "sigma16_1000_2", 100., 20., 500.);
  RooGaussian gauss16_1000_2("gauss16_1000_2", "gauss16_1000_2", x_1000, mean16_1000, sigma16_1000_2);
  

  RooRealVar signal1frac_1000_16("signal1frac_1000_16","signal1frac_1000_16",0.,1.);
  RooAddPdf signal_1000_16("signal_1000_16","signal_1000_16",{gauss16_1000_1, gauss16_1000_2},signal1frac_1000_16);
     
  RooRealVar bkgn16_1000("bkgn16_1000","bkgn16_1000",0.,1000000.);
  RooRealVar sign16_1000("sign16_1000","sign16_1000",0.,100000.);
  
  RooAddPdf model16_1000("model16_1000", "model16_1000", {gaussbkg_1000_16,signal_1000_16}, RooArgList(bkgn16_1000,sign16_1000));

  std::unique_ptr<RooFitResult> fitResult16_1000{model16_1000.fitTo(rHisto16_1000, Save(), PrintLevel(-1), Range("signala"))};
  
  fitResult16_1000->Print();

  Float_t min16_1000 = mean16_1000.getVal()-n*sigma16_1000_1.getVal();
  Float_t max16_1000 = mean16_1000.getVal()+n*sigma16_1000_1.getVal();
  
  x_1000.setRange("gaus16_1000",min16_1000,max16_1000);

  RooAbsReal* integral16_1000 = signal_1000_16.createIntegral(x_1000,NormSet(x_1000),Range("gaus16_1000"));
  std::cout<<"Integral16_1000: "<<integral16_1000->getVal()*sign16_1000.getVal()<<std::endl;
 
  rHisto16_1000.plotOn(frame_1000, DrawOption( "HIST" ), LineColor(kCyan+1));
  model16_1000.plotOn(frame_1000, Range("signala"));
    
  sigmas_1000[7] = sqrt(sigma16_1000_1.getVal()*sigma16_1000_1.getVal()+sigma16_1000_2.getVal()*sigma16_1000_2.getVal());
  means_1000[7] = mean16_1000.getVal();
  entries_1000[7] = integral16_1000->getVal()*sign16_1000.getVal();
 
//--------------------------------------------- 1000e+--18 MeV----------------------------------------------
      
  RooRealVar meanbkg_1000_18("meanbkg_1000_18", "meanbkg_1000_18", 400., 400., 200000.);
  RooRealVar sigmabkg_1000_18("sigmabkg_1000_18", "sigmabkg_1000_18", 100., 0., 5000.);
  RooGaussian gaussbkg_1000_18("gaussbkg_1000_18", "gaussbkg_1000_18", x_1000, meanbkg_1000_18, sigmabkg_1000_18);
   
  RooRealVar mean18_1000("mean18_1000", "mean18_1000", 300., 120., 350.);
  RooRealVar sigma18_1000_1("sigma18_1000_1", "sigma18_1000_1", 5., 0., 50.);
  RooGaussian gauss18_1000_1("gauss18_1000_1", "gauss18_1000_1", x_1000, mean18_1000, sigma18_1000_1);
  RooRealVar sigma18_1000_2("sigma18_1000_2", "sigma18_1000_2", 100., 10., 300.);
  RooGaussian gauss18_1000_2("gauss18_1000_2", "gauss18_1000_2", x_1000, mean18_1000, sigma18_1000_2);

  RooRealVar signal1frac_1000_18("signal1frac_1000_18","signal1frac_1000_18",0.,1.);
  RooAddPdf signal_1000_18("signal_1000_18","signal_1000_18",{gauss18_1000_1, gauss18_1000_2},signal1frac_1000_18);
       
  RooRealVar bkgn18_1000("bkgn18_1000","bkgn18_1000",0.,1000000.);
  RooRealVar sign18_1000("sign18_1000","sign18_1000",0.,1000000.);
  
  RooAddPdf model18_1000("model18_1000", "model18_1000", {gaussbkg_1000_18,signal_1000_18}, RooArgList(bkgn18_1000,sign18_1000));

  std::unique_ptr<RooFitResult> fitResult18_1000{model18_1000.fitTo(rHisto18_1000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult18_1000->Print();

  Float_t min18_1000 = mean18_1000.getVal()-n*sigma18_1000_1.getVal();
  Float_t max18_1000 = mean18_1000.getVal()+n*sigma18_1000_1.getVal();
  
  x_1000.setRange("gaus18_1000",min18_1000,max18_1000);

  RooAbsReal* integral18_1000 = signal_1000_18.createIntegral(x_1000,NormSet(x_1000),Range("gaus18_1000"));
  std::cout<<"Integral18_1000: "<<integral18_1000->getVal()*sign18_1000.getVal()<<std::endl;

  rHisto18_1000.plotOn(frame_1000, DrawOption( "HIST" ), LineColor(kCyan+3));
  model18_1000.plotOn(frame_1000);
    
  sigmas_1000[8] = sqrt(sigma18_1000_1.getVal()*sigma18_1000_1.getVal()+sigma18_1000_2.getVal()*sigma18_1000_2.getVal());
  means_1000[8] = mean18_1000.getVal();
  entries_1000[8] = integral18_1000->getVal()*sign18_1000.getVal();
 
//--------------------------------------------- 1000e+--19 MeV----------------------------------------------
        
  RooRealVar meanbkg_1000_19("meanbkg_1000_19", "meanbkg_1000_19", 400., 400., 200000.);
  RooRealVar sigmabkg_1000_19("sigmabkg_1000_19", "sigmabkg_1000_19", 500., 0., 2000.);
  RooGaussian gaussbkg_1000_19("gaussbkg_1000_19", "gaussbkg_1000_19", x_1000, meanbkg_1000_19, sigmabkg_1000_19);
  
  RooRealVar mean19_1000("mean19_1000", "mean19_1000", 350., 120., 450.);
  RooRealVar sigma19_1000_1("sigma19_1000_1", "sigma19_1000_1", 5., 0., 50.);
  RooGaussian gauss19_1000_1("gauss19_1000_1", "gauss19_1000_1", x_1000, mean19_1000, sigma19_1000_1);
  RooRealVar sigma19_1000_2("sigma19_1000_2", "sigma19_1000_2", 100., 5., 300.);
  RooGaussian gauss19_1000_2("gauss19_1000_2", "gauss19_1000_2", x_1000, mean19_1000, sigma19_1000_2);

  RooRealVar signal1frac_1000_19("signal1frac_1000_19","signal1frac_1000_19",0.,1.);
  RooAddPdf signal_1000_19("signal_1000_19","signal_1000_19",{gauss19_1000_1, gauss19_1000_2},signal1frac_1000_19);
       
  RooRealVar bkgn19_1000("bkgn19_1000","bkgn19_1000",0.,1000000.);
  RooRealVar sign19_1000("sign19_1000","sign19_1000",0.,1000000.);
  
  RooAddPdf model19_1000("model19_1000", "model19_1000", {gaussbkg_1000_19,signal_1000_19}, RooArgList(bkgn19_1000,sign19_1000));

  std::unique_ptr<RooFitResult> fitResult19_1000{model19_1000.fitTo(rHisto19_1000, Save(), PrintLevel(-1), Range("signalaa"))};
  fitResult19_1000->Print();

  Float_t min19_1000 = mean19_1000.getVal()-n*sigma19_1000_1.getVal();
  Float_t max19_1000 = mean19_1000.getVal()+n*sigma19_1000_1.getVal();
  
  x_1000.setRange("gaus19_1000",min19_1000,max19_1000);

  RooAbsReal* integral19_1000 = signal_1000_19.createIntegral(x_1000,NormSet(x_1000),Range("gaus19_1000"));
  std::cout<<"Integral19_1000: "<<integral19_1000->getVal()*sign19_1000.getVal()<<std::endl;

  rHisto19_1000.plotOn(frame_1000, DrawOption( "HIST" ), LineColor(kBlue));
  model19_1000.plotOn(frame_1000);
  
  sigmas_1000[9] = sqrt(sigma19_1000_1.getVal()*sigma19_1000_1.getVal()+sigma19_1000_2.getVal()*sigma19_1000_2.getVal());
  means_1000[9] = mean19_1000.getVal();
  entries_1000[9] = integral19_1000->getVal()*sign19_1000.getVal();

 
  outRoot->WriteObject(frame_1000,"M^{2}_{miss} for different A' masses (1000 e+ per bunch)");


  
  //------------------------------------------------------------------------------------------------------------
  //Calculate for 10000 e+

  RooRealVar x_10000("x_10000", "x_10000", -200, 400);
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

  RooPlot *frame_10000 = x_10000.frame(Title("Test"));
  x_10000.setRange("signalaaa",-250.,390.);
  // x_10000.setRange("signalaaa",-100.,100.);
  x_10000.setRange("signala",140.,320.);
  x_10000.setRange("signalaa",240.,395.);
  x_10000.setRange("signalaaaa",280.,350.);
  
  //--------------------------------------------- 10000e+---2 MeV----------------------------------------------

  RooRealVar meanbkg_10000_2("meanbkg_10000_2", "meanbkg_10000_2", 400, 400., 200000.);
  RooRealVar sigmabkg_10000_2("sigmabkg_10000_2", "sigmabkg_10000_2", 500., 0., 2000.);
  RooGaussian gaussbkg_10000_2("gaussbkg_10000_2", "gaussbkg_10000_2", x_10000, meanbkg_10000_2, sigmabkg_10000_2);

  RooRealVar mean2_10000("mean2_10000", "mean2_10000", 0.3, -5., 5.);
  RooRealVar sigma2_10000_1("sigma2_10000_1", "sigma2_10000_1", 5., 0., 50.);
  RooGaussian gauss2_10000_1("gauss2_10000_1", "gauss2_10000_1", x_10000, mean2_10000, sigma2_10000_1); 
  RooRealVar sigma2_10000_2("sigma2_10000_2", "sigma2_10000_2",100., 20., 500.);
  RooGaussian gauss2_10000_2("gauss2_10000_2", "gauss2_10000_2", x_10000, mean2_10000, sigma2_10000_2);

  RooRealVar signal1frac_10000_2("signal1frac_10000_2","signal1frac_10000_2",0.,1.);
  RooAddPdf signal_10000_2("signal_10000_2","signal_10000_2",{gauss2_10000_1, gauss2_10000_2},signal1frac_10000_2); 
   
  RooRealVar bkgn2_10000("bkgn2_10000","bkgn2_10000",0.,1000000.);
  RooRealVar sign2_10000("sign2_10000","sign2_10000",0.,100000.);
  
  RooAddPdf model2_10000("model2_10000", "model2_10000", {gaussbkg_10000_2,signal_10000_2}, RooArgList(bkgn2_10000,sign2_10000));

  std::unique_ptr<RooFitResult> fitResult2_10000{model2_10000.fitTo(rHisto2_10000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult2_10000->Print();

  Float_t min2_10000 = mean2_10000.getVal()-n*sigma2_10000_1.getVal();
  Float_t max2_10000 = mean2_10000.getVal()+n*sigma2_10000_1.getVal();
  
  x_10000.setRange("gaus2_10000",min2_10000,max2_10000);

  RooAbsReal* integral2_10000 = signal_10000_2.createIntegral(x_10000,NormSet(x_10000),Range("gaus2_10000"));
  std::cout<<"Integral2_10000: "<<integral2_10000->getVal()*sign2_10000.getVal()<<std::endl;

  rHisto2_10000.plotOn(frame_10000, DrawOption( "HIST" ), LineColor(kMagenta+1));
  model2_10000.plotOn(frame_10000);

  sigmas_10000[0] = sqrt(sigma2_10000_1.getVal()*sigma2_10000_1.getVal()+sigma2_10000_2.getVal()*sigma2_10000_2.getVal());
  means_10000[0] = mean2_10000.getVal();
  entries_10000[0] = integral2_10000->getVal()*sign2_10000.getVal();
  
  //--------------------------------------------- 10000e+---4 MeV----------------------------------------------

  RooRealVar meanbkg_10000_4("meanbkg_10000_4", "meanbkg_10000_4", 400, 400., 200000.);
  RooRealVar sigmabkg_10000_4("sigmabkg_10000_4", "sigmabkg_10000_4", 100., 0., 5000.);
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

  RooAddPdf model4_10000("model4_10000", "model4_10000", {gaussbkg_10000_4,signal_10000_4}, RooArgList(bkgn4_10000,sign4_10000));

  std::unique_ptr<RooFitResult> fitResult4_10000{model4_10000.fitTo(rHisto4_10000, Save(), PrintLevel(-1), Range("signalааа"))};
  fitResult4_10000->Print();

  Float_t min4_10000 = mean4_10000.getVal()-n*sigma4_10000_1.getVal();
  Float_t max4_10000 = mean4_10000.getVal()+n*sigma4_10000_1.getVal();
  
  x_10000.setRange("gaus4_10000",min4_10000,max4_10000);

  RooAbsReal* integral4_10000 = signal_10000_4.createIntegral(x_10000,NormSet(x_10000),Range("gaus4_10000"));
  std::cout<<"Integral4_10000: "<<integral4_10000->getVal()*sign4_10000.getVal()<<std::endl;

  rHisto4_10000.plotOn(frame_10000, DrawOption( "HIST" ), LineColor(kRed));
  model4_10000.plotOn(frame_10000);
  // model4_10000.plotOn(frame_10000,LineColor(kGreen),Components("signal_10000_4"));
  // model4_10000.plotOn(frame_10000,LineColor(kBlack),Components("gaus4_10000_1"));
  // model4_10000.plotOn(frame_10000,LineColor(kOrange),Components("gaus4_10000_2"));
    
  
  //frame_10000->Draw();
  sigmas_10000[1] = sqrt(sigma4_10000_1.getVal()*sigma4_10000_1.getVal()+sigma4_10000_2.getVal()*sigma4_10000_2.getVal());
  means_10000[1] = mean4_10000.getVal();
  entries_10000[1] = integral4_10000->getVal()*sign4_10000.getVal();

  //return;
  //--------------------------------------------- 10000e+---6 MeV----------------------------------------------

  RooRealVar meanbkg_10000_6("meanbkg_10000_6", "meanbkg_10000_6", 400., 400., 200000.);
  RooRealVar sigmabkg_10000_6("sigmabkg_10000_6", "sigmabkg_10000_6", 100., 0., 5000.);
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
  
  RooAddPdf model6_10000("model6_10000", "model6_10000", {gaussbkg_10000_6,signal_10000_6}, RooArgList(bkgn6_10000,sign6_10000));

  std::unique_ptr<RooFitResult> fitResult6_10000{model6_10000.fitTo(rHisto6_10000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult6_10000->Print();

  Float_t min6_10000 = mean6_10000.getVal()-n*sigma6_10000_1.getVal();
  Float_t max6_10000 = mean6_10000.getVal()+n*sigma6_10000_1.getVal();
  
  x_10000.setRange("gaus6_10000",min6_10000,max6_10000);

  RooAbsReal* integral6_10000 = signal_10000_6.createIntegral(x_10000,NormSet(x_10000),Range("gaus6_10000"));
  std::cout<<"Integral6_10000: "<<integral6_10000->getVal()*sign6_10000.getVal()<<std::endl;

  rHisto6_10000.plotOn(frame_10000, DrawOption( "HIST" ), LineColor(kRed+2));
  model6_10000.plotOn(frame_10000);
  
  sigmas_10000[2] = sqrt(sigma6_10000_1.getVal()*sigma6_10000_1.getVal()+sigma6_10000_2.getVal()*sigma6_10000_2.getVal());
  means_10000[2] = mean6_10000.getVal();
  entries_10000[2] = integral6_10000->getVal()*sign6_10000.getVal();
 
//--------------------------------------------- 10000e+---8 MeV----------------------------------------------

  RooRealVar meanbkg_10000_8("meanbkg_10000_8", "meanbkg_10000_8", 400., 400., 200000.);
  RooRealVar sigmabkg_10000_8("sigmabkg_10000_8", "sigmabkg_10000_8", 100., 0., 5000.);
  RooGaussian gaussbkg_10000_8("gaussbkg_10000_8", "gaussbkg_10000_8", x_10000, meanbkg_10000_8, sigmabkg_10000_8);
  
  RooRealVar mean8_10000("mean8_10000", "mean8_10000", 57., 40., 70.);
  RooRealVar sigma8_10000_1("sigma8_10000_1", "sigma8_10000_1", 5., 0., 50.);
  RooGaussian gauss8_10000_1("gauss8_10000_1", "gauss8_10000_1", x_10000, mean8_10000, sigma8_10000_1);
  RooRealVar sigma8_10000_2("sigma8_10000_2", "sigma8_10000_2", 100., 20., 300.);
  RooGaussian gauss8_10000_2("gauss8_10000_2", "gauss8_10000_2", x_10000, mean8_10000, sigma8_10000_2);

  RooRealVar signal1frac_10000_8("signal1frac_10000_8","signal1frac_10000_8",0.,1.);
  RooAddPdf signal_10000_8("signal_10000_8","signal_10000_8",{gauss8_10000_1, gauss8_10000_2},signal1frac_10000_8);

  RooRealVar bkgn8_10000("bkgn8_10000","bkgn8_10000",0.,1000000.);
  RooRealVar sign8_10000("sign8_10000","sign8_10000",0.,100000.);
  
  RooAddPdf model8_10000("model8_10000", "model8_10000", {gaussbkg_10000_8,signal_10000_8}, RooArgList(bkgn8_10000,sign8_10000));

  std::unique_ptr<RooFitResult> fitResult8_10000{model8_10000.fitTo(rHisto8_10000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult8_10000->Print();

  Float_t min8_10000 = mean8_10000.getVal()-n*sigma8_10000_1.getVal();
  Float_t max8_10000 = mean8_10000.getVal()+n*sigma8_10000_1.getVal();
  
  x_10000.setRange("gaus8_10000",min8_10000,max8_10000);

  RooAbsReal* integral8_10000 = signal_10000_8.createIntegral(x_10000,NormSet(x_10000),Range("gaus8_10000"));
  std::cout<<"Integral8_10000: "<<integral8_10000->getVal()*sign8_10000.getVal()<<std::endl;

  rHisto8_10000.plotOn(frame_10000, DrawOption( "HIST" ), LineColor(kOrange-3));
  model8_10000.plotOn(frame_10000);
  
  sigmas_10000[3] = sqrt(sigma8_10000_1.getVal()*sigma8_10000_1.getVal()+sigma8_10000_2.getVal()*sigma8_10000_2.getVal());
  means_10000[3] = mean8_10000.getVal();
  entries_10000[3] = integral8_10000->getVal()*sign8_10000.getVal();
 
//--------------------------------------------- 10000e+--10 MeV----------------------------------------------
  
  RooRealVar meanbkg_10000_10("meanbkg_10000_10", "meanbkg_10000_10", 400., 400., 200000.);
  RooRealVar sigmabkg_10000_10("sigmabkg_10000_10", "sigmabkg_10000_10", 100., 0., 5000.);
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
  
  RooAddPdf model10_10000("model10_10000", "model10_10000", {gaussbkg_10000_10,signal_10000_10}, RooArgList(bkgn10_10000,sign10_10000));

  std::unique_ptr<RooFitResult> fitResult10_10000{model10_10000.fitTo(rHisto10_10000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult10_10000->Print();

  Float_t min10_10000 = mean10_10000.getVal()-n*sigma10_10000_1.getVal();
  Float_t max10_10000 = mean10_10000.getVal()+n*sigma10_10000_1.getVal();
  
  x_10000.setRange("gaus10_10000",min10_10000,max10_10000);

  RooAbsReal* integral10_10000 = signal_10000_10.createIntegral(x_10000,NormSet(x_10000),Range("gaus10_10000"));
  std::cout<<"Integral10_10000: "<<integral10_10000->getVal()*sign10_10000.getVal()<<std::endl;

  rHisto10_10000.plotOn(frame_10000, DrawOption( "HIST" ), LineColor(kYellow+2));
  model10_10000.plotOn(frame_10000);
  
  sigmas_10000[4] = sqrt(sigma10_10000_1.getVal()*sigma10_10000_1.getVal()+sigma10_10000_2.getVal()*sigma10_10000_2.getVal());
  means_10000[4] = mean10_10000.getVal();
  entries_10000[4] = integral10_10000->getVal()*sign10_10000.getVal();
 
//--------------------------------------------- 10000e+--12 MeV----------------------------------------------
  
  RooRealVar meanbkg_10000_12("meanbkg_10000_12", "meanbkg_10000_12", 400., 400., 200000.);
  RooRealVar sigmabkg_10000_12("sigmabkg_10000_12", "sigmabkg_10000_12", 100., 0., 5000.);
  RooGaussian gaussbkg_10000_12("gaussbkg_10000_12", "gaussbkg_10000_12", x_10000, meanbkg_10000_12, sigmabkg_10000_12);
  
  RooRealVar mean12_10000("mean12_10000", "mean12_10000", 136., 100., 150.);
  RooRealVar sigma12_10000_1("sigma12_10000_1", "sigma12_10000_1", 5., 0., 50.);
  RooGaussian gauss12_10000_1("gauss12_10000_1", "gauss12_10000_1", x_10000, mean12_10000, sigma12_10000_1);
  RooRealVar sigma12_10000_2("sigma12_10000_2", "sigma12_10000_2", 100., 10., 300.);
  RooGaussian gauss12_10000_2("gauss12_10000_2", "gauss12_10000_2", x_10000, mean12_10000, sigma12_10000_2);

  RooRealVar signal1frac_10000_12("signal1frac_10000_12","signal1frac_10000_12",0.,1.);
  RooAddPdf signal_10000_12("signal_10000_12","signal_10000_12",{gauss12_10000_1, gauss12_10000_2},signal1frac_10000_12);
  
  RooRealVar bkgn12_10000("bkgn12_10000","bkgn12_10000",0.,1000000.);
  RooRealVar sign12_10000("sign12_10000","sign12_10000",0.,100000.);
  
  RooAddPdf model12_10000("model12_10000", "model12_10000", {gaussbkg_10000_12,signal_10000_12}, RooArgList(bkgn12_10000,sign12_10000));

  std::unique_ptr<RooFitResult> fitResult12_10000{model12_10000.fitTo(rHisto12_10000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult12_10000->Print();
 
  Float_t min12_10000 = mean12_10000.getVal()-n*sigma12_10000_1.getVal();
  Float_t max12_10000 = mean12_10000.getVal()+n*sigma12_10000_1.getVal();
  
  x_10000.setRange("gaus12_10000",min12_10000,max12_10000);

  RooAbsReal* integral12_10000 = signal_10000_12.createIntegral(x_10000,NormSet(x_10000),Range("gaus12_10000"));
  std::cout<<"Integral12_10000: "<<integral12_10000->getVal()*sign12_10000.getVal()<<std::endl;
  
  rHisto12_10000.plotOn(frame_10000, DrawOption( "HIST" ), LineColor(kGreen+1));
  model12_10000.plotOn(frame_10000);
  
  sigmas_10000[5] = sqrt(sigma12_10000_1.getVal()*sigma12_10000_1.getVal()+sigma12_10000_2.getVal()*sigma12_10000_2.getVal());
  means_10000[5] = mean12_10000.getVal();
  entries_10000[5] = integral12_10000->getVal()*sign12_10000.getVal();
 
//--------------------------------------------- 10000e+--14 MeV----------------------------------------------
    
  RooRealVar meanbkg_10000_14("meanbkg_10000_14", "meanbkg_10000_14", 400., 400., 200000.);
  RooRealVar sigmabkg_10000_14("sigmabkg_10000_14", "sigmabkg_10000_14", 500., 0., 5000.);
  RooGaussian gaussbkg_10000_14("gaussbkg_10000_14", "gaussbkg_10000_14", x_10000, meanbkg_10000_14, sigmabkg_10000_14);
 
  RooRealVar mean14_10000("mean14_10000", "mean14_10000", 188., 120., 200.);
  RooRealVar sigma14_10000_1("sigma14_10000_1", "sigma14_10000_1", 5., 0., 50.);
  RooGaussian gauss14_10000_1("gauss14_10000_1", "gauss14_10000_1", x_10000, mean14_10000, sigma14_10000_1);
  RooRealVar sigma14_10000_2("sigma14_10000_2", "sigma14_10000_2", 100., 0., 300.);
  RooGaussian gauss14_10000_2("gauss14_10000_2", "gauss14_10000_2", x_10000, mean14_10000, sigma14_10000_2);

  RooRealVar signal1frac_10000_14("signal1frac_10000_100004","signal1frac_10000_100004",0.,1.);
  RooAddPdf signal_10000_14("signal_10000_100004","signal_10000_14",{gauss14_10000_1, gauss14_10000_2},signal1frac_10000_14);
   
  RooRealVar bkgn14_10000("bkgn14_10000","bkgn14_10000",0.,1000000.);
  RooRealVar sign14_10000("sign14_10000","sign14_10000",0.,100000.);
  
  RooAddPdf model14_10000("model14_10000", "model14_10000", {gaussbkg_10000_14,signal_10000_14}, RooArgList(bkgn14_10000,sign14_10000));

  std::unique_ptr<RooFitResult> fitResult14_10000{model14_10000.fitTo(rHisto14_10000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult14_10000->Print();

  Float_t min14_10000 = mean14_10000.getVal()-n*sigma14_10000_1.getVal();
  Float_t max14_10000 = mean14_10000.getVal()+n*sigma14_10000_1.getVal();
  
  x_10000.setRange("gaus14_10000",min14_10000,max14_10000);

  RooAbsReal* integral14_10000 = signal_10000_14.createIntegral(x_10000,NormSet(x_10000),Range("gaus14_10000"));
  std::cout<<"Integral14_10000: "<<integral14_10000->getVal()*sign14_10000.getVal()<<std::endl;

  rHisto14_10000.plotOn(frame_10000, DrawOption( "HIST" ), LineColor(kGreen+3));
  model14_10000.plotOn(frame_10000);
  
  sigmas_10000[6] = sqrt(sigma14_10000_1.getVal()*sigma14_10000_1.getVal()+sigma14_10000_2.getVal()*sigma14_10000_2.getVal());
  means_10000[6] = mean14_10000.getVal();
  entries_10000[6] = integral14_10000->getVal()*sign14_10000.getVal();
 
//--------------------------------------------- 10000e+--16 MeV----------------------------------------------
      
  RooRealVar meanbkg_10000_16("meanbkg_10000_16", "meanbkg_10000_16", 400., 400., 200000.);
  RooRealVar sigmabkg_10000_16("sigmabkg_10000_16", "sigmabkg_10000_16", 100., 0., 5000.);
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
  
  RooAddPdf model16_10000("model16_10000", "model16_10000", {gaussbkg_10000_16,signal_10000_16}, RooArgList(bkgn16_10000,sign16_10000));

  std::unique_ptr<RooFitResult> fitResult16_10000{model16_10000.fitTo(rHisto16_10000, Save(), PrintLevel(-1), Range("signala"))};
  
  fitResult16_10000->Print();

  Float_t min16_10000 = mean16_10000.getVal()-n*sigma16_10000_1.getVal();
  Float_t max16_10000 = mean16_10000.getVal()+n*sigma16_10000_1.getVal();
  
  x_10000.setRange("gaus16_10000",min16_10000,max16_10000);

  RooAbsReal* integral16_10000 = signal_10000_16.createIntegral(x_10000,NormSet(x_10000),Range("gaus16_10000"));
  std::cout<<"Integral16_10000: "<<integral16_10000->getVal()*sign16_10000.getVal()<<std::endl;
 
  rHisto16_10000.plotOn(frame_10000, DrawOption( "HIST" ), LineColor(kCyan+1));
  model16_10000.plotOn(frame_10000, Range("signala"));
    
  sigmas_10000[7] = sqrt(sigma16_10000_1.getVal()*sigma16_10000_1.getVal()+sigma16_10000_2.getVal()*sigma16_10000_2.getVal());
  means_10000[7] = mean16_10000.getVal();
  entries_10000[7] = integral16_10000->getVal()*sign16_10000.getVal();
 
//--------------------------------------------- 10000e+--18 MeV----------------------------------------------
      
  RooRealVar meanbkg_10000_18("meanbkg_10000_18", "meanbkg_10000_18", 400., 400., 200000.);
  RooRealVar sigmabkg_10000_18("sigmabkg_10000_18", "sigmabkg_10000_18", 100., 0., 5000.);
  RooGaussian gaussbkg_10000_18("gaussbkg_10000_18", "gaussbkg_10000_18", x_10000, meanbkg_10000_18, sigmabkg_10000_18);
   
  RooRealVar mean18_10000("mean18_10000", "mean18_10000", 300., 120., 350.);
  RooRealVar sigma18_10000_1("sigma18_10000_1", "sigma18_10000_1", 5., 0., 50.);
  RooGaussian gauss18_10000_1("gauss18_10000_1", "gauss18_10000_1", x_10000, mean18_10000, sigma18_10000_1);
  RooRealVar sigma18_10000_2("sigma18_10000_2", "sigma18_10000_2", 100., 10., 300.);
  RooGaussian gauss18_10000_2("gauss18_10000_2", "gauss18_10000_2", x_10000, mean18_10000, sigma18_10000_2);

  RooRealVar signal1frac_10000_18("signal1frac_10000_18","signal1frac_10000_18",0.,1.);
  RooAddPdf signal_10000_18("signal_10000_18","signal_10000_18",{gauss18_10000_1, gauss18_10000_2},signal1frac_10000_18);
       
  RooRealVar bkgn18_10000("bkgn18_10000","bkgn18_10000",0.,1000000.);
  RooRealVar sign18_10000("sign18_10000","sign18_10000",0.,1000000.);
  
  RooAddPdf model18_10000("model18_10000", "model18_10000", {gaussbkg_10000_18,signal_10000_18}, RooArgList(bkgn18_10000,sign18_10000));

    RooAddPdf sigmodel18_10000("model18_10000", "model18_10000", {signal_10000_18}, RooArgList(sign18_10000));

  std::unique_ptr<RooFitResult> fitResult18_10000{model18_10000.fitTo(rHisto18_10000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult18_10000->Print();

  Float_t min18_10000 = mean18_10000.getVal()-n*sigma18_10000_1.getVal();
  Float_t max18_10000 = mean18_10000.getVal()+n*sigma18_10000_1.getVal();
  
  x_10000.setRange("gaus18_10000",min18_10000,max18_10000);

  RooAbsReal* integral18_10000 = signal_10000_18.createIntegral(x_10000,NormSet(x_10000),Range("gaus18_10000"));
  std::cout<<"Integral18_10000: "<<integral18_10000->getVal()*sign18_10000.getVal()<<std::endl;

  rHisto18_10000.plotOn(frame_10000, DrawOption( "HIST" ), LineColor(kCyan+3));
  model18_10000.plotOn(frame_10000);
  
  sigmas_10000[8] = sqrt(sigma18_10000_1.getVal()*sigma18_10000_1.getVal()+sigma18_10000_2.getVal()*sigma18_10000_2.getVal());
  means_10000[8] = mean18_10000.getVal();
  entries_10000[8] = integral18_10000->getVal()*sign18_10000.getVal();
 
//--------------------------------------------- 10000e+--19 MeV----------------------------------------------
        
  RooRealVar meanbkg_10000_19("meanbkg_10000_19", "meanbkg_10000_19", 400., 400., 200000.);
  RooRealVar sigmabkg_10000_19("sigmabkg_10000_19", "sigmabkg_10000_19", 100., 0., 2000.);
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
  
  RooAddPdf model19_10000("model19_10000", "model19_10000", {gaussbkg_10000_19,signal_10000_19}, RooArgList(bkgn19_10000,sign19_10000));

  std::unique_ptr<RooFitResult> fitResult19_10000{model19_10000.fitTo(rHisto19_10000, Save(), PrintLevel(-1), Range("signalaa"))};
  fitResult19_10000->Print();

  Float_t min19_10000 = mean19_10000.getVal()-n*sigma19_10000_1.getVal();
  Float_t max19_10000 = mean19_10000.getVal()+n*sigma19_10000_1.getVal();
  
  x_10000.setRange("gaus19_10000",min19_10000,max19_10000);

  RooAbsReal* integral19_10000 = signal_10000_19.createIntegral(x_10000,NormSet(x_10000),Range("gaus19_10000"));
  std::cout<<"Integral19_10000: "<<integral19_10000->getVal()*sign19_10000.getVal()<<std::endl;

  rHisto19_10000.plotOn(frame_10000, DrawOption( "HIST" ), LineColor(kBlue));
  model19_10000.plotOn(frame_10000);
  
  sigmas_10000[9] = sqrt(sigma19_10000_1.getVal()*sigma19_10000_1.getVal()+sigma19_10000_2.getVal()*sigma19_10000_2.getVal());
  means_10000[9] = mean19_10000.getVal();
  entries_10000[9] = integral19_10000->getVal()*sign19_10000.getVal();

 
  outRoot->WriteObject(frame_10000,"M^{2}_{miss} for different A' masses (10000 e+ per bunch)");


  //------------------------------------------------------------------------------------------------------------
  //Calculate for 25000 e+

  RooRealVar x_25000("x_25000", "x_25000", -200, 400);
  RooDataHist rHisto2_25000("rHisto2_25000", "rHisto2_25000", x_25000, Import(*Histo2_25000));
  RooDataHist rHisto4_25000("rHisto4_25000", "rHisto4_25000", x_25000, Import(*Histo4_25000));
  RooDataHist rHisto6_25000("rHisto6_25000", "rHisto6_25000", x_25000, Import(*Histo6_25000));
  RooDataHist rHisto8_25000("rHisto8_25000", "rHisto8_25000", x_25000, Import(*Histo8_25000));
  RooDataHist rHisto10_25000("rHisto10_25000", "rHisto10_25000", x_25000, Import(*Histo10_25000));
  RooDataHist rHisto12_25000("rHisto12_25000", "rHisto12_25000", x_25000, Import(*Histo12_25000));
  RooDataHist rHisto14_25000("rHisto14_25000", "rHisto14_25000", x_25000, Import(*Histo14_25000));
  RooDataHist rHisto16_25000("rHisto16_25000", "rHisto16_25000", x_25000, Import(*Histo16_25000));
  RooDataHist rHisto18_25000("rHisto18_25000", "rHisto18_25000", x_25000, Import(*Histo18_25000));
  RooDataHist rHisto19_25000("rHisto19_25000", "rHisto19_25000", x_25000, Import(*Histo19_25000));

  RooPlot *frame_25000 = x_25000.frame(Title("Test"));
  x_25000.setRange("signalaaa",-250.,390.);
  x_25000.setRange("signala",140.,320.);
  x_25000.setRange("signalaa",240.,395.);
  
  //--------------------------------------------- 25000e+---2 MeV----------------------------------------------

  RooRealVar meanbkg_25000_2("meanbkg_25000_2", "meanbkg_25000_2", 400, 400., 200000.);
  RooRealVar sigmabkg_25000_2("sigmabkg_25000_2", "sigmabkg_25000_2", 500., 0., 2000.);
  RooGaussian gaussbkg_25000_2("gaussbkg_25000_2", "gaussbkg_25000_2", x_25000, meanbkg_25000_2, sigmabkg_25000_2);

  RooRealVar mean2_25000("mean2_25000", "mean2_25000", 0.3, -5., 5.);
  RooRealVar sigma2_25000_1("sigma2_25000_1", "sigma2_25000_1", 5., 0., 50.);
  RooGaussian gauss2_25000_1("gauss2_25000_1", "gauss2_25000_1", x_25000, mean2_25000, sigma2_25000_1); 
  RooRealVar sigma2_25000_2("sigma2_25000_2", "sigma2_25000_2",100., 20., 500.);
  RooGaussian gauss2_25000_2("gauss2_25000_2", "gauss2_25000_2", x_25000, mean2_25000, sigma2_25000_2);

  RooRealVar signal1frac_25000_2("signal1frac_25000_2","signal1frac_25000_2",0.,1.);
  RooAddPdf signal_25000_2("signal_25000_2","signal_25000_2",{gauss2_25000_1, gauss2_25000_2},signal1frac_25000_2);  
   
  RooRealVar bkgn2_25000("bkgn2_25000","bkgn2_25000",0.,1000000.);
  RooRealVar sign2_25000("sign2_25000","sign2_25000",0.,100000.);
  
  RooAddPdf model2_25000("model2_25000", "model2_25000", {gaussbkg_25000_2,signal_25000_2}, RooArgList(bkgn2_25000,sign2_25000));

  std::unique_ptr<RooFitResult> fitResult2_25000{model2_25000.fitTo(rHisto2_25000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult2_25000->Print();

  Float_t min2_25000 = mean2_25000.getVal()-n*sigma2_25000_1.getVal();
  Float_t max2_25000 = mean2_25000.getVal()+n*sigma2_25000_1.getVal();
  
  x_25000.setRange("gaus2_25000",min2_25000,max2_25000);

  RooAbsReal* integral2_25000 = signal_25000_2.createIntegral(x_25000,NormSet(x_25000),Range("gaus2_25000"));
  std::cout<<"Integral2_25000: "<<integral2_25000->getVal()*sign2_25000.getVal()<<std::endl;

  rHisto2_25000.plotOn(frame_25000, DrawOption( "HIST" ), LineColor(kMagenta+1));
  model2_25000.plotOn(frame_25000);

  sigmas_25000[0] = sqrt(sigma2_25000_1.getVal()*sigma2_25000_1.getVal()+sigma2_25000_2.getVal()*sigma2_25000_2.getVal());
  means_25000[0] = mean2_25000.getVal();
  entries_25000[0] = integral2_25000->getVal()*sign2_25000.getVal();
  
  //--------------------------------------------- 25000e+---4 MeV----------------------------------------------

  RooRealVar meanbkg_25000_4("meanbkg_25000_4", "meanbkg_25000_4", 400, 400., 200000.);
  RooRealVar sigmabkg_25000_4("sigmabkg_25000_4", "sigmabkg_25000_4", 100., 0., 5000.);
  RooGaussian gaussbkg_25000_4("gaussbkg_25000_4", "gaussbkg_25000_4", x_25000, meanbkg_25000_4, sigmabkg_25000_4);
  
  RooRealVar mean4_25000("mean4_25000", "mean4_25000", 10., 0, 20);
  RooRealVar sigma4_25000_1("sigma4_25000_1", "sigma4_25000_1", 5., 0., 50.);
  RooGaussian gauss4_25000_1("gauss4_25000_1", "gauss4_25000_1", x_25000, mean4_25000, sigma4_25000_1);
  RooRealVar sigma4_25000_2("sigma4_25000_2", "sigma4_25000_2", 100., 20., 300.);
  RooGaussian gauss4_25000_2("gauss4_25000_2", "gauss4_25000_2", x_25000, mean4_25000, sigma4_25000_2);
  
  RooRealVar signal1frac_25000_4("signal1frac_25000_4","signal1frac_25000_4",0.,1.);
  RooAddPdf signal_25000_4("signal_25000_4","signal_25000_4",{gauss4_25000_1, gauss4_25000_2},signal1frac_25000_4);

  RooRealVar bkgn4_25000("bkgn4_25000","bkgn4_25000",0.,1000000.);
  RooRealVar sign4_25000("sign4_25000","sign4_25000",0.,100000.);

  RooAddPdf model4_25000("model4_25000", "model4_25000", {gaussbkg_25000_4,signal_25000_4}, RooArgList(bkgn4_25000,sign4_25000));

  std::unique_ptr<RooFitResult> fitResult4_25000{model4_25000.fitTo(rHisto4_25000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult4_25000->Print();

  Float_t min4_25000 = mean4_25000.getVal()-n*sigma4_25000_1.getVal();
  Float_t max4_25000 = mean4_25000.getVal()+n*sigma4_25000_1.getVal();
  
  x_25000.setRange("gaus4_25000",min4_25000,max4_25000);

  RooAbsReal* integral4_25000 = signal_25000_4.createIntegral(x_25000,NormSet(x_25000),Range("gaus4_25000"));
  std::cout<<"Integral4_25000: "<<integral4_25000->getVal()*sign4_25000.getVal()<<std::endl;


  rHisto4_25000.plotOn(frame_25000, DrawOption( "HIST" ), LineColor(kRed));
  model4_25000.plotOn(frame_25000);

  sigmas_25000[1] = sqrt(sigma4_25000_1.getVal()*sigma4_25000_1.getVal()+sigma4_25000_2.getVal()*sigma4_25000_2.getVal());
  means_25000[1] = mean4_25000.getVal();
  entries_25000[1] = integral4_25000->getVal()*sign4_25000.getVal();
  
  //--------------------------------------------- 25000e+---6 MeV----------------------------------------------

  RooRealVar meanbkg_25000_6("meanbkg_25000_6", "meanbkg_25000_6", 400., 400., 200000.);
  RooRealVar sigmabkg_25000_6("sigmabkg_25000_6", "sigmabkg_25000_6", 100., 0., 5000.);
  RooGaussian gaussbkg_25000_6("gaussbkg_25000_6", "gaussbkg_25000_6", x_25000, meanbkg_25000_6, sigmabkg_25000_6);
  
  RooRealVar mean6_25000("mean6_25000", "mean6_25000", 29., 20., 40.);
  RooRealVar sigma6_25000_1("sigma6_25000_1", "sigma6_25000_1", 5., 0., 50.);
  RooGaussian gauss6_25000_1("gauss6_25000_1", "gauss6_25000_1", x_25000, mean6_25000, sigma6_25000_1);
  RooRealVar sigma6_25000_2("sigma6_25000_2", "sigma6_25000_2", 100., 20., 300.);
  RooGaussian gauss6_25000_2("gauss6_25000_2", "gauss6_25000_2", x_25000, mean6_25000, sigma6_25000_2);

  RooRealVar signal1frac_25000_6("signal1frac_25000_6","signal1frac_25000_6",0.,1.);
  RooAddPdf signal_25000_6("signal_25000_6","signal_25000_6",{gauss6_25000_1, gauss6_25000_2},signal1frac_25000_6);

  RooRealVar bkgn6_25000("bkgn6_25000","bkgn6_25000",0.,1000000.);
  RooRealVar sign6_25000("sign6_25000","sign6_25000",0.,100000.);
  
  RooAddPdf model6_25000("model6_25000", "model6_25000", {gaussbkg_25000_6,signal_25000_6}, RooArgList(bkgn6_25000,sign6_25000));

  std::unique_ptr<RooFitResult> fitResult6_25000{model6_25000.fitTo(rHisto6_25000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult6_25000->Print();

  Float_t min6_25000 = mean6_25000.getVal()-n*sigma6_25000_1.getVal();
  Float_t max6_25000 = mean6_25000.getVal()+n*sigma6_25000_1.getVal();
  
  x_25000.setRange("gaus6_25000",min6_25000,max6_25000);

  RooAbsReal* integral6_25000 = signal_25000_6.createIntegral(x_25000,NormSet(x_25000),Range("gaus6_25000"));
  std::cout<<"Integral6_25000: "<<integral6_25000->getVal()*sign6_25000.getVal()<<std::endl;

  rHisto6_25000.plotOn(frame_25000, DrawOption( "HIST" ), LineColor(kRed+2));
  model6_25000.plotOn(frame_25000);
  
  sigmas_25000[2] = sqrt(sigma6_25000_1.getVal()*sigma6_25000_1.getVal()+sigma6_25000_2.getVal()*sigma6_25000_2.getVal());
  means_25000[2] = mean6_25000.getVal();
  entries_25000[2] = integral6_25000->getVal()*sign6_25000.getVal();
 
//--------------------------------------------- 25000e+---8 MeV----------------------------------------------

  RooRealVar meanbkg_25000_8("meanbkg_25000_8", "meanbkg_25000_8", 400., 400., 200000.);
  RooRealVar sigmabkg_25000_8("sigmabkg_25000_8", "sigmabkg_25000_8", 100., 0., 5000.);
  RooGaussian gaussbkg_25000_8("gaussbkg_25000_8", "gaussbkg_25000_8", x_25000, meanbkg_25000_8, sigmabkg_25000_8);
  
  RooRealVar mean8_25000("mean8_25000", "mean8_25000", 57., 40., 70.);
  RooRealVar sigma8_25000_1("sigma8_25000_1", "sigma8_25000_1", 5., 0., 50.);
  RooGaussian gauss8_25000_1("gauss8_25000_1", "gauss8_25000_1", x_25000, mean8_25000, sigma8_25000_1);
  RooRealVar sigma8_25000_2("sigma8_25000_2", "sigma8_25000_2", 100., 20., 300.);
  RooGaussian gauss8_25000_2("gauss8_25000_2", "gauss8_25000_2", x_25000, mean8_25000, sigma8_25000_2);

  RooRealVar signal1frac_25000_8("signal1frac_25000_8","signal1frac_25000_8",0.,1.);
  RooAddPdf signal_25000_8("signal_25000_8","signal_25000_8",{gauss8_25000_1, gauss8_25000_2},signal1frac_25000_8);

  RooRealVar bkgn8_25000("bkgn8_25000","bkgn8_25000",0.,1000000.);
  RooRealVar sign8_25000("sign8_25000","sign8_25000",0.,100000.);
  
  RooAddPdf model8_25000("model8_25000", "model8_25000", {gaussbkg_25000_8,signal_25000_8}, RooArgList(bkgn8_25000,sign8_25000));

  std::unique_ptr<RooFitResult> fitResult8_25000{model8_25000.fitTo(rHisto8_25000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult8_25000->Print();

  Float_t min8_25000 = mean8_25000.getVal()-n*sigma8_25000_1.getVal();
  Float_t max8_25000 = mean8_25000.getVal()+n*sigma8_25000_1.getVal();
  
  x_25000.setRange("gaus8_25000",min8_25000,max8_25000);

  RooAbsReal* integral8_25000 = signal_25000_8.createIntegral(x_25000,NormSet(x_25000),Range("gaus8_25000"));
  std::cout<<"Integral8_25000: "<<integral8_25000->getVal()*sign8_25000.getVal()<<std::endl;

  rHisto8_25000.plotOn(frame_25000, DrawOption( "HIST" ), LineColor(kOrange-3));
  model8_25000.plotOn(frame_25000);
  
  sigmas_25000[3] = sqrt(sigma8_25000_1.getVal()*sigma8_25000_1.getVal()+sigma8_25000_2.getVal()*sigma8_25000_2.getVal());
  means_25000[3] = mean8_25000.getVal();
  entries_25000[3] = integral8_25000->getVal()*sign8_25000.getVal();
 
//--------------------------------------------- 25000e+--10 MeV----------------------------------------------
  
  RooRealVar meanbkg_25000_10("meanbkg_25000_10", "meanbkg_25000_10", 400., 400., 200000.);
  RooRealVar sigmabkg_25000_10("sigmabkg_25000_10", "sigmabkg_25000_10", 100., 0., 5000.);
  RooGaussian gaussbkg_25000_10("gaussbkg_25000_10", "gaussbkg_25000_10", x_25000, meanbkg_25000_10, sigmabkg_25000_10);
 
  RooRealVar mean10_25000("mean10_25000", "mean10_25000", 93., 80., 110.);
  RooRealVar sigma10_25000_1("sigma10_25000_1", "sigma10_25000_1", 5., 0., 50.);
  RooGaussian gauss10_25000_1("gauss10_25000_1", "gauss10_25000_1", x_25000, mean10_25000, sigma10_25000_1);
  RooRealVar sigma10_25000_2("sigma10_25000_2", "sigma10_25000_2", 100., 20., 300.);
  RooGaussian gauss10_25000_2("gauss10_25000_2", "gauss10_25000_2", x_25000, mean10_25000, sigma10_25000_2);
  
  RooRealVar signal1frac_25000_10("signal1frac_25000_10","signal1frac_25000_10",0.,1.);
  RooAddPdf signal_25000_10("signal_25000_10","signal_25000_10",{gauss10_25000_1, gauss10_25000_2},signal1frac_25000_10);
 
  RooRealVar bkgn10_25000("bkgn10_25000","bkgn10_25000",0.,1000000.); 
  RooRealVar sign10_25000("sign10_25000","sign10_25000",0.,100000.);
  
  RooAddPdf model10_25000("model10_25000", "model10_25000", {gaussbkg_25000_10,signal_25000_10}, RooArgList(bkgn10_25000,sign10_25000));

  std::unique_ptr<RooFitResult> fitResult10_25000{model10_25000.fitTo(rHisto10_25000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult10_25000->Print();

  Float_t min10_25000 = mean10_25000.getVal()-n*sigma10_25000_1.getVal();
  Float_t max10_25000 = mean10_25000.getVal()+n*sigma10_25000_1.getVal();
  
  x_25000.setRange("gaus10_25000",min10_25000,max10_25000);

  RooAbsReal* integral10_25000 = signal_25000_10.createIntegral(x_25000,NormSet(x_25000),Range("gaus10_25000"));
  std::cout<<"Integral10_25000: "<<integral10_25000->getVal()*sign10_25000.getVal()<<std::endl;

  rHisto10_25000.plotOn(frame_25000, DrawOption( "HIST" ), LineColor(kYellow+2));
  model10_25000.plotOn(frame_25000);
  
  sigmas_25000[4] = sqrt(sigma10_25000_1.getVal()*sigma10_25000_1.getVal()+sigma10_25000_2.getVal()*sigma10_25000_2.getVal());
  means_25000[4] = mean10_25000.getVal();
  entries_25000[4] = integral10_25000->getVal()*sign10_25000.getVal();
 
//--------------------------------------------- 25000e+--12 MeV----------------------------------------------
  
  RooRealVar meanbkg_25000_12("meanbkg_25000_12", "meanbkg_25000_12", 400., 400., 200000.);
  RooRealVar sigmabkg_25000_12("sigmabkg_25000_12", "sigmabkg_25000_12", 100., 0., 5000.);
  RooGaussian gaussbkg_25000_12("gaussbkg_25000_12", "gaussbkg_25000_12", x_25000, meanbkg_25000_12, sigmabkg_25000_12);
  
  RooRealVar mean12_25000("mean12_25000", "mean12_25000", 136., 100., 150.);
  RooRealVar sigma12_25000_1("sigma12_25000_1", "sigma12_25000_1", 5., 0., 50.);
  RooGaussian gauss12_25000_1("gauss12_25000_1", "gauss12_25000_1", x_25000, mean12_25000, sigma12_25000_1);
  RooRealVar sigma12_25000_2("sigma12_25000_2", "sigma12_25000_2", 100., 10., 300.);
  RooGaussian gauss12_25000_2("gauss12_25000_2", "gauss12_25000_2", x_25000, mean12_25000, sigma12_25000_2);

  RooRealVar signal1frac_25000_12("signal1frac_25000_12","signal1frac_25000_12",0.,1.);
  RooAddPdf signal_25000_12("signal_25000_12","signal_25000_12",{gauss12_25000_1, gauss12_25000_2},signal1frac_25000_12);
  
  RooRealVar bkgn12_25000("bkgn12_25000","bkgn12_25000",0.,1000000.);
  RooRealVar sign12_25000("sign12_25000","sign12_25000",0.,100000.);
  
  RooAddPdf model12_25000("model12_25000", "model12_25000", {gaussbkg_25000_12,signal_25000_12}, RooArgList(bkgn12_25000,sign12_25000));

  std::unique_ptr<RooFitResult> fitResult12_25000{model12_25000.fitTo(rHisto12_25000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult12_25000->Print();

  Float_t min12_25000 = mean12_25000.getVal()-n*sigma12_25000_1.getVal();
  Float_t max12_25000 = mean12_25000.getVal()+n*sigma12_25000_1.getVal();
  
  x_25000.setRange("gaus12_25000",min12_25000,max12_25000);

  RooAbsReal* integral12_25000 = signal_25000_12.createIntegral(x_25000,NormSet(x_25000),Range("gaus12_25000"));
  std::cout<<"Integral12_25000: "<<integral12_25000->getVal()*sign12_25000.getVal()<<std::endl;
 
  rHisto12_25000.plotOn(frame_25000, DrawOption( "HIST" ), LineColor(kGreen+1));
  model12_25000.plotOn(frame_25000);
  
  sigmas_25000[5] = sqrt(sigma12_25000_1.getVal()*sigma12_25000_1.getVal()+sigma12_25000_2.getVal()*sigma12_25000_2.getVal());
  means_25000[5] = mean12_25000.getVal();
  entries_25000[5] = integral12_25000->getVal()*sign12_25000.getVal();
 
//--------------------------------------------- 25000e+--14 MeV----------------------------------------------
    
  RooRealVar meanbkg_25000_14("meanbkg_25000_14", "meanbkg_25000_14", 400., 400., 200000.);
  RooRealVar sigmabkg_25000_14("sigmabkg_25000_14", "sigmabkg_25000_14", 500., 0., 5000.);
  RooGaussian gaussbkg_25000_14("gaussbkg_25000_14", "gaussbkg_25000_14", x_25000, meanbkg_25000_14, sigmabkg_25000_14);
 
  RooRealVar mean14_25000("mean14_25000", "mean14_25000", 188., 120., 200.);
  RooRealVar sigma14_25000_1("sigma14_25000_1", "sigma14_25000_1", 5., 0., 50.);
  RooGaussian gauss14_25000_1("gauss14_25000_1", "gauss14_25000_1", x_25000, mean14_25000, sigma14_25000_1);
  RooRealVar sigma14_25000_2("sigma14_25000_2", "sigma14_25000_2", 100., 0., 300.);
  RooGaussian gauss14_25000_2("gauss14_25000_2", "gauss14_25000_2", x_25000, mean14_25000, sigma14_25000_2);

  RooRealVar signal1frac_25000_14("signal1frac_25000_250004","signal1frac_25000_250004",0.,1.);
  RooAddPdf signal_25000_14("signal_25000_250004","signal_25000_14",{gauss14_25000_1, gauss14_25000_2},signal1frac_25000_14);
   
  RooRealVar bkgn14_25000("bkgn14_25000","bkgn14_25000",0.,1000000.);
  RooRealVar sign14_25000("sign14_25000","sign14_25000",0.,100000.);
  
  RooAddPdf model14_25000("model14_25000", "model14_25000", {gaussbkg_25000_14,signal_25000_14}, RooArgList(bkgn14_25000,sign14_25000));

  std::unique_ptr<RooFitResult> fitResult14_25000{model14_25000.fitTo(rHisto14_25000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult14_25000->Print();

  Float_t min14_25000 = mean14_25000.getVal()-n*sigma14_25000_1.getVal();
  Float_t max14_25000 = mean14_25000.getVal()+n*sigma14_25000_1.getVal();
  
  x_25000.setRange("gaus14_25000",min14_25000,max14_25000);

  RooAbsReal* integral14_25000 = signal_25000_14.createIntegral(x_25000,NormSet(x_25000),Range("gaus14_25000"));
  std::cout<<"Integral14_25000: "<<integral14_25000->getVal()*sign14_25000.getVal()<<std::endl;

  rHisto14_25000.plotOn(frame_25000, DrawOption( "HIST" ), LineColor(kGreen+3));
  model14_25000.plotOn(frame_25000);
  
  sigmas_25000[6] = sqrt(sigma14_25000_1.getVal()*sigma14_25000_1.getVal()+sigma14_25000_2.getVal()*sigma14_25000_2.getVal());
  means_25000[6] = mean14_25000.getVal();
  entries_25000[6] = integral14_25000->getVal()*sign14_25000.getVal();
 
//--------------------------------------------- 25000e+--16 MeV----------------------------------------------
      
  RooRealVar meanbkg_25000_16("meanbkg_25000_16", "meanbkg_25000_16", 400., 400., 200000.);
  RooRealVar sigmabkg_25000_16("sigmabkg_25000_16", "sigmabkg_25000_16", 100., 0., 5000.);
  RooGaussian gaussbkg_25000_16("gaussbkg_25000_16", "gaussbkg_25000_16", x_25000, meanbkg_25000_16, sigmabkg_25000_16);
 
  RooRealVar mean16_25000("mean16_25000", "mean16_25000", 240., 120., 350.);
  RooRealVar sigma16_25000_1("sigma16_25000_1", "sigma16_25000_1", 5., 0., 50.);
  RooGaussian gauss16_25000_1("gauss16_25000_1", "gauss16_25000_1", x_25000, mean16_25000, sigma16_25000_1);
  RooRealVar sigma16_25000_2("sigma16_25000_2", "sigma16_25000_2", 100., 20., 500.);
  RooGaussian gauss16_25000_2("gauss16_25000_2", "gauss16_25000_2", x_25000, mean16_25000, sigma16_25000_2);
  

  RooRealVar signal1frac_25000_16("signal1frac_25000_16","signal1frac_25000_16",0.,1.);
  RooAddPdf signal_25000_16("signal_25000_16","signal_25000_16",{gauss16_25000_1, gauss16_25000_2},signal1frac_25000_16);
     
  RooRealVar bkgn16_25000("bkgn16_25000","bkgn16_25000",0.,1000000.);
  RooRealVar sign16_25000("sign16_25000","sign16_25000",0.,100000.);
  
  RooAddPdf model16_25000("model16_25000", "model16_25000", {gaussbkg_25000_16,signal_25000_16}, RooArgList(bkgn16_25000,sign16_25000));

  std::unique_ptr<RooFitResult> fitResult16_25000{model16_25000.fitTo(rHisto16_25000, Save(), PrintLevel(-1), Range("signala"))};
  
  fitResult16_25000->Print();

  Float_t min16_25000 = mean16_25000.getVal()-n*sigma16_25000_1.getVal();
  Float_t max16_25000 = mean16_25000.getVal()+n*sigma16_25000_1.getVal();
  
  x_25000.setRange("gaus16_25000",min16_25000,max16_25000);

  RooAbsReal* integral16_25000 = signal_25000_16.createIntegral(x_25000,NormSet(x_25000),Range("gaus16_25000"));
  std::cout<<"Integral16_25000: "<<integral16_25000->getVal()*sign16_25000.getVal()<<std::endl;
 
  rHisto16_25000.plotOn(frame_25000, DrawOption( "HIST" ), LineColor(kCyan+1));
  model16_25000.plotOn(frame_25000, Range("signala"));
    
  sigmas_25000[7] = sqrt(sigma16_25000_1.getVal()*sigma16_25000_1.getVal()+sigma16_25000_2.getVal()*sigma16_25000_2.getVal());
  means_25000[7] = mean16_25000.getVal();
  entries_25000[7] = integral16_25000->getVal()*sign16_25000.getVal();
 
//--------------------------------------------- 25000e+--18 MeV----------------------------------------------
      
  RooRealVar meanbkg_25000_18("meanbkg_25000_18", "meanbkg_25000_18", 400., 400., 200000.);
  RooRealVar sigmabkg_25000_18("sigmabkg_25000_18", "sigmabkg_25000_18", 100., 0., 5000.);
  RooGaussian gaussbkg_25000_18("gaussbkg_25000_18", "gaussbkg_25000_18", x_25000, meanbkg_25000_18, sigmabkg_25000_18);
   
  RooRealVar mean18_25000("mean18_25000", "mean18_25000", 300., 120., 350.);
  RooRealVar sigma18_25000_1("sigma18_25000_1", "sigma18_25000_1", 5., 0., 50.);
  RooGaussian gauss18_25000_1("gauss18_25000_1", "gauss18_25000_1", x_25000, mean18_25000, sigma18_25000_1);
  RooRealVar sigma18_25000_2("sigma18_25000_2", "sigma18_25000_2", 100., 10., 300.);
  RooGaussian gauss18_25000_2("gauss18_25000_2", "gauss18_25000_2", x_25000, mean18_25000, sigma18_25000_2);

  RooRealVar signal1frac_25000_18("signal1frac_25000_18","signal1frac_25000_18",0.,1.);
  RooAddPdf signal_25000_18("signal_25000_18","signal_25000_18",{gauss18_25000_1, gauss18_25000_2},signal1frac_25000_18);
       
  RooRealVar bkgn18_25000("bkgn18_25000","bkgn18_25000",0.,1000000.);
  RooRealVar sign18_25000("sign18_25000","sign18_25000",0.,1000000.);
  
  RooAddPdf model18_25000("model18_25000", "model18_25000", {gaussbkg_25000_18,signal_25000_18}, RooArgList(bkgn18_25000,sign18_25000));

  std::unique_ptr<RooFitResult> fitResult18_25000{model18_25000.fitTo(rHisto18_25000, Save(), PrintLevel(-1), Range("signalaaa"))};
  fitResult18_25000->Print();

  Float_t min18_25000 = mean18_25000.getVal()-n*sigma18_25000_1.getVal();
  Float_t max18_25000 = mean18_25000.getVal()+n*sigma18_25000_1.getVal();
  
  x_25000.setRange("gaus18_25000",min18_25000,max18_25000);

  RooAbsReal* integral18_25000 = signal_25000_18.createIntegral(x_25000,NormSet(x_25000),Range("gaus18_25000"));
  std::cout<<"Integral18_25000: "<<integral18_25000->getVal()*sign18_25000.getVal()<<std::endl;

  rHisto18_25000.plotOn(frame_25000, DrawOption( "HIST" ), LineColor(kCyan+3));
  model18_25000.plotOn(frame_25000);
    
  sigmas_25000[8] = sqrt(sigma18_25000_1.getVal()*sigma18_25000_1.getVal()+sigma18_25000_2.getVal()*sigma18_25000_2.getVal());
  means_25000[8] = mean18_25000.getVal();
  entries_25000[8] = integral18_25000->getVal()*sign18_25000.getVal();
 
//--------------------------------------------- 25000e+--19 MeV----------------------------------------------
        
  RooRealVar meanbkg_25000_19("meanbkg_25000_19", "meanbkg_25000_19", 400., 400., 200000.);
  RooRealVar sigmabkg_25000_19("sigmabkg_25000_19", "sigmabkg_25000_19", 100., 0., 2000.);
  RooGaussian gaussbkg_25000_19("gaussbkg_25000_19", "gaussbkg_25000_19", x_25000, meanbkg_25000_19, sigmabkg_25000_19);
  
  RooRealVar mean19_25000("mean19_25000", "mean19_25000", 350., 120., 450.);
  RooRealVar sigma19_25000_1("sigma19_25000_1", "sigma19_25000_1", 5., 0., 50.);
  RooGaussian gauss19_25000_1("gauss19_25000_1", "gauss19_25000_1", x_25000, mean19_25000, sigma19_25000_1);
  RooRealVar sigma19_25000_2("sigma19_25000_2", "sigma19_25000_2", 100., 5., 300.);
  RooGaussian gauss19_25000_2("gauss19_25000_2", "gauss19_25000_2", x_25000, mean19_25000, sigma19_25000_2);

  RooRealVar signal1frac_25000_19("signal1frac_25000_19","signal1frac_25000_19",0.,1.);
  RooAddPdf signal_25000_19("signal_25000_19","signal_25000_19",{gauss19_25000_1, gauss19_25000_2},signal1frac_25000_19);
       
  RooRealVar bkgn19_25000("bkgn19_25000","bkgn19_25000",0.,10000000.);
  RooRealVar sign19_25000("sign19_25000","sign19_25000",0.,1000000.);
  
  RooAddPdf model19_25000("model19_25000", "model19_25000", {gaussbkg_25000_19,signal_25000_19}, RooArgList(bkgn19_25000,sign19_25000));

  std::unique_ptr<RooFitResult> fitResult19_25000{model19_25000.fitTo(rHisto19_25000, Save(), PrintLevel(-1), Range("signalaa"))};
  fitResult19_25000->Print();

  Float_t min19_25000 = mean19_25000.getVal()-n*sigma19_25000_1.getVal();
  Float_t max19_25000 = mean19_25000.getVal()+n*sigma19_25000_1.getVal();
  
  x_25000.setRange("gaus19_25000",min19_25000,max19_25000);

  RooAbsReal* integral19_25000 = signal_25000_19.createIntegral(x_25000,NormSet(x_25000),Range("gaus19_25000"));
  std::cout<<"Integral19_25000: "<<integral19_25000->getVal()*sign19_25000.getVal()<<std::endl;

  rHisto19_25000.plotOn(frame_25000, DrawOption( "HIST" ), LineColor(kBlue));
  model19_25000.plotOn(frame_25000);
  
  sigmas_25000[9] = sqrt(sigma19_25000_1.getVal()*sigma19_25000_1.getVal()+sigma19_25000_2.getVal()*sigma19_25000_2.getVal());
  means_25000[9] = mean19_25000.getVal();
  entries_25000[9] = integral19_25000->getVal()*sign19_25000.getVal();

 
  outRoot->WriteObject(frame_25000,"M^{2}_{miss} for different A' masses (25000 e+ per bunch)");

  
  TCanvas *c1 = new TCanvas();
 
  auto gr_1 = new TGraph (10,masses,entries_1);
  gr_1->SetLineColor(kMagenta);
  gr_1->SetLineWidth(3);
  gr_1->SetTitle("Acceptance");
  gr_1->Draw();
  auto gr_10 = new TGraph (10,masses,entries_10);
  gr_10->SetLineColor(kRed);
  gr_10->SetLineWidth(3);
  gr_10->Draw("same");
  auto gr_100 = new TGraph (10,masses,entries_100);
  gr_100->SetLineColor(kOrange);
  gr_100->SetLineWidth(3);
  gr_100->Draw("same");
  auto gr_1000 = new TGraph (10,masses,entries_1000);
  gr_1000->SetLineColor(kGreen+3);
  gr_1000->SetLineWidth(3);
  gr_1000->Draw("same");
  auto gr_10000 = new TGraph (10,masses,entries_10000);
  gr_10000->SetLineColor(kCyan+1);
  gr_10000->SetLineWidth(3);
  gr_10000->Draw("same");
  auto gr_25000 = new TGraph (10,masses,entries_25000);
  gr_25000->SetLineColor(kBlue);
  gr_25000->SetLineWidth(3);
  gr_25000->Draw("same");
  auto legend_10 = new TLegend(0.15,0.5,0.3,0.85);
  legend_10->AddEntry(gr_1,"1 e+","l");
  legend_10->AddEntry(gr_10,"10 e+","l");
  legend_10->AddEntry(gr_100,"100 e+","l");
  legend_10->AddEntry(gr_1000,"1000 e+","l");
  legend_10->AddEntry(gr_10000,"10000 e+","l");
  legend_10->AddEntry(gr_25000,"25000 e+","l");
  legend_10->Draw();

  
  outRoot->WriteObject(c1,"Acceptance");

  
  outRoot->Write();
  outRoot->Close();
}
