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
  
  TFile *f20_1 = TFile::Open("ana/New_Analysis/MCData_1e_20MeV_ana_new.root");
  TFile *f19_1 = TFile::Open("ana/New_Analysis/MCData_1e_19MeV_ana_new.root");
  TFile *f18_1 = TFile::Open("ana/New_Analysis/MCData_1e_18MeV_ana_new.root");
  TFile *f16_1 = TFile::Open("ana/New_Analysis/MCData_1e_16MeV_ana_new.root");
  TFile *f14_1 = TFile::Open("ana/New_Analysis/MCData_1e_14MeV_ana_new.root");
  TFile *f12_1 = TFile::Open("ana/New_Analysis/MCData_1e_12MeV_ana_new.root");
  TFile *f10_1 = TFile::Open("ana/New_Analysis/MCData_1e_10MeV_ana_new.root");
  TFile *f8_1 = TFile::Open("ana/New_Analysis/MCData_1e_8MeV_ana_new.root");
  TFile *f6_1 = TFile::Open("ana/New_Analysis/MCData_1e_6MeV_ana_new.root");
  TFile *f4_1 = TFile::Open("ana/New_Analysis/MCData_1e_4MeV_ana_new.root");
  TFile *f2_1 = TFile::Open("ana/New_Analysis/MCData_1e_2MeV_ana_new.root");
  TFile *f0_1 = TFile::Open("ana/New_Analysis/MCData_1e_0A_ana_new.root");
  
  TFile *f20_10 = TFile::Open("ana/New_Analysis/MCData_10e_20MeV_ana_new.root");
  TFile *f19_10 = TFile::Open("ana/New_Analysis/MCData_10e_19MeV_ana_new.root");
  TFile *f18_10 = TFile::Open("ana/New_Analysis/MCData_10e_18MeV_ana_new.root");
  TFile *f16_10 = TFile::Open("ana/New_Analysis/MCData_10e_16MeV_ana_new.root");
  TFile *f14_10 = TFile::Open("ana/New_Analysis/MCData_10e_14MeV_ana_new.root");
  TFile *f12_10 = TFile::Open("ana/New_Analysis/MCData_10e_12MeV_ana_new.root");
  TFile *f10_10 = TFile::Open("ana/New_Analysis/MCData_10e_10MeV_ana_new.root");
  TFile *f8_10 = TFile::Open("ana/New_Analysis/MCData_10e_8MeV_ana_new.root");
  TFile *f6_10 = TFile::Open("ana/New_Analysis/MCData_10e_6MeV_ana_new.root");
  TFile *f4_10 = TFile::Open("ana/New_Analysis/MCData_10e_4MeV_ana_new.root");
  TFile *f2_10 = TFile::Open("ana/New_Analysis/MCData_10e_2MeV_ana_new.root");
  TFile *f0_10 = TFile::Open("ana/New_Analysis/MCData_10e_0A_ana_new.root");
    
  TFile *f20_100 = TFile::Open("ana/New_Analysis/MCData_100e_20MeV_ana_new.root");
  TFile *f19_100 = TFile::Open("ana/New_Analysis/MCData_100e_19MeV_ana_new.root");
  TFile *f18_100 = TFile::Open("ana/New_Analysis/MCData_100e_18MeV_ana_new.root");
  TFile *f16_100 = TFile::Open("ana/New_Analysis/MCData_100e_16MeV_ana_new.root");
  TFile *f14_100 = TFile::Open("ana/New_Analysis/MCData_100e_14MeV_ana_new.root");
  TFile *f12_100 = TFile::Open("ana/New_Analysis/MCData_100e_12MeV_ana_new.root");
  TFile *f10_100 = TFile::Open("ana/New_Analysis/MCData_100e_10MeV_ana_new.root");
  TFile *f8_100 = TFile::Open("ana/New_Analysis/MCData_100e_8MeV_ana_new.root");
  TFile *f6_100 = TFile::Open("ana/New_Analysis/MCData_100e_6MeV_ana_new.root");
  TFile *f4_100 = TFile::Open("ana/New_Analysis/MCData_100e_4MeV_ana_new.root");
  TFile *f2_100 = TFile::Open("ana/New_Analysis/MCData_100e_2MeV_ana_new.root");
  TFile *f0_100 = TFile::Open("ana/New_Analysis/MCData_100e_0A_ana_new.root");
      
  TFile *f20_1000 = TFile::Open("ana/New_Analysis/MCData_1000e_20MeV_ana_new.root");
  TFile *f19_1000 = TFile::Open("ana/New_Analysis/MCData_1000e_19MeV_ana_new.root");
  TFile *f18_1000 = TFile::Open("ana/New_Analysis/MCData_1000e_18MeV_ana_new.root");
  TFile *f16_1000 = TFile::Open("ana/New_Analysis/MCData_1000e_16MeV_ana_new.root");
  TFile *f14_1000 = TFile::Open("ana/New_Analysis/MCData_1000e_14MeV_ana_new.root");
  TFile *f12_1000 = TFile::Open("ana/New_Analysis/MCData_1000e_12MeV_ana_new.root");
  TFile *f10_1000 = TFile::Open("ana/New_Analysis/MCData_1000e_10MeV_ana_new.root");
  TFile *f8_1000 = TFile::Open("ana/New_Analysis/MCData_1000e_8MeV_ana_new.root");
  TFile *f6_1000 = TFile::Open("ana/New_Analysis/MCData_1000e_6MeV_ana_new.root");
  TFile *f4_1000 = TFile::Open("ana/New_Analysis/MCData_1000e_4MeV_ana_new.root");
  TFile *f2_1000 = TFile::Open("ana/New_Analysis/MCData_1000e_2MeV_ana_new.root");
  TFile *f0_1000 = TFile::Open("ana/New_Analysis/MCData_1000e_0A_ana_new.root");
  
  TFile *f20_10000 = TFile::Open("ana/New_Analysis/MCData_10000e_20MeV_ana_new.root");
  TFile *f19_10000 = TFile::Open("ana/New_Analysis/MCData_10000e_19MeV_ana_new.root");
  TFile *f18_10000 = TFile::Open("ana/New_Analysis/MCData_10000e_18MeV_ana_new.root");
  TFile *f16_10000 = TFile::Open("ana/New_Analysis/MCData_10000e_16MeV_ana_new.root");
  TFile *f14_10000 = TFile::Open("ana/New_Analysis/MCData_10000e_14MeV_ana_new.root");
  TFile *f12_10000 = TFile::Open("ana/New_Analysis/MCData_10000e_12MeV_ana_new.root");
  TFile *f10_10000 = TFile::Open("ana/New_Analysis/MCData_10000e_10MeV_ana_new.root");
  TFile *f8_10000 = TFile::Open("ana/New_Analysis/MCData_10000e_8MeV_ana_new.root");
  TFile *f6_10000 = TFile::Open("ana/New_Analysis/MCData_10000e_6MeV_ana_new.root");
  TFile *f4_10000 = TFile::Open("ana/New_Analysis/MCData_10000e_4MeV_ana_new.root");
  TFile *f2_10000 = TFile::Open("ana/New_Analysis/MCData_10000e_2MeV_ana_new.root");
  TFile *f0_10000 = TFile::Open("ana/New_Analysis/MCData_10000e_0A_ana_new.root");
  
  TFile *f20_25000 = TFile::Open("ana/New_Analysis/MCData_25000e_20MeV_ana_new.root");
  TFile *f19_25000 = TFile::Open("ana/New_Analysis/MCData_25000e_19MeV_ana_new.root");
  TFile *f18_25000 = TFile::Open("ana/New_Analysis/MCData_25000e_18MeV_ana_new.root");
  TFile *f16_25000 = TFile::Open("ana/New_Analysis/MCData_25000e_16MeV_ana_new.root");
  TFile *f14_25000 = TFile::Open("ana/New_Analysis/MCData_25000e_14MeV_ana_new.root");
  TFile *f12_25000 = TFile::Open("ana/New_Analysis/MCData_25000e_12MeV_ana_new.root");
  TFile *f10_25000 = TFile::Open("ana/New_Analysis/MCData_25000e_10MeV_ana_new.root");
  TFile *f8_25000 = TFile::Open("ana/New_Analysis/MCData_25000e_8MeV_ana_new.root");
  TFile *f6_25000 = TFile::Open("ana/New_Analysis/MCData_25000e_6MeV_ana_new.root");
  TFile *f4_25000 = TFile::Open("ana/New_Analysis/MCData_25000e_4MeV_ana_new.root");
  TFile *f2_25000 = TFile::Open("ana/New_Analysis/MCData_25000e_2MeV_ana_new.root");
  TFile *f0_25000 = TFile::Open("ana/New_Analysis/MCData_25000e_0A_ana_new.root");

   
  //---------------------------------------------- Build Directories --------------------------------------------------
  
  TDirectory* dir20_1 = (TDirectory*) f20_1->Get("SPA_acceptance");
  TDirectory* dir19_1 = (TDirectory*) f19_1->Get("SPA_acceptance");
  TDirectory* dir18_1 = (TDirectory*) f18_1->Get("SPA_acceptance");
  TDirectory* dir16_1 = (TDirectory*) f16_1->Get("SPA_acceptance");
  TDirectory* dir14_1 = (TDirectory*) f14_1->Get("SPA_acceptance");
  TDirectory* dir12_1 = (TDirectory*) f12_1->Get("SPA_acceptance");
  TDirectory* dir10_1 = (TDirectory*) f10_1->Get("SPA_acceptance");
  TDirectory* dir8_1 = (TDirectory*) f8_1->Get("SPA_acceptance");
  TDirectory* dir6_1 = (TDirectory*) f6_1->Get("SPA_acceptance");
  TDirectory* dir4_1 = (TDirectory*) f4_1->Get("SPA_acceptance");
  TDirectory* dir2_1 = (TDirectory*) f2_1->Get("SPA_acceptance");
  TDirectory* dir0_1 = (TDirectory*) f0_1->Get("SPA_acceptance");
  
  TDirectory* dir20_10 = (TDirectory*) f20_10->Get("SPA_acceptance");
  TDirectory* dir19_10 = (TDirectory*) f19_10->Get("SPA_acceptance");
  TDirectory* dir18_10 = (TDirectory*) f18_10->Get("SPA_acceptance");
  TDirectory* dir16_10 = (TDirectory*) f16_10->Get("SPA_acceptance");
  TDirectory* dir14_10 = (TDirectory*) f14_10->Get("SPA_acceptance");
  TDirectory* dir12_10 = (TDirectory*) f12_10->Get("SPA_acceptance");
  TDirectory* dir10_10 = (TDirectory*) f10_10->Get("SPA_acceptance");
  TDirectory* dir8_10 = (TDirectory*) f8_10->Get("SPA_acceptance");
  TDirectory* dir6_10 = (TDirectory*) f6_10->Get("SPA_acceptance");
  TDirectory* dir4_10 = (TDirectory*) f4_10->Get("SPA_acceptance");
  TDirectory* dir2_10 = (TDirectory*) f2_10->Get("SPA_acceptance");
  TDirectory* dir0_10 = (TDirectory*) f0_10->Get("SPA_acceptance");
  
  TDirectory* dir20_100 = (TDirectory*) f20_100->Get("SPA_acceptance");
  TDirectory* dir19_100 = (TDirectory*) f19_100->Get("SPA_acceptance");
  TDirectory* dir18_100 = (TDirectory*) f18_100->Get("SPA_acceptance");
  TDirectory* dir16_100 = (TDirectory*) f16_100->Get("SPA_acceptance");
  TDirectory* dir14_100 = (TDirectory*) f14_100->Get("SPA_acceptance");
  TDirectory* dir12_100 = (TDirectory*) f12_100->Get("SPA_acceptance");
  TDirectory* dir10_100 = (TDirectory*) f10_100->Get("SPA_acceptance");
  TDirectory* dir8_100 = (TDirectory*) f8_100->Get("SPA_acceptance");
  TDirectory* dir6_100 = (TDirectory*) f6_100->Get("SPA_acceptance");
  TDirectory* dir4_100 = (TDirectory*) f4_100->Get("SPA_acceptance");
  TDirectory* dir2_100 = (TDirectory*) f2_100->Get("SPA_acceptance");
  TDirectory* dir0_100 = (TDirectory*) f0_100->Get("SPA_acceptance");

  TDirectory* dir20_1000 = (TDirectory*) f20_1000->Get("SPA_acceptance");
  TDirectory* dir19_1000 = (TDirectory*) f19_1000->Get("SPA_acceptance");
  TDirectory* dir18_1000 = (TDirectory*) f18_1000->Get("SPA_acceptance");
  TDirectory* dir16_1000 = (TDirectory*) f16_1000->Get("SPA_acceptance");
  TDirectory* dir14_1000 = (TDirectory*) f14_1000->Get("SPA_acceptance");
  TDirectory* dir12_1000 = (TDirectory*) f12_1000->Get("SPA_acceptance");
  TDirectory* dir10_1000 = (TDirectory*) f10_1000->Get("SPA_acceptance");
  TDirectory* dir8_1000 = (TDirectory*) f8_1000->Get("SPA_acceptance");
  TDirectory* dir6_1000 = (TDirectory*) f6_1000->Get("SPA_acceptance");
  TDirectory* dir4_1000 = (TDirectory*) f4_1000->Get("SPA_acceptance");
  TDirectory* dir2_1000 = (TDirectory*) f2_1000->Get("SPA_acceptance");
  TDirectory* dir0_1000 = (TDirectory*) f0_1000->Get("SPA_acceptance");

  TDirectory* dir20_10000 = (TDirectory*) f20_10000->Get("SPA_acceptance");
  TDirectory* dir19_10000 = (TDirectory*) f19_10000->Get("SPA_acceptance");
  TDirectory* dir18_10000 = (TDirectory*) f18_10000->Get("SPA_acceptance");
  TDirectory* dir16_10000 = (TDirectory*) f16_10000->Get("SPA_acceptance");
  TDirectory* dir14_10000 = (TDirectory*) f14_10000->Get("SPA_acceptance");
  TDirectory* dir12_10000 = (TDirectory*) f12_10000->Get("SPA_acceptance");
  TDirectory* dir10_10000 = (TDirectory*) f10_10000->Get("SPA_acceptance");
  TDirectory* dir8_10000 = (TDirectory*) f8_10000->Get("SPA_acceptance");
  TDirectory* dir6_10000 = (TDirectory*) f6_10000->Get("SPA_acceptance");
  TDirectory* dir4_10000 = (TDirectory*) f4_10000->Get("SPA_acceptance");
  TDirectory* dir2_10000 = (TDirectory*) f2_10000->Get("SPA_acceptance");
  TDirectory* dir0_10000 = (TDirectory*) f0_10000->Get("SPA_acceptance");

  TDirectory* dir20_25000 = (TDirectory*) f20_25000->Get("SPA_acceptance");
  TDirectory* dir19_25000 = (TDirectory*) f19_25000->Get("SPA_acceptance");
  TDirectory* dir18_25000 = (TDirectory*) f18_25000->Get("SPA_acceptance");
  TDirectory* dir16_25000 = (TDirectory*) f16_25000->Get("SPA_acceptance");
  TDirectory* dir14_25000 = (TDirectory*) f14_25000->Get("SPA_acceptance");
  TDirectory* dir12_25000 = (TDirectory*) f12_25000->Get("SPA_acceptance");
  TDirectory* dir10_25000 = (TDirectory*) f10_25000->Get("SPA_acceptance");
  TDirectory* dir8_25000 = (TDirectory*) f8_25000->Get("SPA_acceptance");
  TDirectory* dir6_25000 = (TDirectory*) f6_25000->Get("SPA_acceptance");
  TDirectory* dir4_25000 = (TDirectory*) f4_25000->Get("SPA_acceptance");
  TDirectory* dir2_25000 = (TDirectory*) f2_25000->Get("SPA_acceptance");
  TDirectory* dir0_25000 = (TDirectory*) f0_25000->Get("SPA_acceptance");

  TFile *outRoot = new TFile("MCDataanalysis_sub.root","RECREATE");

  Int_t nsigma = 5;

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
   
  //---------------------------------------------- Open Histograms --------------------------------------------------

  TH1F *Histo20_1 = (TH1F*) dir20_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo19_1 = (TH1F*) dir19_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo18_1 = (TH1F*) dir18_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo16_1 = (TH1F*) dir16_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo14_1 = (TH1F*) dir14_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo12_1 = (TH1F*) dir12_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo10_1 = (TH1F*) dir10_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo8_1 = (TH1F*) dir8_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo6_1 = (TH1F*) dir6_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo4_1 = (TH1F*) dir4_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo2_1 = (TH1F*) dir2_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo0_1_10 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV");
  TH1F *Histo0_1_20 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV");
  TH1F *Histo0_1_30 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo0_1_40 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV");
  TH1F *Histo0_1_50 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo0_1_60 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  
  TH1F *Histo20_10 = (TH1F*) dir20_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo19_10 = (TH1F*) dir19_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo18_10 = (TH1F*) dir18_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo16_10 = (TH1F*) dir16_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo14_10 = (TH1F*) dir14_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo12_10 = (TH1F*) dir12_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo10_10 = (TH1F*) dir10_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo8_10 = (TH1F*) dir8_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo6_10 = (TH1F*) dir6_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo4_10 = (TH1F*) dir4_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo2_10 = (TH1F*) dir2_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo0_10_10 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV");
  TH1F *Histo0_10_20 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV");
  TH1F *Histo0_10_30 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo0_10_40 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV");
  TH1F *Histo0_10_50 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo0_10_60 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  
  TH1F *Histo20_100 = (TH1F*) dir20_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo19_100 = (TH1F*) dir19_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo18_100 = (TH1F*) dir18_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo16_100 = (TH1F*) dir16_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo14_100 = (TH1F*) dir14_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo12_100 = (TH1F*) dir12_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo10_100 = (TH1F*) dir10_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo8_100 = (TH1F*) dir8_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo6_100 = (TH1F*) dir6_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo4_100 = (TH1F*) dir4_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo2_100 = (TH1F*) dir2_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo0_100_10 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV");
  TH1F *Histo0_100_20 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV");
  TH1F *Histo0_100_30 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo0_100_40 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV");
  TH1F *Histo0_100_50 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo0_100_60 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  
  TH1F *Histo20_1000 = (TH1F*) dir20_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo19_1000 = (TH1F*) dir19_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo18_1000 = (TH1F*) dir18_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo16_1000 = (TH1F*) dir16_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo14_1000 = (TH1F*) dir14_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo12_1000 = (TH1F*) dir12_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo10_1000 = (TH1F*) dir10_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo8_1000 = (TH1F*) dir8_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo6_1000 = (TH1F*) dir6_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo4_1000 = (TH1F*) dir4_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo2_1000 = (TH1F*) dir2_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo0_1000_10 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV");
  TH1F *Histo0_1000_20 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV");
  TH1F *Histo0_1000_30 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo0_1000_40 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV");
  TH1F *Histo0_1000_50 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo0_1000_60 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  
  TH1F *Histo20_10000 = (TH1F*) dir20_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo19_10000 = (TH1F*) dir19_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo18_10000 = (TH1F*) dir18_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo16_10000 = (TH1F*) dir16_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo14_10000 = (TH1F*) dir14_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo12_10000 = (TH1F*) dir12_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo10_10000 = (TH1F*) dir10_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo8_10000 = (TH1F*) dir8_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo6_10000 = (TH1F*) dir6_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo4_10000 = (TH1F*) dir4_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo2_10000 = (TH1F*) dir2_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo0_10000_10 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV");
  TH1F *Histo0_10000_20 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV");
  TH1F *Histo0_10000_30 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo0_10000_40 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV");
  TH1F *Histo0_10000_50 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo0_10000_60 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  
  TH1F *Histo20_25000 = (TH1F*) dir20_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo19_25000 = (TH1F*) dir19_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo18_25000 = (TH1F*) dir18_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo16_25000 = (TH1F*) dir16_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo14_25000 = (TH1F*) dir14_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo12_25000 = (TH1F*) dir12_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo10_25000 = (TH1F*) dir10_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo8_25000 = (TH1F*) dir8_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo6_25000 = (TH1F*) dir6_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo4_25000 = (TH1F*) dir4_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo2_25000 = (TH1F*) dir2_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo0_25000_10 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV");
  TH1F *Histo0_25000_20 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV");
  TH1F *Histo0_25000_30 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo0_25000_40 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV");
  TH1F *Histo0_25000_50 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo0_25000_60 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");

  Float_t masses[11];
  
  for(int i=0;i<9;i++){
    if(i==0) masses[i]=2;
    else masses[i]=masses[i-1]+2;
    std::cout<<"Masses["<<i<<"] = "<<masses[i]<<std::endl;
  }

  masses[9] = 19;
  std::cout<<"Masses[9] = "<<masses[9]<<std::endl;
  masses[10] = 20;
  std::cout<<"Masses[10] = "<<masses[10]<<std::endl;

  TF1 *f_2 = new TF1("f_2","gaus",-30., 25.);
  Histo2_1->Fit(f_2,"R");
  means_1[0]=f_2->GetParameter(1);
  sigmas_1[0]=f_2->GetParameter(2);

  TF1 *f_4 = new TF1("f_4","gaus",-25., 30.);
  Histo4_1->Fit(f_4,"R");
  means_1[1]=f_4->GetParameter(1);
  sigmas_1[1]=f_4->GetParameter(2);
  
  TF1 *f_6 = new TF1("f_6","gaus",-5., 60.);
  Histo6_1->Fit(f_6,"R");
  means_1[2]=f_6->GetParameter(1);
  sigmas_1[2]=f_6->GetParameter(2);
  
  TF1 *f_8 = new TF1("f_8","gaus",25., 85.);
  Histo8_1->Fit(f_8,"R");
  means_1[3]=f_8->GetParameter(1);
  sigmas_1[3]=f_8->GetParameter(2);
  
  TF1 *f_10 = new TF1("f_10","gaus",60., 120.);
  Histo10_1->Fit(f_10,"R");
  means_1[4]=f_10->GetParameter(1);
  sigmas_1[4]=f_10->GetParameter(2);
  
  TF1 *f_12 = new TF1("f_12","gaus",110., 165.);
  Histo12_1->Fit(f_12,"R");
  means_1[5]=f_12->GetParameter(1);
  sigmas_1[5]=f_12->GetParameter(2);
  
  TF1 *f_14 = new TF1("f_14","gaus",165., 210.);
  Histo14_1->Fit(f_14,"R");
  means_1[6]=f_14->GetParameter(1);
  sigmas_1[6]=f_14->GetParameter(2);
  
  TF1 *f_16 = new TF1("f_10","gaus",230., 265.);
  Histo16_1->Fit(f_16,"R");
  means_1[7]=f_16->GetParameter(1);
  sigmas_1[7]=f_16->GetParameter(2);
  
  TF1 *f_18 = new TF1("f_18","gaus",305., 330.);
  Histo18_1->Fit(f_18,"R");
  means_1[8]=f_18->GetParameter(1);
  sigmas_1[8]=f_18->GetParameter(2);
  
  TF1 *f_19 = new TF1("f_19","gaus",342., 365.);
  Histo19_1->Fit(f_19,"R");
  means_1[9]=f_19->GetParameter(1);
  sigmas_1[9]=f_19->GetParameter(2);
  
  TF1 *f_20 = new TF1("f_20","gaus",385., 400.);
  Histo20_1->Fit(f_20,"R");
  means_1[10]=f_20->GetParameter(1);
  sigmas_1[10]=f_20->GetParameter(2);
  
  entries_1[0]=Histo2_1->Integral(500+(means_1[0]-nsigma*sigmas_1[0]),500+(means_1[0]+nsigma*sigmas_1[0]))/1000;
  entries_1[1]=Histo4_1->Integral(500+(means_1[1]-nsigma*sigmas_1[1]),500+(means_1[1]+nsigma*sigmas_1[1]))/1000;
  entries_1[2]=Histo6_1->Integral(500+(means_1[2]-nsigma*sigmas_1[2]),500+(means_1[2]+nsigma*sigmas_1[2]))/1000;
  entries_1[3]=Histo8_1->Integral(500+(means_1[3]-nsigma*sigmas_1[3]),500+(means_1[3]+nsigma*sigmas_1[3]))/1000;
  entries_1[4]=Histo10_1->Integral(500+(means_1[4]-nsigma*sigmas_1[4]),500+(means_1[4]+nsigma*sigmas_1[4]))/1000;
  entries_1[5]=Histo12_1->Integral(500+(means_1[5]-nsigma*sigmas_1[5]),500+(means_1[5]+nsigma*sigmas_1[5]))/1000;
  entries_1[6]=Histo14_1->Integral(500+(means_1[6]-nsigma*sigmas_1[6]),500+(means_1[6]+nsigma*sigmas_1[6]))/1000;
  entries_1[7]=Histo16_1->Integral(500+(means_1[7]-nsigma*sigmas_1[7]),500+(means_1[7]+nsigma*sigmas_1[7]))/1000;
  entries_1[8]=Histo18_1->Integral(500+(means_1[8]-nsigma*sigmas_1[8]),500+(means_1[8]+nsigma*sigmas_1[8]))/1000;
  entries_1[9]=Histo19_1->Integral(500+(means_1[9]-nsigma*sigmas_1[9]),500+(means_1[9]+nsigma*sigmas_1[9]))/1000;
  entries_1[10]=Histo20_1->Integral(500+(means_1[10]-nsigma*sigmas_1[10]),500+(means_1[10]+nsigma*sigmas_1[10]))/1000;
  
  TCanvas *c1 = new TCanvas();
  c1->cd();
  Histo20_1->SetLineColorAlpha(kBlue+3,0.65);
  Histo20_1->SetTitle( "M^{2}_{miss} for different A' masses (1 e+ per bunch)");
  Histo20_1->SetLineWidth(3);
  Histo20_1->GetYaxis()->SetTitle("N_{events}");
  Histo20_1->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo19_1->SetLineColorAlpha(kBlue,0.65);
  Histo19_1->SetLineWidth(3);
  Histo18_1->SetLineColorAlpha(kCyan+3,0.65);
  Histo18_1->SetLineWidth(3);
  Histo16_1->SetLineColorAlpha(kCyan+1,0.65);
  Histo16_1->SetLineWidth(3);
  Histo14_1->SetLineColorAlpha(kGreen+3,0.65);
  Histo14_1->SetLineWidth(3);
  Histo12_1->SetLineColorAlpha(kGreen+1,0.65);
  Histo12_1->SetLineWidth(3);
  Histo10_1->SetLineColorAlpha(kYellow+2,0.65);
  Histo10_1->SetLineWidth(3);
  Histo8_1->SetLineColorAlpha(kOrange-3,0.65);
  Histo8_1->SetLineWidth(3);
  Histo6_1->SetLineColorAlpha(kRed+2,0.65);
  Histo6_1->SetLineWidth(3);
  Histo4_1->SetLineColorAlpha(kRed,0.65);
  Histo4_1->SetLineWidth(3);
  Histo2_1->SetLineColorAlpha(kMagenta+1,0.65);
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

  if(!(Histo20_10->GetSumw2N() > 0)) Histo20_10->Sumw2(kTRUE);
  Histo20_10->Add(Histo0_10_30,-1);
  if(!(Histo19_10->GetSumw2N() > 0)) Histo19_10->Sumw2(kTRUE);
  Histo19_10->Add(Histo0_10_30,-1);
  if(!(Histo18_10->GetSumw2N() > 0)) Histo18_10->Sumw2(kTRUE);
  Histo18_10->Add(Histo0_10_50,-1);
  if(!(Histo16_10->GetSumw2N() > 0)) Histo16_10->Sumw2(kTRUE);
  Histo16_10->Add(Histo0_10_60,-1);
  if(!(Histo14_10->GetSumw2N() > 0)) Histo14_10->Sumw2(kTRUE);
  Histo14_10->Add(Histo0_10_60,-1);
  if(!(Histo12_10->GetSumw2N() > 0)) Histo12_10->Sumw2(kTRUE);
  Histo12_10->Add(Histo0_10_60,-1);
  if(!(Histo10_10->GetSumw2N() > 0)) Histo10_10->Sumw2(kTRUE);
  Histo10_10->Add(Histo0_10_60,-1);
  if(!(Histo8_10->GetSumw2N() > 0)) Histo8_10->Sumw2(kTRUE);
  Histo8_10->Add(Histo0_10_60,-1);
  if(!(Histo6_10->GetSumw2N() > 0)) Histo6_10->Sumw2(kTRUE);
  Histo6_10->Add(Histo0_10_60,-1);
  if(!(Histo4_10->GetSumw2N() > 0)) Histo4_10->Sumw2(kTRUE);
  Histo4_10->Add(Histo0_10_60,-1);
  if(!(Histo2_10->GetSumw2N() > 0)) Histo2_10->Sumw2(kTRUE);
  Histo2_10->Add(Histo0_10_60,-1);
  
  Histo2_10->Fit(f_2,"R");
  means_10[0]=f_2->GetParameter(1);
  sigmas_10[0]=f_2->GetParameter(2);

  Histo4_10->Fit(f_4,"R");
  means_10[1]=f_4->GetParameter(1);
  sigmas_10[1]=f_4->GetParameter(2);
  
  Histo6_10->Fit(f_6,"R");
  means_10[2]=f_6->GetParameter(1);
  sigmas_10[2]=f_6->GetParameter(2);
  
  Histo8_10->Fit(f_8,"R");
  means_10[3]=f_8->GetParameter(1);
  sigmas_10[3]=f_8->GetParameter(2);
 
  Histo10_10->Fit(f_10,"R");
  means_10[4]=f_10->GetParameter(1);
  sigmas_10[4]=f_10->GetParameter(2);
  
  Histo12_10->Fit(f_12,"R");
  means_10[5]=f_12->GetParameter(1);
  sigmas_10[5]=f_12->GetParameter(2);
  
  Histo14_10->Fit(f_14,"R");
  means_10[6]=f_14->GetParameter(1);
  sigmas_10[6]=f_14->GetParameter(2);
  
  Histo16_10->Fit(f_16,"R");
  means_10[7]=f_16->GetParameter(1);
  sigmas_10[7]=f_16->GetParameter(2);
  
  Histo18_10->Fit(f_18,"R");
  means_10[8]=f_18->GetParameter(1);
  sigmas_10[8]=f_18->GetParameter(2);
  
  Histo19_10->Fit(f_19,"R");
  means_10[9]=f_19->GetParameter(1);
  sigmas_10[9]=f_19->GetParameter(2);
  
  Histo20_10->Fit(f_20,"R");
  means_10[10]=f_20->GetParameter(1);
  sigmas_10[10]=f_20->GetParameter(2);

  entries_10[0]=Histo2_10->Integral(500+(means_10[0]-nsigma*sigmas_10[0]),500+(means_10[0]+nsigma*sigmas_10[0]))/1000;
  entries_10[1]=Histo4_10->Integral(500+(means_10[1]-nsigma*sigmas_10[1]),500+(means_10[1]+nsigma*sigmas_10[1]))/1000;
  entries_10[2]=Histo6_10->Integral(500+(means_10[2]-nsigma*sigmas_10[2]),500+(means_10[2]+nsigma*sigmas_10[2]))/1000;
  entries_10[3]=Histo8_10->Integral(500+(means_10[3]-nsigma*sigmas_10[3]),500+(means_10[3]+nsigma*sigmas_10[3]))/1000;
  entries_10[4]=Histo10_10->Integral(500+(means_10[4]-nsigma*sigmas_10[4]),500+(means_10[4]+nsigma*sigmas_10[4]))/1000;
  entries_10[5]=Histo12_10->Integral(500+(means_10[5]-nsigma*sigmas_10[5]),500+(means_10[5]+nsigma*sigmas_10[5]))/1000;
  entries_10[6]=Histo14_10->Integral(500+(means_10[6]-nsigma*sigmas_10[6]),500+(means_10[6]+nsigma*sigmas_10[6]))/1000;
  entries_10[7]=Histo16_10->Integral(500+(means_10[7]-nsigma*sigmas_10[7]),500+(means_10[7]+nsigma*sigmas_10[7]))/1000;
  entries_10[8]=Histo18_10->Integral(500+(means_10[8]-nsigma*sigmas_10[8]),500+(means_10[8]+nsigma*sigmas_10[8]))/1000;
  entries_10[9]=Histo19_10->Integral(500+(means_10[9]-nsigma*sigmas_10[9]),500+(means_10[9]+nsigma*sigmas_10[9]))/1000;
  entries_10[10]=Histo20_10->Integral(500+(means_10[10]-nsigma*sigmas_10[10]),500+(means_10[10]+nsigma*sigmas_10[10]))/1000;

  TCanvas *c10 = new TCanvas();
  c10->cd();
  Histo20_10->SetLineColorAlpha(kBlue+3,0.65);
  Histo20_10->SetTitle( "M^{2}_{miss} for different A' masses (10 e+ per bunch)");
  Histo20_10->SetLineWidth(3);
  Histo20_10->GetYaxis()->SetTitle("N_{events}");
  Histo20_10->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo19_10->SetLineColorAlpha(kBlue,0.65);
  Histo19_10->SetLineWidth(3);
  Histo18_10->SetLineColorAlpha(kCyan+3,0.65);
  Histo18_10->SetLineWidth(3);
  Histo16_10->SetLineColorAlpha(kCyan+1,0.65);
  Histo16_10->SetLineWidth(3);
  Histo14_10->SetLineColorAlpha(kGreen+3,0.65);
  Histo14_10->SetLineWidth(3);
  Histo12_10->SetLineColorAlpha(kGreen+1,0.65);
  Histo12_10->SetLineWidth(3);
  Histo10_10->SetLineColorAlpha(kYellow+2,0.65);
  Histo10_10->SetLineWidth(3);
  Histo8_10->SetLineColorAlpha(kOrange-3,0.65);
  Histo8_10->SetLineWidth(3);
  Histo6_10->SetLineColorAlpha(kRed+2,0.65);
  Histo6_10->SetLineWidth(3);
  Histo4_10->SetLineColorAlpha(kRed,0.65);
  Histo4_10->SetLineWidth(3);
  Histo2_10->SetLineColorAlpha(kMagenta+1,0.65);
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
 
  if(!(Histo20_100->GetSumw2N() > 0)) Histo20_100->Sumw2(kTRUE);
  Histo20_100->Add(Histo0_100_30,-1);
  if(!(Histo19_100->GetSumw2N() > 0)) Histo19_100->Sumw2(kTRUE);
  Histo19_100->Add(Histo0_100_30,-1);
  if(!(Histo18_100->GetSumw2N() > 0)) Histo18_100->Sumw2(kTRUE);
  Histo18_100->Add(Histo0_100_50,-1);
  if(!(Histo16_100->GetSumw2N() > 0)) Histo16_100->Sumw2(kTRUE);
  Histo16_100->Add(Histo0_100_60,-1);
  if(!(Histo14_100->GetSumw2N() > 0)) Histo14_100->Sumw2(kTRUE);
  Histo14_100->Add(Histo0_100_60,-1);
  if(!(Histo12_100->GetSumw2N() > 0)) Histo12_100->Sumw2(kTRUE);
  Histo12_100->Add(Histo0_100_60,-1);
  if(!(Histo10_100->GetSumw2N() > 0)) Histo10_100->Sumw2(kTRUE);
  Histo10_100->Add(Histo0_100_60,-1);
  if(!(Histo8_100->GetSumw2N() > 0)) Histo8_100->Sumw2(kTRUE);
  Histo8_100->Add(Histo0_100_60,-1);
  if(!(Histo6_100->GetSumw2N() > 0)) Histo6_100->Sumw2(kTRUE);
  Histo6_100->Add(Histo0_100_60,-1);
  if(!(Histo4_100->GetSumw2N() > 0)) Histo4_100->Sumw2(kTRUE);
  Histo4_100->Add(Histo0_100_60,-1);
  if(!(Histo2_100->GetSumw2N() > 0)) Histo2_100->Sumw2(kTRUE);
  Histo2_100->Add(Histo0_100_60,-1);


  Histo2_100->Fit(f_2,"R");
  means_100[0]=f_2->GetParameter(1);
  sigmas_100[0]=f_2->GetParameter(2);

  Histo4_100->Fit(f_4,"R");
  means_100[1]=f_4->GetParameter(1);
  sigmas_100[1]=f_4->GetParameter(2);
  
  Histo6_100->Fit(f_6,"R");
  means_100[2]=f_6->GetParameter(1);
  sigmas_100[2]=f_6->GetParameter(2);
  
  Histo8_100->Fit(f_8,"R");
  means_100[3]=f_8->GetParameter(1);
  sigmas_100[3]=f_8->GetParameter(2);
 
  Histo10_100->Fit(f_10,"R");
  means_100[4]=f_10->GetParameter(1);
  sigmas_100[4]=f_10->GetParameter(2);
  
  Histo12_100->Fit(f_12,"R");
  means_100[5]=f_12->GetParameter(1);
  sigmas_100[5]=f_12->GetParameter(2);
  
  Histo14_100->Fit(f_14,"R");
  means_100[6]=f_14->GetParameter(1);
  sigmas_100[6]=f_14->GetParameter(2);
  
  Histo16_100->Fit(f_16,"R");
  means_100[7]=f_16->GetParameter(1);
  sigmas_100[7]=f_16->GetParameter(2);
  
  Histo18_100->Fit(f_18,"R");
  means_100[8]=f_18->GetParameter(1);
  sigmas_100[8]=f_18->GetParameter(2);
  
  Histo19_100->Fit(f_19,"R");
  means_100[9]=f_19->GetParameter(1);
  sigmas_100[9]=f_19->GetParameter(2);
  
  Histo20_100->Fit(f_20,"R");
  means_100[10]=f_20->GetParameter(1);
  sigmas_100[10]=f_20->GetParameter(2);

  entries_100[0]=Histo2_100->Integral(500+(means_100[0]-nsigma*sigmas_100[0]),500+(means_100[0]+nsigma*sigmas_100[0]))/1000;
  entries_100[1]=Histo4_100->Integral(500+(means_100[1]-nsigma*sigmas_100[1]),500+(means_100[1]+nsigma*sigmas_100[1]))/1000;
  entries_100[2]=Histo6_100->Integral(500+(means_100[2]-nsigma*sigmas_100[2]),500+(means_100[2]+nsigma*sigmas_100[2]))/1000;
  entries_100[3]=Histo8_100->Integral(500+(means_100[3]-nsigma*sigmas_100[3]),500+(means_100[3]+nsigma*sigmas_100[3]))/1000;
  entries_100[4]=Histo10_100->Integral(500+(means_100[4]-nsigma*sigmas_100[4]),500+(means_100[4]+nsigma*sigmas_100[4]))/1000;
  entries_100[5]=Histo12_100->Integral(500+(means_100[5]-nsigma*sigmas_100[5]),500+(means_100[5]+nsigma*sigmas_100[5]))/1000;
  entries_100[6]=Histo14_100->Integral(500+(means_100[6]-nsigma*sigmas_100[6]),500+(means_100[6]+nsigma*sigmas_100[6]))/1000;
  entries_100[7]=Histo16_100->Integral(500+(means_100[7]-nsigma*sigmas_100[7]),500+(means_100[7]+nsigma*sigmas_100[7]))/1000;
  entries_100[8]=Histo18_100->Integral(500+(means_100[8]-nsigma*sigmas_100[8]),500+(means_100[8]+nsigma*sigmas_100[8]))/1000;
  entries_100[9]=Histo19_100->Integral(500+(means_100[9]-nsigma*sigmas_100[9]),500+(means_100[9]+nsigma*sigmas_100[9]))/1000;
  entries_100[10]=Histo20_100->Integral(500+(means_100[10]-nsigma*sigmas_100[10]),500+(means_100[10]+nsigma*sigmas_100[10]))/1000;

  TCanvas *c100 = new TCanvas();
  c100->cd();
  Histo20_100->SetLineColorAlpha(kBlue+3,0.65);
  Histo20_100->SetTitle( "M^{2}_{miss} for different A' masses (100 e+ per bunch)");
  Histo20_100->SetLineWidth(3);
  Histo20_100->GetYaxis()->SetTitle("N_{events}");
  Histo20_100->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo19_100->SetLineColorAlpha(kBlue,0.65);
  Histo19_100->SetLineWidth(3);
  Histo18_100->SetLineColorAlpha(kCyan+3,0.65);
  Histo18_100->SetLineWidth(3);
  Histo16_100->SetLineColorAlpha(kCyan+1,0.65);
  Histo16_100->SetLineWidth(3);
  Histo14_100->SetLineColorAlpha(kGreen+3,0.65);
  Histo14_100->SetLineWidth(3);
  Histo12_100->SetLineColorAlpha(kGreen+1,0.65);
  Histo12_100->SetLineWidth(3);
  Histo10_100->SetLineColorAlpha(kYellow+2,0.65);
  Histo10_100->SetLineWidth(3);
  Histo8_100->SetLineColorAlpha(kOrange-3,0.65);
  Histo8_100->SetLineWidth(3);
  Histo6_100->SetLineColorAlpha(kRed+2,0.65);
  Histo6_100->SetLineWidth(3);
  Histo4_100->SetLineColorAlpha(kRed,0.65);
  Histo4_100->SetLineWidth(3);
  Histo2_100->SetLineColorAlpha(kMagenta+1,0.65);
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

  
  if(!(Histo20_1000->GetSumw2N() > 0)) Histo20_1000->Sumw2(kTRUE);
  Histo20_1000->Add(Histo0_1000_30,-1);
  if(!(Histo19_1000->GetSumw2N() > 0)) Histo19_1000->Sumw2(kTRUE);
  Histo19_1000->Add(Histo0_1000_30,-1);
  if(!(Histo18_1000->GetSumw2N() > 0)) Histo18_1000->Sumw2(kTRUE);
  Histo18_1000->Add(Histo0_1000_50,-1);
  if(!(Histo16_1000->GetSumw2N() > 0)) Histo16_1000->Sumw2(kTRUE);
  Histo16_1000->Add(Histo0_1000_60,-1);
  if(!(Histo14_1000->GetSumw2N() > 0)) Histo14_1000->Sumw2(kTRUE);
  Histo14_1000->Add(Histo0_1000_60,-1);
  if(!(Histo12_1000->GetSumw2N() > 0)) Histo12_1000->Sumw2(kTRUE);
  Histo12_1000->Add(Histo0_1000_60,-1);
  if(!(Histo10_1000->GetSumw2N() > 0)) Histo10_1000->Sumw2(kTRUE);
  Histo10_1000->Add(Histo0_1000_60,-1);
  if(!(Histo8_1000->GetSumw2N() > 0)) Histo8_1000->Sumw2(kTRUE);
  Histo8_1000->Add(Histo0_1000_60,-1);
  if(!(Histo6_1000->GetSumw2N() > 0)) Histo6_1000->Sumw2(kTRUE);
  Histo6_1000->Add(Histo0_1000_60,-1);
  if(!(Histo4_1000->GetSumw2N() > 0)) Histo4_1000->Sumw2(kTRUE);
  Histo4_1000->Add(Histo0_1000_60,-1);
  if(!(Histo2_1000->GetSumw2N() > 0)) Histo2_1000->Sumw2(kTRUE);
  Histo2_1000->Add(Histo0_1000_60,-1);


  Histo2_1000->Fit(f_2,"R");
  means_1000[0]=f_2->GetParameter(1);
  sigmas_1000[0]=f_2->GetParameter(2);

  Histo4_1000->Fit(f_4,"R");
  means_1000[1]=f_4->GetParameter(1);
  sigmas_1000[1]=f_4->GetParameter(2);
  
  Histo6_1000->Fit(f_6,"R");
  means_1000[2]=f_6->GetParameter(1);
  sigmas_1000[2]=f_6->GetParameter(2);
  
  Histo8_1000->Fit(f_8,"R");
  means_1000[3]=f_8->GetParameter(1);
  sigmas_1000[3]=f_8->GetParameter(2);
 
  Histo10_1000->Fit(f_10,"R");
  means_1000[4]=f_10->GetParameter(1);
  sigmas_1000[4]=f_10->GetParameter(2);
  
  Histo12_1000->Fit(f_12,"R");
  means_1000[5]=f_12->GetParameter(1);
  sigmas_1000[5]=f_12->GetParameter(2);
  
  Histo14_1000->Fit(f_14,"R");
  means_1000[6]=f_14->GetParameter(1);
  sigmas_1000[6]=f_14->GetParameter(2);
  
  Histo16_1000->Fit(f_16,"R");
  means_1000[7]=f_16->GetParameter(1);
  sigmas_1000[7]=f_16->GetParameter(2);
  
  Histo18_1000->Fit(f_18,"R");
  means_1000[8]=f_18->GetParameter(1);
  sigmas_1000[8]=f_18->GetParameter(2);
  
  Histo19_1000->Fit(f_19,"R");
  means_1000[9]=f_19->GetParameter(1);
  sigmas_1000[9]=f_19->GetParameter(2);
  
  Histo20_1000->Fit(f_20,"R");
  means_1000[10]=f_20->GetParameter(1);
  sigmas_1000[10]=f_20->GetParameter(2);

  entries_1000[0]=Histo2_1000->Integral(500+(means_1000[0]-nsigma*sigmas_1000[0]),500+(means_1000[0]+nsigma*sigmas_1000[0]))/1000;
  entries_1000[1]=Histo4_1000->Integral(500+(means_1000[1]-nsigma*sigmas_1000[1]),500+(means_1000[1]+nsigma*sigmas_1000[1]))/1000;
  entries_1000[2]=Histo6_1000->Integral(500+(means_1000[2]-nsigma*sigmas_1000[2]),500+(means_1000[2]+nsigma*sigmas_1000[2]))/1000;
  entries_1000[3]=Histo8_1000->Integral(500+(means_1000[3]-nsigma*sigmas_1000[3]),500+(means_1000[3]+nsigma*sigmas_1000[3]))/1000;
  entries_1000[4]=Histo10_1000->Integral(500+(means_1000[4]-nsigma*sigmas_1000[4]),500+(means_1000[4]+nsigma*sigmas_1000[4]))/1000;
  entries_1000[5]=Histo12_1000->Integral(500+(means_1000[5]-nsigma*sigmas_1000[5]),500+(means_1000[5]+nsigma*sigmas_1000[5]))/1000;
  entries_1000[6]=Histo14_1000->Integral(500+(means_1000[6]-nsigma*sigmas_1000[6]),500+(means_1000[6]+nsigma*sigmas_1000[6]))/1000;
  entries_1000[7]=Histo16_1000->Integral(500+(means_1000[7]-nsigma*sigmas_1000[7]),500+(means_1000[7]+nsigma*sigmas_1000[7]))/1000;
  entries_1000[8]=Histo18_1000->Integral(500+(means_1000[8]-nsigma*sigmas_1000[8]),500+(means_1000[8]+nsigma*sigmas_1000[8]))/1000;
  entries_1000[9]=Histo19_1000->Integral(500+(means_1000[9]-nsigma*sigmas_1000[9]),500+(means_1000[9]+nsigma*sigmas_1000[9]))/1000;
  entries_1000[10]=Histo20_1000->Integral(500+(means_1000[10]-nsigma*sigmas_1000[10]),500+(means_1000[10]+nsigma*sigmas_1000[10]))/1000;

  TCanvas *c1000 = new TCanvas();
  c1000->cd();
  Histo20_1000->SetLineColorAlpha(kBlue+3,0.65);
  Histo20_1000->SetTitle( "M^{2}_{miss} for different A' masses (1000 e+ per bunch)");
  Histo20_1000->SetLineWidth(3);
  Histo20_1000->GetYaxis()->SetTitle("N_{events}");
  Histo20_1000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo19_1000->SetLineColorAlpha(kBlue,0.65);
  Histo19_1000->SetLineWidth(3);
  Histo18_1000->SetLineColorAlpha(kCyan+3,0.65);
  Histo18_1000->SetLineWidth(3);
  Histo16_1000->SetLineColorAlpha(kCyan+1,0.65);
  Histo16_1000->SetLineWidth(3);
  Histo14_1000->SetLineColorAlpha(kGreen+3,0.65);
  Histo14_1000->SetLineWidth(3);
  Histo12_1000->SetLineColorAlpha(kGreen+1,0.65);
  Histo12_1000->SetLineWidth(3);
  Histo10_1000->SetLineColorAlpha(kYellow+2,0.65);
  Histo10_1000->SetLineWidth(3);
  Histo8_1000->SetLineColorAlpha(kOrange-3,0.65);
  Histo8_1000->SetLineWidth(3);
  Histo6_1000->SetLineColorAlpha(kRed+2,0.65);
  Histo6_1000->SetLineWidth(3);
  Histo4_1000->SetLineColorAlpha(kRed,0.65);
  Histo4_1000->SetLineWidth(3);
  Histo2_1000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo2_1000->SetLineWidth(3);
    
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

  auto legend_1000 = new TLegend(0.15,0.5,0.3,0.85);
  legend_1000->AddEntry(Histo20_1000,"M_{A'} = 20 MeV","l");
  legend_1000->AddEntry(Histo19_1000,"M_{A'} = 19 MeV","l");
  legend_1000->AddEntry(Histo18_1000,"M_{A'} = 18 MeV","l");
  legend_1000->AddEntry(Histo16_1000,"M_{A'} = 16 MeV","l");
  legend_1000->AddEntry(Histo14_1000,"M_{A'} = 14 MeV","l");
  legend_1000->AddEntry(Histo12_1000,"M_{A'} = 12 MeV","l");
  legend_1000->AddEntry(Histo10_1000,"M_{A'} = 10 MeV","l");
  legend_1000->AddEntry(Histo8_1000,"M_{A'} = 8 MeV","l");
  legend_1000->AddEntry(Histo6_1000,"M_{A'} = 6 MeV","l");
  legend_1000->AddEntry(Histo4_1000,"M_{A'} = 4 MeV","l");
  legend_1000->AddEntry(Histo2_1000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_1000->Draw();

  outRoot->WriteObject(c1000,"1000e_all");

  
  if(!(Histo20_10000->GetSumw2N() > 0)) Histo20_10000->Sumw2(kTRUE);
  Histo20_10000->Add(Histo0_10000_30,-1);
  if(!(Histo19_10000->GetSumw2N() > 0)) Histo19_10000->Sumw2(kTRUE);
  Histo19_10000->Add(Histo0_10000_30,-1);
  if(!(Histo18_10000->GetSumw2N() > 0)) Histo18_10000->Sumw2(kTRUE);
  Histo18_10000->Add(Histo0_10000_50,-1);
  if(!(Histo16_10000->GetSumw2N() > 0)) Histo16_10000->Sumw2(kTRUE);
  Histo16_10000->Add(Histo0_10000_60,-1);
  if(!(Histo14_10000->GetSumw2N() > 0)) Histo14_10000->Sumw2(kTRUE);
  Histo14_10000->Add(Histo0_10000_60,-1);
  if(!(Histo12_10000->GetSumw2N() > 0)) Histo12_10000->Sumw2(kTRUE);
  Histo12_10000->Add(Histo0_10000_60,-1);
  if(!(Histo10_10000->GetSumw2N() > 0)) Histo10_10000->Sumw2(kTRUE);
  Histo10_10000->Add(Histo0_10000_60,-1);
  if(!(Histo8_10000->GetSumw2N() > 0)) Histo8_10000->Sumw2(kTRUE);
  Histo8_10000->Add(Histo0_10000_60,-1);
  if(!(Histo6_10000->GetSumw2N() > 0)) Histo6_10000->Sumw2(kTRUE);
  Histo6_10000->Add(Histo0_10000_60,-1);
  if(!(Histo4_10000->GetSumw2N() > 0)) Histo4_10000->Sumw2(kTRUE);
  Histo4_10000->Add(Histo0_10000_60,-1);
  if(!(Histo2_10000->GetSumw2N() > 0)) Histo2_10000->Sumw2(kTRUE);
  Histo2_10000->Add(Histo0_10000_60,-1);


  Histo2_10000->Fit(f_2,"R");
  means_10000[0]=f_2->GetParameter(1);
  sigmas_10000[0]=f_2->GetParameter(2);

  Histo4_10000->Fit(f_4,"R");
  means_10000[1]=f_4->GetParameter(1);
  sigmas_10000[1]=f_4->GetParameter(2);
  
  Histo6_10000->Fit(f_6,"R");
  means_10000[2]=f_6->GetParameter(1);
  sigmas_10000[2]=f_6->GetParameter(2);
  
  Histo8_10000->Fit(f_8,"R");
  means_10000[3]=f_8->GetParameter(1);
  sigmas_10000[3]=f_8->GetParameter(2);
 
  Histo10_10000->Fit(f_10,"R");
  means_10000[4]=f_10->GetParameter(1);
  sigmas_10000[4]=f_10->GetParameter(2);
  
  Histo12_10000->Fit(f_12,"R");
  means_10000[5]=f_12->GetParameter(1);
  sigmas_10000[5]=f_12->GetParameter(2);
  
  Histo14_10000->Fit(f_14,"R");
  means_10000[6]=f_14->GetParameter(1);
  sigmas_10000[6]=f_14->GetParameter(2);
  
  Histo16_10000->Fit(f_16,"R");
  means_10000[7]=f_16->GetParameter(1);
  sigmas_10000[7]=f_16->GetParameter(2);
  
  Histo18_10000->Fit(f_18,"R");
  means_10000[8]=f_18->GetParameter(1);
  sigmas_10000[8]=f_18->GetParameter(2);
  
  Histo19_10000->Fit(f_19,"R");
  means_10000[9]=f_19->GetParameter(1);
  sigmas_10000[9]=f_19->GetParameter(2);
  
  Histo20_10000->Fit(f_20,"R");
  means_10000[10]=f_20->GetParameter(1);
  sigmas_10000[10]=f_20->GetParameter(2);

  entries_10000[0]=Histo2_10000->Integral(500+(means_10000[0]-nsigma*sigmas_10000[0]),500+(means_10000[0]+nsigma*sigmas_10000[0]))/1000;
  entries_10000[1]=Histo4_10000->Integral(500+(means_10000[1]-nsigma*sigmas_10000[1]),500+(means_10000[1]+nsigma*sigmas_10000[1]))/1000;
  entries_10000[2]=Histo6_10000->Integral(500+(means_10000[2]-nsigma*sigmas_10000[2]),500+(means_10000[2]+nsigma*sigmas_10000[2]))/1000;
  entries_10000[3]=Histo8_10000->Integral(500+(means_10000[3]-nsigma*sigmas_10000[3]),500+(means_10000[3]+nsigma*sigmas_10000[3]))/1000;
  entries_10000[4]=Histo10_10000->Integral(500+(means_10000[4]-nsigma*sigmas_10000[4]),500+(means_10000[4]+nsigma*sigmas_10000[4]))/1000;
  entries_10000[5]=Histo12_10000->Integral(500+(means_10000[5]-nsigma*sigmas_10000[5]),500+(means_10000[5]+nsigma*sigmas_10000[5]))/1000;
  entries_10000[6]=Histo14_10000->Integral(500+(means_10000[6]-nsigma*sigmas_10000[6]),500+(means_10000[6]+nsigma*sigmas_10000[6]))/1000;
  entries_10000[7]=Histo16_10000->Integral(500+(means_10000[7]-nsigma*sigmas_10000[7]),500+(means_10000[7]+nsigma*sigmas_10000[7]))/1000;
  entries_10000[8]=Histo18_10000->Integral(500+(means_10000[8]-nsigma*sigmas_10000[8]),500+(means_10000[8]+nsigma*sigmas_10000[8]))/1000;
  entries_10000[9]=Histo19_10000->Integral(500+(means_10000[9]-nsigma*sigmas_10000[9]),500+(means_10000[9]+nsigma*sigmas_10000[9]))/1000;
  entries_10000[10]=Histo20_10000->Integral(500+(means_10000[10]-nsigma*sigmas_10000[10]),500+(means_10000[10]+nsigma*sigmas_10000[10]))/1000;


  TCanvas *c10000 = new TCanvas();
  c10000->cd();
  Histo20_10000->SetLineColorAlpha(kBlue+3,0.65);
  Histo20_10000->SetTitle( "M^{2}_{miss} for different A' masses (10000 e+ per bunch)");
  Histo20_10000->SetLineWidth(3);
  Histo20_10000->GetYaxis()->SetTitle("N_{events}");
  Histo20_10000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo19_10000->SetLineColorAlpha(kBlue,0.65);
  Histo19_10000->SetLineWidth(3);
  Histo18_10000->SetLineColorAlpha(kCyan+3,0.65);
  Histo18_10000->SetLineWidth(3);
  Histo16_10000->SetLineColorAlpha(kCyan+1,0.65);
  Histo16_10000->SetLineWidth(3);
  Histo14_10000->SetLineColorAlpha(kGreen+3,0.65);
  Histo14_10000->SetLineWidth(3);
  Histo12_10000->SetLineColorAlpha(kGreen+1,0.65);
  Histo12_10000->SetLineWidth(3);
  Histo10_10000->SetLineColorAlpha(kYellow+2,0.65);
  Histo10_10000->SetLineWidth(3);
  Histo8_10000->SetLineColorAlpha(kOrange-3,0.65);
  Histo8_10000->SetLineWidth(3);
  Histo6_10000->SetLineColorAlpha(kRed+2,0.65);
  Histo6_10000->SetLineWidth(3);
  Histo4_10000->SetLineColorAlpha(kRed,0.65);
  Histo4_10000->SetLineWidth(3);
  Histo2_10000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo2_10000->SetLineWidth(3);
    
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

  auto legend_10000 = new TLegend(0.15,0.5,0.3,0.85);
  legend_10000->AddEntry(Histo20_10000,"M_{A'} = 20 MeV","l");
  legend_10000->AddEntry(Histo19_10000,"M_{A'} = 19 MeV","l");
  legend_10000->AddEntry(Histo18_10000,"M_{A'} = 18 MeV","l");
  legend_10000->AddEntry(Histo16_10000,"M_{A'} = 16 MeV","l");
  legend_10000->AddEntry(Histo14_10000,"M_{A'} = 14 MeV","l");
  legend_10000->AddEntry(Histo12_10000,"M_{A'} = 12 MeV","l");
  legend_10000->AddEntry(Histo10_10000,"M_{A'} = 10 MeV","l");
  legend_10000->AddEntry(Histo8_10000,"M_{A'} = 8 MeV","l");
  legend_10000->AddEntry(Histo6_10000,"M_{A'} = 6 MeV","l");
  legend_10000->AddEntry(Histo4_10000,"M_{A'} = 4 MeV","l");
  legend_10000->AddEntry(Histo2_10000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_10000->Draw();

  outRoot->WriteObject(c10000,"10000e_all");

  
  if(!(Histo20_25000->GetSumw2N() > 0)) Histo20_25000->Sumw2(kTRUE);
  Histo20_25000->Add(Histo0_25000_30,-1);
  if(!(Histo19_25000->GetSumw2N() > 0)) Histo19_25000->Sumw2(kTRUE);
  Histo19_25000->Add(Histo0_25000_30,-1);
  if(!(Histo18_25000->GetSumw2N() > 0)) Histo18_25000->Sumw2(kTRUE);
  Histo18_25000->Add(Histo0_25000_50,-1);
  if(!(Histo16_25000->GetSumw2N() > 0)) Histo16_25000->Sumw2(kTRUE);
  Histo16_25000->Add(Histo0_25000_60,-1);
  if(!(Histo14_25000->GetSumw2N() > 0)) Histo14_25000->Sumw2(kTRUE);
  Histo14_25000->Add(Histo0_25000_60,-1);
  if(!(Histo12_25000->GetSumw2N() > 0)) Histo12_25000->Sumw2(kTRUE);
  Histo12_25000->Add(Histo0_25000_60,-1);
  if(!(Histo10_25000->GetSumw2N() > 0)) Histo10_25000->Sumw2(kTRUE);
  Histo10_25000->Add(Histo0_25000_60,-1);
  if(!(Histo8_25000->GetSumw2N() > 0)) Histo8_25000->Sumw2(kTRUE);
  Histo8_25000->Add(Histo0_25000_60,-1);
  if(!(Histo6_25000->GetSumw2N() > 0)) Histo6_25000->Sumw2(kTRUE);
  Histo6_25000->Add(Histo0_25000_60,-1);
  if(!(Histo4_25000->GetSumw2N() > 0)) Histo4_25000->Sumw2(kTRUE);
  Histo4_25000->Add(Histo0_25000_60,-1);
  if(!(Histo2_25000->GetSumw2N() > 0)) Histo2_25000->Sumw2(kTRUE);
  Histo2_25000->Add(Histo0_25000_60,-1);

  Histo2_25000->Fit(f_2,"R");
  means_25000[0]=f_2->GetParameter(1);
  sigmas_25000[0]=f_2->GetParameter(2);

  Histo4_25000->Fit(f_4,"R");
  means_25000[1]=f_4->GetParameter(1);
  sigmas_25000[1]=f_4->GetParameter(2);
  
  Histo6_25000->Fit(f_6,"R");
  means_25000[2]=f_6->GetParameter(1);
  sigmas_25000[2]=f_6->GetParameter(2);
  
  Histo8_25000->Fit(f_8,"R");
  means_25000[3]=f_8->GetParameter(1);
  sigmas_25000[3]=f_8->GetParameter(2);
 
  Histo10_25000->Fit(f_10,"R");
  means_25000[4]=f_10->GetParameter(1);
  sigmas_25000[4]=f_10->GetParameter(2);
  
  Histo12_25000->Fit(f_12,"R");
  means_25000[5]=f_12->GetParameter(1);
  sigmas_25000[5]=f_12->GetParameter(2);
  
  Histo14_25000->Fit(f_14,"R");
  means_25000[6]=f_14->GetParameter(1);
  sigmas_25000[6]=f_14->GetParameter(2);
  
  Histo16_25000->Fit(f_16,"R");
  means_25000[7]=f_16->GetParameter(1);
  sigmas_25000[7]=f_16->GetParameter(2);
  
  Histo18_25000->Fit(f_18,"R");
  means_25000[8]=f_18->GetParameter(1);
  sigmas_25000[8]=f_18->GetParameter(2);
  
  Histo19_25000->Fit(f_19,"R");
  means_25000[9]=f_19->GetParameter(1);
  sigmas_25000[9]=f_19->GetParameter(2);
  
  Histo20_25000->Fit(f_20,"R");
  means_25000[10]=f_20->GetParameter(1);
  sigmas_25000[10]=f_20->GetParameter(2);

  entries_25000[0]=Histo2_25000->Integral(500+(means_25000[0]-nsigma*sigmas_25000[0]),500+(means_25000[0]+nsigma*sigmas_25000[0]))/1000;
  entries_25000[1]=Histo4_25000->Integral(500+(means_25000[1]-nsigma*sigmas_25000[1]),500+(means_25000[1]+nsigma*sigmas_25000[1]))/1000;
  entries_25000[2]=Histo6_25000->Integral(500+(means_25000[2]-nsigma*sigmas_25000[2]),500+(means_25000[2]+nsigma*sigmas_25000[2]))/1000;
  entries_25000[3]=Histo8_25000->Integral(500+(means_25000[3]-nsigma*sigmas_25000[3]),500+(means_25000[3]+nsigma*sigmas_25000[3]))/1000;
  entries_25000[4]=Histo10_25000->Integral(500+(means_25000[4]-nsigma*sigmas_25000[4]),500+(means_25000[4]+nsigma*sigmas_25000[4]))/1000;
  entries_25000[5]=Histo12_25000->Integral(500+(means_25000[5]-nsigma*sigmas_25000[5]),500+(means_25000[5]+nsigma*sigmas_25000[5]))/1000;
  entries_25000[6]=Histo14_25000->Integral(500+(means_25000[6]-nsigma*sigmas_25000[6]),500+(means_25000[6]+nsigma*sigmas_25000[6]))/1000;
  entries_25000[7]=Histo16_25000->Integral(500+(means_25000[7]-nsigma*sigmas_25000[7]),500+(means_25000[7]+nsigma*sigmas_25000[7]))/1000;
  entries_25000[8]=Histo18_25000->Integral(500+(means_25000[8]-nsigma*sigmas_25000[8]),500+(means_25000[8]+nsigma*sigmas_25000[8]))/1000;
  entries_25000[9]=Histo19_25000->Integral(500+(means_25000[9]-nsigma*sigmas_25000[9]),500+(means_25000[9]+nsigma*sigmas_25000[9]))/1000;
  entries_25000[10]=Histo20_25000->Integral(500+(means_25000[10]-nsigma*sigmas_25000[10]),500+(means_25000[10]+nsigma*sigmas_25000[10]))/1000;

  TCanvas *c25000 = new TCanvas();
  c25000->cd();
  Histo20_25000->SetLineColorAlpha(kBlue+3,0.65);
  Histo20_25000->SetTitle( "M^{2}_{miss} for different A' masses (25000 e+ per bunch)");
  Histo20_25000->SetLineWidth(3);
  Histo20_25000->GetYaxis()->SetTitle("N_{events}");
  Histo20_25000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo19_25000->SetLineColorAlpha(kBlue,0.65);
  Histo19_25000->SetLineWidth(3);
  Histo18_25000->SetLineColorAlpha(kCyan+3,0.65);
  Histo18_25000->SetLineWidth(3);
  Histo16_25000->SetLineColorAlpha(kCyan+1,0.65);
  Histo16_25000->SetLineWidth(3);
  Histo14_25000->SetLineColorAlpha(kGreen+3,0.65);
  Histo14_25000->SetLineWidth(3);
  Histo12_25000->SetLineColorAlpha(kGreen+1,0.65);
  Histo12_25000->SetLineWidth(3);
  Histo10_25000->SetLineColorAlpha(kYellow+2,0.65);
  Histo10_25000->SetLineWidth(3);
  Histo8_25000->SetLineColorAlpha(kOrange-3,0.65);
  Histo8_25000->SetLineWidth(3);
  Histo6_25000->SetLineColorAlpha(kRed+2,0.65);
  Histo6_25000->SetLineWidth(3);
  Histo4_25000->SetLineColorAlpha(kRed,0.65);
  Histo4_25000->SetLineWidth(3);
  Histo2_25000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo2_25000->SetLineWidth(3);
    
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

  auto legend_25000 = new TLegend(0.15,0.5,0.3,0.85);
  legend_25000->AddEntry(Histo20_25000,"M_{A'} = 20 MeV","l");
  legend_25000->AddEntry(Histo19_25000,"M_{A'} = 19 MeV","l");
  legend_25000->AddEntry(Histo18_25000,"M_{A'} = 18 MeV","l");
  legend_25000->AddEntry(Histo16_25000,"M_{A'} = 16 MeV","l");
  legend_25000->AddEntry(Histo14_25000,"M_{A'} = 14 MeV","l");
  legend_25000->AddEntry(Histo12_25000,"M_{A'} = 12 MeV","l");
  legend_25000->AddEntry(Histo10_25000,"M_{A'} = 10 MeV","l");
  legend_25000->AddEntry(Histo8_25000,"M_{A'} = 8 MeV","l");
  legend_25000->AddEntry(Histo6_25000,"M_{A'} = 6 MeV","l");
  legend_25000->AddEntry(Histo4_25000,"M_{A'} = 4 MeV","l");
  legend_25000->AddEntry(Histo2_25000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_25000->Draw();

  outRoot->WriteObject(c25000,"25000e_all");

  
  TCanvas *cgr = new TCanvas();
 
  auto gr_1 = new TGraph (11,masses,entries_1);
  gr_1->SetLineColor(kMagenta);
  gr_1->SetLineWidth(3); 
  gr_1->GetYaxis()->SetTitle("Acceptance [%]");
  gr_1->GetXaxis()->SetTitle("M_{A'} [MeV]");
  gr_1->SetTitle("Acceptance");
  gr_1->Draw();
  auto gr_10 = new TGraph (11,masses,entries_10);
  gr_10->SetLineColor(kRed);
  gr_10->SetLineWidth(3);
  gr_10->Draw("same");
  auto gr_100 = new TGraph (11,masses,entries_100);
  gr_100->SetLineColor(kOrange);
  gr_100->SetLineWidth(3);
  gr_100->Draw("same");
  auto gr_1000 = new TGraph (11,masses,entries_1000);
  gr_1000->SetLineColor(kGreen+3);
  gr_1000->SetLineWidth(3);
  gr_1000->Draw("same");
  auto gr_10000 = new TGraph (11,masses,entries_10000);
  gr_10000->SetLineColor(kCyan+1);
  gr_10000->SetLineWidth(3);
  gr_10000->Draw("same");
  auto gr_25000 = new TGraph (11,masses,entries_25000);
  gr_25000->SetLineColor(kBlue);
  gr_25000->SetLineWidth(3);
  gr_25000->Draw("same");
  auto legend_gr = new TLegend(0.15,0.5,0.3,0.85);
  legend_gr->AddEntry(gr_1,"1 e+","l");
  legend_gr->AddEntry(gr_10,"10 e+","l");
  legend_gr->AddEntry(gr_100,"100 e+","l");
  legend_gr->AddEntry(gr_1000,"1000 e+","l");
  legend_gr->AddEntry(gr_10000,"10000 e+","l");
  legend_gr->AddEntry(gr_25000,"25000 e+","l");
  legend_gr->Draw();

  TCanvas *csig = new TCanvas();
 
  auto sig_1 = new TGraph (11,masses,sigmas_1);
  sig_1->SetLineColor(kMagenta);
  sig_1->SetLineWidth(3);
  sig_1->GetYaxis()->SetTitle("Sigma");
  sig_1->GetXaxis()->SetTitle("M_{A'} [MeV]");
  sig_1->SetTitle("Sigma");
  sig_1->Draw();
  auto sig_10 = new TGraph (11,masses,sigmas_10);
  sig_10->SetLineColor(kRed);
  sig_10->SetLineWidth(3);
  sig_10->Draw("same");
  auto sig_100 = new TGraph (11,masses,sigmas_100);
  sig_100->SetLineColor(kOrange);
  sig_100->SetLineWidth(3);
  sig_100->Draw("same");
  auto sig_1000 = new TGraph (11,masses,sigmas_1000);
  sig_1000->SetLineColor(kGreen+3);
  sig_1000->SetLineWidth(3);
  sig_1000->Draw("same");
  auto sig_10000 = new TGraph (11,masses,sigmas_10000);
  sig_10000->SetLineColor(kCyan+1);
  sig_10000->SetLineWidth(3);
  sig_10000->Draw("same");
  auto sig_25000 = new TGraph (11,masses,sigmas_25000);
  sig_25000->SetLineColor(kBlue);
  sig_25000->SetLineWidth(3);
  sig_25000->Draw("same");
  auto legend_sig = new TLegend(0.15,0.5,0.3,0.85);
  legend_sig->AddEntry(sig_1,"1 e+","l");
  legend_sig->AddEntry(sig_10,"10 e+","l");
  legend_sig->AddEntry(sig_100,"100 e+","l");
  legend_sig->AddEntry(sig_1000,"1000 e+","l");
  legend_sig->AddEntry(sig_10000,"10000 e+","l");
  legend_sig->AddEntry(sig_25000,"25000 e+","l");
  legend_sig->Draw();

  
  outRoot->WriteObject(cgr,"Acceptance");
  outRoot->WriteObject(csig,"Sigma");
}
