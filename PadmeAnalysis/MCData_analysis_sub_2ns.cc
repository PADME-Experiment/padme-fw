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

  TFile *outRoot = new TFile("MCDataanalysis_sub_2ns.root","RECREATE");

  Int_t nsigma = 5;

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
  TF1 *f_4 = new TF1("f_4","gaus",-25., 30.);
  TF1 *f_6 = new TF1("f_6","gaus",-5., 60.);
  TF1 *f_8 = new TF1("f_8","gaus",25., 85.);
  TF1 *f_10 = new TF1("f_10","gaus",60., 120.);
  TF1 *f_12 = new TF1("f_12","gaus",110., 165.);
  TF1 *f_14 = new TF1("f_14","gaus",165., 210.);
  TF1 *f_16 = new TF1("f_10","gaus",230., 265.);
  TF1 *f_18 = new TF1("f_18","gaus",305., 330.);
  TF1 *f_19 = new TF1("f_19","gaus",342., 365.);
  TF1 *f_20 = new TF1("f_20","gaus",385., 400.);

 //----------------------------------------Do analysis for no time cut-------------------------------------------------Do analysis for no time cut-------------------------------------------------

  
  Float_t sigmas_noTcut_1[11];
  Float_t means_noTcut_1[11];
  Float_t entries_noTcut_1[11];

  Float_t sigmas_noTcut_10[11];
  Float_t means_noTcut_10[11];
  Float_t entries_noTcut_10[11];

  Float_t sigmas_noTcut_100[11];
  Float_t means_noTcut_100[11];
  Float_t entries_noTcut_100[11];

  Float_t sigmas_noTcut_1000[11];
  Float_t means_noTcut_1000[11];
  Float_t entries_noTcut_1000[11];

  Float_t sigmas_noTcut_10000[11];
  Float_t means_noTcut_10000[11];
  Float_t entries_noTcut_10000[11];
  
  Float_t sigmas_noTcut_25000[11];
  Float_t means_noTcut_25000[11];
  Float_t entries_noTcut_25000[11];
   
  //---------------------------------------------- Open Histograms --------------------------------------------------

  TH1F *Histo_noTcut20_1 = (TH1F*) dir20_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut19_1 = (TH1F*) dir19_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut18_1 = (TH1F*) dir18_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo_noTcut16_1 = (TH1F*) dir16_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut14_1 = (TH1F*) dir14_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut12_1 = (TH1F*) dir12_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut10_1 = (TH1F*) dir10_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut8_1 = (TH1F*) dir8_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut6_1 = (TH1F*) dir6_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut4_1 = (TH1F*) dir4_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut2_1 = (TH1F*) dir2_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut0_1_10 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV");
  TH1F *Histo_noTcut0_1_20 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV");
  TH1F *Histo_noTcut0_1_30 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut0_1_40 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV");
  TH1F *Histo_noTcut0_1_50 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo_noTcut0_1_60 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  
  TH1F *Histo_noTcut20_10 = (TH1F*) dir20_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut19_10 = (TH1F*) dir19_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut18_10 = (TH1F*) dir18_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo_noTcut16_10 = (TH1F*) dir16_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut14_10 = (TH1F*) dir14_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut12_10 = (TH1F*) dir12_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut10_10 = (TH1F*) dir10_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut8_10 = (TH1F*) dir8_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut6_10 = (TH1F*) dir6_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut4_10 = (TH1F*) dir4_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut2_10 = (TH1F*) dir2_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut0_10_10 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV");
  TH1F *Histo_noTcut0_10_20 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV");
  TH1F *Histo_noTcut0_10_30 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut0_10_40 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV");
  TH1F *Histo_noTcut0_10_50 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo_noTcut0_10_60 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  
  TH1F *Histo_noTcut20_100 = (TH1F*) dir20_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut19_100 = (TH1F*) dir19_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut18_100 = (TH1F*) dir18_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo_noTcut16_100 = (TH1F*) dir16_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut14_100 = (TH1F*) dir14_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut12_100 = (TH1F*) dir12_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut10_100 = (TH1F*) dir10_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut8_100 = (TH1F*) dir8_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut6_100 = (TH1F*) dir6_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut4_100 = (TH1F*) dir4_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut2_100 = (TH1F*) dir2_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut0_100_10 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV");
  TH1F *Histo_noTcut0_100_20 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV");
  TH1F *Histo_noTcut0_100_30 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut0_100_40 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV");
  TH1F *Histo_noTcut0_100_50 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo_noTcut0_100_60 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  
  TH1F *Histo_noTcut20_1000 = (TH1F*) dir20_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut19_1000 = (TH1F*) dir19_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut18_1000 = (TH1F*) dir18_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo_noTcut16_1000 = (TH1F*) dir16_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut14_1000 = (TH1F*) dir14_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut12_1000 = (TH1F*) dir12_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut10_1000 = (TH1F*) dir10_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut8_1000 = (TH1F*) dir8_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut6_1000 = (TH1F*) dir6_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut4_1000 = (TH1F*) dir4_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut2_1000 = (TH1F*) dir2_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut0_1000_10 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV");
  TH1F *Histo_noTcut0_1000_20 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV");
  TH1F *Histo_noTcut0_1000_30 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut0_1000_40 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV");
  TH1F *Histo_noTcut0_1000_50 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo_noTcut0_1000_60 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  
  TH1F *Histo_noTcut20_10000 = (TH1F*) dir20_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut19_10000 = (TH1F*) dir19_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut18_10000 = (TH1F*) dir18_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo_noTcut16_10000 = (TH1F*) dir16_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut14_10000 = (TH1F*) dir14_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut12_10000 = (TH1F*) dir12_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut10_10000 = (TH1F*) dir10_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut8_10000 = (TH1F*) dir8_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut6_10000 = (TH1F*) dir6_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut4_10000 = (TH1F*) dir4_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut2_10000 = (TH1F*) dir2_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut0_10000_10 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV");
  TH1F *Histo_noTcut0_10000_20 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV");
  TH1F *Histo_noTcut0_10000_30 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut0_10000_40 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV");
  TH1F *Histo_noTcut0_10000_50 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo_noTcut0_10000_60 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  
  TH1F *Histo_noTcut20_25000 = (TH1F*) dir20_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut19_25000 = (TH1F*) dir19_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut18_25000 = (TH1F*) dir18_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo_noTcut16_25000 = (TH1F*) dir16_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut14_25000 = (TH1F*) dir14_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut12_25000 = (TH1F*) dir12_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut10_25000 = (TH1F*) dir10_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut8_25000 = (TH1F*) dir8_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut6_25000 = (TH1F*) dir6_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut4_25000 = (TH1F*) dir4_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut2_25000 = (TH1F*) dir2_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");
  TH1F *Histo_noTcut0_25000_10 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV");
  TH1F *Histo_noTcut0_25000_20 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV");
  TH1F *Histo_noTcut0_25000_30 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV");
  TH1F *Histo_noTcut0_25000_40 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV");
  TH1F *Histo_noTcut0_25000_50 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV");
  TH1F *Histo_noTcut0_25000_60 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV");

 
  Histo_noTcut2_1->Fit(f_2,"R");
  means_noTcut_1[0]=f_2->GetParameter(1);
  sigmas_noTcut_1[0]=f_2->GetParameter(2);

  Histo_noTcut4_1->Fit(f_4,"R");
  means_noTcut_1[1]=f_4->GetParameter(1);
  sigmas_noTcut_1[1]=f_4->GetParameter(2);
  
  Histo_noTcut6_1->Fit(f_6,"R");
  means_noTcut_1[2]=f_6->GetParameter(1);
  sigmas_noTcut_1[2]=f_6->GetParameter(2);
  
  Histo_noTcut8_1->Fit(f_8,"R");
  means_noTcut_1[3]=f_8->GetParameter(1);
  sigmas_noTcut_1[3]=f_8->GetParameter(2);
  
  Histo_noTcut10_1->Fit(f_10,"R");
  means_noTcut_1[4]=f_10->GetParameter(1);
  sigmas_noTcut_1[4]=f_10->GetParameter(2);
  
  Histo_noTcut12_1->Fit(f_12,"R");
  means_noTcut_1[5]=f_12->GetParameter(1);
  sigmas_noTcut_1[5]=f_12->GetParameter(2);
  
  Histo_noTcut14_1->Fit(f_14,"R");
  means_noTcut_1[6]=f_14->GetParameter(1);
  sigmas_noTcut_1[6]=f_14->GetParameter(2);
  
  Histo_noTcut16_1->Fit(f_16,"R");
  means_noTcut_1[7]=f_16->GetParameter(1);
  sigmas_noTcut_1[7]=f_16->GetParameter(2);
  
  Histo_noTcut18_1->Fit(f_18,"R");
  means_noTcut_1[8]=f_18->GetParameter(1);
  sigmas_noTcut_1[8]=f_18->GetParameter(2);
  
  Histo_noTcut19_1->Fit(f_19,"R");
  means_noTcut_1[9]=f_19->GetParameter(1);
  sigmas_noTcut_1[9]=f_19->GetParameter(2);
  
  Histo_noTcut20_1->Fit(f_20,"R");
  means_noTcut_1[10]=f_20->GetParameter(1);
  sigmas_noTcut_1[10]=f_20->GetParameter(2);
  
  entries_noTcut_1[0]=Histo_noTcut2_1->Integral(500+(means_noTcut_1[0]-nsigma*sigmas_noTcut_1[0]),500+(means_noTcut_1[0]+nsigma*sigmas_noTcut_1[0]))/1000;
  entries_noTcut_1[1]=Histo_noTcut4_1->Integral(500+(means_noTcut_1[1]-nsigma*sigmas_noTcut_1[1]),500+(means_noTcut_1[1]+nsigma*sigmas_noTcut_1[1]))/1000;
  entries_noTcut_1[2]=Histo_noTcut6_1->Integral(500+(means_noTcut_1[2]-nsigma*sigmas_noTcut_1[2]),500+(means_noTcut_1[2]+nsigma*sigmas_noTcut_1[2]))/1000;
  entries_noTcut_1[3]=Histo_noTcut8_1->Integral(500+(means_noTcut_1[3]-nsigma*sigmas_noTcut_1[3]),500+(means_noTcut_1[3]+nsigma*sigmas_noTcut_1[3]))/1000;
  entries_noTcut_1[4]=Histo_noTcut10_1->Integral(500+(means_noTcut_1[4]-nsigma*sigmas_noTcut_1[4]),500+(means_noTcut_1[4]+nsigma*sigmas_noTcut_1[4]))/1000;
  entries_noTcut_1[5]=Histo_noTcut12_1->Integral(500+(means_noTcut_1[5]-nsigma*sigmas_noTcut_1[5]),500+(means_noTcut_1[5]+nsigma*sigmas_noTcut_1[5]))/1000;
  entries_noTcut_1[6]=Histo_noTcut14_1->Integral(500+(means_noTcut_1[6]-nsigma*sigmas_noTcut_1[6]),500+(means_noTcut_1[6]+nsigma*sigmas_noTcut_1[6]))/1000;
  entries_noTcut_1[7]=Histo_noTcut16_1->Integral(500+(means_noTcut_1[7]-nsigma*sigmas_noTcut_1[7]),500+(means_noTcut_1[7]+nsigma*sigmas_noTcut_1[7]))/1000;
  entries_noTcut_1[8]=Histo_noTcut18_1->Integral(500+(means_noTcut_1[8]-nsigma*sigmas_noTcut_1[8]),500+(means_noTcut_1[8]+nsigma*sigmas_noTcut_1[8]))/1000;
  entries_noTcut_1[9]=Histo_noTcut19_1->Integral(500+(means_noTcut_1[9]-nsigma*sigmas_noTcut_1[9]),500+(means_noTcut_1[9]+nsigma*sigmas_noTcut_1[9]))/1000;
  entries_noTcut_1[10]=Histo_noTcut20_1->Integral(500+(means_noTcut_1[10]-nsigma*sigmas_noTcut_1[10]),500+(means_noTcut_1[10]+nsigma*sigmas_noTcut_1[10]))/1000;
  
  TCanvas *c1_noTcut = new TCanvas();
  c1_noTcut->cd();
  Histo_noTcut20_1->SetLineColorAlpha(kBlue+3,0.65);
  Histo_noTcut20_1->SetTitle( "M^{2}_{miss} for different A' masses (1 e+ per bunch)");
  Histo_noTcut20_1->SetLineWidth(3);
  Histo_noTcut20_1->GetYaxis()->SetTitle("N_{events}");
  Histo_noTcut20_1->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_noTcut19_1->SetLineColorAlpha(kBlue,0.65);
  Histo_noTcut19_1->SetLineWidth(3);
  Histo_noTcut18_1->SetLineColorAlpha(kCyan+3,0.65);
  Histo_noTcut18_1->SetLineWidth(3);
  Histo_noTcut16_1->SetLineColorAlpha(kCyan+1,0.65);
  Histo_noTcut16_1->SetLineWidth(3);
  Histo_noTcut14_1->SetLineColorAlpha(kGreen+3,0.65);
  Histo_noTcut14_1->SetLineWidth(3);
  Histo_noTcut12_1->SetLineColorAlpha(kGreen+1,0.65);
  Histo_noTcut12_1->SetLineWidth(3);
  Histo_noTcut10_1->SetLineColorAlpha(kYellow+2,0.65);
  Histo_noTcut10_1->SetLineWidth(3);
  Histo_noTcut8_1->SetLineColorAlpha(kOrange-3,0.65);
  Histo_noTcut8_1->SetLineWidth(3);
  Histo_noTcut6_1->SetLineColorAlpha(kRed+2,0.65);
  Histo_noTcut6_1->SetLineWidth(3);
  Histo_noTcut4_1->SetLineColorAlpha(kRed,0.65);
  Histo_noTcut4_1->SetLineWidth(3);
  Histo_noTcut2_1->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_noTcut2_1->SetLineWidth(3);
    
  Histo_noTcut20_1->Draw();
  Histo_noTcut19_1->Draw("same");
  Histo_noTcut18_1->Draw("same");
  Histo_noTcut4_1->Draw("same");
  Histo_noTcut6_1->Draw("same");
  Histo_noTcut8_1->Draw("same");
  Histo_noTcut10_1->Draw("same");
  Histo_noTcut12_1->Draw("same");
  Histo_noTcut14_1->Draw("same");
  Histo_noTcut16_1->Draw("same");
  Histo_noTcut2_1->Draw("same");

  auto legend_1_noTcut = new TLegend(0.15,0.5,0.3,0.85);
  legend_1_noTcut->AddEntry(Histo_noTcut20_1,"M_{A'} = 20 MeV","l");
  legend_1_noTcut->AddEntry(Histo_noTcut19_1,"M_{A'} = 19 MeV","l");
  legend_1_noTcut->AddEntry(Histo_noTcut18_1,"M_{A'} = 18 MeV","l");
  legend_1_noTcut->AddEntry(Histo_noTcut16_1,"M_{A'} = 16 MeV","l");
  legend_1_noTcut->AddEntry(Histo_noTcut14_1,"M_{A'} = 14 MeV","l");
  legend_1_noTcut->AddEntry(Histo_noTcut12_1,"M_{A'} = 12 MeV","l");
  legend_1_noTcut->AddEntry(Histo_noTcut10_1,"M_{A'} = 10 MeV","l");
  legend_1_noTcut->AddEntry(Histo_noTcut8_1,"M_{A'} = 8 MeV","l");
  legend_1_noTcut->AddEntry(Histo_noTcut6_1,"M_{A'} = 6 MeV","l");
  legend_1_noTcut->AddEntry(Histo_noTcut4_1,"M_{A'} = 4 MeV","l");
  legend_1_noTcut->AddEntry(Histo_noTcut2_1,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_1_noTcut->Draw();

  outRoot->WriteObject(c1_noTcut,"1e_all_noTcut");

  if(!(Histo_noTcut20_10->GetSumw2N() > 0)) Histo_noTcut20_10->Sumw2(kTRUE);
  Histo_noTcut20_10->Add(Histo_noTcut0_10_30,-1);
  if(!(Histo_noTcut19_10->GetSumw2N() > 0)) Histo_noTcut19_10->Sumw2(kTRUE);
  Histo_noTcut19_10->Add(Histo_noTcut0_10_30,-1);
  if(!(Histo_noTcut18_10->GetSumw2N() > 0)) Histo_noTcut18_10->Sumw2(kTRUE);
  Histo_noTcut18_10->Add(Histo_noTcut0_10_50,-1);
  if(!(Histo_noTcut16_10->GetSumw2N() > 0)) Histo_noTcut16_10->Sumw2(kTRUE);
  Histo_noTcut16_10->Add(Histo_noTcut0_10_60,-1);
  if(!(Histo_noTcut14_10->GetSumw2N() > 0)) Histo_noTcut14_10->Sumw2(kTRUE);
  Histo_noTcut14_10->Add(Histo_noTcut0_10_60,-1);
  if(!(Histo_noTcut12_10->GetSumw2N() > 0)) Histo_noTcut12_10->Sumw2(kTRUE);
  Histo_noTcut12_10->Add(Histo_noTcut0_10_60,-1);
  if(!(Histo_noTcut10_10->GetSumw2N() > 0)) Histo_noTcut10_10->Sumw2(kTRUE);
  Histo_noTcut10_10->Add(Histo_noTcut0_10_60,-1);
  if(!(Histo_noTcut8_10->GetSumw2N() > 0)) Histo_noTcut8_10->Sumw2(kTRUE);
  Histo_noTcut8_10->Add(Histo_noTcut0_10_60,-1);
  if(!(Histo_noTcut6_10->GetSumw2N() > 0)) Histo_noTcut6_10->Sumw2(kTRUE);
  Histo_noTcut6_10->Add(Histo_noTcut0_10_60,-1);
  if(!(Histo_noTcut4_10->GetSumw2N() > 0)) Histo_noTcut4_10->Sumw2(kTRUE);
  Histo_noTcut4_10->Add(Histo_noTcut0_10_60,-1);
  if(!(Histo_noTcut2_10->GetSumw2N() > 0)) Histo_noTcut2_10->Sumw2(kTRUE);
  Histo_noTcut2_10->Add(Histo_noTcut0_10_60,-1);
  
  Histo_noTcut2_10->Fit(f_2,"R");
  means_noTcut_10[0]=f_2->GetParameter(1);
  sigmas_noTcut_10[0]=f_2->GetParameter(2);

  Histo_noTcut4_10->Fit(f_4,"R");
  means_noTcut_10[1]=f_4->GetParameter(1);
  sigmas_noTcut_10[1]=f_4->GetParameter(2);
  
  Histo_noTcut6_10->Fit(f_6,"R");
  means_noTcut_10[2]=f_6->GetParameter(1);
  sigmas_noTcut_10[2]=f_6->GetParameter(2);
  
  Histo_noTcut8_10->Fit(f_8,"R");
  means_noTcut_10[3]=f_8->GetParameter(1);
  sigmas_noTcut_10[3]=f_8->GetParameter(2);
 
  Histo_noTcut10_10->Fit(f_10,"R");
  means_noTcut_10[4]=f_10->GetParameter(1);
  sigmas_noTcut_10[4]=f_10->GetParameter(2);
  
  Histo_noTcut12_10->Fit(f_12,"R");
  means_noTcut_10[5]=f_12->GetParameter(1);
  sigmas_noTcut_10[5]=f_12->GetParameter(2);
  
  Histo_noTcut14_10->Fit(f_14,"R");
  means_noTcut_10[6]=f_14->GetParameter(1);
  sigmas_noTcut_10[6]=f_14->GetParameter(2);
  
  Histo_noTcut16_10->Fit(f_16,"R");
  means_noTcut_10[7]=f_16->GetParameter(1);
  sigmas_noTcut_10[7]=f_16->GetParameter(2);
  
  Histo_noTcut18_10->Fit(f_18,"R");
  means_noTcut_10[8]=f_18->GetParameter(1);
  sigmas_noTcut_10[8]=f_18->GetParameter(2);
  
  Histo_noTcut19_10->Fit(f_19,"R");
  means_noTcut_10[9]=f_19->GetParameter(1);
  sigmas_noTcut_10[9]=f_19->GetParameter(2);
  
  Histo_noTcut20_10->Fit(f_20,"R");
  means_noTcut_10[10]=f_20->GetParameter(1);
  sigmas_noTcut_10[10]=f_20->GetParameter(2);

  entries_noTcut_10[0]=Histo_noTcut2_10->Integral(500+(means_noTcut_10[0]-nsigma*sigmas_noTcut_10[0]),500+(means_noTcut_10[0]+nsigma*sigmas_noTcut_10[0]))/1000;
  entries_noTcut_10[1]=Histo_noTcut4_10->Integral(500+(means_noTcut_10[1]-nsigma*sigmas_noTcut_10[1]),500+(means_noTcut_10[1]+nsigma*sigmas_noTcut_10[1]))/1000;
  entries_noTcut_10[2]=Histo_noTcut6_10->Integral(500+(means_noTcut_10[2]-nsigma*sigmas_noTcut_10[2]),500+(means_noTcut_10[2]+nsigma*sigmas_noTcut_10[2]))/1000;
  entries_noTcut_10[3]=Histo_noTcut8_10->Integral(500+(means_noTcut_10[3]-nsigma*sigmas_noTcut_10[3]),500+(means_noTcut_10[3]+nsigma*sigmas_noTcut_10[3]))/1000;
  entries_noTcut_10[4]=Histo_noTcut10_10->Integral(500+(means_noTcut_10[4]-nsigma*sigmas_noTcut_10[4]),500+(means_noTcut_10[4]+nsigma*sigmas_noTcut_10[4]))/1000;
  entries_noTcut_10[5]=Histo_noTcut12_10->Integral(500+(means_noTcut_10[5]-nsigma*sigmas_noTcut_10[5]),500+(means_noTcut_10[5]+nsigma*sigmas_noTcut_10[5]))/1000;
  entries_noTcut_10[6]=Histo_noTcut14_10->Integral(500+(means_noTcut_10[6]-nsigma*sigmas_noTcut_10[6]),500+(means_noTcut_10[6]+nsigma*sigmas_noTcut_10[6]))/1000;
  entries_noTcut_10[7]=Histo_noTcut16_10->Integral(500+(means_noTcut_10[7]-nsigma*sigmas_noTcut_10[7]),500+(means_noTcut_10[7]+nsigma*sigmas_noTcut_10[7]))/1000;
  entries_noTcut_10[8]=Histo_noTcut18_10->Integral(500+(means_noTcut_10[8]-nsigma*sigmas_noTcut_10[8]),500+(means_noTcut_10[8]+nsigma*sigmas_noTcut_10[8]))/1000;
  entries_noTcut_10[9]=Histo_noTcut19_10->Integral(500+(means_noTcut_10[9]-nsigma*sigmas_noTcut_10[9]),500+(means_noTcut_10[9]+nsigma*sigmas_noTcut_10[9]))/1000;
  entries_noTcut_10[10]=Histo_noTcut20_10->Integral(500+(means_noTcut_10[10]-nsigma*sigmas_noTcut_10[10]),500+(means_noTcut_10[10]+nsigma*sigmas_noTcut_10[10]))/1000;

  TCanvas *c10_noTcut = new TCanvas();
  c10_noTcut->cd();
  Histo_noTcut20_10->SetLineColorAlpha(kBlue+3,0.65);
  Histo_noTcut20_10->SetTitle( "M^{2}_{miss} for different A' masses (10 e+ per bunch)");
  Histo_noTcut20_10->SetLineWidth(3);
  Histo_noTcut20_10->GetYaxis()->SetTitle("N_{events}");
  Histo_noTcut20_10->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_noTcut19_10->SetLineColorAlpha(kBlue,0.65);
  Histo_noTcut19_10->SetLineWidth(3);
  Histo_noTcut18_10->SetLineColorAlpha(kCyan+3,0.65);
  Histo_noTcut18_10->SetLineWidth(3);
  Histo_noTcut16_10->SetLineColorAlpha(kCyan+1,0.65);
  Histo_noTcut16_10->SetLineWidth(3);
  Histo_noTcut14_10->SetLineColorAlpha(kGreen+3,0.65);
  Histo_noTcut14_10->SetLineWidth(3);
  Histo_noTcut12_10->SetLineColorAlpha(kGreen+1,0.65);
  Histo_noTcut12_10->SetLineWidth(3);
  Histo_noTcut10_10->SetLineColorAlpha(kYellow+2,0.65);
  Histo_noTcut10_10->SetLineWidth(3);
  Histo_noTcut8_10->SetLineColorAlpha(kOrange-3,0.65);
  Histo_noTcut8_10->SetLineWidth(3);
  Histo_noTcut6_10->SetLineColorAlpha(kRed+2,0.65);
  Histo_noTcut6_10->SetLineWidth(3);
  Histo_noTcut4_10->SetLineColorAlpha(kRed,0.65);
  Histo_noTcut4_10->SetLineWidth(3);
  Histo_noTcut2_10->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_noTcut2_10->SetLineWidth(3);
    
  Histo_noTcut20_10->Draw();
  Histo_noTcut19_10->Draw("same");
  Histo_noTcut18_10->Draw("same");
  Histo_noTcut4_10->Draw("same");
  Histo_noTcut6_10->Draw("same");
  Histo_noTcut8_10->Draw("same");
  Histo_noTcut10_10->Draw("same");
  Histo_noTcut12_10->Draw("same");
  Histo_noTcut14_10->Draw("same");
  Histo_noTcut16_10->Draw("same");
  Histo_noTcut2_10->Draw("same");

  auto legend_10_noTcut = new TLegend(0.15,0.5,0.3,0.85);
  legend_10_noTcut->AddEntry(Histo_noTcut20_10,"M_{A'} = 20 MeV","l");
  legend_10_noTcut->AddEntry(Histo_noTcut19_10,"M_{A'} = 19 MeV","l");
  legend_10_noTcut->AddEntry(Histo_noTcut18_10,"M_{A'} = 18 MeV","l");
  legend_10_noTcut->AddEntry(Histo_noTcut16_10,"M_{A'} = 16 MeV","l");
  legend_10_noTcut->AddEntry(Histo_noTcut14_10,"M_{A'} = 14 MeV","l");
  legend_10_noTcut->AddEntry(Histo_noTcut12_10,"M_{A'} = 12 MeV","l");
  legend_10_noTcut->AddEntry(Histo_noTcut10_10,"M_{A'} = 10 MeV","l");
  legend_10_noTcut->AddEntry(Histo_noTcut8_10,"M_{A'} = 8 MeV","l");
  legend_10_noTcut->AddEntry(Histo_noTcut6_10,"M_{A'} = 6 MeV","l");
  legend_10_noTcut->AddEntry(Histo_noTcut4_10,"M_{A'} = 4 MeV","l");
  legend_10_noTcut->AddEntry(Histo_noTcut2_10,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_10_noTcut->Draw();

  outRoot->WriteObject(c10_noTcut,"10e_all_noTcut");
 
  if(!(Histo_noTcut20_100->GetSumw2N() > 0)) Histo_noTcut20_100->Sumw2(kTRUE);
  Histo_noTcut20_100->Add(Histo_noTcut0_100_30,-1);
  if(!(Histo_noTcut19_100->GetSumw2N() > 0)) Histo_noTcut19_100->Sumw2(kTRUE);
  Histo_noTcut19_100->Add(Histo_noTcut0_100_30,-1);
  if(!(Histo_noTcut18_100->GetSumw2N() > 0)) Histo_noTcut18_100->Sumw2(kTRUE);
  Histo_noTcut18_100->Add(Histo_noTcut0_100_50,-1);
  if(!(Histo_noTcut16_100->GetSumw2N() > 0)) Histo_noTcut16_100->Sumw2(kTRUE);
  Histo_noTcut16_100->Add(Histo_noTcut0_100_60,-1);
  if(!(Histo_noTcut14_100->GetSumw2N() > 0)) Histo_noTcut14_100->Sumw2(kTRUE);
  Histo_noTcut14_100->Add(Histo_noTcut0_100_60,-1);
  if(!(Histo_noTcut12_100->GetSumw2N() > 0)) Histo_noTcut12_100->Sumw2(kTRUE);
  Histo_noTcut12_100->Add(Histo_noTcut0_100_60,-1);
  if(!(Histo_noTcut10_100->GetSumw2N() > 0)) Histo_noTcut10_100->Sumw2(kTRUE);
  Histo_noTcut10_100->Add(Histo_noTcut0_100_60,-1);
  if(!(Histo_noTcut8_100->GetSumw2N() > 0)) Histo_noTcut8_100->Sumw2(kTRUE);
  Histo_noTcut8_100->Add(Histo_noTcut0_100_60,-1);
  if(!(Histo_noTcut6_100->GetSumw2N() > 0)) Histo_noTcut6_100->Sumw2(kTRUE);
  Histo_noTcut6_100->Add(Histo_noTcut0_100_60,-1);
  if(!(Histo_noTcut4_100->GetSumw2N() > 0)) Histo_noTcut4_100->Sumw2(kTRUE);
  Histo_noTcut4_100->Add(Histo_noTcut0_100_60,-1);
  if(!(Histo_noTcut2_100->GetSumw2N() > 0)) Histo_noTcut2_100->Sumw2(kTRUE);
  Histo_noTcut2_100->Add(Histo_noTcut0_100_60,-1);


  Histo_noTcut2_100->Fit(f_2,"R");
  means_noTcut_100[0]=f_2->GetParameter(1);
  sigmas_noTcut_100[0]=f_2->GetParameter(2);

  Histo_noTcut4_100->Fit(f_4,"R");
  means_noTcut_100[1]=f_4->GetParameter(1);
  sigmas_noTcut_100[1]=f_4->GetParameter(2);
  
  Histo_noTcut6_100->Fit(f_6,"R");
  means_noTcut_100[2]=f_6->GetParameter(1);
  sigmas_noTcut_100[2]=f_6->GetParameter(2);
  
  Histo_noTcut8_100->Fit(f_8,"R");
  means_noTcut_100[3]=f_8->GetParameter(1);
  sigmas_noTcut_100[3]=f_8->GetParameter(2);
 
  Histo_noTcut10_100->Fit(f_10,"R");
  means_noTcut_100[4]=f_10->GetParameter(1);
  sigmas_noTcut_100[4]=f_10->GetParameter(2);
  
  Histo_noTcut12_100->Fit(f_12,"R");
  means_noTcut_100[5]=f_12->GetParameter(1);
  sigmas_noTcut_100[5]=f_12->GetParameter(2);
  
  Histo_noTcut14_100->Fit(f_14,"R");
  means_noTcut_100[6]=f_14->GetParameter(1);
  sigmas_noTcut_100[6]=f_14->GetParameter(2);
  
  Histo_noTcut16_100->Fit(f_16,"R");
  means_noTcut_100[7]=f_16->GetParameter(1);
  sigmas_noTcut_100[7]=f_16->GetParameter(2);
  
  Histo_noTcut18_100->Fit(f_18,"R");
  means_noTcut_100[8]=f_18->GetParameter(1);
  sigmas_noTcut_100[8]=f_18->GetParameter(2);
  
  Histo_noTcut19_100->Fit(f_19,"R");
  means_noTcut_100[9]=f_19->GetParameter(1);
  sigmas_noTcut_100[9]=f_19->GetParameter(2);
  
  Histo_noTcut20_100->Fit(f_20,"R");
  means_noTcut_100[10]=f_20->GetParameter(1);
  sigmas_noTcut_100[10]=f_20->GetParameter(2);

  entries_noTcut_100[0]=Histo_noTcut2_100->Integral(500+(means_noTcut_100[0]-nsigma*sigmas_noTcut_100[0]),500+(means_noTcut_100[0]+nsigma*sigmas_noTcut_100[0]))/1000;
  entries_noTcut_100[1]=Histo_noTcut4_100->Integral(500+(means_noTcut_100[1]-nsigma*sigmas_noTcut_100[1]),500+(means_noTcut_100[1]+nsigma*sigmas_noTcut_100[1]))/1000;
  entries_noTcut_100[2]=Histo_noTcut6_100->Integral(500+(means_noTcut_100[2]-nsigma*sigmas_noTcut_100[2]),500+(means_noTcut_100[2]+nsigma*sigmas_noTcut_100[2]))/1000;
  entries_noTcut_100[3]=Histo_noTcut8_100->Integral(500+(means_noTcut_100[3]-nsigma*sigmas_noTcut_100[3]),500+(means_noTcut_100[3]+nsigma*sigmas_noTcut_100[3]))/1000;
  entries_noTcut_100[4]=Histo_noTcut10_100->Integral(500+(means_noTcut_100[4]-nsigma*sigmas_noTcut_100[4]),500+(means_noTcut_100[4]+nsigma*sigmas_noTcut_100[4]))/1000;
  entries_noTcut_100[5]=Histo_noTcut12_100->Integral(500+(means_noTcut_100[5]-nsigma*sigmas_noTcut_100[5]),500+(means_noTcut_100[5]+nsigma*sigmas_noTcut_100[5]))/1000;
  entries_noTcut_100[6]=Histo_noTcut14_100->Integral(500+(means_noTcut_100[6]-nsigma*sigmas_noTcut_100[6]),500+(means_noTcut_100[6]+nsigma*sigmas_noTcut_100[6]))/1000;
  entries_noTcut_100[7]=Histo_noTcut16_100->Integral(500+(means_noTcut_100[7]-nsigma*sigmas_noTcut_100[7]),500+(means_noTcut_100[7]+nsigma*sigmas_noTcut_100[7]))/1000;
  entries_noTcut_100[8]=Histo_noTcut18_100->Integral(500+(means_noTcut_100[8]-nsigma*sigmas_noTcut_100[8]),500+(means_noTcut_100[8]+nsigma*sigmas_noTcut_100[8]))/1000;
  entries_noTcut_100[9]=Histo_noTcut19_100->Integral(500+(means_noTcut_100[9]-nsigma*sigmas_noTcut_100[9]),500+(means_noTcut_100[9]+nsigma*sigmas_noTcut_100[9]))/1000;
  entries_noTcut_100[10]=Histo_noTcut20_100->Integral(500+(means_noTcut_100[10]-nsigma*sigmas_noTcut_100[10]),500+(means_noTcut_100[10]+nsigma*sigmas_noTcut_100[10]))/1000;

  TCanvas *c100_noTcut = new TCanvas();
  c100_noTcut->cd();
  Histo_noTcut20_100->SetLineColorAlpha(kBlue+3,0.65);
  Histo_noTcut20_100->SetTitle( "M^{2}_{miss} for different A' masses (100 e+ per bunch)");
  Histo_noTcut20_100->SetLineWidth(3);
  Histo_noTcut20_100->GetYaxis()->SetTitle("N_{events}");
  Histo_noTcut20_100->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_noTcut19_100->SetLineColorAlpha(kBlue,0.65);
  Histo_noTcut19_100->SetLineWidth(3);
  Histo_noTcut18_100->SetLineColorAlpha(kCyan+3,0.65);
  Histo_noTcut18_100->SetLineWidth(3);
  Histo_noTcut16_100->SetLineColorAlpha(kCyan+1,0.65);
  Histo_noTcut16_100->SetLineWidth(3);
  Histo_noTcut14_100->SetLineColorAlpha(kGreen+3,0.65);
  Histo_noTcut14_100->SetLineWidth(3);
  Histo_noTcut12_100->SetLineColorAlpha(kGreen+1,0.65);
  Histo_noTcut12_100->SetLineWidth(3);
  Histo_noTcut10_100->SetLineColorAlpha(kYellow+2,0.65);
  Histo_noTcut10_100->SetLineWidth(3);
  Histo_noTcut8_100->SetLineColorAlpha(kOrange-3,0.65);
  Histo_noTcut8_100->SetLineWidth(3);
  Histo_noTcut6_100->SetLineColorAlpha(kRed+2,0.65);
  Histo_noTcut6_100->SetLineWidth(3);
  Histo_noTcut4_100->SetLineColorAlpha(kRed,0.65);
  Histo_noTcut4_100->SetLineWidth(3);
  Histo_noTcut2_100->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_noTcut2_100->SetLineWidth(3);
    
  Histo_noTcut20_100->Draw();
  Histo_noTcut19_100->Draw("same");
  Histo_noTcut18_100->Draw("same");
  Histo_noTcut4_100->Draw("same");
  Histo_noTcut6_100->Draw("same");
  Histo_noTcut8_100->Draw("same");
  Histo_noTcut10_100->Draw("same");
  Histo_noTcut12_100->Draw("same");
  Histo_noTcut14_100->Draw("same");
  Histo_noTcut16_100->Draw("same");
  Histo_noTcut2_100->Draw("same");

  auto legend_100_noTcut = new TLegend(0.15,0.5,0.3,0.85);
  legend_100_noTcut->AddEntry(Histo_noTcut20_100,"M_{A'} = 20 MeV","l");
  legend_100_noTcut->AddEntry(Histo_noTcut19_100,"M_{A'} = 19 MeV","l");
  legend_100_noTcut->AddEntry(Histo_noTcut18_100,"M_{A'} = 18 MeV","l");
  legend_100_noTcut->AddEntry(Histo_noTcut16_100,"M_{A'} = 16 MeV","l");
  legend_100_noTcut->AddEntry(Histo_noTcut14_100,"M_{A'} = 14 MeV","l");
  legend_100_noTcut->AddEntry(Histo_noTcut12_100,"M_{A'} = 12 MeV","l");
  legend_100_noTcut->AddEntry(Histo_noTcut10_100,"M_{A'} = 10 MeV","l");
  legend_100_noTcut->AddEntry(Histo_noTcut8_100,"M_{A'} = 8 MeV","l");
  legend_100_noTcut->AddEntry(Histo_noTcut6_100,"M_{A'} = 6 MeV","l");
  legend_100_noTcut->AddEntry(Histo_noTcut4_100,"M_{A'} = 4 MeV","l");
  legend_100_noTcut->AddEntry(Histo_noTcut2_100,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_100_noTcut->Draw();

  outRoot->WriteObject(c100_noTcut,"100e_all_noTcut");

  
  if(!(Histo_noTcut20_1000->GetSumw2N() > 0)) Histo_noTcut20_1000->Sumw2(kTRUE);
  Histo_noTcut20_1000->Add(Histo_noTcut0_1000_30,-1);
  if(!(Histo_noTcut19_1000->GetSumw2N() > 0)) Histo_noTcut19_1000->Sumw2(kTRUE);
  Histo_noTcut19_1000->Add(Histo_noTcut0_1000_30,-1);
  if(!(Histo_noTcut18_1000->GetSumw2N() > 0)) Histo_noTcut18_1000->Sumw2(kTRUE);
  Histo_noTcut18_1000->Add(Histo_noTcut0_1000_50,-1);
  if(!(Histo_noTcut16_1000->GetSumw2N() > 0)) Histo_noTcut16_1000->Sumw2(kTRUE);
  Histo_noTcut16_1000->Add(Histo_noTcut0_1000_60,-1);
  if(!(Histo_noTcut14_1000->GetSumw2N() > 0)) Histo_noTcut14_1000->Sumw2(kTRUE);
  Histo_noTcut14_1000->Add(Histo_noTcut0_1000_60,-1);
  if(!(Histo_noTcut12_1000->GetSumw2N() > 0)) Histo_noTcut12_1000->Sumw2(kTRUE);
  Histo_noTcut12_1000->Add(Histo_noTcut0_1000_60,-1);
  if(!(Histo_noTcut10_1000->GetSumw2N() > 0)) Histo_noTcut10_1000->Sumw2(kTRUE);
  Histo_noTcut10_1000->Add(Histo_noTcut0_1000_60,-1);
  if(!(Histo_noTcut8_1000->GetSumw2N() > 0)) Histo_noTcut8_1000->Sumw2(kTRUE);
  Histo_noTcut8_1000->Add(Histo_noTcut0_1000_60,-1);
  if(!(Histo_noTcut6_1000->GetSumw2N() > 0)) Histo_noTcut6_1000->Sumw2(kTRUE);
  Histo_noTcut6_1000->Add(Histo_noTcut0_1000_60,-1);
  if(!(Histo_noTcut4_1000->GetSumw2N() > 0)) Histo_noTcut4_1000->Sumw2(kTRUE);
  Histo_noTcut4_1000->Add(Histo_noTcut0_1000_60,-1);
  if(!(Histo_noTcut2_1000->GetSumw2N() > 0)) Histo_noTcut2_1000->Sumw2(kTRUE);
  Histo_noTcut2_1000->Add(Histo_noTcut0_1000_60,-1);


  Histo_noTcut2_1000->Fit(f_2,"R");
  means_noTcut_1000[0]=f_2->GetParameter(1);
  sigmas_noTcut_1000[0]=f_2->GetParameter(2);

  Histo_noTcut4_1000->Fit(f_4,"R");
  means_noTcut_1000[1]=f_4->GetParameter(1);
  sigmas_noTcut_1000[1]=f_4->GetParameter(2);
  
  Histo_noTcut6_1000->Fit(f_6,"R");
  means_noTcut_1000[2]=f_6->GetParameter(1);
  sigmas_noTcut_1000[2]=f_6->GetParameter(2);
  
  Histo_noTcut8_1000->Fit(f_8,"R");
  means_noTcut_1000[3]=f_8->GetParameter(1);
  sigmas_noTcut_1000[3]=f_8->GetParameter(2);
 
  Histo_noTcut10_1000->Fit(f_10,"R");
  means_noTcut_1000[4]=f_10->GetParameter(1);
  sigmas_noTcut_1000[4]=f_10->GetParameter(2);
  
  Histo_noTcut12_1000->Fit(f_12,"R");
  means_noTcut_1000[5]=f_12->GetParameter(1);
  sigmas_noTcut_1000[5]=f_12->GetParameter(2);
  
  Histo_noTcut14_1000->Fit(f_14,"R");
  means_noTcut_1000[6]=f_14->GetParameter(1);
  sigmas_noTcut_1000[6]=f_14->GetParameter(2);
  
  Histo_noTcut16_1000->Fit(f_16,"R");
  means_noTcut_1000[7]=f_16->GetParameter(1);
  sigmas_noTcut_1000[7]=f_16->GetParameter(2);
  
  Histo_noTcut18_1000->Fit(f_18,"R");
  means_noTcut_1000[8]=f_18->GetParameter(1);
  sigmas_noTcut_1000[8]=f_18->GetParameter(2);
  
  Histo_noTcut19_1000->Fit(f_19,"R");
  means_noTcut_1000[9]=f_19->GetParameter(1);
  sigmas_noTcut_1000[9]=f_19->GetParameter(2);
  
  Histo_noTcut20_1000->Fit(f_20,"R");
  means_noTcut_1000[10]=f_20->GetParameter(1);
  sigmas_noTcut_1000[10]=f_20->GetParameter(2);

  entries_noTcut_1000[0]=Histo_noTcut2_1000->Integral(500+(means_noTcut_1000[0]-nsigma*sigmas_noTcut_1000[0]),500+(means_noTcut_1000[0]+nsigma*sigmas_noTcut_1000[0]))/1000;
  entries_noTcut_1000[1]=Histo_noTcut4_1000->Integral(500+(means_noTcut_1000[1]-nsigma*sigmas_noTcut_1000[1]),500+(means_noTcut_1000[1]+nsigma*sigmas_noTcut_1000[1]))/1000;
  entries_noTcut_1000[2]=Histo_noTcut6_1000->Integral(500+(means_noTcut_1000[2]-nsigma*sigmas_noTcut_1000[2]),500+(means_noTcut_1000[2]+nsigma*sigmas_noTcut_1000[2]))/1000;
  entries_noTcut_1000[3]=Histo_noTcut8_1000->Integral(500+(means_noTcut_1000[3]-nsigma*sigmas_noTcut_1000[3]),500+(means_noTcut_1000[3]+nsigma*sigmas_noTcut_1000[3]))/1000;
  entries_noTcut_1000[4]=Histo_noTcut10_1000->Integral(500+(means_noTcut_1000[4]-nsigma*sigmas_noTcut_1000[4]),500+(means_noTcut_1000[4]+nsigma*sigmas_noTcut_1000[4]))/1000;
  entries_noTcut_1000[5]=Histo_noTcut12_1000->Integral(500+(means_noTcut_1000[5]-nsigma*sigmas_noTcut_1000[5]),500+(means_noTcut_1000[5]+nsigma*sigmas_noTcut_1000[5]))/1000;
  entries_noTcut_1000[6]=Histo_noTcut14_1000->Integral(500+(means_noTcut_1000[6]-nsigma*sigmas_noTcut_1000[6]),500+(means_noTcut_1000[6]+nsigma*sigmas_noTcut_1000[6]))/1000;
  entries_noTcut_1000[7]=Histo_noTcut16_1000->Integral(500+(means_noTcut_1000[7]-nsigma*sigmas_noTcut_1000[7]),500+(means_noTcut_1000[7]+nsigma*sigmas_noTcut_1000[7]))/1000;
  entries_noTcut_1000[8]=Histo_noTcut18_1000->Integral(500+(means_noTcut_1000[8]-nsigma*sigmas_noTcut_1000[8]),500+(means_noTcut_1000[8]+nsigma*sigmas_noTcut_1000[8]))/1000;
  entries_noTcut_1000[9]=Histo_noTcut19_1000->Integral(500+(means_noTcut_1000[9]-nsigma*sigmas_noTcut_1000[9]),500+(means_noTcut_1000[9]+nsigma*sigmas_noTcut_1000[9]))/1000;
  entries_noTcut_1000[10]=Histo_noTcut20_1000->Integral(500+(means_noTcut_1000[10]-nsigma*sigmas_noTcut_1000[10]),500+(means_noTcut_1000[10]+nsigma*sigmas_noTcut_1000[10]))/1000;

  TCanvas *c1000_noTcut = new TCanvas();
  c1000_noTcut->cd();
  Histo_noTcut20_1000->SetLineColorAlpha(kBlue+3,0.65);
  Histo_noTcut20_1000->SetTitle( "M^{2}_{miss} for different A' masses (1000 e+ per bunch)");
  Histo_noTcut20_1000->SetLineWidth(3);
  Histo_noTcut20_1000->GetYaxis()->SetTitle("N_{events}");
  Histo_noTcut20_1000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_noTcut19_1000->SetLineColorAlpha(kBlue,0.65);
  Histo_noTcut19_1000->SetLineWidth(3);
  Histo_noTcut18_1000->SetLineColorAlpha(kCyan+3,0.65);
  Histo_noTcut18_1000->SetLineWidth(3);
  Histo_noTcut16_1000->SetLineColorAlpha(kCyan+1,0.65);
  Histo_noTcut16_1000->SetLineWidth(3);
  Histo_noTcut14_1000->SetLineColorAlpha(kGreen+3,0.65);
  Histo_noTcut14_1000->SetLineWidth(3);
  Histo_noTcut12_1000->SetLineColorAlpha(kGreen+1,0.65);
  Histo_noTcut12_1000->SetLineWidth(3);
  Histo_noTcut10_1000->SetLineColorAlpha(kYellow+2,0.65);
  Histo_noTcut10_1000->SetLineWidth(3);
  Histo_noTcut8_1000->SetLineColorAlpha(kOrange-3,0.65);
  Histo_noTcut8_1000->SetLineWidth(3);
  Histo_noTcut6_1000->SetLineColorAlpha(kRed+2,0.65);
  Histo_noTcut6_1000->SetLineWidth(3);
  Histo_noTcut4_1000->SetLineColorAlpha(kRed,0.65);
  Histo_noTcut4_1000->SetLineWidth(3);
  Histo_noTcut2_1000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_noTcut2_1000->SetLineWidth(3);
    
  Histo_noTcut20_1000->Draw();
  Histo_noTcut19_1000->Draw("same");
  Histo_noTcut18_1000->Draw("same");
  Histo_noTcut4_1000->Draw("same");
  Histo_noTcut6_1000->Draw("same");
  Histo_noTcut8_1000->Draw("same");
  Histo_noTcut10_1000->Draw("same");
  Histo_noTcut12_1000->Draw("same");
  Histo_noTcut14_1000->Draw("same");
  Histo_noTcut16_1000->Draw("same");
  Histo_noTcut2_1000->Draw("same");

  auto legend_1000_noTcut = new TLegend(0.15,0.5,0.3,0.85);
  legend_1000_noTcut->AddEntry(Histo_noTcut20_1000,"M_{A'} = 20 MeV","l");
  legend_1000_noTcut->AddEntry(Histo_noTcut19_1000,"M_{A'} = 19 MeV","l");
  legend_1000_noTcut->AddEntry(Histo_noTcut18_1000,"M_{A'} = 18 MeV","l");
  legend_1000_noTcut->AddEntry(Histo_noTcut16_1000,"M_{A'} = 16 MeV","l");
  legend_1000_noTcut->AddEntry(Histo_noTcut14_1000,"M_{A'} = 14 MeV","l");
  legend_1000_noTcut->AddEntry(Histo_noTcut12_1000,"M_{A'} = 12 MeV","l");
  legend_1000_noTcut->AddEntry(Histo_noTcut10_1000,"M_{A'} = 10 MeV","l");
  legend_1000_noTcut->AddEntry(Histo_noTcut8_1000,"M_{A'} = 8 MeV","l");
  legend_1000_noTcut->AddEntry(Histo_noTcut6_1000,"M_{A'} = 6 MeV","l");
  legend_1000_noTcut->AddEntry(Histo_noTcut4_1000,"M_{A'} = 4 MeV","l");
  legend_1000_noTcut->AddEntry(Histo_noTcut2_1000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_1000_noTcut->Draw();

  outRoot->WriteObject(c1000_noTcut,"1000e_all_noTcut");

  
  if(!(Histo_noTcut20_10000->GetSumw2N() > 0)) Histo_noTcut20_10000->Sumw2(kTRUE);
  Histo_noTcut20_10000->Add(Histo_noTcut0_10000_30,-1);
  if(!(Histo_noTcut19_10000->GetSumw2N() > 0)) Histo_noTcut19_10000->Sumw2(kTRUE);
  Histo_noTcut19_10000->Add(Histo_noTcut0_10000_30,-1);
  if(!(Histo_noTcut18_10000->GetSumw2N() > 0)) Histo_noTcut18_10000->Sumw2(kTRUE);
  Histo_noTcut18_10000->Add(Histo_noTcut0_10000_50,-1);
  if(!(Histo_noTcut16_10000->GetSumw2N() > 0)) Histo_noTcut16_10000->Sumw2(kTRUE);
  Histo_noTcut16_10000->Add(Histo_noTcut0_10000_60,-1);
  if(!(Histo_noTcut14_10000->GetSumw2N() > 0)) Histo_noTcut14_10000->Sumw2(kTRUE);
  Histo_noTcut14_10000->Add(Histo_noTcut0_10000_60,-1);
  if(!(Histo_noTcut12_10000->GetSumw2N() > 0)) Histo_noTcut12_10000->Sumw2(kTRUE);
  Histo_noTcut12_10000->Add(Histo_noTcut0_10000_60,-1);
  if(!(Histo_noTcut10_10000->GetSumw2N() > 0)) Histo_noTcut10_10000->Sumw2(kTRUE);
  Histo_noTcut10_10000->Add(Histo_noTcut0_10000_60,-1);
  if(!(Histo_noTcut8_10000->GetSumw2N() > 0)) Histo_noTcut8_10000->Sumw2(kTRUE);
  Histo_noTcut8_10000->Add(Histo_noTcut0_10000_60,-1);
  if(!(Histo_noTcut6_10000->GetSumw2N() > 0)) Histo_noTcut6_10000->Sumw2(kTRUE);
  Histo_noTcut6_10000->Add(Histo_noTcut0_10000_60,-1);
  if(!(Histo_noTcut4_10000->GetSumw2N() > 0)) Histo_noTcut4_10000->Sumw2(kTRUE);
  Histo_noTcut4_10000->Add(Histo_noTcut0_10000_60,-1);
  if(!(Histo_noTcut2_10000->GetSumw2N() > 0)) Histo_noTcut2_10000->Sumw2(kTRUE);
  Histo_noTcut2_10000->Add(Histo_noTcut0_10000_60,-1);


  Histo_noTcut2_10000->Fit(f_2,"R");
  means_noTcut_10000[0]=f_2->GetParameter(1);
  sigmas_noTcut_10000[0]=f_2->GetParameter(2);

  Histo_noTcut4_10000->Fit(f_4,"R");
  means_noTcut_10000[1]=f_4->GetParameter(1);
  sigmas_noTcut_10000[1]=f_4->GetParameter(2);
  
  Histo_noTcut6_10000->Fit(f_6,"R");
  means_noTcut_10000[2]=f_6->GetParameter(1);
  sigmas_noTcut_10000[2]=f_6->GetParameter(2);
  
  Histo_noTcut8_10000->Fit(f_8,"R");
  means_noTcut_10000[3]=f_8->GetParameter(1);
  sigmas_noTcut_10000[3]=f_8->GetParameter(2);
 
  Histo_noTcut10_10000->Fit(f_10,"R");
  means_noTcut_10000[4]=f_10->GetParameter(1);
  sigmas_noTcut_10000[4]=f_10->GetParameter(2);
  
  Histo_noTcut12_10000->Fit(f_12,"R");
  means_noTcut_10000[5]=f_12->GetParameter(1);
  sigmas_noTcut_10000[5]=f_12->GetParameter(2);
  
  Histo_noTcut14_10000->Fit(f_14,"R");
  means_noTcut_10000[6]=f_14->GetParameter(1);
  sigmas_noTcut_10000[6]=f_14->GetParameter(2);
  
  Histo_noTcut16_10000->Fit(f_16,"R");
  means_noTcut_10000[7]=f_16->GetParameter(1);
  sigmas_noTcut_10000[7]=f_16->GetParameter(2);
  
  Histo_noTcut18_10000->Fit(f_18,"R");
  means_noTcut_10000[8]=f_18->GetParameter(1);
  sigmas_noTcut_10000[8]=f_18->GetParameter(2);
  
  Histo_noTcut19_10000->Fit(f_19,"R");
  means_noTcut_10000[9]=f_19->GetParameter(1);
  sigmas_noTcut_10000[9]=f_19->GetParameter(2);
  
  Histo_noTcut20_10000->Fit(f_20,"R");
  means_noTcut_10000[10]=f_20->GetParameter(1);
  sigmas_noTcut_10000[10]=f_20->GetParameter(2);

  entries_noTcut_10000[0]=Histo_noTcut2_10000->Integral(500+(means_noTcut_10000[0]-nsigma*sigmas_noTcut_10000[0]),500+(means_noTcut_10000[0]+nsigma*sigmas_noTcut_10000[0]))/1000;
  entries_noTcut_10000[1]=Histo_noTcut4_10000->Integral(500+(means_noTcut_10000[1]-nsigma*sigmas_noTcut_10000[1]),500+(means_noTcut_10000[1]+nsigma*sigmas_noTcut_10000[1]))/1000;
  entries_noTcut_10000[2]=Histo_noTcut6_10000->Integral(500+(means_noTcut_10000[2]-nsigma*sigmas_noTcut_10000[2]),500+(means_noTcut_10000[2]+nsigma*sigmas_noTcut_10000[2]))/1000;
  entries_noTcut_10000[3]=Histo_noTcut8_10000->Integral(500+(means_noTcut_10000[3]-nsigma*sigmas_noTcut_10000[3]),500+(means_noTcut_10000[3]+nsigma*sigmas_noTcut_10000[3]))/1000;
  entries_noTcut_10000[4]=Histo_noTcut10_10000->Integral(500+(means_noTcut_10000[4]-nsigma*sigmas_noTcut_10000[4]),500+(means_noTcut_10000[4]+nsigma*sigmas_noTcut_10000[4]))/1000;
  entries_noTcut_10000[5]=Histo_noTcut12_10000->Integral(500+(means_noTcut_10000[5]-nsigma*sigmas_noTcut_10000[5]),500+(means_noTcut_10000[5]+nsigma*sigmas_noTcut_10000[5]))/1000;
  entries_noTcut_10000[6]=Histo_noTcut14_10000->Integral(500+(means_noTcut_10000[6]-nsigma*sigmas_noTcut_10000[6]),500+(means_noTcut_10000[6]+nsigma*sigmas_noTcut_10000[6]))/1000;
  entries_noTcut_10000[7]=Histo_noTcut16_10000->Integral(500+(means_noTcut_10000[7]-nsigma*sigmas_noTcut_10000[7]),500+(means_noTcut_10000[7]+nsigma*sigmas_noTcut_10000[7]))/1000;
  entries_noTcut_10000[8]=Histo_noTcut18_10000->Integral(500+(means_noTcut_10000[8]-nsigma*sigmas_noTcut_10000[8]),500+(means_noTcut_10000[8]+nsigma*sigmas_noTcut_10000[8]))/1000;
  entries_noTcut_10000[9]=Histo_noTcut19_10000->Integral(500+(means_noTcut_10000[9]-nsigma*sigmas_noTcut_10000[9]),500+(means_noTcut_10000[9]+nsigma*sigmas_noTcut_10000[9]))/1000;
  entries_noTcut_10000[10]=Histo_noTcut20_10000->Integral(500+(means_noTcut_10000[10]-nsigma*sigmas_noTcut_10000[10]),500+(means_noTcut_10000[10]+nsigma*sigmas_noTcut_10000[10]))/1000;


  TCanvas *c10000_noTcut = new TCanvas();
  c10000_noTcut->cd();
  Histo_noTcut20_10000->SetLineColorAlpha(kBlue+3,0.65);
  Histo_noTcut20_10000->SetTitle( "M^{2}_{miss} for different A' masses (10000 e+ per bunch)");
  Histo_noTcut20_10000->SetLineWidth(3);
  Histo_noTcut20_10000->GetYaxis()->SetTitle("N_{events}");
  Histo_noTcut20_10000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_noTcut19_10000->SetLineColorAlpha(kBlue,0.65);
  Histo_noTcut19_10000->SetLineWidth(3);
  Histo_noTcut18_10000->SetLineColorAlpha(kCyan+3,0.65);
  Histo_noTcut18_10000->SetLineWidth(3);
  Histo_noTcut16_10000->SetLineColorAlpha(kCyan+1,0.65);
  Histo_noTcut16_10000->SetLineWidth(3);
  Histo_noTcut14_10000->SetLineColorAlpha(kGreen+3,0.65);
  Histo_noTcut14_10000->SetLineWidth(3);
  Histo_noTcut12_10000->SetLineColorAlpha(kGreen+1,0.65);
  Histo_noTcut12_10000->SetLineWidth(3);
  Histo_noTcut10_10000->SetLineColorAlpha(kYellow+2,0.65);
  Histo_noTcut10_10000->SetLineWidth(3);
  Histo_noTcut8_10000->SetLineColorAlpha(kOrange-3,0.65);
  Histo_noTcut8_10000->SetLineWidth(3);
  Histo_noTcut6_10000->SetLineColorAlpha(kRed+2,0.65);
  Histo_noTcut6_10000->SetLineWidth(3);
  Histo_noTcut4_10000->SetLineColorAlpha(kRed,0.65);
  Histo_noTcut4_10000->SetLineWidth(3);
  Histo_noTcut2_10000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_noTcut2_10000->SetLineWidth(3);
    
  Histo_noTcut20_10000->Draw();
  Histo_noTcut19_10000->Draw("same");
  Histo_noTcut18_10000->Draw("same");
  Histo_noTcut4_10000->Draw("same");
  Histo_noTcut6_10000->Draw("same");
  Histo_noTcut8_10000->Draw("same");
  Histo_noTcut10_10000->Draw("same");
  Histo_noTcut12_10000->Draw("same");
  Histo_noTcut14_10000->Draw("same");
  Histo_noTcut16_10000->Draw("same");
  Histo_noTcut2_10000->Draw("same");

  auto legend_10000_noTcut = new TLegend(0.15,0.5,0.3,0.85);
  legend_10000_noTcut->AddEntry(Histo_noTcut20_10000,"M_{A'} = 20 MeV","l");
  legend_10000_noTcut->AddEntry(Histo_noTcut19_10000,"M_{A'} = 19 MeV","l");
  legend_10000_noTcut->AddEntry(Histo_noTcut18_10000,"M_{A'} = 18 MeV","l");
  legend_10000_noTcut->AddEntry(Histo_noTcut16_10000,"M_{A'} = 16 MeV","l");
  legend_10000_noTcut->AddEntry(Histo_noTcut14_10000,"M_{A'} = 14 MeV","l");
  legend_10000_noTcut->AddEntry(Histo_noTcut12_10000,"M_{A'} = 12 MeV","l");
  legend_10000_noTcut->AddEntry(Histo_noTcut10_10000,"M_{A'} = 10 MeV","l");
  legend_10000_noTcut->AddEntry(Histo_noTcut8_10000,"M_{A'} = 8 MeV","l");
  legend_10000_noTcut->AddEntry(Histo_noTcut6_10000,"M_{A'} = 6 MeV","l");
  legend_10000_noTcut->AddEntry(Histo_noTcut4_10000,"M_{A'} = 4 MeV","l");
  legend_10000_noTcut->AddEntry(Histo_noTcut2_10000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_10000_noTcut->Draw();

  outRoot->WriteObject(c10000_noTcut,"10000e_all_noTcut");

  
  if(!(Histo_noTcut20_25000->GetSumw2N() > 0)) Histo_noTcut20_25000->Sumw2(kTRUE);
  Histo_noTcut20_25000->Add(Histo_noTcut0_25000_30,-1);
  if(!(Histo_noTcut19_25000->GetSumw2N() > 0)) Histo_noTcut19_25000->Sumw2(kTRUE);
  Histo_noTcut19_25000->Add(Histo_noTcut0_25000_30,-1);
  if(!(Histo_noTcut18_25000->GetSumw2N() > 0)) Histo_noTcut18_25000->Sumw2(kTRUE);
  Histo_noTcut18_25000->Add(Histo_noTcut0_25000_50,-1);
  if(!(Histo_noTcut16_25000->GetSumw2N() > 0)) Histo_noTcut16_25000->Sumw2(kTRUE);
  Histo_noTcut16_25000->Add(Histo_noTcut0_25000_60,-1);
  if(!(Histo_noTcut14_25000->GetSumw2N() > 0)) Histo_noTcut14_25000->Sumw2(kTRUE);
  Histo_noTcut14_25000->Add(Histo_noTcut0_25000_60,-1);
  if(!(Histo_noTcut12_25000->GetSumw2N() > 0)) Histo_noTcut12_25000->Sumw2(kTRUE);
  Histo_noTcut12_25000->Add(Histo_noTcut0_25000_60,-1);
  if(!(Histo_noTcut10_25000->GetSumw2N() > 0)) Histo_noTcut10_25000->Sumw2(kTRUE);
  Histo_noTcut10_25000->Add(Histo_noTcut0_25000_60,-1);
  if(!(Histo_noTcut8_25000->GetSumw2N() > 0)) Histo_noTcut8_25000->Sumw2(kTRUE);
  Histo_noTcut8_25000->Add(Histo_noTcut0_25000_60,-1);
  if(!(Histo_noTcut6_25000->GetSumw2N() > 0)) Histo_noTcut6_25000->Sumw2(kTRUE);
  Histo_noTcut6_25000->Add(Histo_noTcut0_25000_60,-1);
  if(!(Histo_noTcut4_25000->GetSumw2N() > 0)) Histo_noTcut4_25000->Sumw2(kTRUE);
  Histo_noTcut4_25000->Add(Histo_noTcut0_25000_60,-1);
  if(!(Histo_noTcut2_25000->GetSumw2N() > 0)) Histo_noTcut2_25000->Sumw2(kTRUE);
  Histo_noTcut2_25000->Add(Histo_noTcut0_25000_60,-1);

  Histo_noTcut2_25000->Fit(f_2,"R");
  means_noTcut_25000[0]=f_2->GetParameter(1);
  sigmas_noTcut_25000[0]=f_2->GetParameter(2);

  Histo_noTcut4_25000->Fit(f_4,"R");
  means_noTcut_25000[1]=f_4->GetParameter(1);
  sigmas_noTcut_25000[1]=f_4->GetParameter(2);
  
  Histo_noTcut6_25000->Fit(f_6,"R");
  means_noTcut_25000[2]=f_6->GetParameter(1);
  sigmas_noTcut_25000[2]=f_6->GetParameter(2);
  
  Histo_noTcut8_25000->Fit(f_8,"R");
  means_noTcut_25000[3]=f_8->GetParameter(1);
  sigmas_noTcut_25000[3]=f_8->GetParameter(2);
 
  Histo_noTcut10_25000->Fit(f_10,"R");
  means_noTcut_25000[4]=f_10->GetParameter(1);
  sigmas_noTcut_25000[4]=f_10->GetParameter(2);
  
  Histo_noTcut12_25000->Fit(f_12,"R");
  means_noTcut_25000[5]=f_12->GetParameter(1);
  sigmas_noTcut_25000[5]=f_12->GetParameter(2);
  
  Histo_noTcut14_25000->Fit(f_14,"R");
  means_noTcut_25000[6]=f_14->GetParameter(1);
  sigmas_noTcut_25000[6]=f_14->GetParameter(2);
  
  Histo_noTcut16_25000->Fit(f_16,"R");
  means_noTcut_25000[7]=f_16->GetParameter(1);
  sigmas_noTcut_25000[7]=f_16->GetParameter(2);
  
  Histo_noTcut18_25000->Fit(f_18,"R");
  means_noTcut_25000[8]=f_18->GetParameter(1);
  sigmas_noTcut_25000[8]=f_18->GetParameter(2);
  
  Histo_noTcut19_25000->Fit(f_19,"R");
  means_noTcut_25000[9]=f_19->GetParameter(1);
  sigmas_noTcut_25000[9]=f_19->GetParameter(2);
  
  Histo_noTcut20_25000->Fit(f_20,"R");
  means_noTcut_25000[10]=f_20->GetParameter(1);
  sigmas_noTcut_25000[10]=f_20->GetParameter(2);

  entries_noTcut_25000[0]=Histo_noTcut2_25000->Integral(500+(means_noTcut_25000[0]-nsigma*sigmas_noTcut_25000[0]),500+(means_noTcut_25000[0]+nsigma*sigmas_noTcut_25000[0]))/1000;
  entries_noTcut_25000[1]=Histo_noTcut4_25000->Integral(500+(means_noTcut_25000[1]-nsigma*sigmas_noTcut_25000[1]),500+(means_noTcut_25000[1]+nsigma*sigmas_noTcut_25000[1]))/1000;
  entries_noTcut_25000[2]=Histo_noTcut6_25000->Integral(500+(means_noTcut_25000[2]-nsigma*sigmas_noTcut_25000[2]),500+(means_noTcut_25000[2]+nsigma*sigmas_noTcut_25000[2]))/1000;
  entries_noTcut_25000[3]=Histo_noTcut8_25000->Integral(500+(means_noTcut_25000[3]-nsigma*sigmas_noTcut_25000[3]),500+(means_noTcut_25000[3]+nsigma*sigmas_noTcut_25000[3]))/1000;
  entries_noTcut_25000[4]=Histo_noTcut10_25000->Integral(500+(means_noTcut_25000[4]-nsigma*sigmas_noTcut_25000[4]),500+(means_noTcut_25000[4]+nsigma*sigmas_noTcut_25000[4]))/1000;
  entries_noTcut_25000[5]=Histo_noTcut12_25000->Integral(500+(means_noTcut_25000[5]-nsigma*sigmas_noTcut_25000[5]),500+(means_noTcut_25000[5]+nsigma*sigmas_noTcut_25000[5]))/1000;
  entries_noTcut_25000[6]=Histo_noTcut14_25000->Integral(500+(means_noTcut_25000[6]-nsigma*sigmas_noTcut_25000[6]),500+(means_noTcut_25000[6]+nsigma*sigmas_noTcut_25000[6]))/1000;
  entries_noTcut_25000[7]=Histo_noTcut16_25000->Integral(500+(means_noTcut_25000[7]-nsigma*sigmas_noTcut_25000[7]),500+(means_noTcut_25000[7]+nsigma*sigmas_noTcut_25000[7]))/1000;
  entries_noTcut_25000[8]=Histo_noTcut18_25000->Integral(500+(means_noTcut_25000[8]-nsigma*sigmas_noTcut_25000[8]),500+(means_noTcut_25000[8]+nsigma*sigmas_noTcut_25000[8]))/1000;
  entries_noTcut_25000[9]=Histo_noTcut19_25000->Integral(500+(means_noTcut_25000[9]-nsigma*sigmas_noTcut_25000[9]),500+(means_noTcut_25000[9]+nsigma*sigmas_noTcut_25000[9]))/1000;
  entries_noTcut_25000[10]=Histo_noTcut20_25000->Integral(500+(means_noTcut_25000[10]-nsigma*sigmas_noTcut_25000[10]),500+(means_noTcut_25000[10]+nsigma*sigmas_noTcut_25000[10]))/1000;

  TCanvas *c25000_noTcut = new TCanvas();
  c25000_noTcut->cd();
  Histo_noTcut20_25000->SetLineColorAlpha(kBlue+3,0.65);
  Histo_noTcut20_25000->SetTitle( "M^{2}_{miss} for different A' masses (25000 e+ per bunch)");
  Histo_noTcut20_25000->SetLineWidth(3);
  Histo_noTcut20_25000->GetYaxis()->SetTitle("N_{events}");
  Histo_noTcut20_25000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_noTcut19_25000->SetLineColorAlpha(kBlue,0.65);
  Histo_noTcut19_25000->SetLineWidth(3);
  Histo_noTcut18_25000->SetLineColorAlpha(kCyan+3,0.65);
  Histo_noTcut18_25000->SetLineWidth(3);
  Histo_noTcut16_25000->SetLineColorAlpha(kCyan+1,0.65);
  Histo_noTcut16_25000->SetLineWidth(3);
  Histo_noTcut14_25000->SetLineColorAlpha(kGreen+3,0.65);
  Histo_noTcut14_25000->SetLineWidth(3);
  Histo_noTcut12_25000->SetLineColorAlpha(kGreen+1,0.65);
  Histo_noTcut12_25000->SetLineWidth(3);
  Histo_noTcut10_25000->SetLineColorAlpha(kYellow+2,0.65);
  Histo_noTcut10_25000->SetLineWidth(3);
  Histo_noTcut8_25000->SetLineColorAlpha(kOrange-3,0.65);
  Histo_noTcut8_25000->SetLineWidth(3);
  Histo_noTcut6_25000->SetLineColorAlpha(kRed+2,0.65);
  Histo_noTcut6_25000->SetLineWidth(3);
  Histo_noTcut4_25000->SetLineColorAlpha(kRed,0.65);
  Histo_noTcut4_25000->SetLineWidth(3);
  Histo_noTcut2_25000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_noTcut2_25000->SetLineWidth(3);
    
  Histo_noTcut20_25000->Draw();
  Histo_noTcut19_25000->Draw("same");
  Histo_noTcut18_25000->Draw("same");
  Histo_noTcut4_25000->Draw("same");
  Histo_noTcut6_25000->Draw("same");
  Histo_noTcut8_25000->Draw("same");
  Histo_noTcut10_25000->Draw("same");
  Histo_noTcut12_25000->Draw("same");
  Histo_noTcut14_25000->Draw("same");
  Histo_noTcut16_25000->Draw("same");
  Histo_noTcut2_25000->Draw("same");

  auto legend_25000_noTcut = new TLegend(0.15,0.5,0.3,0.85);
  legend_25000_noTcut->AddEntry(Histo_noTcut20_25000,"M_{A'} = 20 MeV","l");
  legend_25000_noTcut->AddEntry(Histo_noTcut19_25000,"M_{A'} = 19 MeV","l");
  legend_25000_noTcut->AddEntry(Histo_noTcut18_25000,"M_{A'} = 18 MeV","l");
  legend_25000_noTcut->AddEntry(Histo_noTcut16_25000,"M_{A'} = 16 MeV","l");
  legend_25000_noTcut->AddEntry(Histo_noTcut14_25000,"M_{A'} = 14 MeV","l");
  legend_25000_noTcut->AddEntry(Histo_noTcut12_25000,"M_{A'} = 12 MeV","l");
  legend_25000_noTcut->AddEntry(Histo_noTcut10_25000,"M_{A'} = 10 MeV","l");
  legend_25000_noTcut->AddEntry(Histo_noTcut8_25000,"M_{A'} = 8 MeV","l");
  legend_25000_noTcut->AddEntry(Histo_noTcut6_25000,"M_{A'} = 6 MeV","l");
  legend_25000_noTcut->AddEntry(Histo_noTcut4_25000,"M_{A'} = 4 MeV","l");
  legend_25000_noTcut->AddEntry(Histo_noTcut2_25000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_25000_noTcut->Draw();

  outRoot->WriteObject(c25000_noTcut,"25000e_all_noTcut");

  
  TCanvas *cgr_noTcut = new TCanvas();
 
  auto gr_1_noTcut = new TGraph (11,masses,entries_noTcut_1);
  gr_1_noTcut->SetLineColor(kMagenta);
  gr_1_noTcut->SetLineWidth(3); 
  gr_1_noTcut->GetYaxis()->SetTitle("Acceptance [%]");
  gr_1_noTcut->GetXaxis()->SetTitle("M_{A'} [MeV]");
  gr_1_noTcut->SetTitle("Acceptance");
  gr_1_noTcut->Draw();
  auto gr_10_noTcut = new TGraph (11,masses,entries_noTcut_10);
  gr_10_noTcut->SetLineColor(kRed);
  gr_10_noTcut->SetLineWidth(3);
  gr_10_noTcut->Draw("same");
  auto gr_100_noTcut = new TGraph (11,masses,entries_noTcut_100);
  gr_100_noTcut->SetLineColor(kOrange);
  gr_100_noTcut->SetLineWidth(3);
  gr_100_noTcut->Draw("same");
  auto gr_1000_noTcut = new TGraph (11,masses,entries_noTcut_1000);
  gr_1000_noTcut->SetLineColor(kGreen+3);
  gr_1000_noTcut->SetLineWidth(3);
  gr_1000_noTcut->Draw("same");
  auto gr_10000_noTcut = new TGraph (11,masses,entries_noTcut_10000);
  gr_10000_noTcut->SetLineColor(kCyan+1);
  gr_10000_noTcut->SetLineWidth(3);
  gr_10000_noTcut->Draw("same");
  auto gr_25000_noTcut = new TGraph (11,masses,entries_noTcut_25000);
  gr_25000_noTcut->SetLineColor(kBlue);
  gr_25000_noTcut->SetLineWidth(3);
  gr_25000_noTcut->Draw("same");
  auto legend_gr_noTcut = new TLegend(0.15,0.5,0.3,0.85);
  legend_gr_noTcut->AddEntry(gr_1_noTcut,"1 e+","l");
  legend_gr_noTcut->AddEntry(gr_10_noTcut,"10 e+","l");
  legend_gr_noTcut->AddEntry(gr_100_noTcut,"100 e+","l");
  legend_gr_noTcut->AddEntry(gr_1000_noTcut,"1000 e+","l");
  legend_gr_noTcut->AddEntry(gr_10000_noTcut,"10000 e+","l");
  legend_gr_noTcut->AddEntry(gr_25000_noTcut,"25000 e+","l");
  legend_gr_noTcut->Draw();

  TCanvas *csig_noTcut = new TCanvas();
 
  auto sig_1_noTcut = new TGraph (11,masses,sigmas_noTcut_1);
  sig_1_noTcut->SetLineColor(kMagenta);
  sig_1_noTcut->SetLineWidth(3);
  sig_1_noTcut->GetYaxis()->SetTitle("Sigma");
  sig_1_noTcut->GetXaxis()->SetTitle("M_{A'} [MeV]");
  sig_1_noTcut->SetTitle("Sigma");
  sig_1_noTcut->Draw();
  auto sig_10_noTcut = new TGraph (11,masses,sigmas_noTcut_10);
  sig_10_noTcut->SetLineColor(kRed);
  sig_10_noTcut->SetLineWidth(3);
  sig_10_noTcut->Draw("same");
  auto sig_100_noTcut = new TGraph (11,masses,sigmas_noTcut_100);
  sig_100_noTcut->SetLineColor(kOrange);
  sig_100_noTcut->SetLineWidth(3);
  sig_100_noTcut->Draw("same");
  auto sig_1000_noTcut = new TGraph (11,masses,sigmas_noTcut_1000);
  sig_1000_noTcut->SetLineColor(kGreen+3);
  sig_1000_noTcut->SetLineWidth(3);
  sig_1000_noTcut->Draw("same");
  auto sig_10000_noTcut = new TGraph (11,masses,sigmas_noTcut_10000);
  sig_10000_noTcut->SetLineColor(kCyan+1);
  sig_10000_noTcut->SetLineWidth(3);
  sig_10000_noTcut->Draw("same");
  auto sig_25000_noTcut = new TGraph (11,masses,sigmas_noTcut_25000);
  sig_25000_noTcut->SetLineColor(kBlue);
  sig_25000_noTcut->SetLineWidth(3);
  sig_25000_noTcut->Draw("same");
  auto legend_sig_noTcut = new TLegend(0.15,0.5,0.3,0.85);
  legend_sig_noTcut->AddEntry(sig_1_noTcut,"1 e+","l");
  legend_sig_noTcut->AddEntry(sig_10_noTcut,"10 e+","l");
  legend_sig_noTcut->AddEntry(sig_100_noTcut,"100 e+","l");
  legend_sig_noTcut->AddEntry(sig_1000_noTcut,"1000 e+","l");
  legend_sig_noTcut->AddEntry(sig_10000_noTcut,"10000 e+","l");
  legend_sig_noTcut->AddEntry(sig_25000_noTcut,"25000 e+","l");
  legend_sig_noTcut->Draw();

  
  outRoot->WriteObject(cgr_noTcut,"Acceptance_noTcut");
  outRoot->WriteObject(csig_noTcut,"Sigma_noTcut");



 //----------------------------------------Do analysis for deltaT=0.5ns-------------------------------------------------Do analysis for deltaT=0.5ns-------------------------------------------------

  Float_t sigmas_05ns_1[11];
  Float_t means_05ns_1[11];
  Float_t entries_05ns_1[11];

  Float_t sigmas_05ns_10[11];
  Float_t means_05ns_10[11];
  Float_t entries_05ns_10[11];

  Float_t sigmas_05ns_100[11];
  Float_t means_05ns_100[11];
  Float_t entries_05ns_100[11];

  Float_t sigmas_05ns_1000[11];
  Float_t means_05ns_1000[11];
  Float_t entries_05ns_1000[11];

  Float_t sigmas_05ns_10000[11];
  Float_t means_05ns_10000[11];
  Float_t entries_05ns_10000[11];
  
  Float_t sigmas_05ns_25000[11];
  Float_t means_05ns_25000[11];
  Float_t entries_05ns_25000[11];
   
  //---------------------------------------------- Open Histograms --------------------------------------------------

  TH1F *Histo_05ns20_1 = (TH1F*) dir20_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns19_1 = (TH1F*) dir19_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns18_1 = (TH1F*) dir18_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_05ns");
  TH1F *Histo_05ns16_1 = (TH1F*) dir16_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns14_1 = (TH1F*) dir14_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns12_1 = (TH1F*) dir12_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns10_1 = (TH1F*) dir10_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns8_1 = (TH1F*) dir8_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns6_1 = (TH1F*) dir6_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns4_1 = (TH1F*) dir4_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns2_1 = (TH1F*) dir2_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns0_1_10 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_05ns");
  TH1F *Histo_05ns0_1_20 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_05ns");
  TH1F *Histo_05ns0_1_30 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns0_1_40 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_05ns");
  TH1F *Histo_05ns0_1_50 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_05ns");
  TH1F *Histo_05ns0_1_60 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  
  TH1F *Histo_05ns20_10 = (TH1F*) dir20_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns19_10 = (TH1F*) dir19_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns18_10 = (TH1F*) dir18_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_05ns");
  TH1F *Histo_05ns16_10 = (TH1F*) dir16_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns14_10 = (TH1F*) dir14_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns12_10 = (TH1F*) dir12_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns10_10 = (TH1F*) dir10_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns8_10 = (TH1F*) dir8_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns6_10 = (TH1F*) dir6_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns4_10 = (TH1F*) dir4_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns2_10 = (TH1F*) dir2_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns0_10_10 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_05ns");
  TH1F *Histo_05ns0_10_20 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_05ns");
  TH1F *Histo_05ns0_10_30 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns0_10_40 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_05ns");
  TH1F *Histo_05ns0_10_50 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_05ns");
  TH1F *Histo_05ns0_10_60 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  
  TH1F *Histo_05ns20_100 = (TH1F*) dir20_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns19_100 = (TH1F*) dir19_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns18_100 = (TH1F*) dir18_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_05ns");
  TH1F *Histo_05ns16_100 = (TH1F*) dir16_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns14_100 = (TH1F*) dir14_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns12_100 = (TH1F*) dir12_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns10_100 = (TH1F*) dir10_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns8_100 = (TH1F*) dir8_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns6_100 = (TH1F*) dir6_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns4_100 = (TH1F*) dir4_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns2_100 = (TH1F*) dir2_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns0_100_10 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_05ns");
  TH1F *Histo_05ns0_100_20 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_05ns");
  TH1F *Histo_05ns0_100_30 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns0_100_40 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_05ns");
  TH1F *Histo_05ns0_100_50 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_05ns");
  TH1F *Histo_05ns0_100_60 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  
  TH1F *Histo_05ns20_1000 = (TH1F*) dir20_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns19_1000 = (TH1F*) dir19_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns18_1000 = (TH1F*) dir18_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_05ns");
  TH1F *Histo_05ns16_1000 = (TH1F*) dir16_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns14_1000 = (TH1F*) dir14_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns12_1000 = (TH1F*) dir12_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns10_1000 = (TH1F*) dir10_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns8_1000 = (TH1F*) dir8_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns6_1000 = (TH1F*) dir6_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns4_1000 = (TH1F*) dir4_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns2_1000 = (TH1F*) dir2_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns0_1000_10 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_05ns");
  TH1F *Histo_05ns0_1000_20 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_05ns");
  TH1F *Histo_05ns0_1000_30 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns0_1000_40 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_05ns");
  TH1F *Histo_05ns0_1000_50 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_05ns");
  TH1F *Histo_05ns0_1000_60 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  
  TH1F *Histo_05ns20_10000 = (TH1F*) dir20_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns19_10000 = (TH1F*) dir19_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns18_10000 = (TH1F*) dir18_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_05ns");
  TH1F *Histo_05ns16_10000 = (TH1F*) dir16_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns14_10000 = (TH1F*) dir14_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns12_10000 = (TH1F*) dir12_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns10_10000 = (TH1F*) dir10_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns8_10000 = (TH1F*) dir8_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns6_10000 = (TH1F*) dir6_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns4_10000 = (TH1F*) dir4_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns2_10000 = (TH1F*) dir2_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns0_10000_10 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_05ns");
  TH1F *Histo_05ns0_10000_20 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_05ns");
  TH1F *Histo_05ns0_10000_30 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns0_10000_40 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_05ns");
  TH1F *Histo_05ns0_10000_50 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_05ns");
  TH1F *Histo_05ns0_10000_60 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  
  TH1F *Histo_05ns20_25000 = (TH1F*) dir20_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns19_25000 = (TH1F*) dir19_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns18_25000 = (TH1F*) dir18_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_05ns");
  TH1F *Histo_05ns16_25000 = (TH1F*) dir16_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns14_25000 = (TH1F*) dir14_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns12_25000 = (TH1F*) dir12_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns10_25000 = (TH1F*) dir10_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns8_25000 = (TH1F*) dir8_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns6_25000 = (TH1F*) dir6_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns4_25000 = (TH1F*) dir4_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns2_25000 = (TH1F*) dir2_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");
  TH1F *Histo_05ns0_25000_10 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_05ns");
  TH1F *Histo_05ns0_25000_20 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_05ns");
  TH1F *Histo_05ns0_25000_30 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_05ns");
  TH1F *Histo_05ns0_25000_40 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_05ns");
  TH1F *Histo_05ns0_25000_50 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_05ns");
  TH1F *Histo_05ns0_25000_60 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_05ns");

 
  Histo_05ns2_1->Fit(f_2,"R");
  means_05ns_1[0]=f_2->GetParameter(1);
  sigmas_05ns_1[0]=f_2->GetParameter(2);

  Histo_05ns4_1->Fit(f_4,"R");
  means_05ns_1[1]=f_4->GetParameter(1);
  sigmas_05ns_1[1]=f_4->GetParameter(2);
  
  Histo_05ns6_1->Fit(f_6,"R");
  means_05ns_1[2]=f_6->GetParameter(1);
  sigmas_05ns_1[2]=f_6->GetParameter(2);
  
  Histo_05ns8_1->Fit(f_8,"R");
  means_05ns_1[3]=f_8->GetParameter(1);
  sigmas_05ns_1[3]=f_8->GetParameter(2);
  
  Histo_05ns10_1->Fit(f_10,"R");
  means_05ns_1[4]=f_10->GetParameter(1);
  sigmas_05ns_1[4]=f_10->GetParameter(2);
  
  Histo_05ns12_1->Fit(f_12,"R");
  means_05ns_1[5]=f_12->GetParameter(1);
  sigmas_05ns_1[5]=f_12->GetParameter(2);
  
  Histo_05ns14_1->Fit(f_14,"R");
  means_05ns_1[6]=f_14->GetParameter(1);
  sigmas_05ns_1[6]=f_14->GetParameter(2);
  
  Histo_05ns16_1->Fit(f_16,"R");
  means_05ns_1[7]=f_16->GetParameter(1);
  sigmas_05ns_1[7]=f_16->GetParameter(2);
  
  Histo_05ns18_1->Fit(f_18,"R");
  means_05ns_1[8]=f_18->GetParameter(1);
  sigmas_05ns_1[8]=f_18->GetParameter(2);
  
  Histo_05ns19_1->Fit(f_19,"R");
  means_05ns_1[9]=f_19->GetParameter(1);
  sigmas_05ns_1[9]=f_19->GetParameter(2);
  
  Histo_05ns20_1->Fit(f_20,"R");
  means_05ns_1[10]=f_20->GetParameter(1);
  sigmas_05ns_1[10]=f_20->GetParameter(2);
  
  entries_05ns_1[0]=Histo_05ns2_1->Integral(500+(means_05ns_1[0]-nsigma*sigmas_05ns_1[0]),500+(means_05ns_1[0]+nsigma*sigmas_05ns_1[0]))/1000;
  entries_05ns_1[1]=Histo_05ns4_1->Integral(500+(means_05ns_1[1]-nsigma*sigmas_05ns_1[1]),500+(means_05ns_1[1]+nsigma*sigmas_05ns_1[1]))/1000;
  entries_05ns_1[2]=Histo_05ns6_1->Integral(500+(means_05ns_1[2]-nsigma*sigmas_05ns_1[2]),500+(means_05ns_1[2]+nsigma*sigmas_05ns_1[2]))/1000;
  entries_05ns_1[3]=Histo_05ns8_1->Integral(500+(means_05ns_1[3]-nsigma*sigmas_05ns_1[3]),500+(means_05ns_1[3]+nsigma*sigmas_05ns_1[3]))/1000;
  entries_05ns_1[4]=Histo_05ns10_1->Integral(500+(means_05ns_1[4]-nsigma*sigmas_05ns_1[4]),500+(means_05ns_1[4]+nsigma*sigmas_05ns_1[4]))/1000;
  entries_05ns_1[5]=Histo_05ns12_1->Integral(500+(means_05ns_1[5]-nsigma*sigmas_05ns_1[5]),500+(means_05ns_1[5]+nsigma*sigmas_05ns_1[5]))/1000;
  entries_05ns_1[6]=Histo_05ns14_1->Integral(500+(means_05ns_1[6]-nsigma*sigmas_05ns_1[6]),500+(means_05ns_1[6]+nsigma*sigmas_05ns_1[6]))/1000;
  entries_05ns_1[7]=Histo_05ns16_1->Integral(500+(means_05ns_1[7]-nsigma*sigmas_05ns_1[7]),500+(means_05ns_1[7]+nsigma*sigmas_05ns_1[7]))/1000;
  entries_05ns_1[8]=Histo_05ns18_1->Integral(500+(means_05ns_1[8]-nsigma*sigmas_05ns_1[8]),500+(means_05ns_1[8]+nsigma*sigmas_05ns_1[8]))/1000;
  entries_05ns_1[9]=Histo_05ns19_1->Integral(500+(means_05ns_1[9]-nsigma*sigmas_05ns_1[9]),500+(means_05ns_1[9]+nsigma*sigmas_05ns_1[9]))/1000;
  entries_05ns_1[10]=Histo_05ns20_1->Integral(500+(means_05ns_1[10]-nsigma*sigmas_05ns_1[10]),500+(means_05ns_1[10]+nsigma*sigmas_05ns_1[10]))/1000;
  
  TCanvas *c1_05ns = new TCanvas();
  c1_05ns->cd();
  Histo_05ns20_1->SetLineColorAlpha(kBlue+3,0.65);
  Histo_05ns20_1->SetTitle( "M^{2}_{miss} for different A' masses (1 e+ per bunch)");
  Histo_05ns20_1->SetLineWidth(3);
  Histo_05ns20_1->GetYaxis()->SetTitle("N_{events}");
  Histo_05ns20_1->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_05ns19_1->SetLineColorAlpha(kBlue,0.65);
  Histo_05ns19_1->SetLineWidth(3);
  Histo_05ns18_1->SetLineColorAlpha(kCyan+3,0.65);
  Histo_05ns18_1->SetLineWidth(3);
  Histo_05ns16_1->SetLineColorAlpha(kCyan+1,0.65);
  Histo_05ns16_1->SetLineWidth(3);
  Histo_05ns14_1->SetLineColorAlpha(kGreen+3,0.65);
  Histo_05ns14_1->SetLineWidth(3);
  Histo_05ns12_1->SetLineColorAlpha(kGreen+1,0.65);
  Histo_05ns12_1->SetLineWidth(3);
  Histo_05ns10_1->SetLineColorAlpha(kYellow+2,0.65);
  Histo_05ns10_1->SetLineWidth(3);
  Histo_05ns8_1->SetLineColorAlpha(kOrange-3,0.65);
  Histo_05ns8_1->SetLineWidth(3);
  Histo_05ns6_1->SetLineColorAlpha(kRed+2,0.65);
  Histo_05ns6_1->SetLineWidth(3);
  Histo_05ns4_1->SetLineColorAlpha(kRed,0.65);
  Histo_05ns4_1->SetLineWidth(3);
  Histo_05ns2_1->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_05ns2_1->SetLineWidth(3);
    
  Histo_05ns20_1->Draw();
  Histo_05ns19_1->Draw("same");
  Histo_05ns18_1->Draw("same");
  Histo_05ns4_1->Draw("same");
  Histo_05ns6_1->Draw("same");
  Histo_05ns8_1->Draw("same");
  Histo_05ns10_1->Draw("same");
  Histo_05ns12_1->Draw("same");
  Histo_05ns14_1->Draw("same");
  Histo_05ns16_1->Draw("same");
  Histo_05ns2_1->Draw("same");

  auto legend_1_05ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_1_05ns->AddEntry(Histo_05ns20_1,"M_{A'} = 20 MeV","l");
  legend_1_05ns->AddEntry(Histo_05ns19_1,"M_{A'} = 19 MeV","l");
  legend_1_05ns->AddEntry(Histo_05ns18_1,"M_{A'} = 18 MeV","l");
  legend_1_05ns->AddEntry(Histo_05ns16_1,"M_{A'} = 16 MeV","l");
  legend_1_05ns->AddEntry(Histo_05ns14_1,"M_{A'} = 14 MeV","l");
  legend_1_05ns->AddEntry(Histo_05ns12_1,"M_{A'} = 12 MeV","l");
  legend_1_05ns->AddEntry(Histo_05ns10_1,"M_{A'} = 10 MeV","l");
  legend_1_05ns->AddEntry(Histo_05ns8_1,"M_{A'} = 8 MeV","l");
  legend_1_05ns->AddEntry(Histo_05ns6_1,"M_{A'} = 6 MeV","l");
  legend_1_05ns->AddEntry(Histo_05ns4_1,"M_{A'} = 4 MeV","l");
  legend_1_05ns->AddEntry(Histo_05ns2_1,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_1_05ns->Draw();

  outRoot->WriteObject(c1_05ns,"1e_all_05ns");

  if(!(Histo_05ns20_10->GetSumw2N() > 0)) Histo_05ns20_10->Sumw2(kTRUE);
  Histo_05ns20_10->Add(Histo_05ns0_10_30,-1);
  if(!(Histo_05ns19_10->GetSumw2N() > 0)) Histo_05ns19_10->Sumw2(kTRUE);
  Histo_05ns19_10->Add(Histo_05ns0_10_30,-1);
  if(!(Histo_05ns18_10->GetSumw2N() > 0)) Histo_05ns18_10->Sumw2(kTRUE);
  Histo_05ns18_10->Add(Histo_05ns0_10_50,-1);
  if(!(Histo_05ns16_10->GetSumw2N() > 0)) Histo_05ns16_10->Sumw2(kTRUE);
  Histo_05ns16_10->Add(Histo_05ns0_10_60,-1);
  if(!(Histo_05ns14_10->GetSumw2N() > 0)) Histo_05ns14_10->Sumw2(kTRUE);
  Histo_05ns14_10->Add(Histo_05ns0_10_60,-1);
  if(!(Histo_05ns12_10->GetSumw2N() > 0)) Histo_05ns12_10->Sumw2(kTRUE);
  Histo_05ns12_10->Add(Histo_05ns0_10_60,-1);
  if(!(Histo_05ns10_10->GetSumw2N() > 0)) Histo_05ns10_10->Sumw2(kTRUE);
  Histo_05ns10_10->Add(Histo_05ns0_10_60,-1);
  if(!(Histo_05ns8_10->GetSumw2N() > 0)) Histo_05ns8_10->Sumw2(kTRUE);
  Histo_05ns8_10->Add(Histo_05ns0_10_60,-1);
  if(!(Histo_05ns6_10->GetSumw2N() > 0)) Histo_05ns6_10->Sumw2(kTRUE);
  Histo_05ns6_10->Add(Histo_05ns0_10_60,-1);
  if(!(Histo_05ns4_10->GetSumw2N() > 0)) Histo_05ns4_10->Sumw2(kTRUE);
  Histo_05ns4_10->Add(Histo_05ns0_10_60,-1);
  if(!(Histo_05ns2_10->GetSumw2N() > 0)) Histo_05ns2_10->Sumw2(kTRUE);
  Histo_05ns2_10->Add(Histo_05ns0_10_60,-1);
  
  Histo_05ns2_10->Fit(f_2,"R");
  means_05ns_10[0]=f_2->GetParameter(1);
  sigmas_05ns_10[0]=f_2->GetParameter(2);

  Histo_05ns4_10->Fit(f_4,"R");
  means_05ns_10[1]=f_4->GetParameter(1);
  sigmas_05ns_10[1]=f_4->GetParameter(2);
  
  Histo_05ns6_10->Fit(f_6,"R");
  means_05ns_10[2]=f_6->GetParameter(1);
  sigmas_05ns_10[2]=f_6->GetParameter(2);
  
  Histo_05ns8_10->Fit(f_8,"R");
  means_05ns_10[3]=f_8->GetParameter(1);
  sigmas_05ns_10[3]=f_8->GetParameter(2);
 
  Histo_05ns10_10->Fit(f_10,"R");
  means_05ns_10[4]=f_10->GetParameter(1);
  sigmas_05ns_10[4]=f_10->GetParameter(2);
  
  Histo_05ns12_10->Fit(f_12,"R");
  means_05ns_10[5]=f_12->GetParameter(1);
  sigmas_05ns_10[5]=f_12->GetParameter(2);
  
  Histo_05ns14_10->Fit(f_14,"R");
  means_05ns_10[6]=f_14->GetParameter(1);
  sigmas_05ns_10[6]=f_14->GetParameter(2);
  
  Histo_05ns16_10->Fit(f_16,"R");
  means_05ns_10[7]=f_16->GetParameter(1);
  sigmas_05ns_10[7]=f_16->GetParameter(2);
  
  Histo_05ns18_10->Fit(f_18,"R");
  means_05ns_10[8]=f_18->GetParameter(1);
  sigmas_05ns_10[8]=f_18->GetParameter(2);
  
  Histo_05ns19_10->Fit(f_19,"R");
  means_05ns_10[9]=f_19->GetParameter(1);
  sigmas_05ns_10[9]=f_19->GetParameter(2);
  
  Histo_05ns20_10->Fit(f_20,"R");
  means_05ns_10[10]=f_20->GetParameter(1);
  sigmas_05ns_10[10]=f_20->GetParameter(2);

  entries_05ns_10[0]=Histo_05ns2_10->Integral(500+(means_05ns_10[0]-nsigma*sigmas_05ns_10[0]),500+(means_05ns_10[0]+nsigma*sigmas_05ns_10[0]))/1000;
  entries_05ns_10[1]=Histo_05ns4_10->Integral(500+(means_05ns_10[1]-nsigma*sigmas_05ns_10[1]),500+(means_05ns_10[1]+nsigma*sigmas_05ns_10[1]))/1000;
  entries_05ns_10[2]=Histo_05ns6_10->Integral(500+(means_05ns_10[2]-nsigma*sigmas_05ns_10[2]),500+(means_05ns_10[2]+nsigma*sigmas_05ns_10[2]))/1000;
  entries_05ns_10[3]=Histo_05ns8_10->Integral(500+(means_05ns_10[3]-nsigma*sigmas_05ns_10[3]),500+(means_05ns_10[3]+nsigma*sigmas_05ns_10[3]))/1000;
  entries_05ns_10[4]=Histo_05ns10_10->Integral(500+(means_05ns_10[4]-nsigma*sigmas_05ns_10[4]),500+(means_05ns_10[4]+nsigma*sigmas_05ns_10[4]))/1000;
  entries_05ns_10[5]=Histo_05ns12_10->Integral(500+(means_05ns_10[5]-nsigma*sigmas_05ns_10[5]),500+(means_05ns_10[5]+nsigma*sigmas_05ns_10[5]))/1000;
  entries_05ns_10[6]=Histo_05ns14_10->Integral(500+(means_05ns_10[6]-nsigma*sigmas_05ns_10[6]),500+(means_05ns_10[6]+nsigma*sigmas_05ns_10[6]))/1000;
  entries_05ns_10[7]=Histo_05ns16_10->Integral(500+(means_05ns_10[7]-nsigma*sigmas_05ns_10[7]),500+(means_05ns_10[7]+nsigma*sigmas_05ns_10[7]))/1000;
  entries_05ns_10[8]=Histo_05ns18_10->Integral(500+(means_05ns_10[8]-nsigma*sigmas_05ns_10[8]),500+(means_05ns_10[8]+nsigma*sigmas_05ns_10[8]))/1000;
  entries_05ns_10[9]=Histo_05ns19_10->Integral(500+(means_05ns_10[9]-nsigma*sigmas_05ns_10[9]),500+(means_05ns_10[9]+nsigma*sigmas_05ns_10[9]))/1000;
  entries_05ns_10[10]=Histo_05ns20_10->Integral(500+(means_05ns_10[10]-nsigma*sigmas_05ns_10[10]),500+(means_05ns_10[10]+nsigma*sigmas_05ns_10[10]))/1000;

  TCanvas *c10_05ns = new TCanvas();
  c10_05ns->cd();
  Histo_05ns20_10->SetLineColorAlpha(kBlue+3,0.65);
  Histo_05ns20_10->SetTitle( "M^{2}_{miss} for different A' masses (10 e+ per bunch)");
  Histo_05ns20_10->SetLineWidth(3);
  Histo_05ns20_10->GetYaxis()->SetTitle("N_{events}");
  Histo_05ns20_10->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_05ns19_10->SetLineColorAlpha(kBlue,0.65);
  Histo_05ns19_10->SetLineWidth(3);
  Histo_05ns18_10->SetLineColorAlpha(kCyan+3,0.65);
  Histo_05ns18_10->SetLineWidth(3);
  Histo_05ns16_10->SetLineColorAlpha(kCyan+1,0.65);
  Histo_05ns16_10->SetLineWidth(3);
  Histo_05ns14_10->SetLineColorAlpha(kGreen+3,0.65);
  Histo_05ns14_10->SetLineWidth(3);
  Histo_05ns12_10->SetLineColorAlpha(kGreen+1,0.65);
  Histo_05ns12_10->SetLineWidth(3);
  Histo_05ns10_10->SetLineColorAlpha(kYellow+2,0.65);
  Histo_05ns10_10->SetLineWidth(3);
  Histo_05ns8_10->SetLineColorAlpha(kOrange-3,0.65);
  Histo_05ns8_10->SetLineWidth(3);
  Histo_05ns6_10->SetLineColorAlpha(kRed+2,0.65);
  Histo_05ns6_10->SetLineWidth(3);
  Histo_05ns4_10->SetLineColorAlpha(kRed,0.65);
  Histo_05ns4_10->SetLineWidth(3);
  Histo_05ns2_10->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_05ns2_10->SetLineWidth(3);
    
  Histo_05ns20_10->Draw();
  Histo_05ns19_10->Draw("same");
  Histo_05ns18_10->Draw("same");
  Histo_05ns4_10->Draw("same");
  Histo_05ns6_10->Draw("same");
  Histo_05ns8_10->Draw("same");
  Histo_05ns10_10->Draw("same");
  Histo_05ns12_10->Draw("same");
  Histo_05ns14_10->Draw("same");
  Histo_05ns16_10->Draw("same");
  Histo_05ns2_10->Draw("same");

  auto legend_10_05ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_10_05ns->AddEntry(Histo_05ns20_10,"M_{A'} = 20 MeV","l");
  legend_10_05ns->AddEntry(Histo_05ns19_10,"M_{A'} = 19 MeV","l");
  legend_10_05ns->AddEntry(Histo_05ns18_10,"M_{A'} = 18 MeV","l");
  legend_10_05ns->AddEntry(Histo_05ns16_10,"M_{A'} = 16 MeV","l");
  legend_10_05ns->AddEntry(Histo_05ns14_10,"M_{A'} = 14 MeV","l");
  legend_10_05ns->AddEntry(Histo_05ns12_10,"M_{A'} = 12 MeV","l");
  legend_10_05ns->AddEntry(Histo_05ns10_10,"M_{A'} = 10 MeV","l");
  legend_10_05ns->AddEntry(Histo_05ns8_10,"M_{A'} = 8 MeV","l");
  legend_10_05ns->AddEntry(Histo_05ns6_10,"M_{A'} = 6 MeV","l");
  legend_10_05ns->AddEntry(Histo_05ns4_10,"M_{A'} = 4 MeV","l");
  legend_10_05ns->AddEntry(Histo_05ns2_10,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_10_05ns->Draw();

  outRoot->WriteObject(c10_05ns,"10e_all_05ns");
 
  if(!(Histo_05ns20_100->GetSumw2N() > 0)) Histo_05ns20_100->Sumw2(kTRUE);
  Histo_05ns20_100->Add(Histo_05ns0_100_30,-1);
  if(!(Histo_05ns19_100->GetSumw2N() > 0)) Histo_05ns19_100->Sumw2(kTRUE);
  Histo_05ns19_100->Add(Histo_05ns0_100_30,-1);
  if(!(Histo_05ns18_100->GetSumw2N() > 0)) Histo_05ns18_100->Sumw2(kTRUE);
  Histo_05ns18_100->Add(Histo_05ns0_100_50,-1);
  if(!(Histo_05ns16_100->GetSumw2N() > 0)) Histo_05ns16_100->Sumw2(kTRUE);
  Histo_05ns16_100->Add(Histo_05ns0_100_60,-1);
  if(!(Histo_05ns14_100->GetSumw2N() > 0)) Histo_05ns14_100->Sumw2(kTRUE);
  Histo_05ns14_100->Add(Histo_05ns0_100_60,-1);
  if(!(Histo_05ns12_100->GetSumw2N() > 0)) Histo_05ns12_100->Sumw2(kTRUE);
  Histo_05ns12_100->Add(Histo_05ns0_100_60,-1);
  if(!(Histo_05ns10_100->GetSumw2N() > 0)) Histo_05ns10_100->Sumw2(kTRUE);
  Histo_05ns10_100->Add(Histo_05ns0_100_60,-1);
  if(!(Histo_05ns8_100->GetSumw2N() > 0)) Histo_05ns8_100->Sumw2(kTRUE);
  Histo_05ns8_100->Add(Histo_05ns0_100_60,-1);
  if(!(Histo_05ns6_100->GetSumw2N() > 0)) Histo_05ns6_100->Sumw2(kTRUE);
  Histo_05ns6_100->Add(Histo_05ns0_100_60,-1);
  if(!(Histo_05ns4_100->GetSumw2N() > 0)) Histo_05ns4_100->Sumw2(kTRUE);
  Histo_05ns4_100->Add(Histo_05ns0_100_60,-1);
  if(!(Histo_05ns2_100->GetSumw2N() > 0)) Histo_05ns2_100->Sumw2(kTRUE);
  Histo_05ns2_100->Add(Histo_05ns0_100_60,-1);


  Histo_05ns2_100->Fit(f_2,"R");
  means_05ns_100[0]=f_2->GetParameter(1);
  sigmas_05ns_100[0]=f_2->GetParameter(2);

  Histo_05ns4_100->Fit(f_4,"R");
  means_05ns_100[1]=f_4->GetParameter(1);
  sigmas_05ns_100[1]=f_4->GetParameter(2);
  
  Histo_05ns6_100->Fit(f_6,"R");
  means_05ns_100[2]=f_6->GetParameter(1);
  sigmas_05ns_100[2]=f_6->GetParameter(2);
  
  Histo_05ns8_100->Fit(f_8,"R");
  means_05ns_100[3]=f_8->GetParameter(1);
  sigmas_05ns_100[3]=f_8->GetParameter(2);
 
  Histo_05ns10_100->Fit(f_10,"R");
  means_05ns_100[4]=f_10->GetParameter(1);
  sigmas_05ns_100[4]=f_10->GetParameter(2);
  
  Histo_05ns12_100->Fit(f_12,"R");
  means_05ns_100[5]=f_12->GetParameter(1);
  sigmas_05ns_100[5]=f_12->GetParameter(2);
  
  Histo_05ns14_100->Fit(f_14,"R");
  means_05ns_100[6]=f_14->GetParameter(1);
  sigmas_05ns_100[6]=f_14->GetParameter(2);
  
  Histo_05ns16_100->Fit(f_16,"R");
  means_05ns_100[7]=f_16->GetParameter(1);
  sigmas_05ns_100[7]=f_16->GetParameter(2);
  
  Histo_05ns18_100->Fit(f_18,"R");
  means_05ns_100[8]=f_18->GetParameter(1);
  sigmas_05ns_100[8]=f_18->GetParameter(2);
  
  Histo_05ns19_100->Fit(f_19,"R");
  means_05ns_100[9]=f_19->GetParameter(1);
  sigmas_05ns_100[9]=f_19->GetParameter(2);
  
  Histo_05ns20_100->Fit(f_20,"R");
  means_05ns_100[10]=f_20->GetParameter(1);
  sigmas_05ns_100[10]=f_20->GetParameter(2);

  entries_05ns_100[0]=Histo_05ns2_100->Integral(500+(means_05ns_100[0]-nsigma*sigmas_05ns_100[0]),500+(means_05ns_100[0]+nsigma*sigmas_05ns_100[0]))/1000;
  entries_05ns_100[1]=Histo_05ns4_100->Integral(500+(means_05ns_100[1]-nsigma*sigmas_05ns_100[1]),500+(means_05ns_100[1]+nsigma*sigmas_05ns_100[1]))/1000;
  entries_05ns_100[2]=Histo_05ns6_100->Integral(500+(means_05ns_100[2]-nsigma*sigmas_05ns_100[2]),500+(means_05ns_100[2]+nsigma*sigmas_05ns_100[2]))/1000;
  entries_05ns_100[3]=Histo_05ns8_100->Integral(500+(means_05ns_100[3]-nsigma*sigmas_05ns_100[3]),500+(means_05ns_100[3]+nsigma*sigmas_05ns_100[3]))/1000;
  entries_05ns_100[4]=Histo_05ns10_100->Integral(500+(means_05ns_100[4]-nsigma*sigmas_05ns_100[4]),500+(means_05ns_100[4]+nsigma*sigmas_05ns_100[4]))/1000;
  entries_05ns_100[5]=Histo_05ns12_100->Integral(500+(means_05ns_100[5]-nsigma*sigmas_05ns_100[5]),500+(means_05ns_100[5]+nsigma*sigmas_05ns_100[5]))/1000;
  entries_05ns_100[6]=Histo_05ns14_100->Integral(500+(means_05ns_100[6]-nsigma*sigmas_05ns_100[6]),500+(means_05ns_100[6]+nsigma*sigmas_05ns_100[6]))/1000;
  entries_05ns_100[7]=Histo_05ns16_100->Integral(500+(means_05ns_100[7]-nsigma*sigmas_05ns_100[7]),500+(means_05ns_100[7]+nsigma*sigmas_05ns_100[7]))/1000;
  entries_05ns_100[8]=Histo_05ns18_100->Integral(500+(means_05ns_100[8]-nsigma*sigmas_05ns_100[8]),500+(means_05ns_100[8]+nsigma*sigmas_05ns_100[8]))/1000;
  entries_05ns_100[9]=Histo_05ns19_100->Integral(500+(means_05ns_100[9]-nsigma*sigmas_05ns_100[9]),500+(means_05ns_100[9]+nsigma*sigmas_05ns_100[9]))/1000;
  entries_05ns_100[10]=Histo_05ns20_100->Integral(500+(means_05ns_100[10]-nsigma*sigmas_05ns_100[10]),500+(means_05ns_100[10]+nsigma*sigmas_05ns_100[10]))/1000;

  TCanvas *c100_05ns = new TCanvas();
  c100_05ns->cd();
  Histo_05ns20_100->SetLineColorAlpha(kBlue+3,0.65);
  Histo_05ns20_100->SetTitle( "M^{2}_{miss} for different A' masses (100 e+ per bunch)");
  Histo_05ns20_100->SetLineWidth(3);
  Histo_05ns20_100->GetYaxis()->SetTitle("N_{events}");
  Histo_05ns20_100->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_05ns19_100->SetLineColorAlpha(kBlue,0.65);
  Histo_05ns19_100->SetLineWidth(3);
  Histo_05ns18_100->SetLineColorAlpha(kCyan+3,0.65);
  Histo_05ns18_100->SetLineWidth(3);
  Histo_05ns16_100->SetLineColorAlpha(kCyan+1,0.65);
  Histo_05ns16_100->SetLineWidth(3);
  Histo_05ns14_100->SetLineColorAlpha(kGreen+3,0.65);
  Histo_05ns14_100->SetLineWidth(3);
  Histo_05ns12_100->SetLineColorAlpha(kGreen+1,0.65);
  Histo_05ns12_100->SetLineWidth(3);
  Histo_05ns10_100->SetLineColorAlpha(kYellow+2,0.65);
  Histo_05ns10_100->SetLineWidth(3);
  Histo_05ns8_100->SetLineColorAlpha(kOrange-3,0.65);
  Histo_05ns8_100->SetLineWidth(3);
  Histo_05ns6_100->SetLineColorAlpha(kRed+2,0.65);
  Histo_05ns6_100->SetLineWidth(3);
  Histo_05ns4_100->SetLineColorAlpha(kRed,0.65);
  Histo_05ns4_100->SetLineWidth(3);
  Histo_05ns2_100->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_05ns2_100->SetLineWidth(3);
    
  Histo_05ns20_100->Draw();
  Histo_05ns19_100->Draw("same");
  Histo_05ns18_100->Draw("same");
  Histo_05ns4_100->Draw("same");
  Histo_05ns6_100->Draw("same");
  Histo_05ns8_100->Draw("same");
  Histo_05ns10_100->Draw("same");
  Histo_05ns12_100->Draw("same");
  Histo_05ns14_100->Draw("same");
  Histo_05ns16_100->Draw("same");
  Histo_05ns2_100->Draw("same");

  auto legend_100_05ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_100_05ns->AddEntry(Histo_05ns20_100,"M_{A'} = 20 MeV","l");
  legend_100_05ns->AddEntry(Histo_05ns19_100,"M_{A'} = 19 MeV","l");
  legend_100_05ns->AddEntry(Histo_05ns18_100,"M_{A'} = 18 MeV","l");
  legend_100_05ns->AddEntry(Histo_05ns16_100,"M_{A'} = 16 MeV","l");
  legend_100_05ns->AddEntry(Histo_05ns14_100,"M_{A'} = 14 MeV","l");
  legend_100_05ns->AddEntry(Histo_05ns12_100,"M_{A'} = 12 MeV","l");
  legend_100_05ns->AddEntry(Histo_05ns10_100,"M_{A'} = 10 MeV","l");
  legend_100_05ns->AddEntry(Histo_05ns8_100,"M_{A'} = 8 MeV","l");
  legend_100_05ns->AddEntry(Histo_05ns6_100,"M_{A'} = 6 MeV","l");
  legend_100_05ns->AddEntry(Histo_05ns4_100,"M_{A'} = 4 MeV","l");
  legend_100_05ns->AddEntry(Histo_05ns2_100,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_100_05ns->Draw();

  outRoot->WriteObject(c100_05ns,"100e_all_05ns");

  
  if(!(Histo_05ns20_1000->GetSumw2N() > 0)) Histo_05ns20_1000->Sumw2(kTRUE);
  Histo_05ns20_1000->Add(Histo_05ns0_1000_30,-1);
  if(!(Histo_05ns19_1000->GetSumw2N() > 0)) Histo_05ns19_1000->Sumw2(kTRUE);
  Histo_05ns19_1000->Add(Histo_05ns0_1000_30,-1);
  if(!(Histo_05ns18_1000->GetSumw2N() > 0)) Histo_05ns18_1000->Sumw2(kTRUE);
  Histo_05ns18_1000->Add(Histo_05ns0_1000_50,-1);
  if(!(Histo_05ns16_1000->GetSumw2N() > 0)) Histo_05ns16_1000->Sumw2(kTRUE);
  Histo_05ns16_1000->Add(Histo_05ns0_1000_60,-1);
  if(!(Histo_05ns14_1000->GetSumw2N() > 0)) Histo_05ns14_1000->Sumw2(kTRUE);
  Histo_05ns14_1000->Add(Histo_05ns0_1000_60,-1);
  if(!(Histo_05ns12_1000->GetSumw2N() > 0)) Histo_05ns12_1000->Sumw2(kTRUE);
  Histo_05ns12_1000->Add(Histo_05ns0_1000_60,-1);
  if(!(Histo_05ns10_1000->GetSumw2N() > 0)) Histo_05ns10_1000->Sumw2(kTRUE);
  Histo_05ns10_1000->Add(Histo_05ns0_1000_60,-1);
  if(!(Histo_05ns8_1000->GetSumw2N() > 0)) Histo_05ns8_1000->Sumw2(kTRUE);
  Histo_05ns8_1000->Add(Histo_05ns0_1000_60,-1);
  if(!(Histo_05ns6_1000->GetSumw2N() > 0)) Histo_05ns6_1000->Sumw2(kTRUE);
  Histo_05ns6_1000->Add(Histo_05ns0_1000_60,-1);
  if(!(Histo_05ns4_1000->GetSumw2N() > 0)) Histo_05ns4_1000->Sumw2(kTRUE);
  Histo_05ns4_1000->Add(Histo_05ns0_1000_60,-1);
  if(!(Histo_05ns2_1000->GetSumw2N() > 0)) Histo_05ns2_1000->Sumw2(kTRUE);
  Histo_05ns2_1000->Add(Histo_05ns0_1000_60,-1);


  Histo_05ns2_1000->Fit(f_2,"R");
  means_05ns_1000[0]=f_2->GetParameter(1);
  sigmas_05ns_1000[0]=f_2->GetParameter(2);

  Histo_05ns4_1000->Fit(f_4,"R");
  means_05ns_1000[1]=f_4->GetParameter(1);
  sigmas_05ns_1000[1]=f_4->GetParameter(2);
  
  Histo_05ns6_1000->Fit(f_6,"R");
  means_05ns_1000[2]=f_6->GetParameter(1);
  sigmas_05ns_1000[2]=f_6->GetParameter(2);
  
  Histo_05ns8_1000->Fit(f_8,"R");
  means_05ns_1000[3]=f_8->GetParameter(1);
  sigmas_05ns_1000[3]=f_8->GetParameter(2);
 
  Histo_05ns10_1000->Fit(f_10,"R");
  means_05ns_1000[4]=f_10->GetParameter(1);
  sigmas_05ns_1000[4]=f_10->GetParameter(2);
  
  Histo_05ns12_1000->Fit(f_12,"R");
  means_05ns_1000[5]=f_12->GetParameter(1);
  sigmas_05ns_1000[5]=f_12->GetParameter(2);
  
  Histo_05ns14_1000->Fit(f_14,"R");
  means_05ns_1000[6]=f_14->GetParameter(1);
  sigmas_05ns_1000[6]=f_14->GetParameter(2);
  
  Histo_05ns16_1000->Fit(f_16,"R");
  means_05ns_1000[7]=f_16->GetParameter(1);
  sigmas_05ns_1000[7]=f_16->GetParameter(2);
  
  Histo_05ns18_1000->Fit(f_18,"R");
  means_05ns_1000[8]=f_18->GetParameter(1);
  sigmas_05ns_1000[8]=f_18->GetParameter(2);
  
  Histo_05ns19_1000->Fit(f_19,"R");
  means_05ns_1000[9]=f_19->GetParameter(1);
  sigmas_05ns_1000[9]=f_19->GetParameter(2);
  
  Histo_05ns20_1000->Fit(f_20,"R");
  means_05ns_1000[10]=f_20->GetParameter(1);
  sigmas_05ns_1000[10]=f_20->GetParameter(2);

  entries_05ns_1000[0]=Histo_05ns2_1000->Integral(500+(means_05ns_1000[0]-nsigma*sigmas_05ns_1000[0]),500+(means_05ns_1000[0]+nsigma*sigmas_05ns_1000[0]))/1000;
  entries_05ns_1000[1]=Histo_05ns4_1000->Integral(500+(means_05ns_1000[1]-nsigma*sigmas_05ns_1000[1]),500+(means_05ns_1000[1]+nsigma*sigmas_05ns_1000[1]))/1000;
  entries_05ns_1000[2]=Histo_05ns6_1000->Integral(500+(means_05ns_1000[2]-nsigma*sigmas_05ns_1000[2]),500+(means_05ns_1000[2]+nsigma*sigmas_05ns_1000[2]))/1000;
  entries_05ns_1000[3]=Histo_05ns8_1000->Integral(500+(means_05ns_1000[3]-nsigma*sigmas_05ns_1000[3]),500+(means_05ns_1000[3]+nsigma*sigmas_05ns_1000[3]))/1000;
  entries_05ns_1000[4]=Histo_05ns10_1000->Integral(500+(means_05ns_1000[4]-nsigma*sigmas_05ns_1000[4]),500+(means_05ns_1000[4]+nsigma*sigmas_05ns_1000[4]))/1000;
  entries_05ns_1000[5]=Histo_05ns12_1000->Integral(500+(means_05ns_1000[5]-nsigma*sigmas_05ns_1000[5]),500+(means_05ns_1000[5]+nsigma*sigmas_05ns_1000[5]))/1000;
  entries_05ns_1000[6]=Histo_05ns14_1000->Integral(500+(means_05ns_1000[6]-nsigma*sigmas_05ns_1000[6]),500+(means_05ns_1000[6]+nsigma*sigmas_05ns_1000[6]))/1000;
  entries_05ns_1000[7]=Histo_05ns16_1000->Integral(500+(means_05ns_1000[7]-nsigma*sigmas_05ns_1000[7]),500+(means_05ns_1000[7]+nsigma*sigmas_05ns_1000[7]))/1000;
  entries_05ns_1000[8]=Histo_05ns18_1000->Integral(500+(means_05ns_1000[8]-nsigma*sigmas_05ns_1000[8]),500+(means_05ns_1000[8]+nsigma*sigmas_05ns_1000[8]))/1000;
  entries_05ns_1000[9]=Histo_05ns19_1000->Integral(500+(means_05ns_1000[9]-nsigma*sigmas_05ns_1000[9]),500+(means_05ns_1000[9]+nsigma*sigmas_05ns_1000[9]))/1000;
  entries_05ns_1000[10]=Histo_05ns20_1000->Integral(500+(means_05ns_1000[10]-nsigma*sigmas_05ns_1000[10]),500+(means_05ns_1000[10]+nsigma*sigmas_05ns_1000[10]))/1000;

  TCanvas *c1000_05ns = new TCanvas();
  c1000_05ns->cd();
  Histo_05ns20_1000->SetLineColorAlpha(kBlue+3,0.65);
  Histo_05ns20_1000->SetTitle( "M^{2}_{miss} for different A' masses (1000 e+ per bunch)");
  Histo_05ns20_1000->SetLineWidth(3);
  Histo_05ns20_1000->GetYaxis()->SetTitle("N_{events}");
  Histo_05ns20_1000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_05ns19_1000->SetLineColorAlpha(kBlue,0.65);
  Histo_05ns19_1000->SetLineWidth(3);
  Histo_05ns18_1000->SetLineColorAlpha(kCyan+3,0.65);
  Histo_05ns18_1000->SetLineWidth(3);
  Histo_05ns16_1000->SetLineColorAlpha(kCyan+1,0.65);
  Histo_05ns16_1000->SetLineWidth(3);
  Histo_05ns14_1000->SetLineColorAlpha(kGreen+3,0.65);
  Histo_05ns14_1000->SetLineWidth(3);
  Histo_05ns12_1000->SetLineColorAlpha(kGreen+1,0.65);
  Histo_05ns12_1000->SetLineWidth(3);
  Histo_05ns10_1000->SetLineColorAlpha(kYellow+2,0.65);
  Histo_05ns10_1000->SetLineWidth(3);
  Histo_05ns8_1000->SetLineColorAlpha(kOrange-3,0.65);
  Histo_05ns8_1000->SetLineWidth(3);
  Histo_05ns6_1000->SetLineColorAlpha(kRed+2,0.65);
  Histo_05ns6_1000->SetLineWidth(3);
  Histo_05ns4_1000->SetLineColorAlpha(kRed,0.65);
  Histo_05ns4_1000->SetLineWidth(3);
  Histo_05ns2_1000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_05ns2_1000->SetLineWidth(3);
    
  Histo_05ns20_1000->Draw();
  Histo_05ns19_1000->Draw("same");
  Histo_05ns18_1000->Draw("same");
  Histo_05ns4_1000->Draw("same");
  Histo_05ns6_1000->Draw("same");
  Histo_05ns8_1000->Draw("same");
  Histo_05ns10_1000->Draw("same");
  Histo_05ns12_1000->Draw("same");
  Histo_05ns14_1000->Draw("same");
  Histo_05ns16_1000->Draw("same");
  Histo_05ns2_1000->Draw("same");

  auto legend_1000_05ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_1000_05ns->AddEntry(Histo_05ns20_1000,"M_{A'} = 20 MeV","l");
  legend_1000_05ns->AddEntry(Histo_05ns19_1000,"M_{A'} = 19 MeV","l");
  legend_1000_05ns->AddEntry(Histo_05ns18_1000,"M_{A'} = 18 MeV","l");
  legend_1000_05ns->AddEntry(Histo_05ns16_1000,"M_{A'} = 16 MeV","l");
  legend_1000_05ns->AddEntry(Histo_05ns14_1000,"M_{A'} = 14 MeV","l");
  legend_1000_05ns->AddEntry(Histo_05ns12_1000,"M_{A'} = 12 MeV","l");
  legend_1000_05ns->AddEntry(Histo_05ns10_1000,"M_{A'} = 10 MeV","l");
  legend_1000_05ns->AddEntry(Histo_05ns8_1000,"M_{A'} = 8 MeV","l");
  legend_1000_05ns->AddEntry(Histo_05ns6_1000,"M_{A'} = 6 MeV","l");
  legend_1000_05ns->AddEntry(Histo_05ns4_1000,"M_{A'} = 4 MeV","l");
  legend_1000_05ns->AddEntry(Histo_05ns2_1000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_1000_05ns->Draw();

  outRoot->WriteObject(c1000_05ns,"1000e_all_05ns");

  
  if(!(Histo_05ns20_10000->GetSumw2N() > 0)) Histo_05ns20_10000->Sumw2(kTRUE);
  Histo_05ns20_10000->Add(Histo_05ns0_10000_30,-1);
  if(!(Histo_05ns19_10000->GetSumw2N() > 0)) Histo_05ns19_10000->Sumw2(kTRUE);
  Histo_05ns19_10000->Add(Histo_05ns0_10000_30,-1);
  if(!(Histo_05ns18_10000->GetSumw2N() > 0)) Histo_05ns18_10000->Sumw2(kTRUE);
  Histo_05ns18_10000->Add(Histo_05ns0_10000_50,-1);
  if(!(Histo_05ns16_10000->GetSumw2N() > 0)) Histo_05ns16_10000->Sumw2(kTRUE);
  Histo_05ns16_10000->Add(Histo_05ns0_10000_60,-1);
  if(!(Histo_05ns14_10000->GetSumw2N() > 0)) Histo_05ns14_10000->Sumw2(kTRUE);
  Histo_05ns14_10000->Add(Histo_05ns0_10000_60,-1);
  if(!(Histo_05ns12_10000->GetSumw2N() > 0)) Histo_05ns12_10000->Sumw2(kTRUE);
  Histo_05ns12_10000->Add(Histo_05ns0_10000_60,-1);
  if(!(Histo_05ns10_10000->GetSumw2N() > 0)) Histo_05ns10_10000->Sumw2(kTRUE);
  Histo_05ns10_10000->Add(Histo_05ns0_10000_60,-1);
  if(!(Histo_05ns8_10000->GetSumw2N() > 0)) Histo_05ns8_10000->Sumw2(kTRUE);
  Histo_05ns8_10000->Add(Histo_05ns0_10000_60,-1);
  if(!(Histo_05ns6_10000->GetSumw2N() > 0)) Histo_05ns6_10000->Sumw2(kTRUE);
  Histo_05ns6_10000->Add(Histo_05ns0_10000_60,-1);
  if(!(Histo_05ns4_10000->GetSumw2N() > 0)) Histo_05ns4_10000->Sumw2(kTRUE);
  Histo_05ns4_10000->Add(Histo_05ns0_10000_60,-1);
  if(!(Histo_05ns2_10000->GetSumw2N() > 0)) Histo_05ns2_10000->Sumw2(kTRUE);
  Histo_05ns2_10000->Add(Histo_05ns0_10000_60,-1);


  Histo_05ns2_10000->Fit(f_2,"R");
  means_05ns_10000[0]=f_2->GetParameter(1);
  sigmas_05ns_10000[0]=f_2->GetParameter(2);

  Histo_05ns4_10000->Fit(f_4,"R");
  means_05ns_10000[1]=f_4->GetParameter(1);
  sigmas_05ns_10000[1]=f_4->GetParameter(2);
  
  Histo_05ns6_10000->Fit(f_6,"R");
  means_05ns_10000[2]=f_6->GetParameter(1);
  sigmas_05ns_10000[2]=f_6->GetParameter(2);
  
  Histo_05ns8_10000->Fit(f_8,"R");
  means_05ns_10000[3]=f_8->GetParameter(1);
  sigmas_05ns_10000[3]=f_8->GetParameter(2);
 
  Histo_05ns10_10000->Fit(f_10,"R");
  means_05ns_10000[4]=f_10->GetParameter(1);
  sigmas_05ns_10000[4]=f_10->GetParameter(2);
  
  Histo_05ns12_10000->Fit(f_12,"R");
  means_05ns_10000[5]=f_12->GetParameter(1);
  sigmas_05ns_10000[5]=f_12->GetParameter(2);
  
  Histo_05ns14_10000->Fit(f_14,"R");
  means_05ns_10000[6]=f_14->GetParameter(1);
  sigmas_05ns_10000[6]=f_14->GetParameter(2);
  
  Histo_05ns16_10000->Fit(f_16,"R");
  means_05ns_10000[7]=f_16->GetParameter(1);
  sigmas_05ns_10000[7]=f_16->GetParameter(2);
  
  Histo_05ns18_10000->Fit(f_18,"R");
  means_05ns_10000[8]=f_18->GetParameter(1);
  sigmas_05ns_10000[8]=f_18->GetParameter(2);
  
  Histo_05ns19_10000->Fit(f_19,"R");
  means_05ns_10000[9]=f_19->GetParameter(1);
  sigmas_05ns_10000[9]=f_19->GetParameter(2);
  
  Histo_05ns20_10000->Fit(f_20,"R");
  means_05ns_10000[10]=f_20->GetParameter(1);
  sigmas_05ns_10000[10]=f_20->GetParameter(2);

  entries_05ns_10000[0]=Histo_05ns2_10000->Integral(500+(means_05ns_10000[0]-nsigma*sigmas_05ns_10000[0]),500+(means_05ns_10000[0]+nsigma*sigmas_05ns_10000[0]))/1000;
  entries_05ns_10000[1]=Histo_05ns4_10000->Integral(500+(means_05ns_10000[1]-nsigma*sigmas_05ns_10000[1]),500+(means_05ns_10000[1]+nsigma*sigmas_05ns_10000[1]))/1000;
  entries_05ns_10000[2]=Histo_05ns6_10000->Integral(500+(means_05ns_10000[2]-nsigma*sigmas_05ns_10000[2]),500+(means_05ns_10000[2]+nsigma*sigmas_05ns_10000[2]))/1000;
  entries_05ns_10000[3]=Histo_05ns8_10000->Integral(500+(means_05ns_10000[3]-nsigma*sigmas_05ns_10000[3]),500+(means_05ns_10000[3]+nsigma*sigmas_05ns_10000[3]))/1000;
  entries_05ns_10000[4]=Histo_05ns10_10000->Integral(500+(means_05ns_10000[4]-nsigma*sigmas_05ns_10000[4]),500+(means_05ns_10000[4]+nsigma*sigmas_05ns_10000[4]))/1000;
  entries_05ns_10000[5]=Histo_05ns12_10000->Integral(500+(means_05ns_10000[5]-nsigma*sigmas_05ns_10000[5]),500+(means_05ns_10000[5]+nsigma*sigmas_05ns_10000[5]))/1000;
  entries_05ns_10000[6]=Histo_05ns14_10000->Integral(500+(means_05ns_10000[6]-nsigma*sigmas_05ns_10000[6]),500+(means_05ns_10000[6]+nsigma*sigmas_05ns_10000[6]))/1000;
  entries_05ns_10000[7]=Histo_05ns16_10000->Integral(500+(means_05ns_10000[7]-nsigma*sigmas_05ns_10000[7]),500+(means_05ns_10000[7]+nsigma*sigmas_05ns_10000[7]))/1000;
  entries_05ns_10000[8]=Histo_05ns18_10000->Integral(500+(means_05ns_10000[8]-nsigma*sigmas_05ns_10000[8]),500+(means_05ns_10000[8]+nsigma*sigmas_05ns_10000[8]))/1000;
  entries_05ns_10000[9]=Histo_05ns19_10000->Integral(500+(means_05ns_10000[9]-nsigma*sigmas_05ns_10000[9]),500+(means_05ns_10000[9]+nsigma*sigmas_05ns_10000[9]))/1000;
  entries_05ns_10000[10]=Histo_05ns20_10000->Integral(500+(means_05ns_10000[10]-nsigma*sigmas_05ns_10000[10]),500+(means_05ns_10000[10]+nsigma*sigmas_05ns_10000[10]))/1000;


  TCanvas *c10000_05ns = new TCanvas();
  c10000_05ns->cd();
  Histo_05ns20_10000->SetLineColorAlpha(kBlue+3,0.65);
  Histo_05ns20_10000->SetTitle( "M^{2}_{miss} for different A' masses (10000 e+ per bunch)");
  Histo_05ns20_10000->SetLineWidth(3);
  Histo_05ns20_10000->GetYaxis()->SetTitle("N_{events}");
  Histo_05ns20_10000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_05ns19_10000->SetLineColorAlpha(kBlue,0.65);
  Histo_05ns19_10000->SetLineWidth(3);
  Histo_05ns18_10000->SetLineColorAlpha(kCyan+3,0.65);
  Histo_05ns18_10000->SetLineWidth(3);
  Histo_05ns16_10000->SetLineColorAlpha(kCyan+1,0.65);
  Histo_05ns16_10000->SetLineWidth(3);
  Histo_05ns14_10000->SetLineColorAlpha(kGreen+3,0.65);
  Histo_05ns14_10000->SetLineWidth(3);
  Histo_05ns12_10000->SetLineColorAlpha(kGreen+1,0.65);
  Histo_05ns12_10000->SetLineWidth(3);
  Histo_05ns10_10000->SetLineColorAlpha(kYellow+2,0.65);
  Histo_05ns10_10000->SetLineWidth(3);
  Histo_05ns8_10000->SetLineColorAlpha(kOrange-3,0.65);
  Histo_05ns8_10000->SetLineWidth(3);
  Histo_05ns6_10000->SetLineColorAlpha(kRed+2,0.65);
  Histo_05ns6_10000->SetLineWidth(3);
  Histo_05ns4_10000->SetLineColorAlpha(kRed,0.65);
  Histo_05ns4_10000->SetLineWidth(3);
  Histo_05ns2_10000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_05ns2_10000->SetLineWidth(3);
    
  Histo_05ns20_10000->Draw();
  Histo_05ns19_10000->Draw("same");
  Histo_05ns18_10000->Draw("same");
  Histo_05ns4_10000->Draw("same");
  Histo_05ns6_10000->Draw("same");
  Histo_05ns8_10000->Draw("same");
  Histo_05ns10_10000->Draw("same");
  Histo_05ns12_10000->Draw("same");
  Histo_05ns14_10000->Draw("same");
  Histo_05ns16_10000->Draw("same");
  Histo_05ns2_10000->Draw("same");

  auto legend_10000_05ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_10000_05ns->AddEntry(Histo_05ns20_10000,"M_{A'} = 20 MeV","l");
  legend_10000_05ns->AddEntry(Histo_05ns19_10000,"M_{A'} = 19 MeV","l");
  legend_10000_05ns->AddEntry(Histo_05ns18_10000,"M_{A'} = 18 MeV","l");
  legend_10000_05ns->AddEntry(Histo_05ns16_10000,"M_{A'} = 16 MeV","l");
  legend_10000_05ns->AddEntry(Histo_05ns14_10000,"M_{A'} = 14 MeV","l");
  legend_10000_05ns->AddEntry(Histo_05ns12_10000,"M_{A'} = 12 MeV","l");
  legend_10000_05ns->AddEntry(Histo_05ns10_10000,"M_{A'} = 10 MeV","l");
  legend_10000_05ns->AddEntry(Histo_05ns8_10000,"M_{A'} = 8 MeV","l");
  legend_10000_05ns->AddEntry(Histo_05ns6_10000,"M_{A'} = 6 MeV","l");
  legend_10000_05ns->AddEntry(Histo_05ns4_10000,"M_{A'} = 4 MeV","l");
  legend_10000_05ns->AddEntry(Histo_05ns2_10000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_10000_05ns->Draw();

  outRoot->WriteObject(c10000_05ns,"10000e_all_05ns");

  
  if(!(Histo_05ns20_25000->GetSumw2N() > 0)) Histo_05ns20_25000->Sumw2(kTRUE);
  Histo_05ns20_25000->Add(Histo_05ns0_25000_30,-1);
  if(!(Histo_05ns19_25000->GetSumw2N() > 0)) Histo_05ns19_25000->Sumw2(kTRUE);
  Histo_05ns19_25000->Add(Histo_05ns0_25000_30,-1);
  if(!(Histo_05ns18_25000->GetSumw2N() > 0)) Histo_05ns18_25000->Sumw2(kTRUE);
  Histo_05ns18_25000->Add(Histo_05ns0_25000_50,-1);
  if(!(Histo_05ns16_25000->GetSumw2N() > 0)) Histo_05ns16_25000->Sumw2(kTRUE);
  Histo_05ns16_25000->Add(Histo_05ns0_25000_60,-1);
  if(!(Histo_05ns14_25000->GetSumw2N() > 0)) Histo_05ns14_25000->Sumw2(kTRUE);
  Histo_05ns14_25000->Add(Histo_05ns0_25000_60,-1);
  if(!(Histo_05ns12_25000->GetSumw2N() > 0)) Histo_05ns12_25000->Sumw2(kTRUE);
  Histo_05ns12_25000->Add(Histo_05ns0_25000_60,-1);
  if(!(Histo_05ns10_25000->GetSumw2N() > 0)) Histo_05ns10_25000->Sumw2(kTRUE);
  Histo_05ns10_25000->Add(Histo_05ns0_25000_60,-1);
  if(!(Histo_05ns8_25000->GetSumw2N() > 0)) Histo_05ns8_25000->Sumw2(kTRUE);
  Histo_05ns8_25000->Add(Histo_05ns0_25000_60,-1);
  if(!(Histo_05ns6_25000->GetSumw2N() > 0)) Histo_05ns6_25000->Sumw2(kTRUE);
  Histo_05ns6_25000->Add(Histo_05ns0_25000_60,-1);
  if(!(Histo_05ns4_25000->GetSumw2N() > 0)) Histo_05ns4_25000->Sumw2(kTRUE);
  Histo_05ns4_25000->Add(Histo_05ns0_25000_60,-1);
  if(!(Histo_05ns2_25000->GetSumw2N() > 0)) Histo_05ns2_25000->Sumw2(kTRUE);
  Histo_05ns2_25000->Add(Histo_05ns0_25000_60,-1);

  Histo_05ns2_25000->Fit(f_2,"R");
  means_05ns_25000[0]=f_2->GetParameter(1);
  sigmas_05ns_25000[0]=f_2->GetParameter(2);

  Histo_05ns4_25000->Fit(f_4,"R");
  means_05ns_25000[1]=f_4->GetParameter(1);
  sigmas_05ns_25000[1]=f_4->GetParameter(2);
  
  Histo_05ns6_25000->Fit(f_6,"R");
  means_05ns_25000[2]=f_6->GetParameter(1);
  sigmas_05ns_25000[2]=f_6->GetParameter(2);
  
  Histo_05ns8_25000->Fit(f_8,"R");
  means_05ns_25000[3]=f_8->GetParameter(1);
  sigmas_05ns_25000[3]=f_8->GetParameter(2);
 
  Histo_05ns10_25000->Fit(f_10,"R");
  means_05ns_25000[4]=f_10->GetParameter(1);
  sigmas_05ns_25000[4]=f_10->GetParameter(2);
  
  Histo_05ns12_25000->Fit(f_12,"R");
  means_05ns_25000[5]=f_12->GetParameter(1);
  sigmas_05ns_25000[5]=f_12->GetParameter(2);
  
  Histo_05ns14_25000->Fit(f_14,"R");
  means_05ns_25000[6]=f_14->GetParameter(1);
  sigmas_05ns_25000[6]=f_14->GetParameter(2);
  
  Histo_05ns16_25000->Fit(f_16,"R");
  means_05ns_25000[7]=f_16->GetParameter(1);
  sigmas_05ns_25000[7]=f_16->GetParameter(2);
  
  Histo_05ns18_25000->Fit(f_18,"R");
  means_05ns_25000[8]=f_18->GetParameter(1);
  sigmas_05ns_25000[8]=f_18->GetParameter(2);
  
  Histo_05ns19_25000->Fit(f_19,"R");
  means_05ns_25000[9]=f_19->GetParameter(1);
  sigmas_05ns_25000[9]=f_19->GetParameter(2);
  
  Histo_05ns20_25000->Fit(f_20,"R");
  means_05ns_25000[10]=f_20->GetParameter(1);
  sigmas_05ns_25000[10]=f_20->GetParameter(2);

  entries_05ns_25000[0]=Histo_05ns2_25000->Integral(500+(means_05ns_25000[0]-nsigma*sigmas_05ns_25000[0]),500+(means_05ns_25000[0]+nsigma*sigmas_05ns_25000[0]))/1000;
  entries_05ns_25000[1]=Histo_05ns4_25000->Integral(500+(means_05ns_25000[1]-nsigma*sigmas_05ns_25000[1]),500+(means_05ns_25000[1]+nsigma*sigmas_05ns_25000[1]))/1000;
  entries_05ns_25000[2]=Histo_05ns6_25000->Integral(500+(means_05ns_25000[2]-nsigma*sigmas_05ns_25000[2]),500+(means_05ns_25000[2]+nsigma*sigmas_05ns_25000[2]))/1000;
  entries_05ns_25000[3]=Histo_05ns8_25000->Integral(500+(means_05ns_25000[3]-nsigma*sigmas_05ns_25000[3]),500+(means_05ns_25000[3]+nsigma*sigmas_05ns_25000[3]))/1000;
  entries_05ns_25000[4]=Histo_05ns10_25000->Integral(500+(means_05ns_25000[4]-nsigma*sigmas_05ns_25000[4]),500+(means_05ns_25000[4]+nsigma*sigmas_05ns_25000[4]))/1000;
  entries_05ns_25000[5]=Histo_05ns12_25000->Integral(500+(means_05ns_25000[5]-nsigma*sigmas_05ns_25000[5]),500+(means_05ns_25000[5]+nsigma*sigmas_05ns_25000[5]))/1000;
  entries_05ns_25000[6]=Histo_05ns14_25000->Integral(500+(means_05ns_25000[6]-nsigma*sigmas_05ns_25000[6]),500+(means_05ns_25000[6]+nsigma*sigmas_05ns_25000[6]))/1000;
  entries_05ns_25000[7]=Histo_05ns16_25000->Integral(500+(means_05ns_25000[7]-nsigma*sigmas_05ns_25000[7]),500+(means_05ns_25000[7]+nsigma*sigmas_05ns_25000[7]))/1000;
  entries_05ns_25000[8]=Histo_05ns18_25000->Integral(500+(means_05ns_25000[8]-nsigma*sigmas_05ns_25000[8]),500+(means_05ns_25000[8]+nsigma*sigmas_05ns_25000[8]))/1000;
  entries_05ns_25000[9]=Histo_05ns19_25000->Integral(500+(means_05ns_25000[9]-nsigma*sigmas_05ns_25000[9]),500+(means_05ns_25000[9]+nsigma*sigmas_05ns_25000[9]))/1000;
  entries_05ns_25000[10]=Histo_05ns20_25000->Integral(500+(means_05ns_25000[10]-nsigma*sigmas_05ns_25000[10]),500+(means_05ns_25000[10]+nsigma*sigmas_05ns_25000[10]))/1000;

  TCanvas *c25000_05ns = new TCanvas();
  c25000_05ns->cd();
  Histo_05ns20_25000->SetLineColorAlpha(kBlue+3,0.65);
  Histo_05ns20_25000->SetTitle( "M^{2}_{miss} for different A' masses (25000 e+ per bunch)");
  Histo_05ns20_25000->SetLineWidth(3);
  Histo_05ns20_25000->GetYaxis()->SetTitle("N_{events}");
  Histo_05ns20_25000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_05ns19_25000->SetLineColorAlpha(kBlue,0.65);
  Histo_05ns19_25000->SetLineWidth(3);
  Histo_05ns18_25000->SetLineColorAlpha(kCyan+3,0.65);
  Histo_05ns18_25000->SetLineWidth(3);
  Histo_05ns16_25000->SetLineColorAlpha(kCyan+1,0.65);
  Histo_05ns16_25000->SetLineWidth(3);
  Histo_05ns14_25000->SetLineColorAlpha(kGreen+3,0.65);
  Histo_05ns14_25000->SetLineWidth(3);
  Histo_05ns12_25000->SetLineColorAlpha(kGreen+1,0.65);
  Histo_05ns12_25000->SetLineWidth(3);
  Histo_05ns10_25000->SetLineColorAlpha(kYellow+2,0.65);
  Histo_05ns10_25000->SetLineWidth(3);
  Histo_05ns8_25000->SetLineColorAlpha(kOrange-3,0.65);
  Histo_05ns8_25000->SetLineWidth(3);
  Histo_05ns6_25000->SetLineColorAlpha(kRed+2,0.65);
  Histo_05ns6_25000->SetLineWidth(3);
  Histo_05ns4_25000->SetLineColorAlpha(kRed,0.65);
  Histo_05ns4_25000->SetLineWidth(3);
  Histo_05ns2_25000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_05ns2_25000->SetLineWidth(3);
    
  Histo_05ns20_25000->Draw();
  Histo_05ns19_25000->Draw("same");
  Histo_05ns18_25000->Draw("same");
  Histo_05ns4_25000->Draw("same");
  Histo_05ns6_25000->Draw("same");
  Histo_05ns8_25000->Draw("same");
  Histo_05ns10_25000->Draw("same");
  Histo_05ns12_25000->Draw("same");
  Histo_05ns14_25000->Draw("same");
  Histo_05ns16_25000->Draw("same");
  Histo_05ns2_25000->Draw("same");

  auto legend_25000_05ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_25000_05ns->AddEntry(Histo_05ns20_25000,"M_{A'} = 20 MeV","l");
  legend_25000_05ns->AddEntry(Histo_05ns19_25000,"M_{A'} = 19 MeV","l");
  legend_25000_05ns->AddEntry(Histo_05ns18_25000,"M_{A'} = 18 MeV","l");
  legend_25000_05ns->AddEntry(Histo_05ns16_25000,"M_{A'} = 16 MeV","l");
  legend_25000_05ns->AddEntry(Histo_05ns14_25000,"M_{A'} = 14 MeV","l");
  legend_25000_05ns->AddEntry(Histo_05ns12_25000,"M_{A'} = 12 MeV","l");
  legend_25000_05ns->AddEntry(Histo_05ns10_25000,"M_{A'} = 10 MeV","l");
  legend_25000_05ns->AddEntry(Histo_05ns8_25000,"M_{A'} = 8 MeV","l");
  legend_25000_05ns->AddEntry(Histo_05ns6_25000,"M_{A'} = 6 MeV","l");
  legend_25000_05ns->AddEntry(Histo_05ns4_25000,"M_{A'} = 4 MeV","l");
  legend_25000_05ns->AddEntry(Histo_05ns2_25000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_25000_05ns->Draw();

  outRoot->WriteObject(c25000_05ns,"25000e_all_05ns");

  
  TCanvas *cgr_05ns = new TCanvas();
 
  auto gr_1_05ns = new TGraph (11,masses,entries_05ns_1);
  gr_1_05ns->SetLineColor(kMagenta);
  gr_1_05ns->SetLineWidth(3); 
  gr_1_05ns->GetYaxis()->SetTitle("Acceptance [%]");
  gr_1_05ns->GetXaxis()->SetTitle("M_{A'} [MeV]");
  gr_1_05ns->SetTitle("Acceptance");
  gr_1_05ns->Draw();
  auto gr_10_05ns = new TGraph (11,masses,entries_05ns_10);
  gr_10_05ns->SetLineColor(kRed);
  gr_10_05ns->SetLineWidth(3);
  gr_10_05ns->Draw("same");
  auto gr_100_05ns = new TGraph (11,masses,entries_05ns_100);
  gr_100_05ns->SetLineColor(kOrange);
  gr_100_05ns->SetLineWidth(3);
  gr_100_05ns->Draw("same");
  auto gr_1000_05ns = new TGraph (11,masses,entries_05ns_1000);
  gr_1000_05ns->SetLineColor(kGreen+3);
  gr_1000_05ns->SetLineWidth(3);
  gr_1000_05ns->Draw("same");
  auto gr_10000_05ns = new TGraph (11,masses,entries_05ns_10000);
  gr_10000_05ns->SetLineColor(kCyan+1);
  gr_10000_05ns->SetLineWidth(3);
  gr_10000_05ns->Draw("same");
  auto gr_25000_05ns = new TGraph (11,masses,entries_05ns_25000);
  gr_25000_05ns->SetLineColor(kBlue);
  gr_25000_05ns->SetLineWidth(3);
  gr_25000_05ns->Draw("same");
  auto legend_gr_05ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_gr_05ns->AddEntry(gr_1_05ns,"1 e+","l");
  legend_gr_05ns->AddEntry(gr_10_05ns,"10 e+","l");
  legend_gr_05ns->AddEntry(gr_100_05ns,"100 e+","l");
  legend_gr_05ns->AddEntry(gr_1000_05ns,"1000 e+","l");
  legend_gr_05ns->AddEntry(gr_10000_05ns,"10000 e+","l");
  legend_gr_05ns->AddEntry(gr_25000_05ns,"25000 e+","l");
  legend_gr_05ns->Draw();

  TCanvas *csig_05ns = new TCanvas();
 
  auto sig_1_05ns = new TGraph (11,masses,sigmas_05ns_1);
  sig_1_05ns->SetLineColor(kMagenta);
  sig_1_05ns->SetLineWidth(3);
  sig_1_05ns->GetYaxis()->SetTitle("Sigma");
  sig_1_05ns->GetXaxis()->SetTitle("M_{A'} [MeV]");
  sig_1_05ns->SetTitle("Sigma");
  sig_1_05ns->Draw();
  auto sig_10_05ns = new TGraph (11,masses,sigmas_05ns_10);
  sig_10_05ns->SetLineColor(kRed);
  sig_10_05ns->SetLineWidth(3);
  sig_10_05ns->Draw("same");
  auto sig_100_05ns = new TGraph (11,masses,sigmas_05ns_100);
  sig_100_05ns->SetLineColor(kOrange);
  sig_100_05ns->SetLineWidth(3);
  sig_100_05ns->Draw("same");
  auto sig_1000_05ns = new TGraph (11,masses,sigmas_05ns_1000);
  sig_1000_05ns->SetLineColor(kGreen+3);
  sig_1000_05ns->SetLineWidth(3);
  sig_1000_05ns->Draw("same");
  auto sig_10000_05ns = new TGraph (11,masses,sigmas_05ns_10000);
  sig_10000_05ns->SetLineColor(kCyan+1);
  sig_10000_05ns->SetLineWidth(3);
  sig_10000_05ns->Draw("same");
  auto sig_25000_05ns = new TGraph (11,masses,sigmas_05ns_25000);
  sig_25000_05ns->SetLineColor(kBlue);
  sig_25000_05ns->SetLineWidth(3);
  sig_25000_05ns->Draw("same");
  auto legend_sig_05ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_sig_05ns->AddEntry(sig_1_05ns,"1 e+","l");
  legend_sig_05ns->AddEntry(sig_10_05ns,"10 e+","l");
  legend_sig_05ns->AddEntry(sig_100_05ns,"100 e+","l");
  legend_sig_05ns->AddEntry(sig_1000_05ns,"1000 e+","l");
  legend_sig_05ns->AddEntry(sig_10000_05ns,"10000 e+","l");
  legend_sig_05ns->AddEntry(sig_25000_05ns,"25000 e+","l");
  legend_sig_05ns->Draw();

  
  outRoot->WriteObject(cgr_05ns,"Acceptance_05ns");
  outRoot->WriteObject(csig_05ns,"Sigma_05ns");

  


 //----------------------------------------Do analysis for deltaT=1ns-------------------------------------------------Do analysis for deltaT=1ns-------------------------------------------------


  Float_t sigmas_1ns_1[11];
  Float_t means_1ns_1[11];
  Float_t entries_1ns_1[11];

  Float_t sigmas_1ns_10[11];
  Float_t means_1ns_10[11];
  Float_t entries_1ns_10[11];

  Float_t sigmas_1ns_100[11];
  Float_t means_1ns_100[11];
  Float_t entries_1ns_100[11];

  Float_t sigmas_1ns_1000[11];
  Float_t means_1ns_1000[11];
  Float_t entries_1ns_1000[11];

  Float_t sigmas_1ns_10000[11];
  Float_t means_1ns_10000[11];
  Float_t entries_1ns_10000[11];
  
  Float_t sigmas_1ns_25000[11];
  Float_t means_1ns_25000[11];
  Float_t entries_1ns_25000[11];
   
  //---------------------------------------------- Open Histograms --------------------------------------------------

  TH1F *Histo_1ns20_1 = (TH1F*) dir20_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns19_1 = (TH1F*) dir19_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns18_1 = (TH1F*) dir18_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_1ns");
  TH1F *Histo_1ns16_1 = (TH1F*) dir16_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns14_1 = (TH1F*) dir14_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns12_1 = (TH1F*) dir12_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns10_1 = (TH1F*) dir10_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns8_1 = (TH1F*) dir8_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns6_1 = (TH1F*) dir6_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns4_1 = (TH1F*) dir4_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns2_1 = (TH1F*) dir2_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns0_1_10 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_1ns");
  TH1F *Histo_1ns0_1_20 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_1ns");
  TH1F *Histo_1ns0_1_30 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns0_1_40 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_1ns");
  TH1F *Histo_1ns0_1_50 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_1ns");
  TH1F *Histo_1ns0_1_60 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  
  TH1F *Histo_1ns20_10 = (TH1F*) dir20_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns19_10 = (TH1F*) dir19_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns18_10 = (TH1F*) dir18_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_1ns");
  TH1F *Histo_1ns16_10 = (TH1F*) dir16_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns14_10 = (TH1F*) dir14_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns12_10 = (TH1F*) dir12_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns10_10 = (TH1F*) dir10_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns8_10 = (TH1F*) dir8_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns6_10 = (TH1F*) dir6_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns4_10 = (TH1F*) dir4_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns2_10 = (TH1F*) dir2_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns0_10_10 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_1ns");
  TH1F *Histo_1ns0_10_20 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_1ns");
  TH1F *Histo_1ns0_10_30 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns0_10_40 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_1ns");
  TH1F *Histo_1ns0_10_50 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_1ns");
  TH1F *Histo_1ns0_10_60 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  
  TH1F *Histo_1ns20_100 = (TH1F*) dir20_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns19_100 = (TH1F*) dir19_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns18_100 = (TH1F*) dir18_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_1ns");
  TH1F *Histo_1ns16_100 = (TH1F*) dir16_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns14_100 = (TH1F*) dir14_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns12_100 = (TH1F*) dir12_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns10_100 = (TH1F*) dir10_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns8_100 = (TH1F*) dir8_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns6_100 = (TH1F*) dir6_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns4_100 = (TH1F*) dir4_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns2_100 = (TH1F*) dir2_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns0_100_10 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_1ns");
  TH1F *Histo_1ns0_100_20 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_1ns");
  TH1F *Histo_1ns0_100_30 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns0_100_40 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_1ns");
  TH1F *Histo_1ns0_100_50 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_1ns");
  TH1F *Histo_1ns0_100_60 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  
  TH1F *Histo_1ns20_1000 = (TH1F*) dir20_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns19_1000 = (TH1F*) dir19_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns18_1000 = (TH1F*) dir18_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_1ns");
  TH1F *Histo_1ns16_1000 = (TH1F*) dir16_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns14_1000 = (TH1F*) dir14_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns12_1000 = (TH1F*) dir12_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns10_1000 = (TH1F*) dir10_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns8_1000 = (TH1F*) dir8_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns6_1000 = (TH1F*) dir6_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns4_1000 = (TH1F*) dir4_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns2_1000 = (TH1F*) dir2_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns0_1000_10 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_1ns");
  TH1F *Histo_1ns0_1000_20 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_1ns");
  TH1F *Histo_1ns0_1000_30 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns0_1000_40 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_1ns");
  TH1F *Histo_1ns0_1000_50 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_1ns");
  TH1F *Histo_1ns0_1000_60 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  
  TH1F *Histo_1ns20_10000 = (TH1F*) dir20_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns19_10000 = (TH1F*) dir19_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns18_10000 = (TH1F*) dir18_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_1ns");
  TH1F *Histo_1ns16_10000 = (TH1F*) dir16_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns14_10000 = (TH1F*) dir14_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns12_10000 = (TH1F*) dir12_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns10_10000 = (TH1F*) dir10_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns8_10000 = (TH1F*) dir8_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns6_10000 = (TH1F*) dir6_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns4_10000 = (TH1F*) dir4_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns2_10000 = (TH1F*) dir2_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns0_10000_10 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_1ns");
  TH1F *Histo_1ns0_10000_20 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_1ns");
  TH1F *Histo_1ns0_10000_30 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns0_10000_40 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_1ns");
  TH1F *Histo_1ns0_10000_50 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_1ns");
  TH1F *Histo_1ns0_10000_60 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  
  TH1F *Histo_1ns20_25000 = (TH1F*) dir20_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns19_25000 = (TH1F*) dir19_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns18_25000 = (TH1F*) dir18_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_1ns");
  TH1F *Histo_1ns16_25000 = (TH1F*) dir16_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns14_25000 = (TH1F*) dir14_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns12_25000 = (TH1F*) dir12_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns10_25000 = (TH1F*) dir10_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns8_25000 = (TH1F*) dir8_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns6_25000 = (TH1F*) dir6_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns4_25000 = (TH1F*) dir4_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns2_25000 = (TH1F*) dir2_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");
  TH1F *Histo_1ns0_25000_10 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_1ns");
  TH1F *Histo_1ns0_25000_20 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_1ns");
  TH1F *Histo_1ns0_25000_30 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_1ns");
  TH1F *Histo_1ns0_25000_40 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_1ns");
  TH1F *Histo_1ns0_25000_50 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_1ns");
  TH1F *Histo_1ns0_25000_60 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_1ns");

 
  Histo_1ns2_1->Fit(f_2,"R");
  means_1ns_1[0]=f_2->GetParameter(1);
  sigmas_1ns_1[0]=f_2->GetParameter(2);

  Histo_1ns4_1->Fit(f_4,"R");
  means_1ns_1[1]=f_4->GetParameter(1);
  sigmas_1ns_1[1]=f_4->GetParameter(2);
  
  Histo_1ns6_1->Fit(f_6,"R");
  means_1ns_1[2]=f_6->GetParameter(1);
  sigmas_1ns_1[2]=f_6->GetParameter(2);
  
  Histo_1ns8_1->Fit(f_8,"R");
  means_1ns_1[3]=f_8->GetParameter(1);
  sigmas_1ns_1[3]=f_8->GetParameter(2);
  
  Histo_1ns10_1->Fit(f_10,"R");
  means_1ns_1[4]=f_10->GetParameter(1);
  sigmas_1ns_1[4]=f_10->GetParameter(2);
  
  Histo_1ns12_1->Fit(f_12,"R");
  means_1ns_1[5]=f_12->GetParameter(1);
  sigmas_1ns_1[5]=f_12->GetParameter(2);
  
  Histo_1ns14_1->Fit(f_14,"R");
  means_1ns_1[6]=f_14->GetParameter(1);
  sigmas_1ns_1[6]=f_14->GetParameter(2);
  
  Histo_1ns16_1->Fit(f_16,"R");
  means_1ns_1[7]=f_16->GetParameter(1);
  sigmas_1ns_1[7]=f_16->GetParameter(2);
  
  Histo_1ns18_1->Fit(f_18,"R");
  means_1ns_1[8]=f_18->GetParameter(1);
  sigmas_1ns_1[8]=f_18->GetParameter(2);
  
  Histo_1ns19_1->Fit(f_19,"R");
  means_1ns_1[9]=f_19->GetParameter(1);
  sigmas_1ns_1[9]=f_19->GetParameter(2);
  
  Histo_1ns20_1->Fit(f_20,"R");
  means_1ns_1[10]=f_20->GetParameter(1);
  sigmas_1ns_1[10]=f_20->GetParameter(2);
  
  entries_1ns_1[0]=Histo_1ns2_1->Integral(500+(means_1ns_1[0]-nsigma*sigmas_1ns_1[0]),500+(means_1ns_1[0]+nsigma*sigmas_1ns_1[0]))/1000;
  entries_1ns_1[1]=Histo_1ns4_1->Integral(500+(means_1ns_1[1]-nsigma*sigmas_1ns_1[1]),500+(means_1ns_1[1]+nsigma*sigmas_1ns_1[1]))/1000;
  entries_1ns_1[2]=Histo_1ns6_1->Integral(500+(means_1ns_1[2]-nsigma*sigmas_1ns_1[2]),500+(means_1ns_1[2]+nsigma*sigmas_1ns_1[2]))/1000;
  entries_1ns_1[3]=Histo_1ns8_1->Integral(500+(means_1ns_1[3]-nsigma*sigmas_1ns_1[3]),500+(means_1ns_1[3]+nsigma*sigmas_1ns_1[3]))/1000;
  entries_1ns_1[4]=Histo_1ns10_1->Integral(500+(means_1ns_1[4]-nsigma*sigmas_1ns_1[4]),500+(means_1ns_1[4]+nsigma*sigmas_1ns_1[4]))/1000;
  entries_1ns_1[5]=Histo_1ns12_1->Integral(500+(means_1ns_1[5]-nsigma*sigmas_1ns_1[5]),500+(means_1ns_1[5]+nsigma*sigmas_1ns_1[5]))/1000;
  entries_1ns_1[6]=Histo_1ns14_1->Integral(500+(means_1ns_1[6]-nsigma*sigmas_1ns_1[6]),500+(means_1ns_1[6]+nsigma*sigmas_1ns_1[6]))/1000;
  entries_1ns_1[7]=Histo_1ns16_1->Integral(500+(means_1ns_1[7]-nsigma*sigmas_1ns_1[7]),500+(means_1ns_1[7]+nsigma*sigmas_1ns_1[7]))/1000;
  entries_1ns_1[8]=Histo_1ns18_1->Integral(500+(means_1ns_1[8]-nsigma*sigmas_1ns_1[8]),500+(means_1ns_1[8]+nsigma*sigmas_1ns_1[8]))/1000;
  entries_1ns_1[9]=Histo_1ns19_1->Integral(500+(means_1ns_1[9]-nsigma*sigmas_1ns_1[9]),500+(means_1ns_1[9]+nsigma*sigmas_1ns_1[9]))/1000;
  entries_1ns_1[10]=Histo_1ns20_1->Integral(500+(means_1ns_1[10]-nsigma*sigmas_1ns_1[10]),500+(means_1ns_1[10]+nsigma*sigmas_1ns_1[10]))/1000;
  
  TCanvas *c1_1ns = new TCanvas();
  c1_1ns->cd();
  Histo_1ns20_1->SetLineColorAlpha(kBlue+3,0.65);
  Histo_1ns20_1->SetTitle( "M^{2}_{miss} for different A' masses (1 e+ per bunch)");
  Histo_1ns20_1->SetLineWidth(3);
  Histo_1ns20_1->GetYaxis()->SetTitle("N_{events}");
  Histo_1ns20_1->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_1ns19_1->SetLineColorAlpha(kBlue,0.65);
  Histo_1ns19_1->SetLineWidth(3);
  Histo_1ns18_1->SetLineColorAlpha(kCyan+3,0.65);
  Histo_1ns18_1->SetLineWidth(3);
  Histo_1ns16_1->SetLineColorAlpha(kCyan+1,0.65);
  Histo_1ns16_1->SetLineWidth(3);
  Histo_1ns14_1->SetLineColorAlpha(kGreen+3,0.65);
  Histo_1ns14_1->SetLineWidth(3);
  Histo_1ns12_1->SetLineColorAlpha(kGreen+1,0.65);
  Histo_1ns12_1->SetLineWidth(3);
  Histo_1ns10_1->SetLineColorAlpha(kYellow+2,0.65);
  Histo_1ns10_1->SetLineWidth(3);
  Histo_1ns8_1->SetLineColorAlpha(kOrange-3,0.65);
  Histo_1ns8_1->SetLineWidth(3);
  Histo_1ns6_1->SetLineColorAlpha(kRed+2,0.65);
  Histo_1ns6_1->SetLineWidth(3);
  Histo_1ns4_1->SetLineColorAlpha(kRed,0.65);
  Histo_1ns4_1->SetLineWidth(3);
  Histo_1ns2_1->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_1ns2_1->SetLineWidth(3);
    
  Histo_1ns20_1->Draw();
  Histo_1ns19_1->Draw("same");
  Histo_1ns18_1->Draw("same");
  Histo_1ns4_1->Draw("same");
  Histo_1ns6_1->Draw("same");
  Histo_1ns8_1->Draw("same");
  Histo_1ns10_1->Draw("same");
  Histo_1ns12_1->Draw("same");
  Histo_1ns14_1->Draw("same");
  Histo_1ns16_1->Draw("same");
  Histo_1ns2_1->Draw("same");

  auto legend_1_1ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_1_1ns->AddEntry(Histo_1ns20_1,"M_{A'} = 20 MeV","l");
  legend_1_1ns->AddEntry(Histo_1ns19_1,"M_{A'} = 19 MeV","l");
  legend_1_1ns->AddEntry(Histo_1ns18_1,"M_{A'} = 18 MeV","l");
  legend_1_1ns->AddEntry(Histo_1ns16_1,"M_{A'} = 16 MeV","l");
  legend_1_1ns->AddEntry(Histo_1ns14_1,"M_{A'} = 14 MeV","l");
  legend_1_1ns->AddEntry(Histo_1ns12_1,"M_{A'} = 12 MeV","l");
  legend_1_1ns->AddEntry(Histo_1ns10_1,"M_{A'} = 10 MeV","l");
  legend_1_1ns->AddEntry(Histo_1ns8_1,"M_{A'} = 8 MeV","l");
  legend_1_1ns->AddEntry(Histo_1ns6_1,"M_{A'} = 6 MeV","l");
  legend_1_1ns->AddEntry(Histo_1ns4_1,"M_{A'} = 4 MeV","l");
  legend_1_1ns->AddEntry(Histo_1ns2_1,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_1_1ns->Draw();

  outRoot->WriteObject(c1_1ns,"1e_all_1ns");

  if(!(Histo_1ns20_10->GetSumw2N() > 0)) Histo_1ns20_10->Sumw2(kTRUE);
  Histo_1ns20_10->Add(Histo_1ns0_10_30,-1);
  if(!(Histo_1ns19_10->GetSumw2N() > 0)) Histo_1ns19_10->Sumw2(kTRUE);
  Histo_1ns19_10->Add(Histo_1ns0_10_30,-1);
  if(!(Histo_1ns18_10->GetSumw2N() > 0)) Histo_1ns18_10->Sumw2(kTRUE);
  Histo_1ns18_10->Add(Histo_1ns0_10_50,-1);
  if(!(Histo_1ns16_10->GetSumw2N() > 0)) Histo_1ns16_10->Sumw2(kTRUE);
  Histo_1ns16_10->Add(Histo_1ns0_10_60,-1);
  if(!(Histo_1ns14_10->GetSumw2N() > 0)) Histo_1ns14_10->Sumw2(kTRUE);
  Histo_1ns14_10->Add(Histo_1ns0_10_60,-1);
  if(!(Histo_1ns12_10->GetSumw2N() > 0)) Histo_1ns12_10->Sumw2(kTRUE);
  Histo_1ns12_10->Add(Histo_1ns0_10_60,-1);
  if(!(Histo_1ns10_10->GetSumw2N() > 0)) Histo_1ns10_10->Sumw2(kTRUE);
  Histo_1ns10_10->Add(Histo_1ns0_10_60,-1);
  if(!(Histo_1ns8_10->GetSumw2N() > 0)) Histo_1ns8_10->Sumw2(kTRUE);
  Histo_1ns8_10->Add(Histo_1ns0_10_60,-1);
  if(!(Histo_1ns6_10->GetSumw2N() > 0)) Histo_1ns6_10->Sumw2(kTRUE);
  Histo_1ns6_10->Add(Histo_1ns0_10_60,-1);
  if(!(Histo_1ns4_10->GetSumw2N() > 0)) Histo_1ns4_10->Sumw2(kTRUE);
  Histo_1ns4_10->Add(Histo_1ns0_10_60,-1);
  if(!(Histo_1ns2_10->GetSumw2N() > 0)) Histo_1ns2_10->Sumw2(kTRUE);
  Histo_1ns2_10->Add(Histo_1ns0_10_60,-1);
  
  Histo_1ns2_10->Fit(f_2,"R");
  means_1ns_10[0]=f_2->GetParameter(1);
  sigmas_1ns_10[0]=f_2->GetParameter(2);

  Histo_1ns4_10->Fit(f_4,"R");
  means_1ns_10[1]=f_4->GetParameter(1);
  sigmas_1ns_10[1]=f_4->GetParameter(2);
  
  Histo_1ns6_10->Fit(f_6,"R");
  means_1ns_10[2]=f_6->GetParameter(1);
  sigmas_1ns_10[2]=f_6->GetParameter(2);
  
  Histo_1ns8_10->Fit(f_8,"R");
  means_1ns_10[3]=f_8->GetParameter(1);
  sigmas_1ns_10[3]=f_8->GetParameter(2);
 
  Histo_1ns10_10->Fit(f_10,"R");
  means_1ns_10[4]=f_10->GetParameter(1);
  sigmas_1ns_10[4]=f_10->GetParameter(2);
  
  Histo_1ns12_10->Fit(f_12,"R");
  means_1ns_10[5]=f_12->GetParameter(1);
  sigmas_1ns_10[5]=f_12->GetParameter(2);
  
  Histo_1ns14_10->Fit(f_14,"R");
  means_1ns_10[6]=f_14->GetParameter(1);
  sigmas_1ns_10[6]=f_14->GetParameter(2);
  
  Histo_1ns16_10->Fit(f_16,"R");
  means_1ns_10[7]=f_16->GetParameter(1);
  sigmas_1ns_10[7]=f_16->GetParameter(2);
  
  Histo_1ns18_10->Fit(f_18,"R");
  means_1ns_10[8]=f_18->GetParameter(1);
  sigmas_1ns_10[8]=f_18->GetParameter(2);
  
  Histo_1ns19_10->Fit(f_19,"R");
  means_1ns_10[9]=f_19->GetParameter(1);
  sigmas_1ns_10[9]=f_19->GetParameter(2);
  
  Histo_1ns20_10->Fit(f_20,"R");
  means_1ns_10[10]=f_20->GetParameter(1);
  sigmas_1ns_10[10]=f_20->GetParameter(2);

  entries_1ns_10[0]=Histo_1ns2_10->Integral(500+(means_1ns_10[0]-nsigma*sigmas_1ns_10[0]),500+(means_1ns_10[0]+nsigma*sigmas_1ns_10[0]))/1000;
  entries_1ns_10[1]=Histo_1ns4_10->Integral(500+(means_1ns_10[1]-nsigma*sigmas_1ns_10[1]),500+(means_1ns_10[1]+nsigma*sigmas_1ns_10[1]))/1000;
  entries_1ns_10[2]=Histo_1ns6_10->Integral(500+(means_1ns_10[2]-nsigma*sigmas_1ns_10[2]),500+(means_1ns_10[2]+nsigma*sigmas_1ns_10[2]))/1000;
  entries_1ns_10[3]=Histo_1ns8_10->Integral(500+(means_1ns_10[3]-nsigma*sigmas_1ns_10[3]),500+(means_1ns_10[3]+nsigma*sigmas_1ns_10[3]))/1000;
  entries_1ns_10[4]=Histo_1ns10_10->Integral(500+(means_1ns_10[4]-nsigma*sigmas_1ns_10[4]),500+(means_1ns_10[4]+nsigma*sigmas_1ns_10[4]))/1000;
  entries_1ns_10[5]=Histo_1ns12_10->Integral(500+(means_1ns_10[5]-nsigma*sigmas_1ns_10[5]),500+(means_1ns_10[5]+nsigma*sigmas_1ns_10[5]))/1000;
  entries_1ns_10[6]=Histo_1ns14_10->Integral(500+(means_1ns_10[6]-nsigma*sigmas_1ns_10[6]),500+(means_1ns_10[6]+nsigma*sigmas_1ns_10[6]))/1000;
  entries_1ns_10[7]=Histo_1ns16_10->Integral(500+(means_1ns_10[7]-nsigma*sigmas_1ns_10[7]),500+(means_1ns_10[7]+nsigma*sigmas_1ns_10[7]))/1000;
  entries_1ns_10[8]=Histo_1ns18_10->Integral(500+(means_1ns_10[8]-nsigma*sigmas_1ns_10[8]),500+(means_1ns_10[8]+nsigma*sigmas_1ns_10[8]))/1000;
  entries_1ns_10[9]=Histo_1ns19_10->Integral(500+(means_1ns_10[9]-nsigma*sigmas_1ns_10[9]),500+(means_1ns_10[9]+nsigma*sigmas_1ns_10[9]))/1000;
  entries_1ns_10[10]=Histo_1ns20_10->Integral(500+(means_1ns_10[10]-nsigma*sigmas_1ns_10[10]),500+(means_1ns_10[10]+nsigma*sigmas_1ns_10[10]))/1000;

  TCanvas *c10_1ns = new TCanvas();
  c10_1ns->cd();
  Histo_1ns20_10->SetLineColorAlpha(kBlue+3,0.65);
  Histo_1ns20_10->SetTitle( "M^{2}_{miss} for different A' masses (10 e+ per bunch)");
  Histo_1ns20_10->SetLineWidth(3);
  Histo_1ns20_10->GetYaxis()->SetTitle("N_{events}");
  Histo_1ns20_10->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_1ns19_10->SetLineColorAlpha(kBlue,0.65);
  Histo_1ns19_10->SetLineWidth(3);
  Histo_1ns18_10->SetLineColorAlpha(kCyan+3,0.65);
  Histo_1ns18_10->SetLineWidth(3);
  Histo_1ns16_10->SetLineColorAlpha(kCyan+1,0.65);
  Histo_1ns16_10->SetLineWidth(3);
  Histo_1ns14_10->SetLineColorAlpha(kGreen+3,0.65);
  Histo_1ns14_10->SetLineWidth(3);
  Histo_1ns12_10->SetLineColorAlpha(kGreen+1,0.65);
  Histo_1ns12_10->SetLineWidth(3);
  Histo_1ns10_10->SetLineColorAlpha(kYellow+2,0.65);
  Histo_1ns10_10->SetLineWidth(3);
  Histo_1ns8_10->SetLineColorAlpha(kOrange-3,0.65);
  Histo_1ns8_10->SetLineWidth(3);
  Histo_1ns6_10->SetLineColorAlpha(kRed+2,0.65);
  Histo_1ns6_10->SetLineWidth(3);
  Histo_1ns4_10->SetLineColorAlpha(kRed,0.65);
  Histo_1ns4_10->SetLineWidth(3);
  Histo_1ns2_10->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_1ns2_10->SetLineWidth(3);
    
  Histo_1ns20_10->Draw();
  Histo_1ns19_10->Draw("same");
  Histo_1ns18_10->Draw("same");
  Histo_1ns4_10->Draw("same");
  Histo_1ns6_10->Draw("same");
  Histo_1ns8_10->Draw("same");
  Histo_1ns10_10->Draw("same");
  Histo_1ns12_10->Draw("same");
  Histo_1ns14_10->Draw("same");
  Histo_1ns16_10->Draw("same");
  Histo_1ns2_10->Draw("same");

  auto legend_10_1ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_10_1ns->AddEntry(Histo_1ns20_10,"M_{A'} = 20 MeV","l");
  legend_10_1ns->AddEntry(Histo_1ns19_10,"M_{A'} = 19 MeV","l");
  legend_10_1ns->AddEntry(Histo_1ns18_10,"M_{A'} = 18 MeV","l");
  legend_10_1ns->AddEntry(Histo_1ns16_10,"M_{A'} = 16 MeV","l");
  legend_10_1ns->AddEntry(Histo_1ns14_10,"M_{A'} = 14 MeV","l");
  legend_10_1ns->AddEntry(Histo_1ns12_10,"M_{A'} = 12 MeV","l");
  legend_10_1ns->AddEntry(Histo_1ns10_10,"M_{A'} = 10 MeV","l");
  legend_10_1ns->AddEntry(Histo_1ns8_10,"M_{A'} = 8 MeV","l");
  legend_10_1ns->AddEntry(Histo_1ns6_10,"M_{A'} = 6 MeV","l");
  legend_10_1ns->AddEntry(Histo_1ns4_10,"M_{A'} = 4 MeV","l");
  legend_10_1ns->AddEntry(Histo_1ns2_10,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_10_1ns->Draw();

  outRoot->WriteObject(c10_1ns,"10e_all_1ns");
 
  if(!(Histo_1ns20_100->GetSumw2N() > 0)) Histo_1ns20_100->Sumw2(kTRUE);
  Histo_1ns20_100->Add(Histo_1ns0_100_30,-1);
  if(!(Histo_1ns19_100->GetSumw2N() > 0)) Histo_1ns19_100->Sumw2(kTRUE);
  Histo_1ns19_100->Add(Histo_1ns0_100_30,-1);
  if(!(Histo_1ns18_100->GetSumw2N() > 0)) Histo_1ns18_100->Sumw2(kTRUE);
  Histo_1ns18_100->Add(Histo_1ns0_100_50,-1);
  if(!(Histo_1ns16_100->GetSumw2N() > 0)) Histo_1ns16_100->Sumw2(kTRUE);
  Histo_1ns16_100->Add(Histo_1ns0_100_60,-1);
  if(!(Histo_1ns14_100->GetSumw2N() > 0)) Histo_1ns14_100->Sumw2(kTRUE);
  Histo_1ns14_100->Add(Histo_1ns0_100_60,-1);
  if(!(Histo_1ns12_100->GetSumw2N() > 0)) Histo_1ns12_100->Sumw2(kTRUE);
  Histo_1ns12_100->Add(Histo_1ns0_100_60,-1);
  if(!(Histo_1ns10_100->GetSumw2N() > 0)) Histo_1ns10_100->Sumw2(kTRUE);
  Histo_1ns10_100->Add(Histo_1ns0_100_60,-1);
  if(!(Histo_1ns8_100->GetSumw2N() > 0)) Histo_1ns8_100->Sumw2(kTRUE);
  Histo_1ns8_100->Add(Histo_1ns0_100_60,-1);
  if(!(Histo_1ns6_100->GetSumw2N() > 0)) Histo_1ns6_100->Sumw2(kTRUE);
  Histo_1ns6_100->Add(Histo_1ns0_100_60,-1);
  if(!(Histo_1ns4_100->GetSumw2N() > 0)) Histo_1ns4_100->Sumw2(kTRUE);
  Histo_1ns4_100->Add(Histo_1ns0_100_60,-1);
  if(!(Histo_1ns2_100->GetSumw2N() > 0)) Histo_1ns2_100->Sumw2(kTRUE);
  Histo_1ns2_100->Add(Histo_1ns0_100_60,-1);


  Histo_1ns2_100->Fit(f_2,"R");
  means_1ns_100[0]=f_2->GetParameter(1);
  sigmas_1ns_100[0]=f_2->GetParameter(2);

  Histo_1ns4_100->Fit(f_4,"R");
  means_1ns_100[1]=f_4->GetParameter(1);
  sigmas_1ns_100[1]=f_4->GetParameter(2);
  
  Histo_1ns6_100->Fit(f_6,"R");
  means_1ns_100[2]=f_6->GetParameter(1);
  sigmas_1ns_100[2]=f_6->GetParameter(2);
  
  Histo_1ns8_100->Fit(f_8,"R");
  means_1ns_100[3]=f_8->GetParameter(1);
  sigmas_1ns_100[3]=f_8->GetParameter(2);
 
  Histo_1ns10_100->Fit(f_10,"R");
  means_1ns_100[4]=f_10->GetParameter(1);
  sigmas_1ns_100[4]=f_10->GetParameter(2);
  
  Histo_1ns12_100->Fit(f_12,"R");
  means_1ns_100[5]=f_12->GetParameter(1);
  sigmas_1ns_100[5]=f_12->GetParameter(2);
  
  Histo_1ns14_100->Fit(f_14,"R");
  means_1ns_100[6]=f_14->GetParameter(1);
  sigmas_1ns_100[6]=f_14->GetParameter(2);
  
  Histo_1ns16_100->Fit(f_16,"R");
  means_1ns_100[7]=f_16->GetParameter(1);
  sigmas_1ns_100[7]=f_16->GetParameter(2);
  
  Histo_1ns18_100->Fit(f_18,"R");
  means_1ns_100[8]=f_18->GetParameter(1);
  sigmas_1ns_100[8]=f_18->GetParameter(2);
  
  Histo_1ns19_100->Fit(f_19,"R");
  means_1ns_100[9]=f_19->GetParameter(1);
  sigmas_1ns_100[9]=f_19->GetParameter(2);
  
  Histo_1ns20_100->Fit(f_20,"R");
  means_1ns_100[10]=f_20->GetParameter(1);
  sigmas_1ns_100[10]=f_20->GetParameter(2);

  entries_1ns_100[0]=Histo_1ns2_100->Integral(500+(means_1ns_100[0]-nsigma*sigmas_1ns_100[0]),500+(means_1ns_100[0]+nsigma*sigmas_1ns_100[0]))/1000;
  entries_1ns_100[1]=Histo_1ns4_100->Integral(500+(means_1ns_100[1]-nsigma*sigmas_1ns_100[1]),500+(means_1ns_100[1]+nsigma*sigmas_1ns_100[1]))/1000;
  entries_1ns_100[2]=Histo_1ns6_100->Integral(500+(means_1ns_100[2]-nsigma*sigmas_1ns_100[2]),500+(means_1ns_100[2]+nsigma*sigmas_1ns_100[2]))/1000;
  entries_1ns_100[3]=Histo_1ns8_100->Integral(500+(means_1ns_100[3]-nsigma*sigmas_1ns_100[3]),500+(means_1ns_100[3]+nsigma*sigmas_1ns_100[3]))/1000;
  entries_1ns_100[4]=Histo_1ns10_100->Integral(500+(means_1ns_100[4]-nsigma*sigmas_1ns_100[4]),500+(means_1ns_100[4]+nsigma*sigmas_1ns_100[4]))/1000;
  entries_1ns_100[5]=Histo_1ns12_100->Integral(500+(means_1ns_100[5]-nsigma*sigmas_1ns_100[5]),500+(means_1ns_100[5]+nsigma*sigmas_1ns_100[5]))/1000;
  entries_1ns_100[6]=Histo_1ns14_100->Integral(500+(means_1ns_100[6]-nsigma*sigmas_1ns_100[6]),500+(means_1ns_100[6]+nsigma*sigmas_1ns_100[6]))/1000;
  entries_1ns_100[7]=Histo_1ns16_100->Integral(500+(means_1ns_100[7]-nsigma*sigmas_1ns_100[7]),500+(means_1ns_100[7]+nsigma*sigmas_1ns_100[7]))/1000;
  entries_1ns_100[8]=Histo_1ns18_100->Integral(500+(means_1ns_100[8]-nsigma*sigmas_1ns_100[8]),500+(means_1ns_100[8]+nsigma*sigmas_1ns_100[8]))/1000;
  entries_1ns_100[9]=Histo_1ns19_100->Integral(500+(means_1ns_100[9]-nsigma*sigmas_1ns_100[9]),500+(means_1ns_100[9]+nsigma*sigmas_1ns_100[9]))/1000;
  entries_1ns_100[10]=Histo_1ns20_100->Integral(500+(means_1ns_100[10]-nsigma*sigmas_1ns_100[10]),500+(means_1ns_100[10]+nsigma*sigmas_1ns_100[10]))/1000;

  TCanvas *c100_1ns = new TCanvas();
  c100_1ns->cd();
  Histo_1ns20_100->SetLineColorAlpha(kBlue+3,0.65);
  Histo_1ns20_100->SetTitle( "M^{2}_{miss} for different A' masses (100 e+ per bunch)");
  Histo_1ns20_100->SetLineWidth(3);
  Histo_1ns20_100->GetYaxis()->SetTitle("N_{events}");
  Histo_1ns20_100->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_1ns19_100->SetLineColorAlpha(kBlue,0.65);
  Histo_1ns19_100->SetLineWidth(3);
  Histo_1ns18_100->SetLineColorAlpha(kCyan+3,0.65);
  Histo_1ns18_100->SetLineWidth(3);
  Histo_1ns16_100->SetLineColorAlpha(kCyan+1,0.65);
  Histo_1ns16_100->SetLineWidth(3);
  Histo_1ns14_100->SetLineColorAlpha(kGreen+3,0.65);
  Histo_1ns14_100->SetLineWidth(3);
  Histo_1ns12_100->SetLineColorAlpha(kGreen+1,0.65);
  Histo_1ns12_100->SetLineWidth(3);
  Histo_1ns10_100->SetLineColorAlpha(kYellow+2,0.65);
  Histo_1ns10_100->SetLineWidth(3);
  Histo_1ns8_100->SetLineColorAlpha(kOrange-3,0.65);
  Histo_1ns8_100->SetLineWidth(3);
  Histo_1ns6_100->SetLineColorAlpha(kRed+2,0.65);
  Histo_1ns6_100->SetLineWidth(3);
  Histo_1ns4_100->SetLineColorAlpha(kRed,0.65);
  Histo_1ns4_100->SetLineWidth(3);
  Histo_1ns2_100->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_1ns2_100->SetLineWidth(3);
    
  Histo_1ns20_100->Draw();
  Histo_1ns19_100->Draw("same");
  Histo_1ns18_100->Draw("same");
  Histo_1ns4_100->Draw("same");
  Histo_1ns6_100->Draw("same");
  Histo_1ns8_100->Draw("same");
  Histo_1ns10_100->Draw("same");
  Histo_1ns12_100->Draw("same");
  Histo_1ns14_100->Draw("same");
  Histo_1ns16_100->Draw("same");
  Histo_1ns2_100->Draw("same");

  auto legend_100_1ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_100_1ns->AddEntry(Histo_1ns20_100,"M_{A'} = 20 MeV","l");
  legend_100_1ns->AddEntry(Histo_1ns19_100,"M_{A'} = 19 MeV","l");
  legend_100_1ns->AddEntry(Histo_1ns18_100,"M_{A'} = 18 MeV","l");
  legend_100_1ns->AddEntry(Histo_1ns16_100,"M_{A'} = 16 MeV","l");
  legend_100_1ns->AddEntry(Histo_1ns14_100,"M_{A'} = 14 MeV","l");
  legend_100_1ns->AddEntry(Histo_1ns12_100,"M_{A'} = 12 MeV","l");
  legend_100_1ns->AddEntry(Histo_1ns10_100,"M_{A'} = 10 MeV","l");
  legend_100_1ns->AddEntry(Histo_1ns8_100,"M_{A'} = 8 MeV","l");
  legend_100_1ns->AddEntry(Histo_1ns6_100,"M_{A'} = 6 MeV","l");
  legend_100_1ns->AddEntry(Histo_1ns4_100,"M_{A'} = 4 MeV","l");
  legend_100_1ns->AddEntry(Histo_1ns2_100,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_100_1ns->Draw();

  outRoot->WriteObject(c100_1ns,"100e_all_1ns");

  
  if(!(Histo_1ns20_1000->GetSumw2N() > 0)) Histo_1ns20_1000->Sumw2(kTRUE);
  Histo_1ns20_1000->Add(Histo_1ns0_1000_30,-1);
  if(!(Histo_1ns19_1000->GetSumw2N() > 0)) Histo_1ns19_1000->Sumw2(kTRUE);
  Histo_1ns19_1000->Add(Histo_1ns0_1000_30,-1);
  if(!(Histo_1ns18_1000->GetSumw2N() > 0)) Histo_1ns18_1000->Sumw2(kTRUE);
  Histo_1ns18_1000->Add(Histo_1ns0_1000_50,-1);
  if(!(Histo_1ns16_1000->GetSumw2N() > 0)) Histo_1ns16_1000->Sumw2(kTRUE);
  Histo_1ns16_1000->Add(Histo_1ns0_1000_60,-1);
  if(!(Histo_1ns14_1000->GetSumw2N() > 0)) Histo_1ns14_1000->Sumw2(kTRUE);
  Histo_1ns14_1000->Add(Histo_1ns0_1000_60,-1);
  if(!(Histo_1ns12_1000->GetSumw2N() > 0)) Histo_1ns12_1000->Sumw2(kTRUE);
  Histo_1ns12_1000->Add(Histo_1ns0_1000_60,-1);
  if(!(Histo_1ns10_1000->GetSumw2N() > 0)) Histo_1ns10_1000->Sumw2(kTRUE);
  Histo_1ns10_1000->Add(Histo_1ns0_1000_60,-1);
  if(!(Histo_1ns8_1000->GetSumw2N() > 0)) Histo_1ns8_1000->Sumw2(kTRUE);
  Histo_1ns8_1000->Add(Histo_1ns0_1000_60,-1);
  if(!(Histo_1ns6_1000->GetSumw2N() > 0)) Histo_1ns6_1000->Sumw2(kTRUE);
  Histo_1ns6_1000->Add(Histo_1ns0_1000_60,-1);
  if(!(Histo_1ns4_1000->GetSumw2N() > 0)) Histo_1ns4_1000->Sumw2(kTRUE);
  Histo_1ns4_1000->Add(Histo_1ns0_1000_60,-1);
  if(!(Histo_1ns2_1000->GetSumw2N() > 0)) Histo_1ns2_1000->Sumw2(kTRUE);
  Histo_1ns2_1000->Add(Histo_1ns0_1000_60,-1);


  Histo_1ns2_1000->Fit(f_2,"R");
  means_1ns_1000[0]=f_2->GetParameter(1);
  sigmas_1ns_1000[0]=f_2->GetParameter(2);

  Histo_1ns4_1000->Fit(f_4,"R");
  means_1ns_1000[1]=f_4->GetParameter(1);
  sigmas_1ns_1000[1]=f_4->GetParameter(2);
  
  Histo_1ns6_1000->Fit(f_6,"R");
  means_1ns_1000[2]=f_6->GetParameter(1);
  sigmas_1ns_1000[2]=f_6->GetParameter(2);
  
  Histo_1ns8_1000->Fit(f_8,"R");
  means_1ns_1000[3]=f_8->GetParameter(1);
  sigmas_1ns_1000[3]=f_8->GetParameter(2);
 
  Histo_1ns10_1000->Fit(f_10,"R");
  means_1ns_1000[4]=f_10->GetParameter(1);
  sigmas_1ns_1000[4]=f_10->GetParameter(2);
  
  Histo_1ns12_1000->Fit(f_12,"R");
  means_1ns_1000[5]=f_12->GetParameter(1);
  sigmas_1ns_1000[5]=f_12->GetParameter(2);
  
  Histo_1ns14_1000->Fit(f_14,"R");
  means_1ns_1000[6]=f_14->GetParameter(1);
  sigmas_1ns_1000[6]=f_14->GetParameter(2);
  
  Histo_1ns16_1000->Fit(f_16,"R");
  means_1ns_1000[7]=f_16->GetParameter(1);
  sigmas_1ns_1000[7]=f_16->GetParameter(2);
  
  Histo_1ns18_1000->Fit(f_18,"R");
  means_1ns_1000[8]=f_18->GetParameter(1);
  sigmas_1ns_1000[8]=f_18->GetParameter(2);
  
  Histo_1ns19_1000->Fit(f_19,"R");
  means_1ns_1000[9]=f_19->GetParameter(1);
  sigmas_1ns_1000[9]=f_19->GetParameter(2);
  
  Histo_1ns20_1000->Fit(f_20,"R");
  means_1ns_1000[10]=f_20->GetParameter(1);
  sigmas_1ns_1000[10]=f_20->GetParameter(2);

  entries_1ns_1000[0]=Histo_1ns2_1000->Integral(500+(means_1ns_1000[0]-nsigma*sigmas_1ns_1000[0]),500+(means_1ns_1000[0]+nsigma*sigmas_1ns_1000[0]))/1000;
  entries_1ns_1000[1]=Histo_1ns4_1000->Integral(500+(means_1ns_1000[1]-nsigma*sigmas_1ns_1000[1]),500+(means_1ns_1000[1]+nsigma*sigmas_1ns_1000[1]))/1000;
  entries_1ns_1000[2]=Histo_1ns6_1000->Integral(500+(means_1ns_1000[2]-nsigma*sigmas_1ns_1000[2]),500+(means_1ns_1000[2]+nsigma*sigmas_1ns_1000[2]))/1000;
  entries_1ns_1000[3]=Histo_1ns8_1000->Integral(500+(means_1ns_1000[3]-nsigma*sigmas_1ns_1000[3]),500+(means_1ns_1000[3]+nsigma*sigmas_1ns_1000[3]))/1000;
  entries_1ns_1000[4]=Histo_1ns10_1000->Integral(500+(means_1ns_1000[4]-nsigma*sigmas_1ns_1000[4]),500+(means_1ns_1000[4]+nsigma*sigmas_1ns_1000[4]))/1000;
  entries_1ns_1000[5]=Histo_1ns12_1000->Integral(500+(means_1ns_1000[5]-nsigma*sigmas_1ns_1000[5]),500+(means_1ns_1000[5]+nsigma*sigmas_1ns_1000[5]))/1000;
  entries_1ns_1000[6]=Histo_1ns14_1000->Integral(500+(means_1ns_1000[6]-nsigma*sigmas_1ns_1000[6]),500+(means_1ns_1000[6]+nsigma*sigmas_1ns_1000[6]))/1000;
  entries_1ns_1000[7]=Histo_1ns16_1000->Integral(500+(means_1ns_1000[7]-nsigma*sigmas_1ns_1000[7]),500+(means_1ns_1000[7]+nsigma*sigmas_1ns_1000[7]))/1000;
  entries_1ns_1000[8]=Histo_1ns18_1000->Integral(500+(means_1ns_1000[8]-nsigma*sigmas_1ns_1000[8]),500+(means_1ns_1000[8]+nsigma*sigmas_1ns_1000[8]))/1000;
  entries_1ns_1000[9]=Histo_1ns19_1000->Integral(500+(means_1ns_1000[9]-nsigma*sigmas_1ns_1000[9]),500+(means_1ns_1000[9]+nsigma*sigmas_1ns_1000[9]))/1000;
  entries_1ns_1000[10]=Histo_1ns20_1000->Integral(500+(means_1ns_1000[10]-nsigma*sigmas_1ns_1000[10]),500+(means_1ns_1000[10]+nsigma*sigmas_1ns_1000[10]))/1000;

  TCanvas *c1000_1ns = new TCanvas();
  c1000_1ns->cd();
  Histo_1ns20_1000->SetLineColorAlpha(kBlue+3,0.65);
  Histo_1ns20_1000->SetTitle( "M^{2}_{miss} for different A' masses (1000 e+ per bunch)");
  Histo_1ns20_1000->SetLineWidth(3);
  Histo_1ns20_1000->GetYaxis()->SetTitle("N_{events}");
  Histo_1ns20_1000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_1ns19_1000->SetLineColorAlpha(kBlue,0.65);
  Histo_1ns19_1000->SetLineWidth(3);
  Histo_1ns18_1000->SetLineColorAlpha(kCyan+3,0.65);
  Histo_1ns18_1000->SetLineWidth(3);
  Histo_1ns16_1000->SetLineColorAlpha(kCyan+1,0.65);
  Histo_1ns16_1000->SetLineWidth(3);
  Histo_1ns14_1000->SetLineColorAlpha(kGreen+3,0.65);
  Histo_1ns14_1000->SetLineWidth(3);
  Histo_1ns12_1000->SetLineColorAlpha(kGreen+1,0.65);
  Histo_1ns12_1000->SetLineWidth(3);
  Histo_1ns10_1000->SetLineColorAlpha(kYellow+2,0.65);
  Histo_1ns10_1000->SetLineWidth(3);
  Histo_1ns8_1000->SetLineColorAlpha(kOrange-3,0.65);
  Histo_1ns8_1000->SetLineWidth(3);
  Histo_1ns6_1000->SetLineColorAlpha(kRed+2,0.65);
  Histo_1ns6_1000->SetLineWidth(3);
  Histo_1ns4_1000->SetLineColorAlpha(kRed,0.65);
  Histo_1ns4_1000->SetLineWidth(3);
  Histo_1ns2_1000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_1ns2_1000->SetLineWidth(3);
    
  Histo_1ns20_1000->Draw();
  Histo_1ns19_1000->Draw("same");
  Histo_1ns18_1000->Draw("same");
  Histo_1ns4_1000->Draw("same");
  Histo_1ns6_1000->Draw("same");
  Histo_1ns8_1000->Draw("same");
  Histo_1ns10_1000->Draw("same");
  Histo_1ns12_1000->Draw("same");
  Histo_1ns14_1000->Draw("same");
  Histo_1ns16_1000->Draw("same");
  Histo_1ns2_1000->Draw("same");

  auto legend_1000_1ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_1000_1ns->AddEntry(Histo_1ns20_1000,"M_{A'} = 20 MeV","l");
  legend_1000_1ns->AddEntry(Histo_1ns19_1000,"M_{A'} = 19 MeV","l");
  legend_1000_1ns->AddEntry(Histo_1ns18_1000,"M_{A'} = 18 MeV","l");
  legend_1000_1ns->AddEntry(Histo_1ns16_1000,"M_{A'} = 16 MeV","l");
  legend_1000_1ns->AddEntry(Histo_1ns14_1000,"M_{A'} = 14 MeV","l");
  legend_1000_1ns->AddEntry(Histo_1ns12_1000,"M_{A'} = 12 MeV","l");
  legend_1000_1ns->AddEntry(Histo_1ns10_1000,"M_{A'} = 10 MeV","l");
  legend_1000_1ns->AddEntry(Histo_1ns8_1000,"M_{A'} = 8 MeV","l");
  legend_1000_1ns->AddEntry(Histo_1ns6_1000,"M_{A'} = 6 MeV","l");
  legend_1000_1ns->AddEntry(Histo_1ns4_1000,"M_{A'} = 4 MeV","l");
  legend_1000_1ns->AddEntry(Histo_1ns2_1000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_1000_1ns->Draw();

  outRoot->WriteObject(c1000_1ns,"1000e_all_1ns");

  
  if(!(Histo_1ns20_10000->GetSumw2N() > 0)) Histo_1ns20_10000->Sumw2(kTRUE);
  Histo_1ns20_10000->Add(Histo_1ns0_10000_30,-1);
  if(!(Histo_1ns19_10000->GetSumw2N() > 0)) Histo_1ns19_10000->Sumw2(kTRUE);
  Histo_1ns19_10000->Add(Histo_1ns0_10000_30,-1);
  if(!(Histo_1ns18_10000->GetSumw2N() > 0)) Histo_1ns18_10000->Sumw2(kTRUE);
  Histo_1ns18_10000->Add(Histo_1ns0_10000_50,-1);
  if(!(Histo_1ns16_10000->GetSumw2N() > 0)) Histo_1ns16_10000->Sumw2(kTRUE);
  Histo_1ns16_10000->Add(Histo_1ns0_10000_60,-1);
  if(!(Histo_1ns14_10000->GetSumw2N() > 0)) Histo_1ns14_10000->Sumw2(kTRUE);
  Histo_1ns14_10000->Add(Histo_1ns0_10000_60,-1);
  if(!(Histo_1ns12_10000->GetSumw2N() > 0)) Histo_1ns12_10000->Sumw2(kTRUE);
  Histo_1ns12_10000->Add(Histo_1ns0_10000_60,-1);
  if(!(Histo_1ns10_10000->GetSumw2N() > 0)) Histo_1ns10_10000->Sumw2(kTRUE);
  Histo_1ns10_10000->Add(Histo_1ns0_10000_60,-1);
  if(!(Histo_1ns8_10000->GetSumw2N() > 0)) Histo_1ns8_10000->Sumw2(kTRUE);
  Histo_1ns8_10000->Add(Histo_1ns0_10000_60,-1);
  if(!(Histo_1ns6_10000->GetSumw2N() > 0)) Histo_1ns6_10000->Sumw2(kTRUE);
  Histo_1ns6_10000->Add(Histo_1ns0_10000_60,-1);
  if(!(Histo_1ns4_10000->GetSumw2N() > 0)) Histo_1ns4_10000->Sumw2(kTRUE);
  Histo_1ns4_10000->Add(Histo_1ns0_10000_60,-1);
  if(!(Histo_1ns2_10000->GetSumw2N() > 0)) Histo_1ns2_10000->Sumw2(kTRUE);
  Histo_1ns2_10000->Add(Histo_1ns0_10000_60,-1);


  Histo_1ns2_10000->Fit(f_2,"R");
  means_1ns_10000[0]=f_2->GetParameter(1);
  sigmas_1ns_10000[0]=f_2->GetParameter(2);

  Histo_1ns4_10000->Fit(f_4,"R");
  means_1ns_10000[1]=f_4->GetParameter(1);
  sigmas_1ns_10000[1]=f_4->GetParameter(2);
  
  Histo_1ns6_10000->Fit(f_6,"R");
  means_1ns_10000[2]=f_6->GetParameter(1);
  sigmas_1ns_10000[2]=f_6->GetParameter(2);
  
  Histo_1ns8_10000->Fit(f_8,"R");
  means_1ns_10000[3]=f_8->GetParameter(1);
  sigmas_1ns_10000[3]=f_8->GetParameter(2);
 
  Histo_1ns10_10000->Fit(f_10,"R");
  means_1ns_10000[4]=f_10->GetParameter(1);
  sigmas_1ns_10000[4]=f_10->GetParameter(2);
  
  Histo_1ns12_10000->Fit(f_12,"R");
  means_1ns_10000[5]=f_12->GetParameter(1);
  sigmas_1ns_10000[5]=f_12->GetParameter(2);
  
  Histo_1ns14_10000->Fit(f_14,"R");
  means_1ns_10000[6]=f_14->GetParameter(1);
  sigmas_1ns_10000[6]=f_14->GetParameter(2);
  
  Histo_1ns16_10000->Fit(f_16,"R");
  means_1ns_10000[7]=f_16->GetParameter(1);
  sigmas_1ns_10000[7]=f_16->GetParameter(2);
  
  Histo_1ns18_10000->Fit(f_18,"R");
  means_1ns_10000[8]=f_18->GetParameter(1);
  sigmas_1ns_10000[8]=f_18->GetParameter(2);
  
  Histo_1ns19_10000->Fit(f_19,"R");
  means_1ns_10000[9]=f_19->GetParameter(1);
  sigmas_1ns_10000[9]=f_19->GetParameter(2);
  
  Histo_1ns20_10000->Fit(f_20,"R");
  means_1ns_10000[10]=f_20->GetParameter(1);
  sigmas_1ns_10000[10]=f_20->GetParameter(2);

  entries_1ns_10000[0]=Histo_1ns2_10000->Integral(500+(means_1ns_10000[0]-nsigma*sigmas_1ns_10000[0]),500+(means_1ns_10000[0]+nsigma*sigmas_1ns_10000[0]))/1000;
  entries_1ns_10000[1]=Histo_1ns4_10000->Integral(500+(means_1ns_10000[1]-nsigma*sigmas_1ns_10000[1]),500+(means_1ns_10000[1]+nsigma*sigmas_1ns_10000[1]))/1000;
  entries_1ns_10000[2]=Histo_1ns6_10000->Integral(500+(means_1ns_10000[2]-nsigma*sigmas_1ns_10000[2]),500+(means_1ns_10000[2]+nsigma*sigmas_1ns_10000[2]))/1000;
  entries_1ns_10000[3]=Histo_1ns8_10000->Integral(500+(means_1ns_10000[3]-nsigma*sigmas_1ns_10000[3]),500+(means_1ns_10000[3]+nsigma*sigmas_1ns_10000[3]))/1000;
  entries_1ns_10000[4]=Histo_1ns10_10000->Integral(500+(means_1ns_10000[4]-nsigma*sigmas_1ns_10000[4]),500+(means_1ns_10000[4]+nsigma*sigmas_1ns_10000[4]))/1000;
  entries_1ns_10000[5]=Histo_1ns12_10000->Integral(500+(means_1ns_10000[5]-nsigma*sigmas_1ns_10000[5]),500+(means_1ns_10000[5]+nsigma*sigmas_1ns_10000[5]))/1000;
  entries_1ns_10000[6]=Histo_1ns14_10000->Integral(500+(means_1ns_10000[6]-nsigma*sigmas_1ns_10000[6]),500+(means_1ns_10000[6]+nsigma*sigmas_1ns_10000[6]))/1000;
  entries_1ns_10000[7]=Histo_1ns16_10000->Integral(500+(means_1ns_10000[7]-nsigma*sigmas_1ns_10000[7]),500+(means_1ns_10000[7]+nsigma*sigmas_1ns_10000[7]))/1000;
  entries_1ns_10000[8]=Histo_1ns18_10000->Integral(500+(means_1ns_10000[8]-nsigma*sigmas_1ns_10000[8]),500+(means_1ns_10000[8]+nsigma*sigmas_1ns_10000[8]))/1000;
  entries_1ns_10000[9]=Histo_1ns19_10000->Integral(500+(means_1ns_10000[9]-nsigma*sigmas_1ns_10000[9]),500+(means_1ns_10000[9]+nsigma*sigmas_1ns_10000[9]))/1000;
  entries_1ns_10000[10]=Histo_1ns20_10000->Integral(500+(means_1ns_10000[10]-nsigma*sigmas_1ns_10000[10]),500+(means_1ns_10000[10]+nsigma*sigmas_1ns_10000[10]))/1000;


  TCanvas *c10000_1ns = new TCanvas();
  c10000_1ns->cd();
  Histo_1ns20_10000->SetLineColorAlpha(kBlue+3,0.65);
  Histo_1ns20_10000->SetTitle( "M^{2}_{miss} for different A' masses (10000 e+ per bunch)");
  Histo_1ns20_10000->SetLineWidth(3);
  Histo_1ns20_10000->GetYaxis()->SetTitle("N_{events}");
  Histo_1ns20_10000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_1ns19_10000->SetLineColorAlpha(kBlue,0.65);
  Histo_1ns19_10000->SetLineWidth(3);
  Histo_1ns18_10000->SetLineColorAlpha(kCyan+3,0.65);
  Histo_1ns18_10000->SetLineWidth(3);
  Histo_1ns16_10000->SetLineColorAlpha(kCyan+1,0.65);
  Histo_1ns16_10000->SetLineWidth(3);
  Histo_1ns14_10000->SetLineColorAlpha(kGreen+3,0.65);
  Histo_1ns14_10000->SetLineWidth(3);
  Histo_1ns12_10000->SetLineColorAlpha(kGreen+1,0.65);
  Histo_1ns12_10000->SetLineWidth(3);
  Histo_1ns10_10000->SetLineColorAlpha(kYellow+2,0.65);
  Histo_1ns10_10000->SetLineWidth(3);
  Histo_1ns8_10000->SetLineColorAlpha(kOrange-3,0.65);
  Histo_1ns8_10000->SetLineWidth(3);
  Histo_1ns6_10000->SetLineColorAlpha(kRed+2,0.65);
  Histo_1ns6_10000->SetLineWidth(3);
  Histo_1ns4_10000->SetLineColorAlpha(kRed,0.65);
  Histo_1ns4_10000->SetLineWidth(3);
  Histo_1ns2_10000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_1ns2_10000->SetLineWidth(3);
    
  Histo_1ns20_10000->Draw();
  Histo_1ns19_10000->Draw("same");
  Histo_1ns18_10000->Draw("same");
  Histo_1ns4_10000->Draw("same");
  Histo_1ns6_10000->Draw("same");
  Histo_1ns8_10000->Draw("same");
  Histo_1ns10_10000->Draw("same");
  Histo_1ns12_10000->Draw("same");
  Histo_1ns14_10000->Draw("same");
  Histo_1ns16_10000->Draw("same");
  Histo_1ns2_10000->Draw("same");

  auto legend_10000_1ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_10000_1ns->AddEntry(Histo_1ns20_10000,"M_{A'} = 20 MeV","l");
  legend_10000_1ns->AddEntry(Histo_1ns19_10000,"M_{A'} = 19 MeV","l");
  legend_10000_1ns->AddEntry(Histo_1ns18_10000,"M_{A'} = 18 MeV","l");
  legend_10000_1ns->AddEntry(Histo_1ns16_10000,"M_{A'} = 16 MeV","l");
  legend_10000_1ns->AddEntry(Histo_1ns14_10000,"M_{A'} = 14 MeV","l");
  legend_10000_1ns->AddEntry(Histo_1ns12_10000,"M_{A'} = 12 MeV","l");
  legend_10000_1ns->AddEntry(Histo_1ns10_10000,"M_{A'} = 10 MeV","l");
  legend_10000_1ns->AddEntry(Histo_1ns8_10000,"M_{A'} = 8 MeV","l");
  legend_10000_1ns->AddEntry(Histo_1ns6_10000,"M_{A'} = 6 MeV","l");
  legend_10000_1ns->AddEntry(Histo_1ns4_10000,"M_{A'} = 4 MeV","l");
  legend_10000_1ns->AddEntry(Histo_1ns2_10000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_10000_1ns->Draw();

  outRoot->WriteObject(c10000_1ns,"10000e_all_1ns");

  
  if(!(Histo_1ns20_25000->GetSumw2N() > 0)) Histo_1ns20_25000->Sumw2(kTRUE);
  Histo_1ns20_25000->Add(Histo_1ns0_25000_30,-1);
  if(!(Histo_1ns19_25000->GetSumw2N() > 0)) Histo_1ns19_25000->Sumw2(kTRUE);
  Histo_1ns19_25000->Add(Histo_1ns0_25000_30,-1);
  if(!(Histo_1ns18_25000->GetSumw2N() > 0)) Histo_1ns18_25000->Sumw2(kTRUE);
  Histo_1ns18_25000->Add(Histo_1ns0_25000_50,-1);
  if(!(Histo_1ns16_25000->GetSumw2N() > 0)) Histo_1ns16_25000->Sumw2(kTRUE);
  Histo_1ns16_25000->Add(Histo_1ns0_25000_60,-1);
  if(!(Histo_1ns14_25000->GetSumw2N() > 0)) Histo_1ns14_25000->Sumw2(kTRUE);
  Histo_1ns14_25000->Add(Histo_1ns0_25000_60,-1);
  if(!(Histo_1ns12_25000->GetSumw2N() > 0)) Histo_1ns12_25000->Sumw2(kTRUE);
  Histo_1ns12_25000->Add(Histo_1ns0_25000_60,-1);
  if(!(Histo_1ns10_25000->GetSumw2N() > 0)) Histo_1ns10_25000->Sumw2(kTRUE);
  Histo_1ns10_25000->Add(Histo_1ns0_25000_60,-1);
  if(!(Histo_1ns8_25000->GetSumw2N() > 0)) Histo_1ns8_25000->Sumw2(kTRUE);
  Histo_1ns8_25000->Add(Histo_1ns0_25000_60,-1);
  if(!(Histo_1ns6_25000->GetSumw2N() > 0)) Histo_1ns6_25000->Sumw2(kTRUE);
  Histo_1ns6_25000->Add(Histo_1ns0_25000_60,-1);
  if(!(Histo_1ns4_25000->GetSumw2N() > 0)) Histo_1ns4_25000->Sumw2(kTRUE);
  Histo_1ns4_25000->Add(Histo_1ns0_25000_60,-1);
  if(!(Histo_1ns2_25000->GetSumw2N() > 0)) Histo_1ns2_25000->Sumw2(kTRUE);
  Histo_1ns2_25000->Add(Histo_1ns0_25000_60,-1);

  Histo_1ns2_25000->Fit(f_2,"R");
  means_1ns_25000[0]=f_2->GetParameter(1);
  sigmas_1ns_25000[0]=f_2->GetParameter(2);

  Histo_1ns4_25000->Fit(f_4,"R");
  means_1ns_25000[1]=f_4->GetParameter(1);
  sigmas_1ns_25000[1]=f_4->GetParameter(2);
  
  Histo_1ns6_25000->Fit(f_6,"R");
  means_1ns_25000[2]=f_6->GetParameter(1);
  sigmas_1ns_25000[2]=f_6->GetParameter(2);
  
  Histo_1ns8_25000->Fit(f_8,"R");
  means_1ns_25000[3]=f_8->GetParameter(1);
  sigmas_1ns_25000[3]=f_8->GetParameter(2);
 
  Histo_1ns10_25000->Fit(f_10,"R");
  means_1ns_25000[4]=f_10->GetParameter(1);
  sigmas_1ns_25000[4]=f_10->GetParameter(2);
  
  Histo_1ns12_25000->Fit(f_12,"R");
  means_1ns_25000[5]=f_12->GetParameter(1);
  sigmas_1ns_25000[5]=f_12->GetParameter(2);
  
  Histo_1ns14_25000->Fit(f_14,"R");
  means_1ns_25000[6]=f_14->GetParameter(1);
  sigmas_1ns_25000[6]=f_14->GetParameter(2);
  
  Histo_1ns16_25000->Fit(f_16,"R");
  means_1ns_25000[7]=f_16->GetParameter(1);
  sigmas_1ns_25000[7]=f_16->GetParameter(2);
  
  Histo_1ns18_25000->Fit(f_18,"R");
  means_1ns_25000[8]=f_18->GetParameter(1);
  sigmas_1ns_25000[8]=f_18->GetParameter(2);
  
  Histo_1ns19_25000->Fit(f_19,"R");
  means_1ns_25000[9]=f_19->GetParameter(1);
  sigmas_1ns_25000[9]=f_19->GetParameter(2);
  
  Histo_1ns20_25000->Fit(f_20,"R");
  means_1ns_25000[10]=f_20->GetParameter(1);
  sigmas_1ns_25000[10]=f_20->GetParameter(2);

  entries_1ns_25000[0]=Histo_1ns2_25000->Integral(500+(means_1ns_25000[0]-nsigma*sigmas_1ns_25000[0]),500+(means_1ns_25000[0]+nsigma*sigmas_1ns_25000[0]))/1000;
  entries_1ns_25000[1]=Histo_1ns4_25000->Integral(500+(means_1ns_25000[1]-nsigma*sigmas_1ns_25000[1]),500+(means_1ns_25000[1]+nsigma*sigmas_1ns_25000[1]))/1000;
  entries_1ns_25000[2]=Histo_1ns6_25000->Integral(500+(means_1ns_25000[2]-nsigma*sigmas_1ns_25000[2]),500+(means_1ns_25000[2]+nsigma*sigmas_1ns_25000[2]))/1000;
  entries_1ns_25000[3]=Histo_1ns8_25000->Integral(500+(means_1ns_25000[3]-nsigma*sigmas_1ns_25000[3]),500+(means_1ns_25000[3]+nsigma*sigmas_1ns_25000[3]))/1000;
  entries_1ns_25000[4]=Histo_1ns10_25000->Integral(500+(means_1ns_25000[4]-nsigma*sigmas_1ns_25000[4]),500+(means_1ns_25000[4]+nsigma*sigmas_1ns_25000[4]))/1000;
  entries_1ns_25000[5]=Histo_1ns12_25000->Integral(500+(means_1ns_25000[5]-nsigma*sigmas_1ns_25000[5]),500+(means_1ns_25000[5]+nsigma*sigmas_1ns_25000[5]))/1000;
  entries_1ns_25000[6]=Histo_1ns14_25000->Integral(500+(means_1ns_25000[6]-nsigma*sigmas_1ns_25000[6]),500+(means_1ns_25000[6]+nsigma*sigmas_1ns_25000[6]))/1000;
  entries_1ns_25000[7]=Histo_1ns16_25000->Integral(500+(means_1ns_25000[7]-nsigma*sigmas_1ns_25000[7]),500+(means_1ns_25000[7]+nsigma*sigmas_1ns_25000[7]))/1000;
  entries_1ns_25000[8]=Histo_1ns18_25000->Integral(500+(means_1ns_25000[8]-nsigma*sigmas_1ns_25000[8]),500+(means_1ns_25000[8]+nsigma*sigmas_1ns_25000[8]))/1000;
  entries_1ns_25000[9]=Histo_1ns19_25000->Integral(500+(means_1ns_25000[9]-nsigma*sigmas_1ns_25000[9]),500+(means_1ns_25000[9]+nsigma*sigmas_1ns_25000[9]))/1000;
  entries_1ns_25000[10]=Histo_1ns20_25000->Integral(500+(means_1ns_25000[10]-nsigma*sigmas_1ns_25000[10]),500+(means_1ns_25000[10]+nsigma*sigmas_1ns_25000[10]))/1000;

  TCanvas *c25000_1ns = new TCanvas();
  c25000_1ns->cd();
  Histo_1ns20_25000->SetLineColorAlpha(kBlue+3,0.65);
  Histo_1ns20_25000->SetTitle( "M^{2}_{miss} for different A' masses (25000 e+ per bunch)");
  Histo_1ns20_25000->SetLineWidth(3);
  Histo_1ns20_25000->GetYaxis()->SetTitle("N_{events}");
  Histo_1ns20_25000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_1ns19_25000->SetLineColorAlpha(kBlue,0.65);
  Histo_1ns19_25000->SetLineWidth(3);
  Histo_1ns18_25000->SetLineColorAlpha(kCyan+3,0.65);
  Histo_1ns18_25000->SetLineWidth(3);
  Histo_1ns16_25000->SetLineColorAlpha(kCyan+1,0.65);
  Histo_1ns16_25000->SetLineWidth(3);
  Histo_1ns14_25000->SetLineColorAlpha(kGreen+3,0.65);
  Histo_1ns14_25000->SetLineWidth(3);
  Histo_1ns12_25000->SetLineColorAlpha(kGreen+1,0.65);
  Histo_1ns12_25000->SetLineWidth(3);
  Histo_1ns10_25000->SetLineColorAlpha(kYellow+2,0.65);
  Histo_1ns10_25000->SetLineWidth(3);
  Histo_1ns8_25000->SetLineColorAlpha(kOrange-3,0.65);
  Histo_1ns8_25000->SetLineWidth(3);
  Histo_1ns6_25000->SetLineColorAlpha(kRed+2,0.65);
  Histo_1ns6_25000->SetLineWidth(3);
  Histo_1ns4_25000->SetLineColorAlpha(kRed,0.65);
  Histo_1ns4_25000->SetLineWidth(3);
  Histo_1ns2_25000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_1ns2_25000->SetLineWidth(3);
    
  Histo_1ns20_25000->Draw();
  Histo_1ns19_25000->Draw("same");
  Histo_1ns18_25000->Draw("same");
  Histo_1ns4_25000->Draw("same");
  Histo_1ns6_25000->Draw("same");
  Histo_1ns8_25000->Draw("same");
  Histo_1ns10_25000->Draw("same");
  Histo_1ns12_25000->Draw("same");
  Histo_1ns14_25000->Draw("same");
  Histo_1ns16_25000->Draw("same");
  Histo_1ns2_25000->Draw("same");

  auto legend_25000_1ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_25000_1ns->AddEntry(Histo_1ns20_25000,"M_{A'} = 20 MeV","l");
  legend_25000_1ns->AddEntry(Histo_1ns19_25000,"M_{A'} = 19 MeV","l");
  legend_25000_1ns->AddEntry(Histo_1ns18_25000,"M_{A'} = 18 MeV","l");
  legend_25000_1ns->AddEntry(Histo_1ns16_25000,"M_{A'} = 16 MeV","l");
  legend_25000_1ns->AddEntry(Histo_1ns14_25000,"M_{A'} = 14 MeV","l");
  legend_25000_1ns->AddEntry(Histo_1ns12_25000,"M_{A'} = 12 MeV","l");
  legend_25000_1ns->AddEntry(Histo_1ns10_25000,"M_{A'} = 10 MeV","l");
  legend_25000_1ns->AddEntry(Histo_1ns8_25000,"M_{A'} = 8 MeV","l");
  legend_25000_1ns->AddEntry(Histo_1ns6_25000,"M_{A'} = 6 MeV","l");
  legend_25000_1ns->AddEntry(Histo_1ns4_25000,"M_{A'} = 4 MeV","l");
  legend_25000_1ns->AddEntry(Histo_1ns2_25000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_25000_1ns->Draw();

  outRoot->WriteObject(c25000_1ns,"25000e_all_1ns");

  
  TCanvas *cgr_1ns = new TCanvas();
 
  auto gr_1_1ns = new TGraph (11,masses,entries_1ns_1);
  gr_1_1ns->SetLineColor(kMagenta);
  gr_1_1ns->SetLineWidth(3); 
  gr_1_1ns->GetYaxis()->SetTitle("Acceptance [%]");
  gr_1_1ns->GetXaxis()->SetTitle("M_{A'} [MeV]");
  gr_1_1ns->SetTitle("Acceptance");
  gr_1_1ns->Draw();
  auto gr_10_1ns = new TGraph (11,masses,entries_1ns_10);
  gr_10_1ns->SetLineColor(kRed);
  gr_10_1ns->SetLineWidth(3);
  gr_10_1ns->Draw("same");
  auto gr_100_1ns = new TGraph (11,masses,entries_1ns_100);
  gr_100_1ns->SetLineColor(kOrange);
  gr_100_1ns->SetLineWidth(3);
  gr_100_1ns->Draw("same");
  auto gr_1000_1ns = new TGraph (11,masses,entries_1ns_1000);
  gr_1000_1ns->SetLineColor(kGreen+3);
  gr_1000_1ns->SetLineWidth(3);
  gr_1000_1ns->Draw("same");
  auto gr_10000_1ns = new TGraph (11,masses,entries_1ns_10000);
  gr_10000_1ns->SetLineColor(kCyan+1);
  gr_10000_1ns->SetLineWidth(3);
  gr_10000_1ns->Draw("same");
  auto gr_25000_1ns = new TGraph (11,masses,entries_1ns_25000);
  gr_25000_1ns->SetLineColor(kBlue);
  gr_25000_1ns->SetLineWidth(3);
  gr_25000_1ns->Draw("same");
  auto legend_gr_1ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_gr_1ns->AddEntry(gr_1_1ns,"1 e+","l");
  legend_gr_1ns->AddEntry(gr_10_1ns,"10 e+","l");
  legend_gr_1ns->AddEntry(gr_100_1ns,"100 e+","l");
  legend_gr_1ns->AddEntry(gr_1000_1ns,"1000 e+","l");
  legend_gr_1ns->AddEntry(gr_10000_1ns,"10000 e+","l");
  legend_gr_1ns->AddEntry(gr_25000_1ns,"25000 e+","l");
  legend_gr_1ns->Draw();

  TCanvas *csig_1ns = new TCanvas();
 
  auto sig_1_1ns = new TGraph (11,masses,sigmas_1ns_1);
  sig_1_1ns->SetLineColor(kMagenta);
  sig_1_1ns->SetLineWidth(3);
  sig_1_1ns->GetYaxis()->SetTitle("Sigma");
  sig_1_1ns->GetXaxis()->SetTitle("M_{A'} [MeV]");
  sig_1_1ns->SetTitle("Sigma");
  sig_1_1ns->Draw();
  auto sig_10_1ns = new TGraph (11,masses,sigmas_1ns_10);
  sig_10_1ns->SetLineColor(kRed);
  sig_10_1ns->SetLineWidth(3);
  sig_10_1ns->Draw("same");
  auto sig_100_1ns = new TGraph (11,masses,sigmas_1ns_100);
  sig_100_1ns->SetLineColor(kOrange);
  sig_100_1ns->SetLineWidth(3);
  sig_100_1ns->Draw("same");
  auto sig_1000_1ns = new TGraph (11,masses,sigmas_1ns_1000);
  sig_1000_1ns->SetLineColor(kGreen+3);
  sig_1000_1ns->SetLineWidth(3);
  sig_1000_1ns->Draw("same");
  auto sig_10000_1ns = new TGraph (11,masses,sigmas_1ns_10000);
  sig_10000_1ns->SetLineColor(kCyan+1);
  sig_10000_1ns->SetLineWidth(3);
  sig_10000_1ns->Draw("same");
  auto sig_25000_1ns = new TGraph (11,masses,sigmas_1ns_25000);
  sig_25000_1ns->SetLineColor(kBlue);
  sig_25000_1ns->SetLineWidth(3);
  sig_25000_1ns->Draw("same");
  auto legend_sig_1ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_sig_1ns->AddEntry(sig_1_1ns,"1 e+","l");
  legend_sig_1ns->AddEntry(sig_10_1ns,"10 e+","l");
  legend_sig_1ns->AddEntry(sig_100_1ns,"100 e+","l");
  legend_sig_1ns->AddEntry(sig_1000_1ns,"1000 e+","l");
  legend_sig_1ns->AddEntry(sig_10000_1ns,"10000 e+","l");
  legend_sig_1ns->AddEntry(sig_25000_1ns,"25000 e+","l");
  legend_sig_1ns->Draw();

  
  outRoot->WriteObject(cgr_1ns,"Acceptance_1ns");
  outRoot->WriteObject(csig_1ns,"Sigma_1ns");






  

  //----------------------------------------Do analysis for deltaT=2ns-------------------------------------------------Do analysis for deltaT=2ns-------------------------------------------------

  Float_t sigmas_2ns_1[11];
  Float_t means_2ns_1[11];
  Float_t entries_2ns_1[11];

  Float_t sigmas_2ns_10[11];
  Float_t means_2ns_10[11];
  Float_t entries_2ns_10[11];

  Float_t sigmas_2ns_100[11];
  Float_t means_2ns_100[11];
  Float_t entries_2ns_100[11];

  Float_t sigmas_2ns_1000[11];
  Float_t means_2ns_1000[11];
  Float_t entries_2ns_1000[11];

  Float_t sigmas_2ns_10000[11];
  Float_t means_2ns_10000[11];
  Float_t entries_2ns_10000[11];
  
  Float_t sigmas_2ns_25000[11];
  Float_t means_2ns_25000[11];
  Float_t entries_2ns_25000[11];
   
  //---------------------------------------------- Open Histograms --------------------------------------------------

  TH1F *Histo_2ns20_1 = (TH1F*) dir20_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns19_1 = (TH1F*) dir19_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns18_1 = (TH1F*) dir18_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_2ns");
  TH1F *Histo_2ns16_1 = (TH1F*) dir16_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns14_1 = (TH1F*) dir14_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns12_1 = (TH1F*) dir12_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns10_1 = (TH1F*) dir10_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns8_1 = (TH1F*) dir8_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns6_1 = (TH1F*) dir6_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns4_1 = (TH1F*) dir4_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns2_1 = (TH1F*) dir2_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns0_1_10 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_2ns");
  TH1F *Histo_2ns0_1_20 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_2ns");
  TH1F *Histo_2ns0_1_30 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns0_1_40 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_2ns");
  TH1F *Histo_2ns0_1_50 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_2ns");
  TH1F *Histo_2ns0_1_60 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  
  TH1F *Histo_2ns20_10 = (TH1F*) dir20_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns19_10 = (TH1F*) dir19_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns18_10 = (TH1F*) dir18_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_2ns");
  TH1F *Histo_2ns16_10 = (TH1F*) dir16_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns14_10 = (TH1F*) dir14_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns12_10 = (TH1F*) dir12_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns10_10 = (TH1F*) dir10_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns8_10 = (TH1F*) dir8_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns6_10 = (TH1F*) dir6_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns4_10 = (TH1F*) dir4_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns2_10 = (TH1F*) dir2_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns0_10_10 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_2ns");
  TH1F *Histo_2ns0_10_20 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_2ns");
  TH1F *Histo_2ns0_10_30 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns0_10_40 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_2ns");
  TH1F *Histo_2ns0_10_50 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_2ns");
  TH1F *Histo_2ns0_10_60 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  
  TH1F *Histo_2ns20_100 = (TH1F*) dir20_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns19_100 = (TH1F*) dir19_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns18_100 = (TH1F*) dir18_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_2ns");
  TH1F *Histo_2ns16_100 = (TH1F*) dir16_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns14_100 = (TH1F*) dir14_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns12_100 = (TH1F*) dir12_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns10_100 = (TH1F*) dir10_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns8_100 = (TH1F*) dir8_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns6_100 = (TH1F*) dir6_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns4_100 = (TH1F*) dir4_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns2_100 = (TH1F*) dir2_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns0_100_10 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_2ns");
  TH1F *Histo_2ns0_100_20 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_2ns");
  TH1F *Histo_2ns0_100_30 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns0_100_40 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_2ns");
  TH1F *Histo_2ns0_100_50 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_2ns");
  TH1F *Histo_2ns0_100_60 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  
  TH1F *Histo_2ns20_1000 = (TH1F*) dir20_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns19_1000 = (TH1F*) dir19_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns18_1000 = (TH1F*) dir18_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_2ns");
  TH1F *Histo_2ns16_1000 = (TH1F*) dir16_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns14_1000 = (TH1F*) dir14_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns12_1000 = (TH1F*) dir12_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns10_1000 = (TH1F*) dir10_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns8_1000 = (TH1F*) dir8_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns6_1000 = (TH1F*) dir6_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns4_1000 = (TH1F*) dir4_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns2_1000 = (TH1F*) dir2_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns0_1000_10 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_2ns");
  TH1F *Histo_2ns0_1000_20 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_2ns");
  TH1F *Histo_2ns0_1000_30 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns0_1000_40 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_2ns");
  TH1F *Histo_2ns0_1000_50 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_2ns");
  TH1F *Histo_2ns0_1000_60 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  
  TH1F *Histo_2ns20_10000 = (TH1F*) dir20_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns19_10000 = (TH1F*) dir19_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns18_10000 = (TH1F*) dir18_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_2ns");
  TH1F *Histo_2ns16_10000 = (TH1F*) dir16_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns14_10000 = (TH1F*) dir14_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns12_10000 = (TH1F*) dir12_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns10_10000 = (TH1F*) dir10_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns8_10000 = (TH1F*) dir8_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns6_10000 = (TH1F*) dir6_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns4_10000 = (TH1F*) dir4_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns2_10000 = (TH1F*) dir2_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns0_10000_10 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_2ns");
  TH1F *Histo_2ns0_10000_20 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_2ns");
  TH1F *Histo_2ns0_10000_30 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns0_10000_40 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_2ns");
  TH1F *Histo_2ns0_10000_50 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_2ns");
  TH1F *Histo_2ns0_10000_60 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  
  TH1F *Histo_2ns20_25000 = (TH1F*) dir20_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns19_25000 = (TH1F*) dir19_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns18_25000 = (TH1F*) dir18_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_2ns");
  TH1F *Histo_2ns16_25000 = (TH1F*) dir16_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns14_25000 = (TH1F*) dir14_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns12_25000 = (TH1F*) dir12_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns10_25000 = (TH1F*) dir10_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns8_25000 = (TH1F*) dir8_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns6_25000 = (TH1F*) dir6_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns4_25000 = (TH1F*) dir4_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns2_25000 = (TH1F*) dir2_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");
  TH1F *Histo_2ns0_25000_10 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_2ns");
  TH1F *Histo_2ns0_25000_20 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_2ns");
  TH1F *Histo_2ns0_25000_30 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_2ns");
  TH1F *Histo_2ns0_25000_40 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_2ns");
  TH1F *Histo_2ns0_25000_50 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_2ns");
  TH1F *Histo_2ns0_25000_60 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_2ns");

 
  Histo_2ns2_1->Fit(f_2,"R");
  means_2ns_1[0]=f_2->GetParameter(1);
  sigmas_2ns_1[0]=f_2->GetParameter(2);

  Histo_2ns4_1->Fit(f_4,"R");
  means_2ns_1[1]=f_4->GetParameter(1);
  sigmas_2ns_1[1]=f_4->GetParameter(2);
  
  Histo_2ns6_1->Fit(f_6,"R");
  means_2ns_1[2]=f_6->GetParameter(1);
  sigmas_2ns_1[2]=f_6->GetParameter(2);
  
  Histo_2ns8_1->Fit(f_8,"R");
  means_2ns_1[3]=f_8->GetParameter(1);
  sigmas_2ns_1[3]=f_8->GetParameter(2);
  
  Histo_2ns10_1->Fit(f_10,"R");
  means_2ns_1[4]=f_10->GetParameter(1);
  sigmas_2ns_1[4]=f_10->GetParameter(2);
  
  Histo_2ns12_1->Fit(f_12,"R");
  means_2ns_1[5]=f_12->GetParameter(1);
  sigmas_2ns_1[5]=f_12->GetParameter(2);
  
  Histo_2ns14_1->Fit(f_14,"R");
  means_2ns_1[6]=f_14->GetParameter(1);
  sigmas_2ns_1[6]=f_14->GetParameter(2);
  
  Histo_2ns16_1->Fit(f_16,"R");
  means_2ns_1[7]=f_16->GetParameter(1);
  sigmas_2ns_1[7]=f_16->GetParameter(2);
  
  Histo_2ns18_1->Fit(f_18,"R");
  means_2ns_1[8]=f_18->GetParameter(1);
  sigmas_2ns_1[8]=f_18->GetParameter(2);
  
  Histo_2ns19_1->Fit(f_19,"R");
  means_2ns_1[9]=f_19->GetParameter(1);
  sigmas_2ns_1[9]=f_19->GetParameter(2);
  
  Histo_2ns20_1->Fit(f_20,"R");
  means_2ns_1[10]=f_20->GetParameter(1);
  sigmas_2ns_1[10]=f_20->GetParameter(2);
  
  entries_2ns_1[0]=Histo_2ns2_1->Integral(500+(means_2ns_1[0]-nsigma*sigmas_2ns_1[0]),500+(means_2ns_1[0]+nsigma*sigmas_2ns_1[0]))/1000;
  entries_2ns_1[1]=Histo_2ns4_1->Integral(500+(means_2ns_1[1]-nsigma*sigmas_2ns_1[1]),500+(means_2ns_1[1]+nsigma*sigmas_2ns_1[1]))/1000;
  entries_2ns_1[2]=Histo_2ns6_1->Integral(500+(means_2ns_1[2]-nsigma*sigmas_2ns_1[2]),500+(means_2ns_1[2]+nsigma*sigmas_2ns_1[2]))/1000;
  entries_2ns_1[3]=Histo_2ns8_1->Integral(500+(means_2ns_1[3]-nsigma*sigmas_2ns_1[3]),500+(means_2ns_1[3]+nsigma*sigmas_2ns_1[3]))/1000;
  entries_2ns_1[4]=Histo_2ns10_1->Integral(500+(means_2ns_1[4]-nsigma*sigmas_2ns_1[4]),500+(means_2ns_1[4]+nsigma*sigmas_2ns_1[4]))/1000;
  entries_2ns_1[5]=Histo_2ns12_1->Integral(500+(means_2ns_1[5]-nsigma*sigmas_2ns_1[5]),500+(means_2ns_1[5]+nsigma*sigmas_2ns_1[5]))/1000;
  entries_2ns_1[6]=Histo_2ns14_1->Integral(500+(means_2ns_1[6]-nsigma*sigmas_2ns_1[6]),500+(means_2ns_1[6]+nsigma*sigmas_2ns_1[6]))/1000;
  entries_2ns_1[7]=Histo_2ns16_1->Integral(500+(means_2ns_1[7]-nsigma*sigmas_2ns_1[7]),500+(means_2ns_1[7]+nsigma*sigmas_2ns_1[7]))/1000;
  entries_2ns_1[8]=Histo_2ns18_1->Integral(500+(means_2ns_1[8]-nsigma*sigmas_2ns_1[8]),500+(means_2ns_1[8]+nsigma*sigmas_2ns_1[8]))/1000;
  entries_2ns_1[9]=Histo_2ns19_1->Integral(500+(means_2ns_1[9]-nsigma*sigmas_2ns_1[9]),500+(means_2ns_1[9]+nsigma*sigmas_2ns_1[9]))/1000;
  entries_2ns_1[10]=Histo_2ns20_1->Integral(500+(means_2ns_1[10]-nsigma*sigmas_2ns_1[10]),500+(means_2ns_1[10]+nsigma*sigmas_2ns_1[10]))/1000;
  
  TCanvas *c1_2ns = new TCanvas();
  c1_2ns->cd();
  Histo_2ns20_1->SetLineColorAlpha(kBlue+3,0.65);
  Histo_2ns20_1->SetTitle( "M^{2}_{miss} for different A' masses (1 e+ per bunch)");
  Histo_2ns20_1->SetLineWidth(3);
  Histo_2ns20_1->GetYaxis()->SetTitle("N_{events}");
  Histo_2ns20_1->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_2ns19_1->SetLineColorAlpha(kBlue,0.65);
  Histo_2ns19_1->SetLineWidth(3);
  Histo_2ns18_1->SetLineColorAlpha(kCyan+3,0.65);
  Histo_2ns18_1->SetLineWidth(3);
  Histo_2ns16_1->SetLineColorAlpha(kCyan+1,0.65);
  Histo_2ns16_1->SetLineWidth(3);
  Histo_2ns14_1->SetLineColorAlpha(kGreen+3,0.65);
  Histo_2ns14_1->SetLineWidth(3);
  Histo_2ns12_1->SetLineColorAlpha(kGreen+1,0.65);
  Histo_2ns12_1->SetLineWidth(3);
  Histo_2ns10_1->SetLineColorAlpha(kYellow+2,0.65);
  Histo_2ns10_1->SetLineWidth(3);
  Histo_2ns8_1->SetLineColorAlpha(kOrange-3,0.65);
  Histo_2ns8_1->SetLineWidth(3);
  Histo_2ns6_1->SetLineColorAlpha(kRed+2,0.65);
  Histo_2ns6_1->SetLineWidth(3);
  Histo_2ns4_1->SetLineColorAlpha(kRed,0.65);
  Histo_2ns4_1->SetLineWidth(3);
  Histo_2ns2_1->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_2ns2_1->SetLineWidth(3);
    
  Histo_2ns20_1->Draw();
  Histo_2ns19_1->Draw("same");
  Histo_2ns18_1->Draw("same");
  Histo_2ns4_1->Draw("same");
  Histo_2ns6_1->Draw("same");
  Histo_2ns8_1->Draw("same");
  Histo_2ns10_1->Draw("same");
  Histo_2ns12_1->Draw("same");
  Histo_2ns14_1->Draw("same");
  Histo_2ns16_1->Draw("same");
  Histo_2ns2_1->Draw("same");

  auto legend_1_2ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_1_2ns->AddEntry(Histo_2ns20_1,"M_{A'} = 20 MeV","l");
  legend_1_2ns->AddEntry(Histo_2ns19_1,"M_{A'} = 19 MeV","l");
  legend_1_2ns->AddEntry(Histo_2ns18_1,"M_{A'} = 18 MeV","l");
  legend_1_2ns->AddEntry(Histo_2ns16_1,"M_{A'} = 16 MeV","l");
  legend_1_2ns->AddEntry(Histo_2ns14_1,"M_{A'} = 14 MeV","l");
  legend_1_2ns->AddEntry(Histo_2ns12_1,"M_{A'} = 12 MeV","l");
  legend_1_2ns->AddEntry(Histo_2ns10_1,"M_{A'} = 10 MeV","l");
  legend_1_2ns->AddEntry(Histo_2ns8_1,"M_{A'} = 8 MeV","l");
  legend_1_2ns->AddEntry(Histo_2ns6_1,"M_{A'} = 6 MeV","l");
  legend_1_2ns->AddEntry(Histo_2ns4_1,"M_{A'} = 4 MeV","l");
  legend_1_2ns->AddEntry(Histo_2ns2_1,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_1_2ns->Draw();

  outRoot->WriteObject(c1_2ns,"1e_all_2ns");

  if(!(Histo_2ns20_10->GetSumw2N() > 0)) Histo_2ns20_10->Sumw2(kTRUE);
  Histo_2ns20_10->Add(Histo_2ns0_10_30,-1);
  if(!(Histo_2ns19_10->GetSumw2N() > 0)) Histo_2ns19_10->Sumw2(kTRUE);
  Histo_2ns19_10->Add(Histo_2ns0_10_30,-1);
  if(!(Histo_2ns18_10->GetSumw2N() > 0)) Histo_2ns18_10->Sumw2(kTRUE);
  Histo_2ns18_10->Add(Histo_2ns0_10_50,-1);
  if(!(Histo_2ns16_10->GetSumw2N() > 0)) Histo_2ns16_10->Sumw2(kTRUE);
  Histo_2ns16_10->Add(Histo_2ns0_10_60,-1);
  if(!(Histo_2ns14_10->GetSumw2N() > 0)) Histo_2ns14_10->Sumw2(kTRUE);
  Histo_2ns14_10->Add(Histo_2ns0_10_60,-1);
  if(!(Histo_2ns12_10->GetSumw2N() > 0)) Histo_2ns12_10->Sumw2(kTRUE);
  Histo_2ns12_10->Add(Histo_2ns0_10_60,-1);
  if(!(Histo_2ns10_10->GetSumw2N() > 0)) Histo_2ns10_10->Sumw2(kTRUE);
  Histo_2ns10_10->Add(Histo_2ns0_10_60,-1);
  if(!(Histo_2ns8_10->GetSumw2N() > 0)) Histo_2ns8_10->Sumw2(kTRUE);
  Histo_2ns8_10->Add(Histo_2ns0_10_60,-1);
  if(!(Histo_2ns6_10->GetSumw2N() > 0)) Histo_2ns6_10->Sumw2(kTRUE);
  Histo_2ns6_10->Add(Histo_2ns0_10_60,-1);
  if(!(Histo_2ns4_10->GetSumw2N() > 0)) Histo_2ns4_10->Sumw2(kTRUE);
  Histo_2ns4_10->Add(Histo_2ns0_10_60,-1);
  if(!(Histo_2ns2_10->GetSumw2N() > 0)) Histo_2ns2_10->Sumw2(kTRUE);
  Histo_2ns2_10->Add(Histo_2ns0_10_60,-1);
  
  Histo_2ns2_10->Fit(f_2,"R");
  means_2ns_10[0]=f_2->GetParameter(1);
  sigmas_2ns_10[0]=f_2->GetParameter(2);

  Histo_2ns4_10->Fit(f_4,"R");
  means_2ns_10[1]=f_4->GetParameter(1);
  sigmas_2ns_10[1]=f_4->GetParameter(2);
  
  Histo_2ns6_10->Fit(f_6,"R");
  means_2ns_10[2]=f_6->GetParameter(1);
  sigmas_2ns_10[2]=f_6->GetParameter(2);
  
  Histo_2ns8_10->Fit(f_8,"R");
  means_2ns_10[3]=f_8->GetParameter(1);
  sigmas_2ns_10[3]=f_8->GetParameter(2);
 
  Histo_2ns10_10->Fit(f_10,"R");
  means_2ns_10[4]=f_10->GetParameter(1);
  sigmas_2ns_10[4]=f_10->GetParameter(2);
  
  Histo_2ns12_10->Fit(f_12,"R");
  means_2ns_10[5]=f_12->GetParameter(1);
  sigmas_2ns_10[5]=f_12->GetParameter(2);
  
  Histo_2ns14_10->Fit(f_14,"R");
  means_2ns_10[6]=f_14->GetParameter(1);
  sigmas_2ns_10[6]=f_14->GetParameter(2);
  
  Histo_2ns16_10->Fit(f_16,"R");
  means_2ns_10[7]=f_16->GetParameter(1);
  sigmas_2ns_10[7]=f_16->GetParameter(2);
  
  Histo_2ns18_10->Fit(f_18,"R");
  means_2ns_10[8]=f_18->GetParameter(1);
  sigmas_2ns_10[8]=f_18->GetParameter(2);
  
  Histo_2ns19_10->Fit(f_19,"R");
  means_2ns_10[9]=f_19->GetParameter(1);
  sigmas_2ns_10[9]=f_19->GetParameter(2);
  
  Histo_2ns20_10->Fit(f_20,"R");
  means_2ns_10[10]=f_20->GetParameter(1);
  sigmas_2ns_10[10]=f_20->GetParameter(2);

  entries_2ns_10[0]=Histo_2ns2_10->Integral(500+(means_2ns_10[0]-nsigma*sigmas_2ns_10[0]),500+(means_2ns_10[0]+nsigma*sigmas_2ns_10[0]))/1000;
  entries_2ns_10[1]=Histo_2ns4_10->Integral(500+(means_2ns_10[1]-nsigma*sigmas_2ns_10[1]),500+(means_2ns_10[1]+nsigma*sigmas_2ns_10[1]))/1000;
  entries_2ns_10[2]=Histo_2ns6_10->Integral(500+(means_2ns_10[2]-nsigma*sigmas_2ns_10[2]),500+(means_2ns_10[2]+nsigma*sigmas_2ns_10[2]))/1000;
  entries_2ns_10[3]=Histo_2ns8_10->Integral(500+(means_2ns_10[3]-nsigma*sigmas_2ns_10[3]),500+(means_2ns_10[3]+nsigma*sigmas_2ns_10[3]))/1000;
  entries_2ns_10[4]=Histo_2ns10_10->Integral(500+(means_2ns_10[4]-nsigma*sigmas_2ns_10[4]),500+(means_2ns_10[4]+nsigma*sigmas_2ns_10[4]))/1000;
  entries_2ns_10[5]=Histo_2ns12_10->Integral(500+(means_2ns_10[5]-nsigma*sigmas_2ns_10[5]),500+(means_2ns_10[5]+nsigma*sigmas_2ns_10[5]))/1000;
  entries_2ns_10[6]=Histo_2ns14_10->Integral(500+(means_2ns_10[6]-nsigma*sigmas_2ns_10[6]),500+(means_2ns_10[6]+nsigma*sigmas_2ns_10[6]))/1000;
  entries_2ns_10[7]=Histo_2ns16_10->Integral(500+(means_2ns_10[7]-nsigma*sigmas_2ns_10[7]),500+(means_2ns_10[7]+nsigma*sigmas_2ns_10[7]))/1000;
  entries_2ns_10[8]=Histo_2ns18_10->Integral(500+(means_2ns_10[8]-nsigma*sigmas_2ns_10[8]),500+(means_2ns_10[8]+nsigma*sigmas_2ns_10[8]))/1000;
  entries_2ns_10[9]=Histo_2ns19_10->Integral(500+(means_2ns_10[9]-nsigma*sigmas_2ns_10[9]),500+(means_2ns_10[9]+nsigma*sigmas_2ns_10[9]))/1000;
  entries_2ns_10[10]=Histo_2ns20_10->Integral(500+(means_2ns_10[10]-nsigma*sigmas_2ns_10[10]),500+(means_2ns_10[10]+nsigma*sigmas_2ns_10[10]))/1000;

  TCanvas *c10_2ns = new TCanvas();
  c10_2ns->cd();
  Histo_2ns20_10->SetLineColorAlpha(kBlue+3,0.65);
  Histo_2ns20_10->SetTitle( "M^{2}_{miss} for different A' masses (10 e+ per bunch)");
  Histo_2ns20_10->SetLineWidth(3);
  Histo_2ns20_10->GetYaxis()->SetTitle("N_{events}");
  Histo_2ns20_10->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_2ns19_10->SetLineColorAlpha(kBlue,0.65);
  Histo_2ns19_10->SetLineWidth(3);
  Histo_2ns18_10->SetLineColorAlpha(kCyan+3,0.65);
  Histo_2ns18_10->SetLineWidth(3);
  Histo_2ns16_10->SetLineColorAlpha(kCyan+1,0.65);
  Histo_2ns16_10->SetLineWidth(3);
  Histo_2ns14_10->SetLineColorAlpha(kGreen+3,0.65);
  Histo_2ns14_10->SetLineWidth(3);
  Histo_2ns12_10->SetLineColorAlpha(kGreen+1,0.65);
  Histo_2ns12_10->SetLineWidth(3);
  Histo_2ns10_10->SetLineColorAlpha(kYellow+2,0.65);
  Histo_2ns10_10->SetLineWidth(3);
  Histo_2ns8_10->SetLineColorAlpha(kOrange-3,0.65);
  Histo_2ns8_10->SetLineWidth(3);
  Histo_2ns6_10->SetLineColorAlpha(kRed+2,0.65);
  Histo_2ns6_10->SetLineWidth(3);
  Histo_2ns4_10->SetLineColorAlpha(kRed,0.65);
  Histo_2ns4_10->SetLineWidth(3);
  Histo_2ns2_10->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_2ns2_10->SetLineWidth(3);
    
  Histo_2ns20_10->Draw();
  Histo_2ns19_10->Draw("same");
  Histo_2ns18_10->Draw("same");
  Histo_2ns4_10->Draw("same");
  Histo_2ns6_10->Draw("same");
  Histo_2ns8_10->Draw("same");
  Histo_2ns10_10->Draw("same");
  Histo_2ns12_10->Draw("same");
  Histo_2ns14_10->Draw("same");
  Histo_2ns16_10->Draw("same");
  Histo_2ns2_10->Draw("same");

  auto legend_10_2ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_10_2ns->AddEntry(Histo_2ns20_10,"M_{A'} = 20 MeV","l");
  legend_10_2ns->AddEntry(Histo_2ns19_10,"M_{A'} = 19 MeV","l");
  legend_10_2ns->AddEntry(Histo_2ns18_10,"M_{A'} = 18 MeV","l");
  legend_10_2ns->AddEntry(Histo_2ns16_10,"M_{A'} = 16 MeV","l");
  legend_10_2ns->AddEntry(Histo_2ns14_10,"M_{A'} = 14 MeV","l");
  legend_10_2ns->AddEntry(Histo_2ns12_10,"M_{A'} = 12 MeV","l");
  legend_10_2ns->AddEntry(Histo_2ns10_10,"M_{A'} = 10 MeV","l");
  legend_10_2ns->AddEntry(Histo_2ns8_10,"M_{A'} = 8 MeV","l");
  legend_10_2ns->AddEntry(Histo_2ns6_10,"M_{A'} = 6 MeV","l");
  legend_10_2ns->AddEntry(Histo_2ns4_10,"M_{A'} = 4 MeV","l");
  legend_10_2ns->AddEntry(Histo_2ns2_10,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_10_2ns->Draw();

  outRoot->WriteObject(c10_2ns,"10e_all_2ns");
 
  if(!(Histo_2ns20_100->GetSumw2N() > 0)) Histo_2ns20_100->Sumw2(kTRUE);
  Histo_2ns20_100->Add(Histo_2ns0_100_30,-1);
  if(!(Histo_2ns19_100->GetSumw2N() > 0)) Histo_2ns19_100->Sumw2(kTRUE);
  Histo_2ns19_100->Add(Histo_2ns0_100_30,-1);
  if(!(Histo_2ns18_100->GetSumw2N() > 0)) Histo_2ns18_100->Sumw2(kTRUE);
  Histo_2ns18_100->Add(Histo_2ns0_100_50,-1);
  if(!(Histo_2ns16_100->GetSumw2N() > 0)) Histo_2ns16_100->Sumw2(kTRUE);
  Histo_2ns16_100->Add(Histo_2ns0_100_60,-1);
  if(!(Histo_2ns14_100->GetSumw2N() > 0)) Histo_2ns14_100->Sumw2(kTRUE);
  Histo_2ns14_100->Add(Histo_2ns0_100_60,-1);
  if(!(Histo_2ns12_100->GetSumw2N() > 0)) Histo_2ns12_100->Sumw2(kTRUE);
  Histo_2ns12_100->Add(Histo_2ns0_100_60,-1);
  if(!(Histo_2ns10_100->GetSumw2N() > 0)) Histo_2ns10_100->Sumw2(kTRUE);
  Histo_2ns10_100->Add(Histo_2ns0_100_60,-1);
  if(!(Histo_2ns8_100->GetSumw2N() > 0)) Histo_2ns8_100->Sumw2(kTRUE);
  Histo_2ns8_100->Add(Histo_2ns0_100_60,-1);
  if(!(Histo_2ns6_100->GetSumw2N() > 0)) Histo_2ns6_100->Sumw2(kTRUE);
  Histo_2ns6_100->Add(Histo_2ns0_100_60,-1);
  if(!(Histo_2ns4_100->GetSumw2N() > 0)) Histo_2ns4_100->Sumw2(kTRUE);
  Histo_2ns4_100->Add(Histo_2ns0_100_60,-1);
  if(!(Histo_2ns2_100->GetSumw2N() > 0)) Histo_2ns2_100->Sumw2(kTRUE);
  Histo_2ns2_100->Add(Histo_2ns0_100_60,-1);


  Histo_2ns2_100->Fit(f_2,"R");
  means_2ns_100[0]=f_2->GetParameter(1);
  sigmas_2ns_100[0]=f_2->GetParameter(2);

  Histo_2ns4_100->Fit(f_4,"R");
  means_2ns_100[1]=f_4->GetParameter(1);
  sigmas_2ns_100[1]=f_4->GetParameter(2);
  
  Histo_2ns6_100->Fit(f_6,"R");
  means_2ns_100[2]=f_6->GetParameter(1);
  sigmas_2ns_100[2]=f_6->GetParameter(2);
  
  Histo_2ns8_100->Fit(f_8,"R");
  means_2ns_100[3]=f_8->GetParameter(1);
  sigmas_2ns_100[3]=f_8->GetParameter(2);
 
  Histo_2ns10_100->Fit(f_10,"R");
  means_2ns_100[4]=f_10->GetParameter(1);
  sigmas_2ns_100[4]=f_10->GetParameter(2);
  
  Histo_2ns12_100->Fit(f_12,"R");
  means_2ns_100[5]=f_12->GetParameter(1);
  sigmas_2ns_100[5]=f_12->GetParameter(2);
  
  Histo_2ns14_100->Fit(f_14,"R");
  means_2ns_100[6]=f_14->GetParameter(1);
  sigmas_2ns_100[6]=f_14->GetParameter(2);
  
  Histo_2ns16_100->Fit(f_16,"R");
  means_2ns_100[7]=f_16->GetParameter(1);
  sigmas_2ns_100[7]=f_16->GetParameter(2);
  
  Histo_2ns18_100->Fit(f_18,"R");
  means_2ns_100[8]=f_18->GetParameter(1);
  sigmas_2ns_100[8]=f_18->GetParameter(2);
  
  Histo_2ns19_100->Fit(f_19,"R");
  means_2ns_100[9]=f_19->GetParameter(1);
  sigmas_2ns_100[9]=f_19->GetParameter(2);
  
  Histo_2ns20_100->Fit(f_20,"R");
  means_2ns_100[10]=f_20->GetParameter(1);
  sigmas_2ns_100[10]=f_20->GetParameter(2);

  entries_2ns_100[0]=Histo_2ns2_100->Integral(500+(means_2ns_100[0]-nsigma*sigmas_2ns_100[0]),500+(means_2ns_100[0]+nsigma*sigmas_2ns_100[0]))/1000;
  entries_2ns_100[1]=Histo_2ns4_100->Integral(500+(means_2ns_100[1]-nsigma*sigmas_2ns_100[1]),500+(means_2ns_100[1]+nsigma*sigmas_2ns_100[1]))/1000;
  entries_2ns_100[2]=Histo_2ns6_100->Integral(500+(means_2ns_100[2]-nsigma*sigmas_2ns_100[2]),500+(means_2ns_100[2]+nsigma*sigmas_2ns_100[2]))/1000;
  entries_2ns_100[3]=Histo_2ns8_100->Integral(500+(means_2ns_100[3]-nsigma*sigmas_2ns_100[3]),500+(means_2ns_100[3]+nsigma*sigmas_2ns_100[3]))/1000;
  entries_2ns_100[4]=Histo_2ns10_100->Integral(500+(means_2ns_100[4]-nsigma*sigmas_2ns_100[4]),500+(means_2ns_100[4]+nsigma*sigmas_2ns_100[4]))/1000;
  entries_2ns_100[5]=Histo_2ns12_100->Integral(500+(means_2ns_100[5]-nsigma*sigmas_2ns_100[5]),500+(means_2ns_100[5]+nsigma*sigmas_2ns_100[5]))/1000;
  entries_2ns_100[6]=Histo_2ns14_100->Integral(500+(means_2ns_100[6]-nsigma*sigmas_2ns_100[6]),500+(means_2ns_100[6]+nsigma*sigmas_2ns_100[6]))/1000;
  entries_2ns_100[7]=Histo_2ns16_100->Integral(500+(means_2ns_100[7]-nsigma*sigmas_2ns_100[7]),500+(means_2ns_100[7]+nsigma*sigmas_2ns_100[7]))/1000;
  entries_2ns_100[8]=Histo_2ns18_100->Integral(500+(means_2ns_100[8]-nsigma*sigmas_2ns_100[8]),500+(means_2ns_100[8]+nsigma*sigmas_2ns_100[8]))/1000;
  entries_2ns_100[9]=Histo_2ns19_100->Integral(500+(means_2ns_100[9]-nsigma*sigmas_2ns_100[9]),500+(means_2ns_100[9]+nsigma*sigmas_2ns_100[9]))/1000;
  entries_2ns_100[10]=Histo_2ns20_100->Integral(500+(means_2ns_100[10]-nsigma*sigmas_2ns_100[10]),500+(means_2ns_100[10]+nsigma*sigmas_2ns_100[10]))/1000;

  TCanvas *c100_2ns = new TCanvas();
  c100_2ns->cd();
  Histo_2ns20_100->SetLineColorAlpha(kBlue+3,0.65);
  Histo_2ns20_100->SetTitle( "M^{2}_{miss} for different A' masses (100 e+ per bunch)");
  Histo_2ns20_100->SetLineWidth(3);
  Histo_2ns20_100->GetYaxis()->SetTitle("N_{events}");
  Histo_2ns20_100->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_2ns19_100->SetLineColorAlpha(kBlue,0.65);
  Histo_2ns19_100->SetLineWidth(3);
  Histo_2ns18_100->SetLineColorAlpha(kCyan+3,0.65);
  Histo_2ns18_100->SetLineWidth(3);
  Histo_2ns16_100->SetLineColorAlpha(kCyan+1,0.65);
  Histo_2ns16_100->SetLineWidth(3);
  Histo_2ns14_100->SetLineColorAlpha(kGreen+3,0.65);
  Histo_2ns14_100->SetLineWidth(3);
  Histo_2ns12_100->SetLineColorAlpha(kGreen+1,0.65);
  Histo_2ns12_100->SetLineWidth(3);
  Histo_2ns10_100->SetLineColorAlpha(kYellow+2,0.65);
  Histo_2ns10_100->SetLineWidth(3);
  Histo_2ns8_100->SetLineColorAlpha(kOrange-3,0.65);
  Histo_2ns8_100->SetLineWidth(3);
  Histo_2ns6_100->SetLineColorAlpha(kRed+2,0.65);
  Histo_2ns6_100->SetLineWidth(3);
  Histo_2ns4_100->SetLineColorAlpha(kRed,0.65);
  Histo_2ns4_100->SetLineWidth(3);
  Histo_2ns2_100->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_2ns2_100->SetLineWidth(3);
    
  Histo_2ns20_100->Draw();
  Histo_2ns19_100->Draw("same");
  Histo_2ns18_100->Draw("same");
  Histo_2ns4_100->Draw("same");
  Histo_2ns6_100->Draw("same");
  Histo_2ns8_100->Draw("same");
  Histo_2ns10_100->Draw("same");
  Histo_2ns12_100->Draw("same");
  Histo_2ns14_100->Draw("same");
  Histo_2ns16_100->Draw("same");
  Histo_2ns2_100->Draw("same");

  auto legend_100_2ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_100_2ns->AddEntry(Histo_2ns20_100,"M_{A'} = 20 MeV","l");
  legend_100_2ns->AddEntry(Histo_2ns19_100,"M_{A'} = 19 MeV","l");
  legend_100_2ns->AddEntry(Histo_2ns18_100,"M_{A'} = 18 MeV","l");
  legend_100_2ns->AddEntry(Histo_2ns16_100,"M_{A'} = 16 MeV","l");
  legend_100_2ns->AddEntry(Histo_2ns14_100,"M_{A'} = 14 MeV","l");
  legend_100_2ns->AddEntry(Histo_2ns12_100,"M_{A'} = 12 MeV","l");
  legend_100_2ns->AddEntry(Histo_2ns10_100,"M_{A'} = 10 MeV","l");
  legend_100_2ns->AddEntry(Histo_2ns8_100,"M_{A'} = 8 MeV","l");
  legend_100_2ns->AddEntry(Histo_2ns6_100,"M_{A'} = 6 MeV","l");
  legend_100_2ns->AddEntry(Histo_2ns4_100,"M_{A'} = 4 MeV","l");
  legend_100_2ns->AddEntry(Histo_2ns2_100,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_100_2ns->Draw();

  outRoot->WriteObject(c100_2ns,"100e_all_2ns");

  
  if(!(Histo_2ns20_1000->GetSumw2N() > 0)) Histo_2ns20_1000->Sumw2(kTRUE);
  Histo_2ns20_1000->Add(Histo_2ns0_1000_30,-1);
  if(!(Histo_2ns19_1000->GetSumw2N() > 0)) Histo_2ns19_1000->Sumw2(kTRUE);
  Histo_2ns19_1000->Add(Histo_2ns0_1000_30,-1);
  if(!(Histo_2ns18_1000->GetSumw2N() > 0)) Histo_2ns18_1000->Sumw2(kTRUE);
  Histo_2ns18_1000->Add(Histo_2ns0_1000_50,-1);
  if(!(Histo_2ns16_1000->GetSumw2N() > 0)) Histo_2ns16_1000->Sumw2(kTRUE);
  Histo_2ns16_1000->Add(Histo_2ns0_1000_60,-1);
  if(!(Histo_2ns14_1000->GetSumw2N() > 0)) Histo_2ns14_1000->Sumw2(kTRUE);
  Histo_2ns14_1000->Add(Histo_2ns0_1000_60,-1);
  if(!(Histo_2ns12_1000->GetSumw2N() > 0)) Histo_2ns12_1000->Sumw2(kTRUE);
  Histo_2ns12_1000->Add(Histo_2ns0_1000_60,-1);
  if(!(Histo_2ns10_1000->GetSumw2N() > 0)) Histo_2ns10_1000->Sumw2(kTRUE);
  Histo_2ns10_1000->Add(Histo_2ns0_1000_60,-1);
  if(!(Histo_2ns8_1000->GetSumw2N() > 0)) Histo_2ns8_1000->Sumw2(kTRUE);
  Histo_2ns8_1000->Add(Histo_2ns0_1000_60,-1);
  if(!(Histo_2ns6_1000->GetSumw2N() > 0)) Histo_2ns6_1000->Sumw2(kTRUE);
  Histo_2ns6_1000->Add(Histo_2ns0_1000_60,-1);
  if(!(Histo_2ns4_1000->GetSumw2N() > 0)) Histo_2ns4_1000->Sumw2(kTRUE);
  Histo_2ns4_1000->Add(Histo_2ns0_1000_60,-1);
  if(!(Histo_2ns2_1000->GetSumw2N() > 0)) Histo_2ns2_1000->Sumw2(kTRUE);
  Histo_2ns2_1000->Add(Histo_2ns0_1000_60,-1);


  Histo_2ns2_1000->Fit(f_2,"R");
  means_2ns_1000[0]=f_2->GetParameter(1);
  sigmas_2ns_1000[0]=f_2->GetParameter(2);

  Histo_2ns4_1000->Fit(f_4,"R");
  means_2ns_1000[1]=f_4->GetParameter(1);
  sigmas_2ns_1000[1]=f_4->GetParameter(2);
  
  Histo_2ns6_1000->Fit(f_6,"R");
  means_2ns_1000[2]=f_6->GetParameter(1);
  sigmas_2ns_1000[2]=f_6->GetParameter(2);
  
  Histo_2ns8_1000->Fit(f_8,"R");
  means_2ns_1000[3]=f_8->GetParameter(1);
  sigmas_2ns_1000[3]=f_8->GetParameter(2);
 
  Histo_2ns10_1000->Fit(f_10,"R");
  means_2ns_1000[4]=f_10->GetParameter(1);
  sigmas_2ns_1000[4]=f_10->GetParameter(2);
  
  Histo_2ns12_1000->Fit(f_12,"R");
  means_2ns_1000[5]=f_12->GetParameter(1);
  sigmas_2ns_1000[5]=f_12->GetParameter(2);
  
  Histo_2ns14_1000->Fit(f_14,"R");
  means_2ns_1000[6]=f_14->GetParameter(1);
  sigmas_2ns_1000[6]=f_14->GetParameter(2);
  
  Histo_2ns16_1000->Fit(f_16,"R");
  means_2ns_1000[7]=f_16->GetParameter(1);
  sigmas_2ns_1000[7]=f_16->GetParameter(2);
  
  Histo_2ns18_1000->Fit(f_18,"R");
  means_2ns_1000[8]=f_18->GetParameter(1);
  sigmas_2ns_1000[8]=f_18->GetParameter(2);
  
  Histo_2ns19_1000->Fit(f_19,"R");
  means_2ns_1000[9]=f_19->GetParameter(1);
  sigmas_2ns_1000[9]=f_19->GetParameter(2);
  
  Histo_2ns20_1000->Fit(f_20,"R");
  means_2ns_1000[10]=f_20->GetParameter(1);
  sigmas_2ns_1000[10]=f_20->GetParameter(2);

  entries_2ns_1000[0]=Histo_2ns2_1000->Integral(500+(means_2ns_1000[0]-nsigma*sigmas_2ns_1000[0]),500+(means_2ns_1000[0]+nsigma*sigmas_2ns_1000[0]))/1000;
  entries_2ns_1000[1]=Histo_2ns4_1000->Integral(500+(means_2ns_1000[1]-nsigma*sigmas_2ns_1000[1]),500+(means_2ns_1000[1]+nsigma*sigmas_2ns_1000[1]))/1000;
  entries_2ns_1000[2]=Histo_2ns6_1000->Integral(500+(means_2ns_1000[2]-nsigma*sigmas_2ns_1000[2]),500+(means_2ns_1000[2]+nsigma*sigmas_2ns_1000[2]))/1000;
  entries_2ns_1000[3]=Histo_2ns8_1000->Integral(500+(means_2ns_1000[3]-nsigma*sigmas_2ns_1000[3]),500+(means_2ns_1000[3]+nsigma*sigmas_2ns_1000[3]))/1000;
  entries_2ns_1000[4]=Histo_2ns10_1000->Integral(500+(means_2ns_1000[4]-nsigma*sigmas_2ns_1000[4]),500+(means_2ns_1000[4]+nsigma*sigmas_2ns_1000[4]))/1000;
  entries_2ns_1000[5]=Histo_2ns12_1000->Integral(500+(means_2ns_1000[5]-nsigma*sigmas_2ns_1000[5]),500+(means_2ns_1000[5]+nsigma*sigmas_2ns_1000[5]))/1000;
  entries_2ns_1000[6]=Histo_2ns14_1000->Integral(500+(means_2ns_1000[6]-nsigma*sigmas_2ns_1000[6]),500+(means_2ns_1000[6]+nsigma*sigmas_2ns_1000[6]))/1000;
  entries_2ns_1000[7]=Histo_2ns16_1000->Integral(500+(means_2ns_1000[7]-nsigma*sigmas_2ns_1000[7]),500+(means_2ns_1000[7]+nsigma*sigmas_2ns_1000[7]))/1000;
  entries_2ns_1000[8]=Histo_2ns18_1000->Integral(500+(means_2ns_1000[8]-nsigma*sigmas_2ns_1000[8]),500+(means_2ns_1000[8]+nsigma*sigmas_2ns_1000[8]))/1000;
  entries_2ns_1000[9]=Histo_2ns19_1000->Integral(500+(means_2ns_1000[9]-nsigma*sigmas_2ns_1000[9]),500+(means_2ns_1000[9]+nsigma*sigmas_2ns_1000[9]))/1000;
  entries_2ns_1000[10]=Histo_2ns20_1000->Integral(500+(means_2ns_1000[10]-nsigma*sigmas_2ns_1000[10]),500+(means_2ns_1000[10]+nsigma*sigmas_2ns_1000[10]))/1000;

  TCanvas *c1000_2ns = new TCanvas();
  c1000_2ns->cd();
  Histo_2ns20_1000->SetLineColorAlpha(kBlue+3,0.65);
  Histo_2ns20_1000->SetTitle( "M^{2}_{miss} for different A' masses (1000 e+ per bunch)");
  Histo_2ns20_1000->SetLineWidth(3);
  Histo_2ns20_1000->GetYaxis()->SetTitle("N_{events}");
  Histo_2ns20_1000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_2ns19_1000->SetLineColorAlpha(kBlue,0.65);
  Histo_2ns19_1000->SetLineWidth(3);
  Histo_2ns18_1000->SetLineColorAlpha(kCyan+3,0.65);
  Histo_2ns18_1000->SetLineWidth(3);
  Histo_2ns16_1000->SetLineColorAlpha(kCyan+1,0.65);
  Histo_2ns16_1000->SetLineWidth(3);
  Histo_2ns14_1000->SetLineColorAlpha(kGreen+3,0.65);
  Histo_2ns14_1000->SetLineWidth(3);
  Histo_2ns12_1000->SetLineColorAlpha(kGreen+1,0.65);
  Histo_2ns12_1000->SetLineWidth(3);
  Histo_2ns10_1000->SetLineColorAlpha(kYellow+2,0.65);
  Histo_2ns10_1000->SetLineWidth(3);
  Histo_2ns8_1000->SetLineColorAlpha(kOrange-3,0.65);
  Histo_2ns8_1000->SetLineWidth(3);
  Histo_2ns6_1000->SetLineColorAlpha(kRed+2,0.65);
  Histo_2ns6_1000->SetLineWidth(3);
  Histo_2ns4_1000->SetLineColorAlpha(kRed,0.65);
  Histo_2ns4_1000->SetLineWidth(3);
  Histo_2ns2_1000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_2ns2_1000->SetLineWidth(3);
    
  Histo_2ns20_1000->Draw();
  Histo_2ns19_1000->Draw("same");
  Histo_2ns18_1000->Draw("same");
  Histo_2ns4_1000->Draw("same");
  Histo_2ns6_1000->Draw("same");
  Histo_2ns8_1000->Draw("same");
  Histo_2ns10_1000->Draw("same");
  Histo_2ns12_1000->Draw("same");
  Histo_2ns14_1000->Draw("same");
  Histo_2ns16_1000->Draw("same");
  Histo_2ns2_1000->Draw("same");

  auto legend_1000_2ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_1000_2ns->AddEntry(Histo_2ns20_1000,"M_{A'} = 20 MeV","l");
  legend_1000_2ns->AddEntry(Histo_2ns19_1000,"M_{A'} = 19 MeV","l");
  legend_1000_2ns->AddEntry(Histo_2ns18_1000,"M_{A'} = 18 MeV","l");
  legend_1000_2ns->AddEntry(Histo_2ns16_1000,"M_{A'} = 16 MeV","l");
  legend_1000_2ns->AddEntry(Histo_2ns14_1000,"M_{A'} = 14 MeV","l");
  legend_1000_2ns->AddEntry(Histo_2ns12_1000,"M_{A'} = 12 MeV","l");
  legend_1000_2ns->AddEntry(Histo_2ns10_1000,"M_{A'} = 10 MeV","l");
  legend_1000_2ns->AddEntry(Histo_2ns8_1000,"M_{A'} = 8 MeV","l");
  legend_1000_2ns->AddEntry(Histo_2ns6_1000,"M_{A'} = 6 MeV","l");
  legend_1000_2ns->AddEntry(Histo_2ns4_1000,"M_{A'} = 4 MeV","l");
  legend_1000_2ns->AddEntry(Histo_2ns2_1000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_1000_2ns->Draw();

  outRoot->WriteObject(c1000_2ns,"1000e_all_2ns");

  
  if(!(Histo_2ns20_10000->GetSumw2N() > 0)) Histo_2ns20_10000->Sumw2(kTRUE);
  Histo_2ns20_10000->Add(Histo_2ns0_10000_30,-1);
  if(!(Histo_2ns19_10000->GetSumw2N() > 0)) Histo_2ns19_10000->Sumw2(kTRUE);
  Histo_2ns19_10000->Add(Histo_2ns0_10000_30,-1);
  if(!(Histo_2ns18_10000->GetSumw2N() > 0)) Histo_2ns18_10000->Sumw2(kTRUE);
  Histo_2ns18_10000->Add(Histo_2ns0_10000_50,-1);
  if(!(Histo_2ns16_10000->GetSumw2N() > 0)) Histo_2ns16_10000->Sumw2(kTRUE);
  Histo_2ns16_10000->Add(Histo_2ns0_10000_60,-1);
  if(!(Histo_2ns14_10000->GetSumw2N() > 0)) Histo_2ns14_10000->Sumw2(kTRUE);
  Histo_2ns14_10000->Add(Histo_2ns0_10000_60,-1);
  if(!(Histo_2ns12_10000->GetSumw2N() > 0)) Histo_2ns12_10000->Sumw2(kTRUE);
  Histo_2ns12_10000->Add(Histo_2ns0_10000_60,-1);
  if(!(Histo_2ns10_10000->GetSumw2N() > 0)) Histo_2ns10_10000->Sumw2(kTRUE);
  Histo_2ns10_10000->Add(Histo_2ns0_10000_60,-1);
  if(!(Histo_2ns8_10000->GetSumw2N() > 0)) Histo_2ns8_10000->Sumw2(kTRUE);
  Histo_2ns8_10000->Add(Histo_2ns0_10000_60,-1);
  if(!(Histo_2ns6_10000->GetSumw2N() > 0)) Histo_2ns6_10000->Sumw2(kTRUE);
  Histo_2ns6_10000->Add(Histo_2ns0_10000_60,-1);
  if(!(Histo_2ns4_10000->GetSumw2N() > 0)) Histo_2ns4_10000->Sumw2(kTRUE);
  Histo_2ns4_10000->Add(Histo_2ns0_10000_60,-1);
  if(!(Histo_2ns2_10000->GetSumw2N() > 0)) Histo_2ns2_10000->Sumw2(kTRUE);
  Histo_2ns2_10000->Add(Histo_2ns0_10000_60,-1);


  Histo_2ns2_10000->Fit(f_2,"R");
  means_2ns_10000[0]=f_2->GetParameter(1);
  sigmas_2ns_10000[0]=f_2->GetParameter(2);

  Histo_2ns4_10000->Fit(f_4,"R");
  means_2ns_10000[1]=f_4->GetParameter(1);
  sigmas_2ns_10000[1]=f_4->GetParameter(2);
  
  Histo_2ns6_10000->Fit(f_6,"R");
  means_2ns_10000[2]=f_6->GetParameter(1);
  sigmas_2ns_10000[2]=f_6->GetParameter(2);
  
  Histo_2ns8_10000->Fit(f_8,"R");
  means_2ns_10000[3]=f_8->GetParameter(1);
  sigmas_2ns_10000[3]=f_8->GetParameter(2);
 
  Histo_2ns10_10000->Fit(f_10,"R");
  means_2ns_10000[4]=f_10->GetParameter(1);
  sigmas_2ns_10000[4]=f_10->GetParameter(2);
  
  Histo_2ns12_10000->Fit(f_12,"R");
  means_2ns_10000[5]=f_12->GetParameter(1);
  sigmas_2ns_10000[5]=f_12->GetParameter(2);
  
  Histo_2ns14_10000->Fit(f_14,"R");
  means_2ns_10000[6]=f_14->GetParameter(1);
  sigmas_2ns_10000[6]=f_14->GetParameter(2);
  
  Histo_2ns16_10000->Fit(f_16,"R");
  means_2ns_10000[7]=f_16->GetParameter(1);
  sigmas_2ns_10000[7]=f_16->GetParameter(2);
  
  Histo_2ns18_10000->Fit(f_18,"R");
  means_2ns_10000[8]=f_18->GetParameter(1);
  sigmas_2ns_10000[8]=f_18->GetParameter(2);
  
  Histo_2ns19_10000->Fit(f_19,"R");
  means_2ns_10000[9]=f_19->GetParameter(1);
  sigmas_2ns_10000[9]=f_19->GetParameter(2);
  
  Histo_2ns20_10000->Fit(f_20,"R");
  means_2ns_10000[10]=f_20->GetParameter(1);
  sigmas_2ns_10000[10]=f_20->GetParameter(2);

  entries_2ns_10000[0]=Histo_2ns2_10000->Integral(500+(means_2ns_10000[0]-nsigma*sigmas_2ns_10000[0]),500+(means_2ns_10000[0]+nsigma*sigmas_2ns_10000[0]))/1000;
  entries_2ns_10000[1]=Histo_2ns4_10000->Integral(500+(means_2ns_10000[1]-nsigma*sigmas_2ns_10000[1]),500+(means_2ns_10000[1]+nsigma*sigmas_2ns_10000[1]))/1000;
  entries_2ns_10000[2]=Histo_2ns6_10000->Integral(500+(means_2ns_10000[2]-nsigma*sigmas_2ns_10000[2]),500+(means_2ns_10000[2]+nsigma*sigmas_2ns_10000[2]))/1000;
  entries_2ns_10000[3]=Histo_2ns8_10000->Integral(500+(means_2ns_10000[3]-nsigma*sigmas_2ns_10000[3]),500+(means_2ns_10000[3]+nsigma*sigmas_2ns_10000[3]))/1000;
  entries_2ns_10000[4]=Histo_2ns10_10000->Integral(500+(means_2ns_10000[4]-nsigma*sigmas_2ns_10000[4]),500+(means_2ns_10000[4]+nsigma*sigmas_2ns_10000[4]))/1000;
  entries_2ns_10000[5]=Histo_2ns12_10000->Integral(500+(means_2ns_10000[5]-nsigma*sigmas_2ns_10000[5]),500+(means_2ns_10000[5]+nsigma*sigmas_2ns_10000[5]))/1000;
  entries_2ns_10000[6]=Histo_2ns14_10000->Integral(500+(means_2ns_10000[6]-nsigma*sigmas_2ns_10000[6]),500+(means_2ns_10000[6]+nsigma*sigmas_2ns_10000[6]))/1000;
  entries_2ns_10000[7]=Histo_2ns16_10000->Integral(500+(means_2ns_10000[7]-nsigma*sigmas_2ns_10000[7]),500+(means_2ns_10000[7]+nsigma*sigmas_2ns_10000[7]))/1000;
  entries_2ns_10000[8]=Histo_2ns18_10000->Integral(500+(means_2ns_10000[8]-nsigma*sigmas_2ns_10000[8]),500+(means_2ns_10000[8]+nsigma*sigmas_2ns_10000[8]))/1000;
  entries_2ns_10000[9]=Histo_2ns19_10000->Integral(500+(means_2ns_10000[9]-nsigma*sigmas_2ns_10000[9]),500+(means_2ns_10000[9]+nsigma*sigmas_2ns_10000[9]))/1000;
  entries_2ns_10000[10]=Histo_2ns20_10000->Integral(500+(means_2ns_10000[10]-nsigma*sigmas_2ns_10000[10]),500+(means_2ns_10000[10]+nsigma*sigmas_2ns_10000[10]))/1000;


  TCanvas *c10000_2ns = new TCanvas();
  c10000_2ns->cd();
  Histo_2ns20_10000->SetLineColorAlpha(kBlue+3,0.65);
  Histo_2ns20_10000->SetTitle( "M^{2}_{miss} for different A' masses (10000 e+ per bunch)");
  Histo_2ns20_10000->SetLineWidth(3);
  Histo_2ns20_10000->GetYaxis()->SetTitle("N_{events}");
  Histo_2ns20_10000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_2ns19_10000->SetLineColorAlpha(kBlue,0.65);
  Histo_2ns19_10000->SetLineWidth(3);
  Histo_2ns18_10000->SetLineColorAlpha(kCyan+3,0.65);
  Histo_2ns18_10000->SetLineWidth(3);
  Histo_2ns16_10000->SetLineColorAlpha(kCyan+1,0.65);
  Histo_2ns16_10000->SetLineWidth(3);
  Histo_2ns14_10000->SetLineColorAlpha(kGreen+3,0.65);
  Histo_2ns14_10000->SetLineWidth(3);
  Histo_2ns12_10000->SetLineColorAlpha(kGreen+1,0.65);
  Histo_2ns12_10000->SetLineWidth(3);
  Histo_2ns10_10000->SetLineColorAlpha(kYellow+2,0.65);
  Histo_2ns10_10000->SetLineWidth(3);
  Histo_2ns8_10000->SetLineColorAlpha(kOrange-3,0.65);
  Histo_2ns8_10000->SetLineWidth(3);
  Histo_2ns6_10000->SetLineColorAlpha(kRed+2,0.65);
  Histo_2ns6_10000->SetLineWidth(3);
  Histo_2ns4_10000->SetLineColorAlpha(kRed,0.65);
  Histo_2ns4_10000->SetLineWidth(3);
  Histo_2ns2_10000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_2ns2_10000->SetLineWidth(3);
    
  Histo_2ns20_10000->Draw();
  Histo_2ns19_10000->Draw("same");
  Histo_2ns18_10000->Draw("same");
  Histo_2ns4_10000->Draw("same");
  Histo_2ns6_10000->Draw("same");
  Histo_2ns8_10000->Draw("same");
  Histo_2ns10_10000->Draw("same");
  Histo_2ns12_10000->Draw("same");
  Histo_2ns14_10000->Draw("same");
  Histo_2ns16_10000->Draw("same");
  Histo_2ns2_10000->Draw("same");

  auto legend_10000_2ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_10000_2ns->AddEntry(Histo_2ns20_10000,"M_{A'} = 20 MeV","l");
  legend_10000_2ns->AddEntry(Histo_2ns19_10000,"M_{A'} = 19 MeV","l");
  legend_10000_2ns->AddEntry(Histo_2ns18_10000,"M_{A'} = 18 MeV","l");
  legend_10000_2ns->AddEntry(Histo_2ns16_10000,"M_{A'} = 16 MeV","l");
  legend_10000_2ns->AddEntry(Histo_2ns14_10000,"M_{A'} = 14 MeV","l");
  legend_10000_2ns->AddEntry(Histo_2ns12_10000,"M_{A'} = 12 MeV","l");
  legend_10000_2ns->AddEntry(Histo_2ns10_10000,"M_{A'} = 10 MeV","l");
  legend_10000_2ns->AddEntry(Histo_2ns8_10000,"M_{A'} = 8 MeV","l");
  legend_10000_2ns->AddEntry(Histo_2ns6_10000,"M_{A'} = 6 MeV","l");
  legend_10000_2ns->AddEntry(Histo_2ns4_10000,"M_{A'} = 4 MeV","l");
  legend_10000_2ns->AddEntry(Histo_2ns2_10000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_10000_2ns->Draw();

  outRoot->WriteObject(c10000_2ns,"10000e_all_2ns");

  
  if(!(Histo_2ns20_25000->GetSumw2N() > 0)) Histo_2ns20_25000->Sumw2(kTRUE);
  Histo_2ns20_25000->Add(Histo_2ns0_25000_30,-1);
  if(!(Histo_2ns19_25000->GetSumw2N() > 0)) Histo_2ns19_25000->Sumw2(kTRUE);
  Histo_2ns19_25000->Add(Histo_2ns0_25000_30,-1);
  if(!(Histo_2ns18_25000->GetSumw2N() > 0)) Histo_2ns18_25000->Sumw2(kTRUE);
  Histo_2ns18_25000->Add(Histo_2ns0_25000_50,-1);
  if(!(Histo_2ns16_25000->GetSumw2N() > 0)) Histo_2ns16_25000->Sumw2(kTRUE);
  Histo_2ns16_25000->Add(Histo_2ns0_25000_60,-1);
  if(!(Histo_2ns14_25000->GetSumw2N() > 0)) Histo_2ns14_25000->Sumw2(kTRUE);
  Histo_2ns14_25000->Add(Histo_2ns0_25000_60,-1);
  if(!(Histo_2ns12_25000->GetSumw2N() > 0)) Histo_2ns12_25000->Sumw2(kTRUE);
  Histo_2ns12_25000->Add(Histo_2ns0_25000_60,-1);
  if(!(Histo_2ns10_25000->GetSumw2N() > 0)) Histo_2ns10_25000->Sumw2(kTRUE);
  Histo_2ns10_25000->Add(Histo_2ns0_25000_60,-1);
  if(!(Histo_2ns8_25000->GetSumw2N() > 0)) Histo_2ns8_25000->Sumw2(kTRUE);
  Histo_2ns8_25000->Add(Histo_2ns0_25000_60,-1);
  if(!(Histo_2ns6_25000->GetSumw2N() > 0)) Histo_2ns6_25000->Sumw2(kTRUE);
  Histo_2ns6_25000->Add(Histo_2ns0_25000_60,-1);
  if(!(Histo_2ns4_25000->GetSumw2N() > 0)) Histo_2ns4_25000->Sumw2(kTRUE);
  Histo_2ns4_25000->Add(Histo_2ns0_25000_60,-1);
  if(!(Histo_2ns2_25000->GetSumw2N() > 0)) Histo_2ns2_25000->Sumw2(kTRUE);
  Histo_2ns2_25000->Add(Histo_2ns0_25000_60,-1);

  Histo_2ns2_25000->Fit(f_2,"R");
  means_2ns_25000[0]=f_2->GetParameter(1);
  sigmas_2ns_25000[0]=f_2->GetParameter(2);

  Histo_2ns4_25000->Fit(f_4,"R");
  means_2ns_25000[1]=f_4->GetParameter(1);
  sigmas_2ns_25000[1]=f_4->GetParameter(2);
  
  Histo_2ns6_25000->Fit(f_6,"R");
  means_2ns_25000[2]=f_6->GetParameter(1);
  sigmas_2ns_25000[2]=f_6->GetParameter(2);
  
  Histo_2ns8_25000->Fit(f_8,"R");
  means_2ns_25000[3]=f_8->GetParameter(1);
  sigmas_2ns_25000[3]=f_8->GetParameter(2);
 
  Histo_2ns10_25000->Fit(f_10,"R");
  means_2ns_25000[4]=f_10->GetParameter(1);
  sigmas_2ns_25000[4]=f_10->GetParameter(2);
  
  Histo_2ns12_25000->Fit(f_12,"R");
  means_2ns_25000[5]=f_12->GetParameter(1);
  sigmas_2ns_25000[5]=f_12->GetParameter(2);
  
  Histo_2ns14_25000->Fit(f_14,"R");
  means_2ns_25000[6]=f_14->GetParameter(1);
  sigmas_2ns_25000[6]=f_14->GetParameter(2);
  
  Histo_2ns16_25000->Fit(f_16,"R");
  means_2ns_25000[7]=f_16->GetParameter(1);
  sigmas_2ns_25000[7]=f_16->GetParameter(2);
  
  Histo_2ns18_25000->Fit(f_18,"R");
  means_2ns_25000[8]=f_18->GetParameter(1);
  sigmas_2ns_25000[8]=f_18->GetParameter(2);
  
  Histo_2ns19_25000->Fit(f_19,"R");
  means_2ns_25000[9]=f_19->GetParameter(1);
  sigmas_2ns_25000[9]=f_19->GetParameter(2);
  
  Histo_2ns20_25000->Fit(f_20,"R");
  means_2ns_25000[10]=f_20->GetParameter(1);
  sigmas_2ns_25000[10]=f_20->GetParameter(2);

  entries_2ns_25000[0]=Histo_2ns2_25000->Integral(500+(means_2ns_25000[0]-nsigma*sigmas_2ns_25000[0]),500+(means_2ns_25000[0]+nsigma*sigmas_2ns_25000[0]))/1000;
  entries_2ns_25000[1]=Histo_2ns4_25000->Integral(500+(means_2ns_25000[1]-nsigma*sigmas_2ns_25000[1]),500+(means_2ns_25000[1]+nsigma*sigmas_2ns_25000[1]))/1000;
  entries_2ns_25000[2]=Histo_2ns6_25000->Integral(500+(means_2ns_25000[2]-nsigma*sigmas_2ns_25000[2]),500+(means_2ns_25000[2]+nsigma*sigmas_2ns_25000[2]))/1000;
  entries_2ns_25000[3]=Histo_2ns8_25000->Integral(500+(means_2ns_25000[3]-nsigma*sigmas_2ns_25000[3]),500+(means_2ns_25000[3]+nsigma*sigmas_2ns_25000[3]))/1000;
  entries_2ns_25000[4]=Histo_2ns10_25000->Integral(500+(means_2ns_25000[4]-nsigma*sigmas_2ns_25000[4]),500+(means_2ns_25000[4]+nsigma*sigmas_2ns_25000[4]))/1000;
  entries_2ns_25000[5]=Histo_2ns12_25000->Integral(500+(means_2ns_25000[5]-nsigma*sigmas_2ns_25000[5]),500+(means_2ns_25000[5]+nsigma*sigmas_2ns_25000[5]))/1000;
  entries_2ns_25000[6]=Histo_2ns14_25000->Integral(500+(means_2ns_25000[6]-nsigma*sigmas_2ns_25000[6]),500+(means_2ns_25000[6]+nsigma*sigmas_2ns_25000[6]))/1000;
  entries_2ns_25000[7]=Histo_2ns16_25000->Integral(500+(means_2ns_25000[7]-nsigma*sigmas_2ns_25000[7]),500+(means_2ns_25000[7]+nsigma*sigmas_2ns_25000[7]))/1000;
  entries_2ns_25000[8]=Histo_2ns18_25000->Integral(500+(means_2ns_25000[8]-nsigma*sigmas_2ns_25000[8]),500+(means_2ns_25000[8]+nsigma*sigmas_2ns_25000[8]))/1000;
  entries_2ns_25000[9]=Histo_2ns19_25000->Integral(500+(means_2ns_25000[9]-nsigma*sigmas_2ns_25000[9]),500+(means_2ns_25000[9]+nsigma*sigmas_2ns_25000[9]))/1000;
  entries_2ns_25000[10]=Histo_2ns20_25000->Integral(500+(means_2ns_25000[10]-nsigma*sigmas_2ns_25000[10]),500+(means_2ns_25000[10]+nsigma*sigmas_2ns_25000[10]))/1000;

  TCanvas *c25000_2ns = new TCanvas();
  c25000_2ns->cd();
  Histo_2ns20_25000->SetLineColorAlpha(kBlue+3,0.65);
  Histo_2ns20_25000->SetTitle( "M^{2}_{miss} for different A' masses (25000 e+ per bunch)");
  Histo_2ns20_25000->SetLineWidth(3);
  Histo_2ns20_25000->GetYaxis()->SetTitle("N_{events}");
  Histo_2ns20_25000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_2ns19_25000->SetLineColorAlpha(kBlue,0.65);
  Histo_2ns19_25000->SetLineWidth(3);
  Histo_2ns18_25000->SetLineColorAlpha(kCyan+3,0.65);
  Histo_2ns18_25000->SetLineWidth(3);
  Histo_2ns16_25000->SetLineColorAlpha(kCyan+1,0.65);
  Histo_2ns16_25000->SetLineWidth(3);
  Histo_2ns14_25000->SetLineColorAlpha(kGreen+3,0.65);
  Histo_2ns14_25000->SetLineWidth(3);
  Histo_2ns12_25000->SetLineColorAlpha(kGreen+1,0.65);
  Histo_2ns12_25000->SetLineWidth(3);
  Histo_2ns10_25000->SetLineColorAlpha(kYellow+2,0.65);
  Histo_2ns10_25000->SetLineWidth(3);
  Histo_2ns8_25000->SetLineColorAlpha(kOrange-3,0.65);
  Histo_2ns8_25000->SetLineWidth(3);
  Histo_2ns6_25000->SetLineColorAlpha(kRed+2,0.65);
  Histo_2ns6_25000->SetLineWidth(3);
  Histo_2ns4_25000->SetLineColorAlpha(kRed,0.65);
  Histo_2ns4_25000->SetLineWidth(3);
  Histo_2ns2_25000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_2ns2_25000->SetLineWidth(3);
    
  Histo_2ns20_25000->Draw();
  Histo_2ns19_25000->Draw("same");
  Histo_2ns18_25000->Draw("same");
  Histo_2ns4_25000->Draw("same");
  Histo_2ns6_25000->Draw("same");
  Histo_2ns8_25000->Draw("same");
  Histo_2ns10_25000->Draw("same");
  Histo_2ns12_25000->Draw("same");
  Histo_2ns14_25000->Draw("same");
  Histo_2ns16_25000->Draw("same");
  Histo_2ns2_25000->Draw("same");

  auto legend_25000_2ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_25000_2ns->AddEntry(Histo_2ns20_25000,"M_{A'} = 20 MeV","l");
  legend_25000_2ns->AddEntry(Histo_2ns19_25000,"M_{A'} = 19 MeV","l");
  legend_25000_2ns->AddEntry(Histo_2ns18_25000,"M_{A'} = 18 MeV","l");
  legend_25000_2ns->AddEntry(Histo_2ns16_25000,"M_{A'} = 16 MeV","l");
  legend_25000_2ns->AddEntry(Histo_2ns14_25000,"M_{A'} = 14 MeV","l");
  legend_25000_2ns->AddEntry(Histo_2ns12_25000,"M_{A'} = 12 MeV","l");
  legend_25000_2ns->AddEntry(Histo_2ns10_25000,"M_{A'} = 10 MeV","l");
  legend_25000_2ns->AddEntry(Histo_2ns8_25000,"M_{A'} = 8 MeV","l");
  legend_25000_2ns->AddEntry(Histo_2ns6_25000,"M_{A'} = 6 MeV","l");
  legend_25000_2ns->AddEntry(Histo_2ns4_25000,"M_{A'} = 4 MeV","l");
  legend_25000_2ns->AddEntry(Histo_2ns2_25000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_25000_2ns->Draw();

  outRoot->WriteObject(c25000_2ns,"25000e_all_2ns");

  
  TCanvas *cgr_2ns = new TCanvas();
 
  auto gr_1_2ns = new TGraph (11,masses,entries_2ns_1);
  gr_1_2ns->SetLineColor(kMagenta);
  gr_1_2ns->SetLineWidth(3); 
  gr_1_2ns->GetYaxis()->SetTitle("Acceptance [%]");
  gr_1_2ns->GetXaxis()->SetTitle("M_{A'} [MeV]");
  gr_1_2ns->SetTitle("Acceptance");
  gr_1_2ns->Draw();
  auto gr_10_2ns = new TGraph (11,masses,entries_2ns_10);
  gr_10_2ns->SetLineColor(kRed);
  gr_10_2ns->SetLineWidth(3);
  gr_10_2ns->Draw("same");
  auto gr_100_2ns = new TGraph (11,masses,entries_2ns_100);
  gr_100_2ns->SetLineColor(kOrange);
  gr_100_2ns->SetLineWidth(3);
  gr_100_2ns->Draw("same");
  auto gr_1000_2ns = new TGraph (11,masses,entries_2ns_1000);
  gr_1000_2ns->SetLineColor(kGreen+3);
  gr_1000_2ns->SetLineWidth(3);
  gr_1000_2ns->Draw("same");
  auto gr_10000_2ns = new TGraph (11,masses,entries_2ns_10000);
  gr_10000_2ns->SetLineColor(kCyan+1);
  gr_10000_2ns->SetLineWidth(3);
  gr_10000_2ns->Draw("same");
  auto gr_25000_2ns = new TGraph (11,masses,entries_2ns_25000);
  gr_25000_2ns->SetLineColor(kBlue);
  gr_25000_2ns->SetLineWidth(3);
  gr_25000_2ns->Draw("same");
  auto legend_gr_2ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_gr_2ns->AddEntry(gr_1_2ns,"1 e+","l");
  legend_gr_2ns->AddEntry(gr_10_2ns,"10 e+","l");
  legend_gr_2ns->AddEntry(gr_100_2ns,"100 e+","l");
  legend_gr_2ns->AddEntry(gr_1000_2ns,"1000 e+","l");
  legend_gr_2ns->AddEntry(gr_10000_2ns,"10000 e+","l");
  legend_gr_2ns->AddEntry(gr_25000_2ns,"25000 e+","l");
  legend_gr_2ns->Draw();

  TCanvas *csig_2ns = new TCanvas();
 
  auto sig_1_2ns = new TGraph (11,masses,sigmas_2ns_1);
  sig_1_2ns->SetLineColor(kMagenta);
  sig_1_2ns->SetLineWidth(3);
  sig_1_2ns->GetYaxis()->SetTitle("Sigma");
  sig_1_2ns->GetXaxis()->SetTitle("M_{A'} [MeV]");
  sig_1_2ns->SetTitle("Sigma");
  sig_1_2ns->Draw();
  auto sig_10_2ns = new TGraph (11,masses,sigmas_2ns_10);
  sig_10_2ns->SetLineColor(kRed);
  sig_10_2ns->SetLineWidth(3);
  sig_10_2ns->Draw("same");
  auto sig_100_2ns = new TGraph (11,masses,sigmas_2ns_100);
  sig_100_2ns->SetLineColor(kOrange);
  sig_100_2ns->SetLineWidth(3);
  sig_100_2ns->Draw("same");
  auto sig_1000_2ns = new TGraph (11,masses,sigmas_2ns_1000);
  sig_1000_2ns->SetLineColor(kGreen+3);
  sig_1000_2ns->SetLineWidth(3);
  sig_1000_2ns->Draw("same");
  auto sig_10000_2ns = new TGraph (11,masses,sigmas_2ns_10000);
  sig_10000_2ns->SetLineColor(kCyan+1);
  sig_10000_2ns->SetLineWidth(3);
  sig_10000_2ns->Draw("same");
  auto sig_25000_2ns = new TGraph (11,masses,sigmas_2ns_25000);
  sig_25000_2ns->SetLineColor(kBlue);
  sig_25000_2ns->SetLineWidth(3);
  sig_25000_2ns->Draw("same");
  auto legend_sig_2ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_sig_2ns->AddEntry(sig_1_2ns,"1 e+","l");
  legend_sig_2ns->AddEntry(sig_10_2ns,"10 e+","l");
  legend_sig_2ns->AddEntry(sig_100_2ns,"100 e+","l");
  legend_sig_2ns->AddEntry(sig_1000_2ns,"1000 e+","l");
  legend_sig_2ns->AddEntry(sig_10000_2ns,"10000 e+","l");
  legend_sig_2ns->AddEntry(sig_25000_2ns,"25000 e+","l");
  legend_sig_2ns->Draw();

  
  outRoot->WriteObject(cgr_2ns,"Acceptance_2ns");
  outRoot->WriteObject(csig_2ns,"Sigma_2ns");

   //----------------------------------------Do analysis for deltaT=5ns-------------------------------------------------Do analysis for deltaT=5ns-------------------------------------------------

  
  Float_t sigmas_5ns_1[11];
  Float_t means_5ns_1[11];
  Float_t entries_5ns_1[11];

  Float_t sigmas_5ns_10[11];
  Float_t means_5ns_10[11];
  Float_t entries_5ns_10[11];

  Float_t sigmas_5ns_100[11];
  Float_t means_5ns_100[11];
  Float_t entries_5ns_100[11];

  Float_t sigmas_5ns_1000[11];
  Float_t means_5ns_1000[11];
  Float_t entries_5ns_1000[11];

  Float_t sigmas_5ns_10000[11];
  Float_t means_5ns_10000[11];
  Float_t entries_5ns_10000[11];
  
  Float_t sigmas_5ns_25000[11];
  Float_t means_5ns_25000[11];
  Float_t entries_5ns_25000[11];
   
  //---------------------------------------------- Open Histograms --------------------------------------------------

  TH1F *Histo_5ns20_1 = (TH1F*) dir20_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns19_1 = (TH1F*) dir19_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns18_1 = (TH1F*) dir18_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_5ns");
  TH1F *Histo_5ns16_1 = (TH1F*) dir16_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns14_1 = (TH1F*) dir14_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns12_1 = (TH1F*) dir12_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns10_1 = (TH1F*) dir10_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns8_1 = (TH1F*) dir8_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns6_1 = (TH1F*) dir6_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns4_1 = (TH1F*) dir4_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns2_1 = (TH1F*) dir2_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns0_1_10 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_5ns");
  TH1F *Histo_5ns0_1_20 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_5ns");
  TH1F *Histo_5ns0_1_30 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns0_1_40 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_5ns");
  TH1F *Histo_5ns0_1_50 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_5ns");
  TH1F *Histo_5ns0_1_60 = (TH1F*) dir0_1->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  
  TH1F *Histo_5ns20_10 = (TH1F*) dir20_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns19_10 = (TH1F*) dir19_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns18_10 = (TH1F*) dir18_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_5ns");
  TH1F *Histo_5ns16_10 = (TH1F*) dir16_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns14_10 = (TH1F*) dir14_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns12_10 = (TH1F*) dir12_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns10_10 = (TH1F*) dir10_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns8_10 = (TH1F*) dir8_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns6_10 = (TH1F*) dir6_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns4_10 = (TH1F*) dir4_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns2_10 = (TH1F*) dir2_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns0_10_10 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_5ns");
  TH1F *Histo_5ns0_10_20 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_5ns");
  TH1F *Histo_5ns0_10_30 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns0_10_40 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_5ns");
  TH1F *Histo_5ns0_10_50 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_5ns");
  TH1F *Histo_5ns0_10_60 = (TH1F*) dir0_10->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  
  TH1F *Histo_5ns20_100 = (TH1F*) dir20_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns19_100 = (TH1F*) dir19_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns18_100 = (TH1F*) dir18_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_5ns");
  TH1F *Histo_5ns16_100 = (TH1F*) dir16_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns14_100 = (TH1F*) dir14_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns12_100 = (TH1F*) dir12_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns10_100 = (TH1F*) dir10_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns8_100 = (TH1F*) dir8_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns6_100 = (TH1F*) dir6_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns4_100 = (TH1F*) dir4_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns2_100 = (TH1F*) dir2_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns0_100_10 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_5ns");
  TH1F *Histo_5ns0_100_20 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_5ns");
  TH1F *Histo_5ns0_100_30 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns0_100_40 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_5ns");
  TH1F *Histo_5ns0_100_50 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_5ns");
  TH1F *Histo_5ns0_100_60 = (TH1F*) dir0_100->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  
  TH1F *Histo_5ns20_1000 = (TH1F*) dir20_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns19_1000 = (TH1F*) dir19_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns18_1000 = (TH1F*) dir18_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_5ns");
  TH1F *Histo_5ns16_1000 = (TH1F*) dir16_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns14_1000 = (TH1F*) dir14_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns12_1000 = (TH1F*) dir12_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns10_1000 = (TH1F*) dir10_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns8_1000 = (TH1F*) dir8_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns6_1000 = (TH1F*) dir6_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns4_1000 = (TH1F*) dir4_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns2_1000 = (TH1F*) dir2_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns0_1000_10 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_5ns");
  TH1F *Histo_5ns0_1000_20 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_5ns");
  TH1F *Histo_5ns0_1000_30 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns0_1000_40 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_5ns");
  TH1F *Histo_5ns0_1000_50 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_5ns");
  TH1F *Histo_5ns0_1000_60 = (TH1F*) dir0_1000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  
  TH1F *Histo_5ns20_10000 = (TH1F*) dir20_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns19_10000 = (TH1F*) dir19_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns18_10000 = (TH1F*) dir18_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_5ns");
  TH1F *Histo_5ns16_10000 = (TH1F*) dir16_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns14_10000 = (TH1F*) dir14_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns12_10000 = (TH1F*) dir12_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns10_10000 = (TH1F*) dir10_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns8_10000 = (TH1F*) dir8_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns6_10000 = (TH1F*) dir6_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns4_10000 = (TH1F*) dir4_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns2_10000 = (TH1F*) dir2_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns0_10000_10 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_5ns");
  TH1F *Histo_5ns0_10000_20 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_5ns");
  TH1F *Histo_5ns0_10000_30 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns0_10000_40 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_5ns");
  TH1F *Histo_5ns0_10000_50 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_5ns");
  TH1F *Histo_5ns0_10000_60 = (TH1F*) dir0_10000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  
  TH1F *Histo_5ns20_25000 = (TH1F*) dir20_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns19_25000 = (TH1F*) dir19_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns18_25000 = (TH1F*) dir18_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_5ns");
  TH1F *Histo_5ns16_25000 = (TH1F*) dir16_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns14_25000 = (TH1F*) dir14_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns12_25000 = (TH1F*) dir12_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns10_25000 = (TH1F*) dir10_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns8_25000 = (TH1F*) dir8_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns6_25000 = (TH1F*) dir6_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns4_25000 = (TH1F*) dir4_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns2_25000 = (TH1F*) dir2_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");
  TH1F *Histo_5ns0_25000_10 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_10MeV_inTime_5ns");
  TH1F *Histo_5ns0_25000_20 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_20MeV_inTime_5ns");
  TH1F *Histo_5ns0_25000_30 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_30MeV_inTime_5ns");
  TH1F *Histo_5ns0_25000_40 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_40MeV_inTime_5ns");
  TH1F *Histo_5ns0_25000_50 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_50MeV_inTime_5ns");
  TH1F *Histo_5ns0_25000_60 = (TH1F*) dir0_25000->Get("SPA_acceptance_ECal_MissingMass_Ecut_60MeV_inTime_5ns");

 
  Histo_5ns2_1->Fit(f_2,"R");
  means_5ns_1[0]=f_2->GetParameter(1);
  sigmas_5ns_1[0]=f_2->GetParameter(2);

  Histo_5ns4_1->Fit(f_4,"R");
  means_5ns_1[1]=f_4->GetParameter(1);
  sigmas_5ns_1[1]=f_4->GetParameter(2);
  
  Histo_5ns6_1->Fit(f_6,"R");
  means_5ns_1[2]=f_6->GetParameter(1);
  sigmas_5ns_1[2]=f_6->GetParameter(2);
  
  Histo_5ns8_1->Fit(f_8,"R");
  means_5ns_1[3]=f_8->GetParameter(1);
  sigmas_5ns_1[3]=f_8->GetParameter(2);
  
  Histo_5ns10_1->Fit(f_10,"R");
  means_5ns_1[4]=f_10->GetParameter(1);
  sigmas_5ns_1[4]=f_10->GetParameter(2);
  
  Histo_5ns12_1->Fit(f_12,"R");
  means_5ns_1[5]=f_12->GetParameter(1);
  sigmas_5ns_1[5]=f_12->GetParameter(2);
  
  Histo_5ns14_1->Fit(f_14,"R");
  means_5ns_1[6]=f_14->GetParameter(1);
  sigmas_5ns_1[6]=f_14->GetParameter(2);
  
  Histo_5ns16_1->Fit(f_16,"R");
  means_5ns_1[7]=f_16->GetParameter(1);
  sigmas_5ns_1[7]=f_16->GetParameter(2);
  
  Histo_5ns18_1->Fit(f_18,"R");
  means_5ns_1[8]=f_18->GetParameter(1);
  sigmas_5ns_1[8]=f_18->GetParameter(2);
  
  Histo_5ns19_1->Fit(f_19,"R");
  means_5ns_1[9]=f_19->GetParameter(1);
  sigmas_5ns_1[9]=f_19->GetParameter(2);
  
  Histo_5ns20_1->Fit(f_20,"R");
  means_5ns_1[10]=f_20->GetParameter(1);
  sigmas_5ns_1[10]=f_20->GetParameter(2);
  
  entries_5ns_1[0]=Histo_5ns2_1->Integral(500+(means_5ns_1[0]-nsigma*sigmas_5ns_1[0]),500+(means_5ns_1[0]+nsigma*sigmas_5ns_1[0]))/1000;
  entries_5ns_1[1]=Histo_5ns4_1->Integral(500+(means_5ns_1[1]-nsigma*sigmas_5ns_1[1]),500+(means_5ns_1[1]+nsigma*sigmas_5ns_1[1]))/1000;
  entries_5ns_1[2]=Histo_5ns6_1->Integral(500+(means_5ns_1[2]-nsigma*sigmas_5ns_1[2]),500+(means_5ns_1[2]+nsigma*sigmas_5ns_1[2]))/1000;
  entries_5ns_1[3]=Histo_5ns8_1->Integral(500+(means_5ns_1[3]-nsigma*sigmas_5ns_1[3]),500+(means_5ns_1[3]+nsigma*sigmas_5ns_1[3]))/1000;
  entries_5ns_1[4]=Histo_5ns10_1->Integral(500+(means_5ns_1[4]-nsigma*sigmas_5ns_1[4]),500+(means_5ns_1[4]+nsigma*sigmas_5ns_1[4]))/1000;
  entries_5ns_1[5]=Histo_5ns12_1->Integral(500+(means_5ns_1[5]-nsigma*sigmas_5ns_1[5]),500+(means_5ns_1[5]+nsigma*sigmas_5ns_1[5]))/1000;
  entries_5ns_1[6]=Histo_5ns14_1->Integral(500+(means_5ns_1[6]-nsigma*sigmas_5ns_1[6]),500+(means_5ns_1[6]+nsigma*sigmas_5ns_1[6]))/1000;
  entries_5ns_1[7]=Histo_5ns16_1->Integral(500+(means_5ns_1[7]-nsigma*sigmas_5ns_1[7]),500+(means_5ns_1[7]+nsigma*sigmas_5ns_1[7]))/1000;
  entries_5ns_1[8]=Histo_5ns18_1->Integral(500+(means_5ns_1[8]-nsigma*sigmas_5ns_1[8]),500+(means_5ns_1[8]+nsigma*sigmas_5ns_1[8]))/1000;
  entries_5ns_1[9]=Histo_5ns19_1->Integral(500+(means_5ns_1[9]-nsigma*sigmas_5ns_1[9]),500+(means_5ns_1[9]+nsigma*sigmas_5ns_1[9]))/1000;
  entries_5ns_1[10]=Histo_5ns20_1->Integral(500+(means_5ns_1[10]-nsigma*sigmas_5ns_1[10]),500+(means_5ns_1[10]+nsigma*sigmas_5ns_1[10]))/1000;
  
  TCanvas *c1_5ns = new TCanvas();
  c1_5ns->cd();
  Histo_5ns20_1->SetLineColorAlpha(kBlue+3,0.65);
  Histo_5ns20_1->SetTitle( "M^{2}_{miss} for different A' masses (1 e+ per bunch)");
  Histo_5ns20_1->SetLineWidth(3);
  Histo_5ns20_1->GetYaxis()->SetTitle("N_{events}");
  Histo_5ns20_1->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_5ns19_1->SetLineColorAlpha(kBlue,0.65);
  Histo_5ns19_1->SetLineWidth(3);
  Histo_5ns18_1->SetLineColorAlpha(kCyan+3,0.65);
  Histo_5ns18_1->SetLineWidth(3);
  Histo_5ns16_1->SetLineColorAlpha(kCyan+1,0.65);
  Histo_5ns16_1->SetLineWidth(3);
  Histo_5ns14_1->SetLineColorAlpha(kGreen+3,0.65);
  Histo_5ns14_1->SetLineWidth(3);
  Histo_5ns12_1->SetLineColorAlpha(kGreen+1,0.65);
  Histo_5ns12_1->SetLineWidth(3);
  Histo_5ns10_1->SetLineColorAlpha(kYellow+2,0.65);
  Histo_5ns10_1->SetLineWidth(3);
  Histo_5ns8_1->SetLineColorAlpha(kOrange-3,0.65);
  Histo_5ns8_1->SetLineWidth(3);
  Histo_5ns6_1->SetLineColorAlpha(kRed+2,0.65);
  Histo_5ns6_1->SetLineWidth(3);
  Histo_5ns4_1->SetLineColorAlpha(kRed,0.65);
  Histo_5ns4_1->SetLineWidth(3);
  Histo_5ns2_1->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_5ns2_1->SetLineWidth(3);
    
  Histo_5ns20_1->Draw();
  Histo_5ns19_1->Draw("same");
  Histo_5ns18_1->Draw("same");
  Histo_5ns4_1->Draw("same");
  Histo_5ns6_1->Draw("same");
  Histo_5ns8_1->Draw("same");
  Histo_5ns10_1->Draw("same");
  Histo_5ns12_1->Draw("same");
  Histo_5ns14_1->Draw("same");
  Histo_5ns16_1->Draw("same");
  Histo_5ns2_1->Draw("same");

  auto legend_1_5ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_1_5ns->AddEntry(Histo_5ns20_1,"M_{A'} = 20 MeV","l");
  legend_1_5ns->AddEntry(Histo_5ns19_1,"M_{A'} = 19 MeV","l");
  legend_1_5ns->AddEntry(Histo_5ns18_1,"M_{A'} = 18 MeV","l");
  legend_1_5ns->AddEntry(Histo_5ns16_1,"M_{A'} = 16 MeV","l");
  legend_1_5ns->AddEntry(Histo_5ns14_1,"M_{A'} = 14 MeV","l");
  legend_1_5ns->AddEntry(Histo_5ns12_1,"M_{A'} = 12 MeV","l");
  legend_1_5ns->AddEntry(Histo_5ns10_1,"M_{A'} = 10 MeV","l");
  legend_1_5ns->AddEntry(Histo_5ns8_1,"M_{A'} = 8 MeV","l");
  legend_1_5ns->AddEntry(Histo_5ns6_1,"M_{A'} = 6 MeV","l");
  legend_1_5ns->AddEntry(Histo_5ns4_1,"M_{A'} = 4 MeV","l");
  legend_1_5ns->AddEntry(Histo_5ns2_1,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_1_5ns->Draw();

  outRoot->WriteObject(c1_5ns,"1e_all_5ns");

  if(!(Histo_5ns20_10->GetSumw2N() > 0)) Histo_5ns20_10->Sumw2(kTRUE);
  Histo_5ns20_10->Add(Histo_5ns0_10_30,-1);
  if(!(Histo_5ns19_10->GetSumw2N() > 0)) Histo_5ns19_10->Sumw2(kTRUE);
  Histo_5ns19_10->Add(Histo_5ns0_10_30,-1);
  if(!(Histo_5ns18_10->GetSumw2N() > 0)) Histo_5ns18_10->Sumw2(kTRUE);
  Histo_5ns18_10->Add(Histo_5ns0_10_50,-1);
  if(!(Histo_5ns16_10->GetSumw2N() > 0)) Histo_5ns16_10->Sumw2(kTRUE);
  Histo_5ns16_10->Add(Histo_5ns0_10_60,-1);
  if(!(Histo_5ns14_10->GetSumw2N() > 0)) Histo_5ns14_10->Sumw2(kTRUE);
  Histo_5ns14_10->Add(Histo_5ns0_10_60,-1);
  if(!(Histo_5ns12_10->GetSumw2N() > 0)) Histo_5ns12_10->Sumw2(kTRUE);
  Histo_5ns12_10->Add(Histo_5ns0_10_60,-1);
  if(!(Histo_5ns10_10->GetSumw2N() > 0)) Histo_5ns10_10->Sumw2(kTRUE);
  Histo_5ns10_10->Add(Histo_5ns0_10_60,-1);
  if(!(Histo_5ns8_10->GetSumw2N() > 0)) Histo_5ns8_10->Sumw2(kTRUE);
  Histo_5ns8_10->Add(Histo_5ns0_10_60,-1);
  if(!(Histo_5ns6_10->GetSumw2N() > 0)) Histo_5ns6_10->Sumw2(kTRUE);
  Histo_5ns6_10->Add(Histo_5ns0_10_60,-1);
  if(!(Histo_5ns4_10->GetSumw2N() > 0)) Histo_5ns4_10->Sumw2(kTRUE);
  Histo_5ns4_10->Add(Histo_5ns0_10_60,-1);
  if(!(Histo_5ns2_10->GetSumw2N() > 0)) Histo_5ns2_10->Sumw2(kTRUE);
  Histo_5ns2_10->Add(Histo_5ns0_10_60,-1);
  
  Histo_5ns2_10->Fit(f_2,"R");
  means_5ns_10[0]=f_2->GetParameter(1);
  sigmas_5ns_10[0]=f_2->GetParameter(2);

  Histo_5ns4_10->Fit(f_4,"R");
  means_5ns_10[1]=f_4->GetParameter(1);
  sigmas_5ns_10[1]=f_4->GetParameter(2);
  
  Histo_5ns6_10->Fit(f_6,"R");
  means_5ns_10[2]=f_6->GetParameter(1);
  sigmas_5ns_10[2]=f_6->GetParameter(2);
  
  Histo_5ns8_10->Fit(f_8,"R");
  means_5ns_10[3]=f_8->GetParameter(1);
  sigmas_5ns_10[3]=f_8->GetParameter(2);
 
  Histo_5ns10_10->Fit(f_10,"R");
  means_5ns_10[4]=f_10->GetParameter(1);
  sigmas_5ns_10[4]=f_10->GetParameter(2);
  
  Histo_5ns12_10->Fit(f_12,"R");
  means_5ns_10[5]=f_12->GetParameter(1);
  sigmas_5ns_10[5]=f_12->GetParameter(2);
  
  Histo_5ns14_10->Fit(f_14,"R");
  means_5ns_10[6]=f_14->GetParameter(1);
  sigmas_5ns_10[6]=f_14->GetParameter(2);
  
  Histo_5ns16_10->Fit(f_16,"R");
  means_5ns_10[7]=f_16->GetParameter(1);
  sigmas_5ns_10[7]=f_16->GetParameter(2);
  
  Histo_5ns18_10->Fit(f_18,"R");
  means_5ns_10[8]=f_18->GetParameter(1);
  sigmas_5ns_10[8]=f_18->GetParameter(2);
  
  Histo_5ns19_10->Fit(f_19,"R");
  means_5ns_10[9]=f_19->GetParameter(1);
  sigmas_5ns_10[9]=f_19->GetParameter(2);
  
  Histo_5ns20_10->Fit(f_20,"R");
  means_5ns_10[10]=f_20->GetParameter(1);
  sigmas_5ns_10[10]=f_20->GetParameter(2);

  entries_5ns_10[0]=Histo_5ns2_10->Integral(500+(means_5ns_10[0]-nsigma*sigmas_5ns_10[0]),500+(means_5ns_10[0]+nsigma*sigmas_5ns_10[0]))/1000;
  entries_5ns_10[1]=Histo_5ns4_10->Integral(500+(means_5ns_10[1]-nsigma*sigmas_5ns_10[1]),500+(means_5ns_10[1]+nsigma*sigmas_5ns_10[1]))/1000;
  entries_5ns_10[2]=Histo_5ns6_10->Integral(500+(means_5ns_10[2]-nsigma*sigmas_5ns_10[2]),500+(means_5ns_10[2]+nsigma*sigmas_5ns_10[2]))/1000;
  entries_5ns_10[3]=Histo_5ns8_10->Integral(500+(means_5ns_10[3]-nsigma*sigmas_5ns_10[3]),500+(means_5ns_10[3]+nsigma*sigmas_5ns_10[3]))/1000;
  entries_5ns_10[4]=Histo_5ns10_10->Integral(500+(means_5ns_10[4]-nsigma*sigmas_5ns_10[4]),500+(means_5ns_10[4]+nsigma*sigmas_5ns_10[4]))/1000;
  entries_5ns_10[5]=Histo_5ns12_10->Integral(500+(means_5ns_10[5]-nsigma*sigmas_5ns_10[5]),500+(means_5ns_10[5]+nsigma*sigmas_5ns_10[5]))/1000;
  entries_5ns_10[6]=Histo_5ns14_10->Integral(500+(means_5ns_10[6]-nsigma*sigmas_5ns_10[6]),500+(means_5ns_10[6]+nsigma*sigmas_5ns_10[6]))/1000;
  entries_5ns_10[7]=Histo_5ns16_10->Integral(500+(means_5ns_10[7]-nsigma*sigmas_5ns_10[7]),500+(means_5ns_10[7]+nsigma*sigmas_5ns_10[7]))/1000;
  entries_5ns_10[8]=Histo_5ns18_10->Integral(500+(means_5ns_10[8]-nsigma*sigmas_5ns_10[8]),500+(means_5ns_10[8]+nsigma*sigmas_5ns_10[8]))/1000;
  entries_5ns_10[9]=Histo_5ns19_10->Integral(500+(means_5ns_10[9]-nsigma*sigmas_5ns_10[9]),500+(means_5ns_10[9]+nsigma*sigmas_5ns_10[9]))/1000;
  entries_5ns_10[10]=Histo_5ns20_10->Integral(500+(means_5ns_10[10]-nsigma*sigmas_5ns_10[10]),500+(means_5ns_10[10]+nsigma*sigmas_5ns_10[10]))/1000;

  TCanvas *c10_5ns = new TCanvas();
  c10_5ns->cd();
  Histo_5ns20_10->SetLineColorAlpha(kBlue+3,0.65);
  Histo_5ns20_10->SetTitle( "M^{2}_{miss} for different A' masses (10 e+ per bunch)");
  Histo_5ns20_10->SetLineWidth(3);
  Histo_5ns20_10->GetYaxis()->SetTitle("N_{events}");
  Histo_5ns20_10->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_5ns19_10->SetLineColorAlpha(kBlue,0.65);
  Histo_5ns19_10->SetLineWidth(3);
  Histo_5ns18_10->SetLineColorAlpha(kCyan+3,0.65);
  Histo_5ns18_10->SetLineWidth(3);
  Histo_5ns16_10->SetLineColorAlpha(kCyan+1,0.65);
  Histo_5ns16_10->SetLineWidth(3);
  Histo_5ns14_10->SetLineColorAlpha(kGreen+3,0.65);
  Histo_5ns14_10->SetLineWidth(3);
  Histo_5ns12_10->SetLineColorAlpha(kGreen+1,0.65);
  Histo_5ns12_10->SetLineWidth(3);
  Histo_5ns10_10->SetLineColorAlpha(kYellow+2,0.65);
  Histo_5ns10_10->SetLineWidth(3);
  Histo_5ns8_10->SetLineColorAlpha(kOrange-3,0.65);
  Histo_5ns8_10->SetLineWidth(3);
  Histo_5ns6_10->SetLineColorAlpha(kRed+2,0.65);
  Histo_5ns6_10->SetLineWidth(3);
  Histo_5ns4_10->SetLineColorAlpha(kRed,0.65);
  Histo_5ns4_10->SetLineWidth(3);
  Histo_5ns2_10->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_5ns2_10->SetLineWidth(3);
    
  Histo_5ns20_10->Draw();
  Histo_5ns19_10->Draw("same");
  Histo_5ns18_10->Draw("same");
  Histo_5ns4_10->Draw("same");
  Histo_5ns6_10->Draw("same");
  Histo_5ns8_10->Draw("same");
  Histo_5ns10_10->Draw("same");
  Histo_5ns12_10->Draw("same");
  Histo_5ns14_10->Draw("same");
  Histo_5ns16_10->Draw("same");
  Histo_5ns2_10->Draw("same");

  auto legend_10_5ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_10_5ns->AddEntry(Histo_5ns20_10,"M_{A'} = 20 MeV","l");
  legend_10_5ns->AddEntry(Histo_5ns19_10,"M_{A'} = 19 MeV","l");
  legend_10_5ns->AddEntry(Histo_5ns18_10,"M_{A'} = 18 MeV","l");
  legend_10_5ns->AddEntry(Histo_5ns16_10,"M_{A'} = 16 MeV","l");
  legend_10_5ns->AddEntry(Histo_5ns14_10,"M_{A'} = 14 MeV","l");
  legend_10_5ns->AddEntry(Histo_5ns12_10,"M_{A'} = 12 MeV","l");
  legend_10_5ns->AddEntry(Histo_5ns10_10,"M_{A'} = 10 MeV","l");
  legend_10_5ns->AddEntry(Histo_5ns8_10,"M_{A'} = 8 MeV","l");
  legend_10_5ns->AddEntry(Histo_5ns6_10,"M_{A'} = 6 MeV","l");
  legend_10_5ns->AddEntry(Histo_5ns4_10,"M_{A'} = 4 MeV","l");
  legend_10_5ns->AddEntry(Histo_5ns2_10,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_10_5ns->Draw();

  outRoot->WriteObject(c10_5ns,"10e_all_5ns");
 
  if(!(Histo_5ns20_100->GetSumw2N() > 0)) Histo_5ns20_100->Sumw2(kTRUE);
  Histo_5ns20_100->Add(Histo_5ns0_100_30,-1);
  if(!(Histo_5ns19_100->GetSumw2N() > 0)) Histo_5ns19_100->Sumw2(kTRUE);
  Histo_5ns19_100->Add(Histo_5ns0_100_30,-1);
  if(!(Histo_5ns18_100->GetSumw2N() > 0)) Histo_5ns18_100->Sumw2(kTRUE);
  Histo_5ns18_100->Add(Histo_5ns0_100_50,-1);
  if(!(Histo_5ns16_100->GetSumw2N() > 0)) Histo_5ns16_100->Sumw2(kTRUE);
  Histo_5ns16_100->Add(Histo_5ns0_100_60,-1);
  if(!(Histo_5ns14_100->GetSumw2N() > 0)) Histo_5ns14_100->Sumw2(kTRUE);
  Histo_5ns14_100->Add(Histo_5ns0_100_60,-1);
  if(!(Histo_5ns12_100->GetSumw2N() > 0)) Histo_5ns12_100->Sumw2(kTRUE);
  Histo_5ns12_100->Add(Histo_5ns0_100_60,-1);
  if(!(Histo_5ns10_100->GetSumw2N() > 0)) Histo_5ns10_100->Sumw2(kTRUE);
  Histo_5ns10_100->Add(Histo_5ns0_100_60,-1);
  if(!(Histo_5ns8_100->GetSumw2N() > 0)) Histo_5ns8_100->Sumw2(kTRUE);
  Histo_5ns8_100->Add(Histo_5ns0_100_60,-1);
  if(!(Histo_5ns6_100->GetSumw2N() > 0)) Histo_5ns6_100->Sumw2(kTRUE);
  Histo_5ns6_100->Add(Histo_5ns0_100_60,-1);
  if(!(Histo_5ns4_100->GetSumw2N() > 0)) Histo_5ns4_100->Sumw2(kTRUE);
  Histo_5ns4_100->Add(Histo_5ns0_100_60,-1);
  if(!(Histo_5ns2_100->GetSumw2N() > 0)) Histo_5ns2_100->Sumw2(kTRUE);
  Histo_5ns2_100->Add(Histo_5ns0_100_60,-1);


  Histo_5ns2_100->Fit(f_2,"R");
  means_5ns_100[0]=f_2->GetParameter(1);
  sigmas_5ns_100[0]=f_2->GetParameter(2);

  Histo_5ns4_100->Fit(f_4,"R");
  means_5ns_100[1]=f_4->GetParameter(1);
  sigmas_5ns_100[1]=f_4->GetParameter(2);
  
  Histo_5ns6_100->Fit(f_6,"R");
  means_5ns_100[2]=f_6->GetParameter(1);
  sigmas_5ns_100[2]=f_6->GetParameter(2);
  
  Histo_5ns8_100->Fit(f_8,"R");
  means_5ns_100[3]=f_8->GetParameter(1);
  sigmas_5ns_100[3]=f_8->GetParameter(2);
 
  Histo_5ns10_100->Fit(f_10,"R");
  means_5ns_100[4]=f_10->GetParameter(1);
  sigmas_5ns_100[4]=f_10->GetParameter(2);
  
  Histo_5ns12_100->Fit(f_12,"R");
  means_5ns_100[5]=f_12->GetParameter(1);
  sigmas_5ns_100[5]=f_12->GetParameter(2);
  
  Histo_5ns14_100->Fit(f_14,"R");
  means_5ns_100[6]=f_14->GetParameter(1);
  sigmas_5ns_100[6]=f_14->GetParameter(2);
  
  Histo_5ns16_100->Fit(f_16,"R");
  means_5ns_100[7]=f_16->GetParameter(1);
  sigmas_5ns_100[7]=f_16->GetParameter(2);
  
  Histo_5ns18_100->Fit(f_18,"R");
  means_5ns_100[8]=f_18->GetParameter(1);
  sigmas_5ns_100[8]=f_18->GetParameter(2);
  
  Histo_5ns19_100->Fit(f_19,"R");
  means_5ns_100[9]=f_19->GetParameter(1);
  sigmas_5ns_100[9]=f_19->GetParameter(2);
  
  Histo_5ns20_100->Fit(f_20,"R");
  means_5ns_100[10]=f_20->GetParameter(1);
  sigmas_5ns_100[10]=f_20->GetParameter(2);

  entries_5ns_100[0]=Histo_5ns2_100->Integral(500+(means_5ns_100[0]-nsigma*sigmas_5ns_100[0]),500+(means_5ns_100[0]+nsigma*sigmas_5ns_100[0]))/1000;
  entries_5ns_100[1]=Histo_5ns4_100->Integral(500+(means_5ns_100[1]-nsigma*sigmas_5ns_100[1]),500+(means_5ns_100[1]+nsigma*sigmas_5ns_100[1]))/1000;
  entries_5ns_100[2]=Histo_5ns6_100->Integral(500+(means_5ns_100[2]-nsigma*sigmas_5ns_100[2]),500+(means_5ns_100[2]+nsigma*sigmas_5ns_100[2]))/1000;
  entries_5ns_100[3]=Histo_5ns8_100->Integral(500+(means_5ns_100[3]-nsigma*sigmas_5ns_100[3]),500+(means_5ns_100[3]+nsigma*sigmas_5ns_100[3]))/1000;
  entries_5ns_100[4]=Histo_5ns10_100->Integral(500+(means_5ns_100[4]-nsigma*sigmas_5ns_100[4]),500+(means_5ns_100[4]+nsigma*sigmas_5ns_100[4]))/1000;
  entries_5ns_100[5]=Histo_5ns12_100->Integral(500+(means_5ns_100[5]-nsigma*sigmas_5ns_100[5]),500+(means_5ns_100[5]+nsigma*sigmas_5ns_100[5]))/1000;
  entries_5ns_100[6]=Histo_5ns14_100->Integral(500+(means_5ns_100[6]-nsigma*sigmas_5ns_100[6]),500+(means_5ns_100[6]+nsigma*sigmas_5ns_100[6]))/1000;
  entries_5ns_100[7]=Histo_5ns16_100->Integral(500+(means_5ns_100[7]-nsigma*sigmas_5ns_100[7]),500+(means_5ns_100[7]+nsigma*sigmas_5ns_100[7]))/1000;
  entries_5ns_100[8]=Histo_5ns18_100->Integral(500+(means_5ns_100[8]-nsigma*sigmas_5ns_100[8]),500+(means_5ns_100[8]+nsigma*sigmas_5ns_100[8]))/1000;
  entries_5ns_100[9]=Histo_5ns19_100->Integral(500+(means_5ns_100[9]-nsigma*sigmas_5ns_100[9]),500+(means_5ns_100[9]+nsigma*sigmas_5ns_100[9]))/1000;
  entries_5ns_100[10]=Histo_5ns20_100->Integral(500+(means_5ns_100[10]-nsigma*sigmas_5ns_100[10]),500+(means_5ns_100[10]+nsigma*sigmas_5ns_100[10]))/1000;

  TCanvas *c100_5ns = new TCanvas();
  c100_5ns->cd();
  Histo_5ns20_100->SetLineColorAlpha(kBlue+3,0.65);
  Histo_5ns20_100->SetTitle( "M^{2}_{miss} for different A' masses (100 e+ per bunch)");
  Histo_5ns20_100->SetLineWidth(3);
  Histo_5ns20_100->GetYaxis()->SetTitle("N_{events}");
  Histo_5ns20_100->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_5ns19_100->SetLineColorAlpha(kBlue,0.65);
  Histo_5ns19_100->SetLineWidth(3);
  Histo_5ns18_100->SetLineColorAlpha(kCyan+3,0.65);
  Histo_5ns18_100->SetLineWidth(3);
  Histo_5ns16_100->SetLineColorAlpha(kCyan+1,0.65);
  Histo_5ns16_100->SetLineWidth(3);
  Histo_5ns14_100->SetLineColorAlpha(kGreen+3,0.65);
  Histo_5ns14_100->SetLineWidth(3);
  Histo_5ns12_100->SetLineColorAlpha(kGreen+1,0.65);
  Histo_5ns12_100->SetLineWidth(3);
  Histo_5ns10_100->SetLineColorAlpha(kYellow+2,0.65);
  Histo_5ns10_100->SetLineWidth(3);
  Histo_5ns8_100->SetLineColorAlpha(kOrange-3,0.65);
  Histo_5ns8_100->SetLineWidth(3);
  Histo_5ns6_100->SetLineColorAlpha(kRed+2,0.65);
  Histo_5ns6_100->SetLineWidth(3);
  Histo_5ns4_100->SetLineColorAlpha(kRed,0.65);
  Histo_5ns4_100->SetLineWidth(3);
  Histo_5ns2_100->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_5ns2_100->SetLineWidth(3);
    
  Histo_5ns20_100->Draw();
  Histo_5ns19_100->Draw("same");
  Histo_5ns18_100->Draw("same");
  Histo_5ns4_100->Draw("same");
  Histo_5ns6_100->Draw("same");
  Histo_5ns8_100->Draw("same");
  Histo_5ns10_100->Draw("same");
  Histo_5ns12_100->Draw("same");
  Histo_5ns14_100->Draw("same");
  Histo_5ns16_100->Draw("same");
  Histo_5ns2_100->Draw("same");

  auto legend_100_5ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_100_5ns->AddEntry(Histo_5ns20_100,"M_{A'} = 20 MeV","l");
  legend_100_5ns->AddEntry(Histo_5ns19_100,"M_{A'} = 19 MeV","l");
  legend_100_5ns->AddEntry(Histo_5ns18_100,"M_{A'} = 18 MeV","l");
  legend_100_5ns->AddEntry(Histo_5ns16_100,"M_{A'} = 16 MeV","l");
  legend_100_5ns->AddEntry(Histo_5ns14_100,"M_{A'} = 14 MeV","l");
  legend_100_5ns->AddEntry(Histo_5ns12_100,"M_{A'} = 12 MeV","l");
  legend_100_5ns->AddEntry(Histo_5ns10_100,"M_{A'} = 10 MeV","l");
  legend_100_5ns->AddEntry(Histo_5ns8_100,"M_{A'} = 8 MeV","l");
  legend_100_5ns->AddEntry(Histo_5ns6_100,"M_{A'} = 6 MeV","l");
  legend_100_5ns->AddEntry(Histo_5ns4_100,"M_{A'} = 4 MeV","l");
  legend_100_5ns->AddEntry(Histo_5ns2_100,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_100_5ns->Draw();

  outRoot->WriteObject(c100_5ns,"100e_all_5ns");

  
  if(!(Histo_5ns20_1000->GetSumw2N() > 0)) Histo_5ns20_1000->Sumw2(kTRUE);
  Histo_5ns20_1000->Add(Histo_5ns0_1000_30,-1);
  if(!(Histo_5ns19_1000->GetSumw2N() > 0)) Histo_5ns19_1000->Sumw2(kTRUE);
  Histo_5ns19_1000->Add(Histo_5ns0_1000_30,-1);
  if(!(Histo_5ns18_1000->GetSumw2N() > 0)) Histo_5ns18_1000->Sumw2(kTRUE);
  Histo_5ns18_1000->Add(Histo_5ns0_1000_50,-1);
  if(!(Histo_5ns16_1000->GetSumw2N() > 0)) Histo_5ns16_1000->Sumw2(kTRUE);
  Histo_5ns16_1000->Add(Histo_5ns0_1000_60,-1);
  if(!(Histo_5ns14_1000->GetSumw2N() > 0)) Histo_5ns14_1000->Sumw2(kTRUE);
  Histo_5ns14_1000->Add(Histo_5ns0_1000_60,-1);
  if(!(Histo_5ns12_1000->GetSumw2N() > 0)) Histo_5ns12_1000->Sumw2(kTRUE);
  Histo_5ns12_1000->Add(Histo_5ns0_1000_60,-1);
  if(!(Histo_5ns10_1000->GetSumw2N() > 0)) Histo_5ns10_1000->Sumw2(kTRUE);
  Histo_5ns10_1000->Add(Histo_5ns0_1000_60,-1);
  if(!(Histo_5ns8_1000->GetSumw2N() > 0)) Histo_5ns8_1000->Sumw2(kTRUE);
  Histo_5ns8_1000->Add(Histo_5ns0_1000_60,-1);
  if(!(Histo_5ns6_1000->GetSumw2N() > 0)) Histo_5ns6_1000->Sumw2(kTRUE);
  Histo_5ns6_1000->Add(Histo_5ns0_1000_60,-1);
  if(!(Histo_5ns4_1000->GetSumw2N() > 0)) Histo_5ns4_1000->Sumw2(kTRUE);
  Histo_5ns4_1000->Add(Histo_5ns0_1000_60,-1);
  if(!(Histo_5ns2_1000->GetSumw2N() > 0)) Histo_5ns2_1000->Sumw2(kTRUE);
  Histo_5ns2_1000->Add(Histo_5ns0_1000_60,-1);


  Histo_5ns2_1000->Fit(f_2,"R");
  means_5ns_1000[0]=f_2->GetParameter(1);
  sigmas_5ns_1000[0]=f_2->GetParameter(2);

  Histo_5ns4_1000->Fit(f_4,"R");
  means_5ns_1000[1]=f_4->GetParameter(1);
  sigmas_5ns_1000[1]=f_4->GetParameter(2);
  
  Histo_5ns6_1000->Fit(f_6,"R");
  means_5ns_1000[2]=f_6->GetParameter(1);
  sigmas_5ns_1000[2]=f_6->GetParameter(2);
  
  Histo_5ns8_1000->Fit(f_8,"R");
  means_5ns_1000[3]=f_8->GetParameter(1);
  sigmas_5ns_1000[3]=f_8->GetParameter(2);
 
  Histo_5ns10_1000->Fit(f_10,"R");
  means_5ns_1000[4]=f_10->GetParameter(1);
  sigmas_5ns_1000[4]=f_10->GetParameter(2);
  
  Histo_5ns12_1000->Fit(f_12,"R");
  means_5ns_1000[5]=f_12->GetParameter(1);
  sigmas_5ns_1000[5]=f_12->GetParameter(2);
  
  Histo_5ns14_1000->Fit(f_14,"R");
  means_5ns_1000[6]=f_14->GetParameter(1);
  sigmas_5ns_1000[6]=f_14->GetParameter(2);
  
  Histo_5ns16_1000->Fit(f_16,"R");
  means_5ns_1000[7]=f_16->GetParameter(1);
  sigmas_5ns_1000[7]=f_16->GetParameter(2);
  
  Histo_5ns18_1000->Fit(f_18,"R");
  means_5ns_1000[8]=f_18->GetParameter(1);
  sigmas_5ns_1000[8]=f_18->GetParameter(2);
  
  Histo_5ns19_1000->Fit(f_19,"R");
  means_5ns_1000[9]=f_19->GetParameter(1);
  sigmas_5ns_1000[9]=f_19->GetParameter(2);
  
  Histo_5ns20_1000->Fit(f_20,"R");
  means_5ns_1000[10]=f_20->GetParameter(1);
  sigmas_5ns_1000[10]=f_20->GetParameter(2);

  entries_5ns_1000[0]=Histo_5ns2_1000->Integral(500+(means_5ns_1000[0]-nsigma*sigmas_5ns_1000[0]),500+(means_5ns_1000[0]+nsigma*sigmas_5ns_1000[0]))/1000;
  entries_5ns_1000[1]=Histo_5ns4_1000->Integral(500+(means_5ns_1000[1]-nsigma*sigmas_5ns_1000[1]),500+(means_5ns_1000[1]+nsigma*sigmas_5ns_1000[1]))/1000;
  entries_5ns_1000[2]=Histo_5ns6_1000->Integral(500+(means_5ns_1000[2]-nsigma*sigmas_5ns_1000[2]),500+(means_5ns_1000[2]+nsigma*sigmas_5ns_1000[2]))/1000;
  entries_5ns_1000[3]=Histo_5ns8_1000->Integral(500+(means_5ns_1000[3]-nsigma*sigmas_5ns_1000[3]),500+(means_5ns_1000[3]+nsigma*sigmas_5ns_1000[3]))/1000;
  entries_5ns_1000[4]=Histo_5ns10_1000->Integral(500+(means_5ns_1000[4]-nsigma*sigmas_5ns_1000[4]),500+(means_5ns_1000[4]+nsigma*sigmas_5ns_1000[4]))/1000;
  entries_5ns_1000[5]=Histo_5ns12_1000->Integral(500+(means_5ns_1000[5]-nsigma*sigmas_5ns_1000[5]),500+(means_5ns_1000[5]+nsigma*sigmas_5ns_1000[5]))/1000;
  entries_5ns_1000[6]=Histo_5ns14_1000->Integral(500+(means_5ns_1000[6]-nsigma*sigmas_5ns_1000[6]),500+(means_5ns_1000[6]+nsigma*sigmas_5ns_1000[6]))/1000;
  entries_5ns_1000[7]=Histo_5ns16_1000->Integral(500+(means_5ns_1000[7]-nsigma*sigmas_5ns_1000[7]),500+(means_5ns_1000[7]+nsigma*sigmas_5ns_1000[7]))/1000;
  entries_5ns_1000[8]=Histo_5ns18_1000->Integral(500+(means_5ns_1000[8]-nsigma*sigmas_5ns_1000[8]),500+(means_5ns_1000[8]+nsigma*sigmas_5ns_1000[8]))/1000;
  entries_5ns_1000[9]=Histo_5ns19_1000->Integral(500+(means_5ns_1000[9]-nsigma*sigmas_5ns_1000[9]),500+(means_5ns_1000[9]+nsigma*sigmas_5ns_1000[9]))/1000;
  entries_5ns_1000[10]=Histo_5ns20_1000->Integral(500+(means_5ns_1000[10]-nsigma*sigmas_5ns_1000[10]),500+(means_5ns_1000[10]+nsigma*sigmas_5ns_1000[10]))/1000;

  TCanvas *c1000_5ns = new TCanvas();
  c1000_5ns->cd();
  Histo_5ns20_1000->SetLineColorAlpha(kBlue+3,0.65);
  Histo_5ns20_1000->SetTitle( "M^{2}_{miss} for different A' masses (1000 e+ per bunch)");
  Histo_5ns20_1000->SetLineWidth(3);
  Histo_5ns20_1000->GetYaxis()->SetTitle("N_{events}");
  Histo_5ns20_1000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_5ns19_1000->SetLineColorAlpha(kBlue,0.65);
  Histo_5ns19_1000->SetLineWidth(3);
  Histo_5ns18_1000->SetLineColorAlpha(kCyan+3,0.65);
  Histo_5ns18_1000->SetLineWidth(3);
  Histo_5ns16_1000->SetLineColorAlpha(kCyan+1,0.65);
  Histo_5ns16_1000->SetLineWidth(3);
  Histo_5ns14_1000->SetLineColorAlpha(kGreen+3,0.65);
  Histo_5ns14_1000->SetLineWidth(3);
  Histo_5ns12_1000->SetLineColorAlpha(kGreen+1,0.65);
  Histo_5ns12_1000->SetLineWidth(3);
  Histo_5ns10_1000->SetLineColorAlpha(kYellow+2,0.65);
  Histo_5ns10_1000->SetLineWidth(3);
  Histo_5ns8_1000->SetLineColorAlpha(kOrange-3,0.65);
  Histo_5ns8_1000->SetLineWidth(3);
  Histo_5ns6_1000->SetLineColorAlpha(kRed+2,0.65);
  Histo_5ns6_1000->SetLineWidth(3);
  Histo_5ns4_1000->SetLineColorAlpha(kRed,0.65);
  Histo_5ns4_1000->SetLineWidth(3);
  Histo_5ns2_1000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_5ns2_1000->SetLineWidth(3);
    
  Histo_5ns20_1000->Draw();
  Histo_5ns19_1000->Draw("same");
  Histo_5ns18_1000->Draw("same");
  Histo_5ns4_1000->Draw("same");
  Histo_5ns6_1000->Draw("same");
  Histo_5ns8_1000->Draw("same");
  Histo_5ns10_1000->Draw("same");
  Histo_5ns12_1000->Draw("same");
  Histo_5ns14_1000->Draw("same");
  Histo_5ns16_1000->Draw("same");
  Histo_5ns2_1000->Draw("same");

  auto legend_1000_5ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_1000_5ns->AddEntry(Histo_5ns20_1000,"M_{A'} = 20 MeV","l");
  legend_1000_5ns->AddEntry(Histo_5ns19_1000,"M_{A'} = 19 MeV","l");
  legend_1000_5ns->AddEntry(Histo_5ns18_1000,"M_{A'} = 18 MeV","l");
  legend_1000_5ns->AddEntry(Histo_5ns16_1000,"M_{A'} = 16 MeV","l");
  legend_1000_5ns->AddEntry(Histo_5ns14_1000,"M_{A'} = 14 MeV","l");
  legend_1000_5ns->AddEntry(Histo_5ns12_1000,"M_{A'} = 12 MeV","l");
  legend_1000_5ns->AddEntry(Histo_5ns10_1000,"M_{A'} = 10 MeV","l");
  legend_1000_5ns->AddEntry(Histo_5ns8_1000,"M_{A'} = 8 MeV","l");
  legend_1000_5ns->AddEntry(Histo_5ns6_1000,"M_{A'} = 6 MeV","l");
  legend_1000_5ns->AddEntry(Histo_5ns4_1000,"M_{A'} = 4 MeV","l");
  legend_1000_5ns->AddEntry(Histo_5ns2_1000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_1000_5ns->Draw();

  outRoot->WriteObject(c1000_5ns,"1000e_all_5ns");

  
  if(!(Histo_5ns20_10000->GetSumw2N() > 0)) Histo_5ns20_10000->Sumw2(kTRUE);
  Histo_5ns20_10000->Add(Histo_5ns0_10000_30,-1);
  if(!(Histo_5ns19_10000->GetSumw2N() > 0)) Histo_5ns19_10000->Sumw2(kTRUE);
  Histo_5ns19_10000->Add(Histo_5ns0_10000_30,-1);
  if(!(Histo_5ns18_10000->GetSumw2N() > 0)) Histo_5ns18_10000->Sumw2(kTRUE);
  Histo_5ns18_10000->Add(Histo_5ns0_10000_50,-1);
  if(!(Histo_5ns16_10000->GetSumw2N() > 0)) Histo_5ns16_10000->Sumw2(kTRUE);
  Histo_5ns16_10000->Add(Histo_5ns0_10000_60,-1);
  if(!(Histo_5ns14_10000->GetSumw2N() > 0)) Histo_5ns14_10000->Sumw2(kTRUE);
  Histo_5ns14_10000->Add(Histo_5ns0_10000_60,-1);
  if(!(Histo_5ns12_10000->GetSumw2N() > 0)) Histo_5ns12_10000->Sumw2(kTRUE);
  Histo_5ns12_10000->Add(Histo_5ns0_10000_60,-1);
  if(!(Histo_5ns10_10000->GetSumw2N() > 0)) Histo_5ns10_10000->Sumw2(kTRUE);
  Histo_5ns10_10000->Add(Histo_5ns0_10000_60,-1);
  if(!(Histo_5ns8_10000->GetSumw2N() > 0)) Histo_5ns8_10000->Sumw2(kTRUE);
  Histo_5ns8_10000->Add(Histo_5ns0_10000_60,-1);
  if(!(Histo_5ns6_10000->GetSumw2N() > 0)) Histo_5ns6_10000->Sumw2(kTRUE);
  Histo_5ns6_10000->Add(Histo_5ns0_10000_60,-1);
  if(!(Histo_5ns4_10000->GetSumw2N() > 0)) Histo_5ns4_10000->Sumw2(kTRUE);
  Histo_5ns4_10000->Add(Histo_5ns0_10000_60,-1);
  if(!(Histo_5ns2_10000->GetSumw2N() > 0)) Histo_5ns2_10000->Sumw2(kTRUE);
  Histo_5ns2_10000->Add(Histo_5ns0_10000_60,-1);


  Histo_5ns2_10000->Fit(f_2,"R");
  means_5ns_10000[0]=f_2->GetParameter(1);
  sigmas_5ns_10000[0]=f_2->GetParameter(2);

  Histo_5ns4_10000->Fit(f_4,"R");
  means_5ns_10000[1]=f_4->GetParameter(1);
  sigmas_5ns_10000[1]=f_4->GetParameter(2);
  
  Histo_5ns6_10000->Fit(f_6,"R");
  means_5ns_10000[2]=f_6->GetParameter(1);
  sigmas_5ns_10000[2]=f_6->GetParameter(2);
  
  Histo_5ns8_10000->Fit(f_8,"R");
  means_5ns_10000[3]=f_8->GetParameter(1);
  sigmas_5ns_10000[3]=f_8->GetParameter(2);
 
  Histo_5ns10_10000->Fit(f_10,"R");
  means_5ns_10000[4]=f_10->GetParameter(1);
  sigmas_5ns_10000[4]=f_10->GetParameter(2);
  
  Histo_5ns12_10000->Fit(f_12,"R");
  means_5ns_10000[5]=f_12->GetParameter(1);
  sigmas_5ns_10000[5]=f_12->GetParameter(2);
  
  Histo_5ns14_10000->Fit(f_14,"R");
  means_5ns_10000[6]=f_14->GetParameter(1);
  sigmas_5ns_10000[6]=f_14->GetParameter(2);
  
  Histo_5ns16_10000->Fit(f_16,"R");
  means_5ns_10000[7]=f_16->GetParameter(1);
  sigmas_5ns_10000[7]=f_16->GetParameter(2);
  
  Histo_5ns18_10000->Fit(f_18,"R");
  means_5ns_10000[8]=f_18->GetParameter(1);
  sigmas_5ns_10000[8]=f_18->GetParameter(2);
  
  Histo_5ns19_10000->Fit(f_19,"R");
  means_5ns_10000[9]=f_19->GetParameter(1);
  sigmas_5ns_10000[9]=f_19->GetParameter(2);
  
  Histo_5ns20_10000->Fit(f_20,"R");
  means_5ns_10000[10]=f_20->GetParameter(1);
  sigmas_5ns_10000[10]=f_20->GetParameter(2);

  entries_5ns_10000[0]=Histo_5ns2_10000->Integral(500+(means_5ns_10000[0]-nsigma*sigmas_5ns_10000[0]),500+(means_5ns_10000[0]+nsigma*sigmas_5ns_10000[0]))/1000;
  entries_5ns_10000[1]=Histo_5ns4_10000->Integral(500+(means_5ns_10000[1]-nsigma*sigmas_5ns_10000[1]),500+(means_5ns_10000[1]+nsigma*sigmas_5ns_10000[1]))/1000;
  entries_5ns_10000[2]=Histo_5ns6_10000->Integral(500+(means_5ns_10000[2]-nsigma*sigmas_5ns_10000[2]),500+(means_5ns_10000[2]+nsigma*sigmas_5ns_10000[2]))/1000;
  entries_5ns_10000[3]=Histo_5ns8_10000->Integral(500+(means_5ns_10000[3]-nsigma*sigmas_5ns_10000[3]),500+(means_5ns_10000[3]+nsigma*sigmas_5ns_10000[3]))/1000;
  entries_5ns_10000[4]=Histo_5ns10_10000->Integral(500+(means_5ns_10000[4]-nsigma*sigmas_5ns_10000[4]),500+(means_5ns_10000[4]+nsigma*sigmas_5ns_10000[4]))/1000;
  entries_5ns_10000[5]=Histo_5ns12_10000->Integral(500+(means_5ns_10000[5]-nsigma*sigmas_5ns_10000[5]),500+(means_5ns_10000[5]+nsigma*sigmas_5ns_10000[5]))/1000;
  entries_5ns_10000[6]=Histo_5ns14_10000->Integral(500+(means_5ns_10000[6]-nsigma*sigmas_5ns_10000[6]),500+(means_5ns_10000[6]+nsigma*sigmas_5ns_10000[6]))/1000;
  entries_5ns_10000[7]=Histo_5ns16_10000->Integral(500+(means_5ns_10000[7]-nsigma*sigmas_5ns_10000[7]),500+(means_5ns_10000[7]+nsigma*sigmas_5ns_10000[7]))/1000;
  entries_5ns_10000[8]=Histo_5ns18_10000->Integral(500+(means_5ns_10000[8]-nsigma*sigmas_5ns_10000[8]),500+(means_5ns_10000[8]+nsigma*sigmas_5ns_10000[8]))/1000;
  entries_5ns_10000[9]=Histo_5ns19_10000->Integral(500+(means_5ns_10000[9]-nsigma*sigmas_5ns_10000[9]),500+(means_5ns_10000[9]+nsigma*sigmas_5ns_10000[9]))/1000;
  entries_5ns_10000[10]=Histo_5ns20_10000->Integral(500+(means_5ns_10000[10]-nsigma*sigmas_5ns_10000[10]),500+(means_5ns_10000[10]+nsigma*sigmas_5ns_10000[10]))/1000;


  TCanvas *c10000_5ns = new TCanvas();
  c10000_5ns->cd();
  Histo_5ns20_10000->SetLineColorAlpha(kBlue+3,0.65);
  Histo_5ns20_10000->SetTitle( "M^{2}_{miss} for different A' masses (10000 e+ per bunch)");
  Histo_5ns20_10000->SetLineWidth(3);
  Histo_5ns20_10000->GetYaxis()->SetTitle("N_{events}");
  Histo_5ns20_10000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_5ns19_10000->SetLineColorAlpha(kBlue,0.65);
  Histo_5ns19_10000->SetLineWidth(3);
  Histo_5ns18_10000->SetLineColorAlpha(kCyan+3,0.65);
  Histo_5ns18_10000->SetLineWidth(3);
  Histo_5ns16_10000->SetLineColorAlpha(kCyan+1,0.65);
  Histo_5ns16_10000->SetLineWidth(3);
  Histo_5ns14_10000->SetLineColorAlpha(kGreen+3,0.65);
  Histo_5ns14_10000->SetLineWidth(3);
  Histo_5ns12_10000->SetLineColorAlpha(kGreen+1,0.65);
  Histo_5ns12_10000->SetLineWidth(3);
  Histo_5ns10_10000->SetLineColorAlpha(kYellow+2,0.65);
  Histo_5ns10_10000->SetLineWidth(3);
  Histo_5ns8_10000->SetLineColorAlpha(kOrange-3,0.65);
  Histo_5ns8_10000->SetLineWidth(3);
  Histo_5ns6_10000->SetLineColorAlpha(kRed+2,0.65);
  Histo_5ns6_10000->SetLineWidth(3);
  Histo_5ns4_10000->SetLineColorAlpha(kRed,0.65);
  Histo_5ns4_10000->SetLineWidth(3);
  Histo_5ns2_10000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_5ns2_10000->SetLineWidth(3);
    
  Histo_5ns20_10000->Draw();
  Histo_5ns19_10000->Draw("same");
  Histo_5ns18_10000->Draw("same");
  Histo_5ns4_10000->Draw("same");
  Histo_5ns6_10000->Draw("same");
  Histo_5ns8_10000->Draw("same");
  Histo_5ns10_10000->Draw("same");
  Histo_5ns12_10000->Draw("same");
  Histo_5ns14_10000->Draw("same");
  Histo_5ns16_10000->Draw("same");
  Histo_5ns2_10000->Draw("same");

  auto legend_10000_5ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_10000_5ns->AddEntry(Histo_5ns20_10000,"M_{A'} = 20 MeV","l");
  legend_10000_5ns->AddEntry(Histo_5ns19_10000,"M_{A'} = 19 MeV","l");
  legend_10000_5ns->AddEntry(Histo_5ns18_10000,"M_{A'} = 18 MeV","l");
  legend_10000_5ns->AddEntry(Histo_5ns16_10000,"M_{A'} = 16 MeV","l");
  legend_10000_5ns->AddEntry(Histo_5ns14_10000,"M_{A'} = 14 MeV","l");
  legend_10000_5ns->AddEntry(Histo_5ns12_10000,"M_{A'} = 12 MeV","l");
  legend_10000_5ns->AddEntry(Histo_5ns10_10000,"M_{A'} = 10 MeV","l");
  legend_10000_5ns->AddEntry(Histo_5ns8_10000,"M_{A'} = 8 MeV","l");
  legend_10000_5ns->AddEntry(Histo_5ns6_10000,"M_{A'} = 6 MeV","l");
  legend_10000_5ns->AddEntry(Histo_5ns4_10000,"M_{A'} = 4 MeV","l");
  legend_10000_5ns->AddEntry(Histo_5ns2_10000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_10000_5ns->Draw();

  outRoot->WriteObject(c10000_5ns,"10000e_all_5ns");

  
  if(!(Histo_5ns20_25000->GetSumw2N() > 0)) Histo_5ns20_25000->Sumw2(kTRUE);
  Histo_5ns20_25000->Add(Histo_5ns0_25000_30,-1);
  if(!(Histo_5ns19_25000->GetSumw2N() > 0)) Histo_5ns19_25000->Sumw2(kTRUE);
  Histo_5ns19_25000->Add(Histo_5ns0_25000_30,-1);
  if(!(Histo_5ns18_25000->GetSumw2N() > 0)) Histo_5ns18_25000->Sumw2(kTRUE);
  Histo_5ns18_25000->Add(Histo_5ns0_25000_50,-1);
  if(!(Histo_5ns16_25000->GetSumw2N() > 0)) Histo_5ns16_25000->Sumw2(kTRUE);
  Histo_5ns16_25000->Add(Histo_5ns0_25000_60,-1);
  if(!(Histo_5ns14_25000->GetSumw2N() > 0)) Histo_5ns14_25000->Sumw2(kTRUE);
  Histo_5ns14_25000->Add(Histo_5ns0_25000_60,-1);
  if(!(Histo_5ns12_25000->GetSumw2N() > 0)) Histo_5ns12_25000->Sumw2(kTRUE);
  Histo_5ns12_25000->Add(Histo_5ns0_25000_60,-1);
  if(!(Histo_5ns10_25000->GetSumw2N() > 0)) Histo_5ns10_25000->Sumw2(kTRUE);
  Histo_5ns10_25000->Add(Histo_5ns0_25000_60,-1);
  if(!(Histo_5ns8_25000->GetSumw2N() > 0)) Histo_5ns8_25000->Sumw2(kTRUE);
  Histo_5ns8_25000->Add(Histo_5ns0_25000_60,-1);
  if(!(Histo_5ns6_25000->GetSumw2N() > 0)) Histo_5ns6_25000->Sumw2(kTRUE);
  Histo_5ns6_25000->Add(Histo_5ns0_25000_60,-1);
  if(!(Histo_5ns4_25000->GetSumw2N() > 0)) Histo_5ns4_25000->Sumw2(kTRUE);
  Histo_5ns4_25000->Add(Histo_5ns0_25000_60,-1);
  if(!(Histo_5ns2_25000->GetSumw2N() > 0)) Histo_5ns2_25000->Sumw2(kTRUE);
  Histo_5ns2_25000->Add(Histo_5ns0_25000_60,-1);

  Histo_5ns2_25000->Fit(f_2,"R");
  means_5ns_25000[0]=f_2->GetParameter(1);
  sigmas_5ns_25000[0]=f_2->GetParameter(2);

  Histo_5ns4_25000->Fit(f_4,"R");
  means_5ns_25000[1]=f_4->GetParameter(1);
  sigmas_5ns_25000[1]=f_4->GetParameter(2);
  
  Histo_5ns6_25000->Fit(f_6,"R");
  means_5ns_25000[2]=f_6->GetParameter(1);
  sigmas_5ns_25000[2]=f_6->GetParameter(2);
  
  Histo_5ns8_25000->Fit(f_8,"R");
  means_5ns_25000[3]=f_8->GetParameter(1);
  sigmas_5ns_25000[3]=f_8->GetParameter(2);
 
  Histo_5ns10_25000->Fit(f_10,"R");
  means_5ns_25000[4]=f_10->GetParameter(1);
  sigmas_5ns_25000[4]=f_10->GetParameter(2);
  
  Histo_5ns12_25000->Fit(f_12,"R");
  means_5ns_25000[5]=f_12->GetParameter(1);
  sigmas_5ns_25000[5]=f_12->GetParameter(2);
  
  Histo_5ns14_25000->Fit(f_14,"R");
  means_5ns_25000[6]=f_14->GetParameter(1);
  sigmas_5ns_25000[6]=f_14->GetParameter(2);
  
  Histo_5ns16_25000->Fit(f_16,"R");
  means_5ns_25000[7]=f_16->GetParameter(1);
  sigmas_5ns_25000[7]=f_16->GetParameter(2);
  
  Histo_5ns18_25000->Fit(f_18,"R");
  means_5ns_25000[8]=f_18->GetParameter(1);
  sigmas_5ns_25000[8]=f_18->GetParameter(2);
  
  Histo_5ns19_25000->Fit(f_19,"R");
  means_5ns_25000[9]=f_19->GetParameter(1);
  sigmas_5ns_25000[9]=f_19->GetParameter(2);
  
  Histo_5ns20_25000->Fit(f_20,"R");
  means_5ns_25000[10]=f_20->GetParameter(1);
  sigmas_5ns_25000[10]=f_20->GetParameter(2);

  entries_5ns_25000[0]=Histo_5ns2_25000->Integral(500+(means_5ns_25000[0]-nsigma*sigmas_5ns_25000[0]),500+(means_5ns_25000[0]+nsigma*sigmas_5ns_25000[0]))/1000;
  entries_5ns_25000[1]=Histo_5ns4_25000->Integral(500+(means_5ns_25000[1]-nsigma*sigmas_5ns_25000[1]),500+(means_5ns_25000[1]+nsigma*sigmas_5ns_25000[1]))/1000;
  entries_5ns_25000[2]=Histo_5ns6_25000->Integral(500+(means_5ns_25000[2]-nsigma*sigmas_5ns_25000[2]),500+(means_5ns_25000[2]+nsigma*sigmas_5ns_25000[2]))/1000;
  entries_5ns_25000[3]=Histo_5ns8_25000->Integral(500+(means_5ns_25000[3]-nsigma*sigmas_5ns_25000[3]),500+(means_5ns_25000[3]+nsigma*sigmas_5ns_25000[3]))/1000;
  entries_5ns_25000[4]=Histo_5ns10_25000->Integral(500+(means_5ns_25000[4]-nsigma*sigmas_5ns_25000[4]),500+(means_5ns_25000[4]+nsigma*sigmas_5ns_25000[4]))/1000;
  entries_5ns_25000[5]=Histo_5ns12_25000->Integral(500+(means_5ns_25000[5]-nsigma*sigmas_5ns_25000[5]),500+(means_5ns_25000[5]+nsigma*sigmas_5ns_25000[5]))/1000;
  entries_5ns_25000[6]=Histo_5ns14_25000->Integral(500+(means_5ns_25000[6]-nsigma*sigmas_5ns_25000[6]),500+(means_5ns_25000[6]+nsigma*sigmas_5ns_25000[6]))/1000;
  entries_5ns_25000[7]=Histo_5ns16_25000->Integral(500+(means_5ns_25000[7]-nsigma*sigmas_5ns_25000[7]),500+(means_5ns_25000[7]+nsigma*sigmas_5ns_25000[7]))/1000;
  entries_5ns_25000[8]=Histo_5ns18_25000->Integral(500+(means_5ns_25000[8]-nsigma*sigmas_5ns_25000[8]),500+(means_5ns_25000[8]+nsigma*sigmas_5ns_25000[8]))/1000;
  entries_5ns_25000[9]=Histo_5ns19_25000->Integral(500+(means_5ns_25000[9]-nsigma*sigmas_5ns_25000[9]),500+(means_5ns_25000[9]+nsigma*sigmas_5ns_25000[9]))/1000;
  entries_5ns_25000[10]=Histo_5ns20_25000->Integral(500+(means_5ns_25000[10]-nsigma*sigmas_5ns_25000[10]),500+(means_5ns_25000[10]+nsigma*sigmas_5ns_25000[10]))/1000;

  TCanvas *c25000_5ns = new TCanvas();
  c25000_5ns->cd();
  Histo_5ns20_25000->SetLineColorAlpha(kBlue+3,0.65);
  Histo_5ns20_25000->SetTitle( "M^{2}_{miss} for different A' masses (25000 e+ per bunch)");
  Histo_5ns20_25000->SetLineWidth(3);
  Histo_5ns20_25000->GetYaxis()->SetTitle("N_{events}");
  Histo_5ns20_25000->GetXaxis()->SetTitle("M^{2}_{miss} [MeV]");
  Histo_5ns19_25000->SetLineColorAlpha(kBlue,0.65);
  Histo_5ns19_25000->SetLineWidth(3);
  Histo_5ns18_25000->SetLineColorAlpha(kCyan+3,0.65);
  Histo_5ns18_25000->SetLineWidth(3);
  Histo_5ns16_25000->SetLineColorAlpha(kCyan+1,0.65);
  Histo_5ns16_25000->SetLineWidth(3);
  Histo_5ns14_25000->SetLineColorAlpha(kGreen+3,0.65);
  Histo_5ns14_25000->SetLineWidth(3);
  Histo_5ns12_25000->SetLineColorAlpha(kGreen+1,0.65);
  Histo_5ns12_25000->SetLineWidth(3);
  Histo_5ns10_25000->SetLineColorAlpha(kYellow+2,0.65);
  Histo_5ns10_25000->SetLineWidth(3);
  Histo_5ns8_25000->SetLineColorAlpha(kOrange-3,0.65);
  Histo_5ns8_25000->SetLineWidth(3);
  Histo_5ns6_25000->SetLineColorAlpha(kRed+2,0.65);
  Histo_5ns6_25000->SetLineWidth(3);
  Histo_5ns4_25000->SetLineColorAlpha(kRed,0.65);
  Histo_5ns4_25000->SetLineWidth(3);
  Histo_5ns2_25000->SetLineColorAlpha(kMagenta+1,0.65);
  Histo_5ns2_25000->SetLineWidth(3);
    
  Histo_5ns20_25000->Draw();
  Histo_5ns19_25000->Draw("same");
  Histo_5ns18_25000->Draw("same");
  Histo_5ns4_25000->Draw("same");
  Histo_5ns6_25000->Draw("same");
  Histo_5ns8_25000->Draw("same");
  Histo_5ns10_25000->Draw("same");
  Histo_5ns12_25000->Draw("same");
  Histo_5ns14_25000->Draw("same");
  Histo_5ns16_25000->Draw("same");
  Histo_5ns2_25000->Draw("same");

  auto legend_25000_5ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_25000_5ns->AddEntry(Histo_5ns20_25000,"M_{A'} = 20 MeV","l");
  legend_25000_5ns->AddEntry(Histo_5ns19_25000,"M_{A'} = 19 MeV","l");
  legend_25000_5ns->AddEntry(Histo_5ns18_25000,"M_{A'} = 18 MeV","l");
  legend_25000_5ns->AddEntry(Histo_5ns16_25000,"M_{A'} = 16 MeV","l");
  legend_25000_5ns->AddEntry(Histo_5ns14_25000,"M_{A'} = 14 MeV","l");
  legend_25000_5ns->AddEntry(Histo_5ns12_25000,"M_{A'} = 12 MeV","l");
  legend_25000_5ns->AddEntry(Histo_5ns10_25000,"M_{A'} = 10 MeV","l");
  legend_25000_5ns->AddEntry(Histo_5ns8_25000,"M_{A'} = 8 MeV","l");
  legend_25000_5ns->AddEntry(Histo_5ns6_25000,"M_{A'} = 6 MeV","l");
  legend_25000_5ns->AddEntry(Histo_5ns4_25000,"M_{A'} = 4 MeV","l");
  legend_25000_5ns->AddEntry(Histo_5ns2_25000,"M_{A'} = 2 MeV","l");
  //gStyle->SetLegendTextSize(6.);
  legend_25000_5ns->Draw();

  outRoot->WriteObject(c25000_5ns,"25000e_all_5ns");

  
  TCanvas *cgr_5ns = new TCanvas();
 
  auto gr_1_5ns = new TGraph (11,masses,entries_5ns_1);
  gr_1_5ns->SetLineColor(kMagenta);
  gr_1_5ns->SetLineWidth(3); 
  gr_1_5ns->GetYaxis()->SetTitle("Acceptance [%]");
  gr_1_5ns->GetXaxis()->SetTitle("M_{A'} [MeV]");
  gr_1_5ns->SetTitle("Acceptance");
  gr_1_5ns->Draw();
  auto gr_10_5ns = new TGraph (11,masses,entries_5ns_10);
  gr_10_5ns->SetLineColor(kRed);
  gr_10_5ns->SetLineWidth(3);
  gr_10_5ns->Draw("same");
  auto gr_100_5ns = new TGraph (11,masses,entries_5ns_100);
  gr_100_5ns->SetLineColor(kOrange);
  gr_100_5ns->SetLineWidth(3);
  gr_100_5ns->Draw("same");
  auto gr_1000_5ns = new TGraph (11,masses,entries_5ns_1000);
  gr_1000_5ns->SetLineColor(kGreen+3);
  gr_1000_5ns->SetLineWidth(3);
  gr_1000_5ns->Draw("same");
  auto gr_10000_5ns = new TGraph (11,masses,entries_5ns_10000);
  gr_10000_5ns->SetLineColor(kCyan+1);
  gr_10000_5ns->SetLineWidth(3);
  gr_10000_5ns->Draw("same");
  auto gr_25000_5ns = new TGraph (11,masses,entries_5ns_25000);
  gr_25000_5ns->SetLineColor(kBlue);
  gr_25000_5ns->SetLineWidth(3);
  gr_25000_5ns->Draw("same");
  auto legend_gr_5ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_gr_5ns->AddEntry(gr_1_5ns,"1 e+","l");
  legend_gr_5ns->AddEntry(gr_10_5ns,"10 e+","l");
  legend_gr_5ns->AddEntry(gr_100_5ns,"100 e+","l");
  legend_gr_5ns->AddEntry(gr_1000_5ns,"1000 e+","l");
  legend_gr_5ns->AddEntry(gr_10000_5ns,"10000 e+","l");
  legend_gr_5ns->AddEntry(gr_25000_5ns,"25000 e+","l");
  legend_gr_5ns->Draw();

  TCanvas *csig_5ns = new TCanvas();
 
  auto sig_1_5ns = new TGraph (11,masses,sigmas_5ns_1);
  sig_1_5ns->SetLineColor(kMagenta);
  sig_1_5ns->SetLineWidth(3);
  sig_1_5ns->GetYaxis()->SetTitle("Sigma");
  sig_1_5ns->GetXaxis()->SetTitle("M_{A'} [MeV]");
  sig_1_5ns->SetTitle("Sigma");
  sig_1_5ns->Draw();
  auto sig_10_5ns = new TGraph (11,masses,sigmas_5ns_10);
  sig_10_5ns->SetLineColor(kRed);
  sig_10_5ns->SetLineWidth(3);
  sig_10_5ns->Draw("same");
  auto sig_100_5ns = new TGraph (11,masses,sigmas_5ns_100);
  sig_100_5ns->SetLineColor(kOrange);
  sig_100_5ns->SetLineWidth(3);
  sig_100_5ns->Draw("same");
  auto sig_1000_5ns = new TGraph (11,masses,sigmas_5ns_1000);
  sig_1000_5ns->SetLineColor(kGreen+3);
  sig_1000_5ns->SetLineWidth(3);
  sig_1000_5ns->Draw("same");
  auto sig_10000_5ns = new TGraph (11,masses,sigmas_5ns_10000);
  sig_10000_5ns->SetLineColor(kCyan+1);
  sig_10000_5ns->SetLineWidth(3);
  sig_10000_5ns->Draw("same");
  auto sig_25000_5ns = new TGraph (11,masses,sigmas_5ns_25000);
  sig_25000_5ns->SetLineColor(kBlue);
  sig_25000_5ns->SetLineWidth(3);
  sig_25000_5ns->Draw("same");
  auto legend_sig_5ns = new TLegend(0.15,0.5,0.3,0.85);
  legend_sig_5ns->AddEntry(sig_1_5ns,"1 e+","l");
  legend_sig_5ns->AddEntry(sig_10_5ns,"10 e+","l");
  legend_sig_5ns->AddEntry(sig_100_5ns,"100 e+","l");
  legend_sig_5ns->AddEntry(sig_1000_5ns,"1000 e+","l");
  legend_sig_5ns->AddEntry(sig_10000_5ns,"10000 e+","l");
  legend_sig_5ns->AddEntry(sig_25000_5ns,"25000 e+","l");
  legend_sig_5ns->Draw();

  
  outRoot->WriteObject(cgr_5ns,"Acceptance_5ns");
  outRoot->WriteObject(csig_5ns,"Sigma_5ns");

  TCanvas *cacc_time = new TCanvas();
  cacc_time->cd();
  
  auto acc_25000_5ns = new TGraph (11,masses,entries_5ns_25000);
  acc_25000_5ns->SetLineColor(kBlue+3);
  acc_25000_5ns->SetLineWidth(3);
  acc_25000_5ns->GetYaxis()->SetTitle("Acceptance [%]");
  acc_25000_5ns->GetXaxis()->SetTitle("M_{A'} [MeV]");
  acc_25000_5ns->SetTitle("Acceptance");
  acc_25000_5ns->Draw();
  auto acc_25000_nocut = new TGraph (11,masses,entries_noTcut_25000);
  acc_25000_nocut->SetLineColor(kBlue);
  acc_25000_nocut->SetLineWidth(3);
  acc_25000_nocut->Draw("same");
  auto acc_25000_05ns = new TGraph (11,masses,entries_05ns_25000);
  acc_25000_05ns->SetLineColor(kBlue-3);
  acc_25000_05ns->SetLineWidth(3);
  acc_25000_05ns->Draw("same");
  auto acc_25000_1ns = new TGraph (11,masses,entries_1ns_25000);
  acc_25000_1ns->SetLineColor(kBlue-2);
  acc_25000_1ns->SetLineWidth(3);
  acc_25000_1ns->Draw("same");
  auto acc_25000_2ns = new TGraph (11,masses,entries_2ns_25000);
  acc_25000_2ns->SetLineColor(kBlue+2);
  acc_25000_2ns->SetLineWidth(3);
  acc_25000_2ns->Draw("same");
  auto legend_acc_time = new TLegend(0.15,0.5,0.3,0.85);
  legend_acc_time->AddEntry(acc_25000_nocut,"Single cluster","l");
  legend_acc_time->AddEntry(acc_25000_05ns,"Delta T < 0.5 ns","l");
  legend_acc_time->AddEntry(acc_25000_1ns,"Delta T < 1 ns","l");
  legend_acc_time->AddEntry(acc_25000_2ns,"Delta T < 2 ns","l");
  legend_acc_time->AddEntry(acc_25000_5ns,"Delta T < 5 ns","l");
  legend_acc_time->Draw();
  
  outRoot->WriteObject(cacc_time,"Acceptance_25k_deltaT");


  outRoot->Write();
  outRoot->Close();
}
