//MANCINI MARCO 03-17-2022 BHABHA SCATTERING ANALYSIS BASED ON CalcHEP VALUES - TTree vector
{
#include <TH1.h>
#include <TH2.h>
#include <TH3.h> 
#include <TStyle.h> 
#include <TCanvas.h> 
#include <TRandom.h> 
#include <TAxis.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <iostream>


  
  using namespace std;  

  Double_t numRow = 1000000;
  Int_t numCol = 11;
  Double_t Positron_InitialP = 0.282;
  Double_t Electron_initialEnergy = 0.;
  Double_t ElectronMass = 0.000511;
  Double_t GammaMass = 0.;
  TLorentzVector InitialPositron, InitialElectron, SPositron_4P, SElectron_4P, TPositron_4P, TElectron_4P, G1_4P, G2_4P;

  Double_t InitialEnergy_positron[1000000];
  Double_t SFinalEn_positron[1000000], SFinalEn_electron[1000000], TFinalEn_positron[1000000], TFinalEn_electron[1000000];
  Double_t SFinal_PosMomentum[1000000], SFinal_ElecMomentum[1000000];
  Double_t TFinal_PosMomentum[1000000], TFinal_ElecMomentum[1000000];
  Double_t SE_tot[1000000], TE_tot[1000000];
  Double_t Stheta_scattering[1000000], Ttheta_scattering[1000000];
  Double_t Stheta_test[1000000], Ttheta_test[1000000];

  Double_t FinalEn_G1[1000000], FinalEn_G2[1000000];
  Double_t Final_G1Momentum[1000000], Final_G2Momentum[1000000];
  Double_t GGE_tot[1000000];
  Double_t AAtheta_scattering[1000000];
  Double_t theta_test[1000000];
  
  Double_t Pos_Stheta_x[1000000], Pos_Sphi_y[1000000], Pos_Ttheta_x[1000000], Pos_Tphi_y[1000000]; //angoli di deviazione dei prodotti di reazione rispetto alla direzione x e y
  Double_t Elec_Stheta_x[1000000], Elec_Sphi_y[1000000], Elec_Ttheta_x[1000000], Elec_Tphi_y[1000000];
  Double_t G1_theta_x[1000000], G1_phi_y[1000000], G2_theta_x[1000000], G2_phi_y[1000000];
  Double_t TargetCalo = 3500; //in mm
  //Double_t semiL = 50; //lato mezzi del quadrato centrale
  //Double_t geometry_radius = 300; //raggio geometrico del calorimetro

  Double_t Pos_Sx[1000000], Pos_Sy[1000000], Elec_Sx[1000000], Elec_Sy[1000000]; //coordinate sul calorimentro nel piano trasverso al fascio xy
  Double_t Pos_Tx[1000000], Pos_Ty[1000000], Elec_Tx[1000000], Elec_Ty[1000000];
  Double_t G1_x[1000000], G1_y[1000000], G2_x[1000000], G2_y[1000000];
  
  TFile *f1 = new TFile("/home/marco1/Documenti/background_SM/BHABHA_T_1e6new.root");
  TFile *f2 = new TFile("/home/marco1/Documenti/background_SM/BHABHA_S_1e6.root");
  TFile *f3 = new TFile("/home/marco1/Documenti/background_SM/Ee_AA_1e6.root");

  TTree *t1 = (TTree*)f1->Get("T1");
  TTree *t2 = (TTree*)f2->Get("t2");
  TTree *t3 = (TTree*)f3->Get("t3");
  Double_t InitialP3_positron, InitialP3_electron, TFinalP1_electron, TFinalP2_electron, TFinalP3_electron, TFinalP1_positron, TFinalP2_positron, TFinalP3_positron;
  Double_t SFinalP1_electron, SFinalP2_electron, SFinalP3_electron, SFinalP1_positron, SFinalP2_positron, SFinalP3_positron;
  Double_t FinalP1_G1, FinalP2_G1, FinalP3_G1, FinalP1_G2, FinalP2_G2, FinalP3_G2;
  
  t1->SetBranchAddress("InitialP3_positron", &InitialP3_positron);
  t1->SetBranchAddress("InitialP3_electron", &InitialP3_electron);
  t1->SetBranchAddress("TFinalP1_electron", &TFinalP1_electron);
  t1->SetBranchAddress("TFinalP2_electron", &TFinalP2_electron);
  t1->SetBranchAddress("TFinalP3_electron", &TFinalP3_electron);
  t1->SetBranchAddress("TFinalP1_positron", &TFinalP1_positron);
  t1->SetBranchAddress("TFinalP2_positron", &TFinalP2_positron);
  t1->SetBranchAddress("TFinalP3_positron", &TFinalP3_positron);
  t2->SetBranchAddress("SFinalP1_electron", &SFinalP1_electron);
  t2->SetBranchAddress("SFinalP2_electron", &SFinalP2_electron);
  t2->SetBranchAddress("SFinalP3_electron", &SFinalP3_electron);
  t2->SetBranchAddress("SFinalP1_positron", &SFinalP1_positron);
  t2->SetBranchAddress("SFinalP2_positron", &SFinalP2_positron);
  t2->SetBranchAddress("SFinalP3_positron", &SFinalP3_positron);
  t3->SetBranchAddress("FinalP1_G1", &FinalP1_G1);
  t3->SetBranchAddress("FinalP2_G1", &FinalP2_G1);
  t3->SetBranchAddress("FinalP3_G1", &FinalP3_G1);
  t3->SetBranchAddress("FinalP1_G2", &FinalP1_G2);
  t3->SetBranchAddress("FinalP2_G2", &FinalP2_G2);
  t3->SetBranchAddress("FinalP3_G2", &FinalP3_G2);

  Long64_t nentries = t1->GetEntries();
  
  for(Long64_t k=0; k<nentries; k++){
    t1->GetEntry(k);
    t2->GetEntry(k);
    t3->GetEntry(k);

    //INITIAL STATE -> T,S CHANNEL and Ee->AA HAVE SAME INITIAL STATE

    InitialEnergy_positron[k] = TMath::Sqrt(ElectronMass*ElectronMass + InitialP3_positron*InitialP3_positron);
    InitialPositron.SetPxPyPzE(0, 0, InitialP3_positron, InitialEnergy_positron[k]);
    InitialElectron.SetPxPyPzE(0, 0, 0, ElectronMass);

    //FINAL STATE 3MOMENTUM DISTRIBUTION - T, S CHANNEL and Ee->AA
    SFinal_PosMomentum[k] = TMath::Sqrt(SFinalP1_positron*SFinalP1_positron + SFinalP2_positron*SFinalP2_positron + SFinalP3_positron*SFinalP3_positron);
    if(TMath::IsNaN(SFinal_PosMomentum[k])){ SFinal_PosMomentum[k] = -0.025;}
       
    SFinal_ElecMomentum[k] = TMath::Sqrt(SFinalP1_electron*SFinalP1_electron + SFinalP2_electron*SFinalP2_electron + SFinalP3_electron*SFinalP3_electron);
    if(TMath::IsNaN(SFinal_ElecMomentum[k])) {SFinal_ElecMomentum[k] = -0.025;}
    
    TFinal_PosMomentum[k] = TMath::Sqrt(TFinalP1_positron*TFinalP1_positron + TFinalP2_positron*TFinalP2_positron + TFinalP3_positron*TFinalP3_positron);
    if(TMath::IsNaN(TFinal_PosMomentum[k])) { TFinal_PosMomentum[k] = -0.025;}
    
    TFinal_ElecMomentum[k] = TMath::Sqrt(TFinalP1_electron*TFinalP1_electron + TFinalP2_electron*TFinalP2_electron + TFinalP3_electron*TFinalP3_electron);
    if(TMath::IsNaN(TFinal_ElecMomentum[k])) {TFinal_ElecMomentum[k] = -0.025;}

    Final_G1Momentum[k] = TMath::Sqrt(FinalP1_G1*FinalP1_G1 + FinalP2_G1*FinalP2_G1 + FinalP3_G1*FinalP3_G1);
    if(TMath::IsNaN(Final_G1Momentum[k])) { Final_G1Momentum[k] = -0.025;}
 
    Final_G2Momentum[k] = TMath::Sqrt(FinalP1_G2*FinalP1_G2 + FinalP2_G2*FinalP2_G2 + FinalP3_G2*FinalP3_G2);
    if(TMath::IsNaN(Final_G2Momentum[k])) { Final_G2Momentum[k] = -0.025;}
    ////////////////// THE OUTPUT HAS NOT NaN!! //////////////////////////

    //FINAL STATE ENERGY DISTRIBUTION - S,T CHANNEL and Ee->AA
    SFinalEn_positron[k] = TMath::Sqrt(ElectronMass*ElectronMass + SFinal_PosMomentum[k]*SFinal_PosMomentum[k]);
    SFinalEn_electron[k] = TMath::Sqrt(ElectronMass*ElectronMass + SFinal_ElecMomentum[k]*SFinal_ElecMomentum[k]);
    TFinalEn_positron[k] = TMath::Sqrt(ElectronMass*ElectronMass + TFinal_PosMomentum[k]*TFinal_PosMomentum[k]);
    TFinalEn_electron[k] = TMath::Sqrt(ElectronMass*ElectronMass + TFinal_ElecMomentum[k]*TFinal_ElecMomentum[k]);
    FinalEn_G1[k] = TMath::Abs(Final_G1Momentum[k]);
    FinalEn_G2[k] = TMath::Abs(Final_G2Momentum[k]);

    //ENERGY CONSERVATION CHECK
    SE_tot[k] = SFinalEn_positron[k] + SFinalEn_electron[k];
    TE_tot[k] = TFinalEn_positron[k] + TFinalEn_electron[k];
    GGE_tot[k] = FinalEn_G1[k] + FinalEn_G2[k];

    //TLorentzVector (inutile?)
    SPositron_4P.SetPxPyPzE(SFinalP1_positron, SFinalP2_positron, SFinalP3_positron, SFinalEn_positron[k]);
    SElectron_4P.SetPxPyPzE(SFinalP1_electron, SFinalP2_electron, SFinalP3_electron, SFinalEn_electron[k]);
    TPositron_4P.SetPxPyPzE(TFinalP1_positron, TFinalP2_positron, TFinalP3_positron, TFinalEn_positron[k]);
    TElectron_4P.SetPxPyPzE(TFinalP1_electron, TFinalP2_electron, TFinalP3_electron, TFinalEn_electron[k]);
    G1_4P.SetPxPyPzE(FinalP1_G1, FinalP2_G1, FinalP3_G1, FinalEn_G1[k]);
    G2_4P.SetPxPyPzE(FinalP1_G2, FinalP2_G2, FinalP3_G2, FinalEn_G2[k]);
    
    
    //FINAL STATE ANGULAR DISTRIBUTION BETWEEN FINAL PARTICLES - S,T CHANNEL and Ee->AA
    Stheta_scattering[k] = TMath::ACos((SFinalEn_positron[k]*SFinalEn_electron[k] - ElectronMass*InitialEnergy_positron[k]) /(SFinal_PosMomentum[k]*SFinal_ElecMomentum[k]));
    if(TMath::IsNaN(Stheta_scattering[k])) {Stheta_scattering[k] = -0.05;}
   
    Ttheta_scattering[k] = TMath::ACos((TFinalEn_positron[k]*TFinalEn_electron[k] - ElectronMass*InitialEnergy_positron[k]) /(TFinal_PosMomentum[k]*TFinal_ElecMomentum[k]));
    if(TMath::IsNaN(Ttheta_scattering[k])) {Ttheta_scattering[k] = -0.07;}

    AAtheta_scattering[k] = TMath::ACos((FinalEn_G1[k]*FinalEn_G2[k] - ElectronMass*InitialEnergy_positron[k] - ElectronMass*ElectronMass) / (Final_G1Momentum[k]*Final_G2Momentum[k]));
    if(TMath::IsNaN(AAtheta_scattering[k])) {AAtheta_scattering[k] = -0.05;}


    //TRANSVERSE FINAL POSITION AT 3500 MM FROM TARGET - THETA PHI DEF
    Pos_Stheta_x[k] = TMath::ATan(SFinalP1_positron/SFinalP3_positron);
    Pos_Sphi_y[k] = TMath::ATan(SFinalP2_positron/SFinalP3_positron);
    Elec_Stheta_x[k] = TMath::ATan(SFinalP1_electron/SFinalP3_electron);
    Elec_Sphi_y[k] = TMath::ATan(SFinalP2_electron/SFinalP3_electron);

    Pos_Ttheta_x[k] = TMath::ATan(TFinalP1_positron/TFinalP3_positron);
    Pos_Tphi_y[k] = TMath::ATan(TFinalP2_positron/TFinalP3_positron);
    Elec_Ttheta_x[k] = TMath::ATan(TFinalP1_electron/TFinalP3_electron);
    Elec_Tphi_y[k] = TMath::ATan(TFinalP2_electron/TFinalP3_electron);

    G1_theta_x[k] = TMath::ATan(FinalP1_G1/FinalP3_G1);
    G1_phi_y[k] = TMath::ATan(FinalP2_G1/FinalP3_G1);
    G2_theta_x[k] = TMath::ATan(FinalP1_G2/FinalP3_G2);
    G2_phi_y[k] = TMath::ATan(FinalP2_G2/FinalP3_G2);

    //POSITION
    Pos_Sx[k] = TMath::Tan(Pos_Stheta_x[k]) * TargetCalo;
    Pos_Sy[k] = TMath::Tan(Pos_Sphi_y[k]) * TargetCalo;
    Elec_Sx[k] = TMath::Tan(Elec_Stheta_x[k]) * TargetCalo;
    Elec_Sy[k] = TMath::Tan(Elec_Sphi_y[k]) * TargetCalo;

    Pos_Tx[k] = TMath::Tan(Pos_Ttheta_x[k]) * TargetCalo;
    Pos_Ty[k] = TMath::Tan(Pos_Tphi_y[k]) * TargetCalo;
    Elec_Tx[k] = TMath::Tan(Elec_Ttheta_x[k]) * TargetCalo;
    Elec_Ty[k] = TMath::Tan(Elec_Tphi_y[k]) * TargetCalo;

    G1_x[k] = TMath::Tan(G1_theta_x[k]) * TargetCalo;
    G1_y[k] = TMath::Tan(G1_phi_y[k]) * TargetCalo;
    G2_x[k] = TMath::Tan(G2_theta_x[k]) * TargetCalo;
    G2_y[k] = TMath::Tan(G2_phi_y[k]) * TargetCalo;

    
 }

  /*Double_t time = 86400; //seconds ->example = 1 day is equal to 86400 s
  Double_t PoT[20] = {1e10, 2e10, 3e10, 4e10, 5e10, 6e10, 7e10, 8e10, 9e10, 10e10, 11e10, 12e10, 13e10, 14e10, 15e10, 16e10, 17e10, 18e10, 19e10, 2e11};
  Double_t Na = 6.022e23;
  Double_t Z_C = 6;
  //Double_t Z_Si = 14;
  Double_t D_C = 3.5;
  //Double_t D_Si = 2.33; //g*cm^-3
  Double_t A_C = 12;
  //Double_t A_Si = 28.08; //non è un materiale puro
  Double_t TThickness = 0.01; //in cm
  Double_t CmtoPb = 1e-36;

  Double_t PreFactor_X17 = 3.8e-7; //DATAS FROM PROCEEDINGS DARME-NARDI
  Double_t g_ve[10] = {1e-4, 2e-4, 3e-4, 4e-4, 5e-4, 6e-4, 7e-4, 8e-4, 9e-4, 10e-4} ;

  //Double_t Sigma_tot = 5.048137e10; //pb
  Double_t Sigma_S = 3.020595e8; //pb
  Double_t Sigma_T = 5.261278e10; //pb -> FOCUS IR divergence saved by energy cut on CalcHEP (E_min = 0.005 GeV)
  Double_t Sigma_AA = 2.732152e9; //pb

  Double_t Luminosity_C;
  //Double_t Luminosity_Si;
  Double_t N_Schannel_C[20], N_Tchannel_C[20], N_AA_C[20], N_X17_C[20][10];

  Luminosity_C = CmtoPb * Na * Z_C * D_C * TThickness / A_C;
  //Luminosity_Si = CmtoPb * Na * Z_Si * D_Si * TThickness / A_Si;

  for(int k = 0; k<20; k++){
  //N_tot_C = Luminosity_C * Sigma_tot * PoT;
  //N_tot_Si = Luminosity_Si * Sigma_tot * PoT;
  N_Schannel_C[k] = Luminosity_C * Sigma_S * PoT[k]; 
  //N_Schannel_Si = Luminosity_Si * Sigma_S * PoT;
  N_Tchannel_C[k] = Luminosity_C * Sigma_T * PoT[k];
  //N_Tchannel_Si = Luminosity_Si * Sigma_T * PoT; 
  N_AA_C[k] = Luminosity_C * Sigma_AA * PoT[k];
  //N_AA_Si = Luminosity_Si * Sigma_AA * PoT;
  
  for(int i = 0; i<10; i++){
    N_X17_C[k][i] = PoT[k] * PreFactor_X17 * ((g_ve[i]*g_ve[i])/(9e-8)) * (1/0.7) * ((Na * Z_C * D_C)/(A_C * 1e24)); //EQUATION 13 Proceedings LUC  <- il primo indice sono i POT, il secondo il valore del COUPLING g_ve
    }
    }*/

  
  //ofstream file;
  //file.open("X17_production.txt");
  
  //file<<"###################### X17 BACKGROUND ANALYSIS ############################"<<"\n";

  Double_t En_min[28]; 
  Double_t Radius_min[32] = {80., 85., 90., 95., 100., 105., 110., 115., 120., 125., 130., 135., 140., 145., 150., 155., 160., 165., 170., 175., 180., 185., 190., 195., 200., 205., 210., 215., 220. ,225., 230., 235.};
  Double_t Radius_max[21] = {180., 185., 190., 195., 200., 205., 210., 215., 220., 225., 230., 235., 240., 245., 250., 255., 260., 265., 270., 275., 280.};
  Double_t Radius_SPos[1000000], Radius_SElec[1000000], Radius_TPos[1000000], Radius_TElec[1000000], Radius_G1[1000000], Radius_G2[1000000];
  //Double_t NC_PADME_S[21][10], NC_PADME_T[21][10],NC_PADME_AA[21][10], NC_PADME_X17[21][10];
  Double_t NC_PADME_S[20], NC_PADME_T[20],NC_PADME_AA[20], NC_PADME_X17[20][10];
  Double_t Squared_gve[20];
  Double_t Scan_S[21][32][28], Scan_T[21][32][28], Scan_AA[21][32][28];
  Double_t S_ACC[21][32][28], T_ACC[21][32][28], AA_ACC[21][32][28];
  Double_t Sensitivity[20][10], LowerLimitX17[20], X17_produced[20];
  Double_t Acceptance_ratio[21][32][28];  
  double a = 0.070;
  for(Int_t l= 0; l<22; l++){
    for(int k = 0; k<33; k++){
      for(Int_t j = 0; j<29; j++){
	Scan_S[l][k][j] = 0.;
	Scan_T[l][k][j] = 0.;
	Scan_AA[l][k][j] = 0.;
	S_ACC[l][k][j] = 0.;
	T_ACC[l][k][j] = 0.;
	AA_ACC[l][k][j] = 0.;
      }
    }
  }

  for(Int_t j = 0; j<29; j++){
    En_min[j] = a;
    a = a +0.0025;
  }
			   
      
  for(Int_t k=0; k<numRow; k++){   
    Radius_SPos[k] = TMath::Sqrt(Pos_Sx[k]*Pos_Sx[k] + Pos_Sy[k]*Pos_Sy[k]);
    Radius_SElec[k] = TMath::Sqrt(Elec_Sx[k]*Elec_Sx[k] + Elec_Sy[k]*Elec_Sy[k]);
    Radius_TPos[k] = TMath::Sqrt(Pos_Tx[k]*Pos_Tx[k] + Pos_Ty[k]*Pos_Ty[k]);
    Radius_TElec[k] = TMath::Sqrt(Elec_Tx[k]*Elec_Tx[k] + Elec_Ty[k]*Elec_Ty[k]);
    Radius_G1[k] = TMath::Sqrt(G1_x[k]*G1_x[k] + G1_y[k]*G1_y[k]);
    Radius_G2[k] = TMath::Sqrt(G2_x[k]*G2_x[k] + G2_y[k]*G2_y[k]);
  }
  
  //MAKE A COUNTER - GEOMETRY CUTS
 

  //TH2F *SCAN2D = new TH2F("SCAN2D", "Scan Acceptance as a function of R_{max} and energy", 13, 217.5, 282.5, 28, 0.0675, 0.14);
   //TH1F *ACC_S = new TH1F("ACC_SD", "PADME S Channel Acceptance as a function of R_{max} and E > 100 MeV", 16, 202.5, 282.5);
   //TH1F *ACC_T = new TH1F("ACC_T", "PADME T Channel Acceptance as a function of R_{max} and E > 100 MeV", 16, 202.5, 282.5);
   //TH1F *ACC_AA = new TH1F("ACC_AA", "PADME #gamma#gamma-Production Acceptance as a function of R_{max} and E > 100 MeV", 16, 202.5, 282.5);
   TH2F *ACC_RATIO1 = new TH2F("ACC_RATIO1", "Ratio between S- and T-Channel Acceptance as a function of R_{max} and energy", 13, 217.5, 282.5, 28, 0.0675, 0.14);
   TH2F *ACC_RATIO2 = new TH2F("ACC_RATIO2", "Ratio between S- and T-Channel Acceptance as a function of R_{min} and energy", 26, 77.5, 207.5, 28, 0.0675, 0.14);
   TH2F *ACC_RATIO3 = new TH2F("ACC_RATIO3", "Ratio between S- and T-Channel Acceptance as a function of R_{max} and R_{min}", 13, 217.5, 282.5, 26, 77.5, 207.5);

   TH2F * Sacc = new TH2F("S-channel", "PADME S-channel acceptance as a function of R_{max} and E_{min}", 13, 217.5, 282.5, 28, 0.0675, 0.14);
   TH2F * Tacc = new TH2F("T-channel", "PADME T-channel acceptance as a function of R_{max} and E_{min}", 13, 217.5, 282.5, 28, 0.0675, 0.14);
   TH2F * AAacc = new TH2F("AA-prod", "PADME #gamma#gamma-production acceptance as a function of R_{max} and E_{min}", 13, 217.5, 282.5, 28, 0.0675, 0.14);

   TH2F * SaccRad = new TH2F("S-channel", "PADME S-channel acceptance as a function of R_{max} and R_{min}", 14, 212.5, 282.5, 27, 77.5, 212.5);
   TH2F * TaccRad = new TH2F("T-channel", "PADME T-channel acceptance as a function of R_{max} and R_{min}", 14, 212.5, 282.5, 27, 77.5, 212.5);
   TH2F * AAaccRad = new TH2F("AA-prod", "PADME #gamma#gamma-production acceptance as a function of R_{max} and R_{min}", 14, 212.5, 282.5, 27, 77.5, 212.5);

   TH2F * Sacc_Radmin = new TH2F("S-channel", "PADME S-channel acceptance as a function of R_{min} and E_{min}", 27, 77.5, 212.5, 28, 0.0675, 0.14);
   TH2F * Tacc_Radmin = new TH2F("T-channel", "PADME T-channel acceptance as a function of R_{min} and E_{min}", 27, 77.5, 212.5, 28, 0.0675, 0.14);
   TH2F * AAacc_Radmin = new TH2F("AA-prod", "PADME #gamma#gamma-production acceptance as a function of R_{min} and E_{min}", 27, 77.5, 212.5, 28, 0.0675, 0.14);


   //ACC_S->SetTitle("PADME S Channel Acceptance as a function of R_{max} and E > 50 MeV - CARBON TARGET; R_{max} [mm]; Acceptance");
  //ACC_T->SetTitle("PADME T Channel Acceptance as a function of R_{max} and E > 50 MeV - CARBON TARGET; R_{max} [mm]; Acceptance");
  //ACC_AA->SetTitle("PADME #gamma#gamma-Production Acceptance as a function of R_{max} and E > 50 MeV - CARBON TARGET; R_{max} [mm]; Acceptance");
  
  //for(Int_t l = 0; l<21; l++){
    //file<<"---------------------R_{max}= "<<Radius_max[l]<<" mm "<<"-------------\n";
    for(Int_t i = 0; i<32; i++){
      for(Int_t j = 0; j<28; j++){
	//file<<"--------------DATA E> "<<En_min[j]<<" GeV "<<"-----------\n";
	for(Int_t k = 0; k<numRow; k++){

  
	  
      if(Radius_SPos[k]>Radius_min[i] && Radius_SElec[k]>Radius_min[i]){
	if(Radius_SPos[k]<Radius_max[19] && Radius_SElec[k]<Radius_max[19]){
	  if(SFinalEn_positron[k]>En_min[j] && SFinalEn_electron[k]>En_min[j]){
	    Scan_S[19][i][j]++;};};};
	    //SCAN2D->Fill(Radius_max[19], En_min[j]);};};};
      
      if(Radius_TPos[k]>Radius_min[i] && Radius_TElec[k]>Radius_min[i]){
	if(Radius_TPos[k]<Radius_max[19] && Radius_TElec[k]<Radius_max[19]){
	  if(TFinalEn_positron[k]>En_min[j] && TFinalEn_electron[k]>En_min[j]){
	    Scan_T[19][i][j]++; };};}
	    //SCAN2D->Fill(Radius_max[19], En_min[j]);};};};

      if(Radius_G1[k]>Radius_min[i] && Radius_G2[k]>Radius_min[i]){
	  if(Radius_G1[k]<Radius_max[19] && Radius_G2[k]<Radius_max[19]){
	    if(FinalEn_G1[k]>En_min[j] && FinalEn_G2[k]>En_min[j]){
	      Scan_AA[19][i][j]++;};};};
      //}
	}
    

    S_ACC[19][i][j] = Scan_S[19][i][j]/numRow;
    T_ACC[19][i][j] = Scan_T[19][i][j]/numRow;
    AA_ACC[19][i][j] = Scan_AA[19][i][j]/numRow;
    Acceptance_ratio[19][i][j] = S_ACC[19][i][j] / T_ACC[19][i][j];
    }
  }
    //ACC_RATIO1->Fill(Radius_max[l], En_min[j], Acceptance_ratio[l][2][j]);

    //Sacc->Fill(Radius_max[l], En_min[j], S_ACC[l][j]);
    //Tacc->Fill(Radius_max[l], En_min[j], T_ACC[l][j]);
    //AAacc->Fill(Radius_max[l], En_min[j], AA_ACC[l][j]);
  
  

  //ACC_S->Fill(Radius_max[l], S_ACC[l][2][1]);
  //ACC_T->Fill(Radius_max[l], T_ACC[l][2][1]);
  //ACC_AA->Fill(Radius_max[l], AA_ACC[l][2][1]);

  //for(Int_t l = 0; l<21; l++){
    for(Int_t i = 0; i<32; i++){
      for(Int_t j = 0; j<28; j++){
	//Sacc->Fill(Radius_max[l], En_min[j], S_ACC[l][2][j]);
      //Tacc->Fill(Radius_max[l], En_min[j], T_ACC[l][2][j]);
      //AAacc->Fill(Radius_max[l], En_min[j], AA_ACC[l][2][j]);
      //SaccRad->Fill(Radius_max[l], Radius_min[i], S_ACC[l][i][11]);
      //TaccRad->Fill(Radius_max[l], Radius_min[i], T_ACC[l][i][11]);
      //AAaccRad->Fill(Radius_max[l], Radius_min[i], AA_ACC[l][i][11]);
      Sacc_Radmin->Fill(Radius_min[i], En_min[j], S_ACC[19][i][j]);
      Tacc_Radmin->Fill(Radius_min[i], En_min[j], T_ACC[19][i][j]);
      AAacc_Radmin->Fill(Radius_min[i], En_min[j], AA_ACC[19][i][j]);
      
      //ACC_RATIO1->Fill(Radius_max[l], En_min[j], Acceptance_ratio[l][2][j]);
      ACC_RATIO2->Fill(Radius_min[i], En_min[j], Acceptance_ratio[19][i][j]);
      //ACC_RATIO3->Fill(Radius_max[l], Radius_min[i], Acceptance_ratio[l][i][11]);
      //}
    }
  }
  

  //running PoT adn coupling
  /*TH2F * SENS = new TH2F("SENS", "PADME X17 detected over sqrt(BG) as a function of N-PoT and g_{Ve}", 20, 0.5e10, 2.05e11, 10, 0.5e-4, 1.05e-3);
  TH1F * CL90 = new TH1F("CL90", "PADME 90% CL as a function of N-PoT", 20, 0.5e10, 2.05e11);
  TH1F * exclusion_X17 = new TH1F("exclusion_X17", "g_{Ve}^{2} exclusion plot as a function of N-PoT", 20, 0.5e10, 2.05e11);

  //NA64 epsilon limit
  TH1F * NA64_limit = new TH1F("NA64_limit", "g_{e}^{2} NA64", 20, 0.5e10, 2.05e11);
  Double_t ge_NA64[20];
  
  for(Int_t l = 0; l<20; l++){ //POT
    file<<"\n ---------------------PoT = "<<PoT[l]<<" -------------\n";
    NC_PADME_S[l] = S_ACC[19][1] * N_Schannel_C[l];
    NC_PADME_T[l] = T_ACC[19][1] * N_Tchannel_C[l];
    NC_PADME_AA[l] = AA_ACC[19][1] * N_AA_C[l];
    ge_NA64[l] = 4e-8;

    LowerLimitX17[l]= 1.2*TMath::Sqrt(NC_PADME_S[l] + NC_PADME_T[l]); //check prefactor: is 1.2 equal to 90% confidency level?
    X17_produced[l] = LowerLimitX17[l] / S_ACC[19][1]; //il numero di candidati X17 entro il calorimetro -> 90CL * Accettanza S Channel
    Squared_gve[l]= (X17_produced[l] * 0.7)/(PoT[l]*PreFactor_X17)*(9e-8) * ((A_C * 1e24)/(Na * Z_C * D_C));
    CL90->Fill(PoT[l], LowerLimitX17[l]);
    exclusion_X17->Fill(PoT[l], Squared_gve[l]);
    NA64_limit->Fill(PoT[l], ge_NA64[l]);

    //file<<"N_tot S-channel : "<<NC_PADME_S[l]<<"\n";
    //file<<"N_tot T-channel : "<<NC_PADME_T[l]<<"\n";
    //file<<"N_tot Gamma-Gamma : "<<NC_PADME_AA[l]<<"\n";
    file<<"90% Confidence level : "<<LowerLimitX17[l]<<"\n"; 
    file<<"Minimal g_Ve^{2}: "<<Squared_gve[l]<<"\n"; 
    
    for(Int_t j = 0; j<10; j++){ //EPSILON
      file<<"--------------g_Ve = "<<g_ve[j]<<" -----------\n";

      NC_PADME_X17[l][j] = S_ACC[19][1] * N_X17_C[l][j]; // questi sono il numero di X17 prodotti dalla formula 13_proceedings moltiplicati per S_Accettanza -> QUELLI RIVELATI DAL CALO
      Sensitivity[l][j] = NC_PADME_X17[l][j] / TMath::Sqrt(NC_PADME_X17[l][j] + NC_PADME_S[l] + NC_PADME_T[l]); //tiene conto di tutte le accettanze
      
      SENS->Fill(PoT[l], g_ve[j], Sensitivity[l][j]);
      
      file<<"N_tot X17 : "<<NC_PADME_X17[l][j]<<"\n";
      file<<"Sensitivity : "<<Sensitivity[l][j]<<"\n";
      
    }
    }*/
  //file.close();


  //DEFINITION OF HISTOGRAMS  

  //SCAN2D->SetXTitle("R_{max} [mm]"); SCAN2D->SetYTitle("E_{min} [GeV]");
  Sacc->SetXTitle("R_{max} [mm]"); Sacc->SetYTitle("E_{min} [GeV]");
  Tacc->SetXTitle("R_{max} [mm]"); Tacc->SetYTitle("E_{min} [GeV]");
  AAacc->SetXTitle("R_{max} [mm]"); AAacc->SetYTitle("E_{min} [GeV]");
  SaccRad->SetXTitle("R_{max} [mm]"); SaccRad->SetYTitle("R_{min} [mm]");
  TaccRad->SetXTitle("R_{max} [mm]"); TaccRad->SetYTitle("R_{min} [mm]");
  AAaccRad->SetXTitle("R_{max} [mm]"); AAaccRad->SetYTitle("R_{min} [mm]");
  Sacc_Radmin->SetXTitle("R_{min} [mm]"); Sacc_Radmin->SetYTitle("E_{min} [GeV]");
  Tacc_Radmin->SetXTitle("R_{min} [mm]"); Tacc_Radmin ->SetYTitle("E_{min} [Gev]");
  AAacc_Radmin->SetXTitle("R_{min} [mm]"); AAacc_Radmin->SetYTitle("E_{min} [Gev]");
  //ACC_S->SetXTitle("R_{max} [mm]"); ACC_S->SetYTitle("Acceptance");
  //ACC_T->SetXTitle("R_{max} [mm]"); ACC_T->SetYTitle("Acceptance");
  //ACC_AA->SetXTitle("R_{max} [mm]"); ACC_AA->SetYTitle("Acceptance");
  ACC_RATIO1->SetXTitle("R_{max} [mm]"); ACC_RATIO1->SetYTitle("E_{min} [GeV]");
  ACC_RATIO2->SetXTitle("R_{min} [mm]"); ACC_RATIO2->SetYTitle("E_{min} [GeV]");
  ACC_RATIO3->SetXTitle("R_{max} [mm]"); ACC_RATIO3->SetYTitle("R_{min} [mm]");
//SENS->SetXTitle("N-PoT"); SENS->SetYTitle("g_{Ve}");
//CL90->SetXTitle("N-PoT"); CL90->SetYTitle("X_{17}^{min}");
//exclusion_X17->SetXTitle("N-PoT"); exclusion_X17->SetYTitle("g_{V_{e}}^{2}");
//NA64_limit->SetXTitle("N-PoT"); NA64_limit->SetYTitle("g_{V_{e}}^{2}");
  
//exclusion_X17->SetMaximum(5e-8); exclusion_X17->SetMinimum(5e-9);
//NA64_limit->SetMaximum(5e-8); NA64_limit->SetMinimum(5e-9);

  //ACC_S->SetFillColor(kRed);
  //ACC_T->SetFillColor(kGreen);
  //ACC_AA->SetFillColor(kCyan);
  //CL90->SetFillColor(kOrange+7);
  //exclusion_X17->SetLineColor(kGreen-10); exclusion_X17->SetFillColor(10); exclusion_X17->SetFillStyle(1001); 
  //NA64_limit->SetLineColor(kRed);
  
    
  /*TCanvas *Acc3D_a = new TCanvas("Acc3D_a", "PADME Acceptance as a function of R_{max} and energy", 900, 700);
  Acc3D_a->SetGrid();
  gStyle->SetPalette(kBird);
  gStyle->SetOptStat(kFALSE);
  SCAN2D->Draw("LEGO2z");

  TCanvas *Acc3D_b = new TCanvas("Acc3D_b", "PADME Acceptance as a function of R_{max} and energy", 900, 700);
  Acc3D_b->SetGrid();
  gStyle->SetPalette(kBird);
  gStyle->SetOptStat(kFALSE);
  SCAN2D->Draw("colz");*/

  //TCanvas *Sacceptance_a = new TCanvas("Sacceptance_a", "PADME S-channel acceptance as a function of R_{max} and energy", 900, 700);
  //Sacceptance_a->SetGrid();
  //gStyle->SetPalette(kBird);
  //gStyle->SetOptStat(kFALSE);
  //Sacc->Draw("LEGO2z");

  //TCanvas *Sacceptance_b = new TCanvas("Sacceptance_b", "PADME S-channel acceptance as a function of R_{max} and energy", 900, 700);
  //Sacceptance_b->SetGrid();
  //gStyle->SetPalette(kBird);
  //gStyle->SetOptStat(kFALSE);
  //Sacc->Draw("colz");

  //TCanvas *Tacceptance_a = new TCanvas("Tacceptance_a", "PADME T-channel acceptance as a function of R_{max} and energy", 900, 700);
  //Tacceptance_a->SetGrid();
  //gStyle->SetPalette(kBird);
  //gStyle->SetOptStat(kFALSE);
  //Tacc->Draw("LEGO2z");

  //TCanvas *Tacceptance_b = new TCanvas("Tacceptance_b", "PADME T-channel acceptance as a function of R_{max} and energy", 900, 700);
  //Tacceptance_b->SetGrid();
  //gStyle->SetPalette(kBird);
  //gStyle->SetOptStat(kFALSE);
  //Tacc->Draw("colz");

  //TCanvas *AAacceptance_a = new TCanvas("AAacceptance_a", "PADME #gamma#gamma acceptance as a function of R_{max} and energy", 900, 700);
  //AAacceptance_a->SetGrid();
  //gStyle->SetPalette(kBird);
  //gStyle->SetOptStat(kFALSE);
  //AAacc->Draw("LEGO2z");

  //TCanvas *AAacceptance_b = new TCanvas("AAacceptance_b", "PADME #gamma#gamma acceptance as a function of R_{max} and energy", 900, 700);
  //AAacceptance_b->SetGrid();
  //gStyle->SetPalette(kBird);
  //gStyle->SetOptStat(kFALSE);
  //AAacc->Draw("colz");

  //TCanvas *SacceptanceRmin_a = new TCanvas("SacceptanceRmin_a", "PADME S-channel acceptance as a function of R_{min} and energy", 900, 700);
  //SacceptanceRmin_a->SetGrid();
  //gStyle->SetPalette(kBird);
  //gStyle->SetOptStat(kFALSE);
  //Sacc_Radmin->Draw("LEGO2z");

  TCanvas *SacceptanceRmin_b = new TCanvas("SacceptanceRmin_b", "PADME S-channel acceptance as a function of R_{min} and energy", 900, 700);
  SacceptanceRmin_b->SetGrid();
  gStyle->SetPalette(kBird);
  gStyle->SetOptStat(kFALSE);
  Sacc_Radmin->Draw("colz");

  //TCanvas *TacceptanceRmin_a = new TCanvas("TacceptanceRmin_a", "PADME T-channel acceptance as a function of R_{min} and energy", 900, 700);
  //TacceptanceRmin_a->SetGrid();
  //gStyle->SetPalette(kBird);
  //gStyle->SetOptStat(kFALSE);
  //Tacc_Radmin->Draw("LEGO2z");

  TCanvas *TacceptanceRmin_b = new TCanvas("TacceptanceRmin_b", "PADME T-channel acceptance as a function of R_{min} and energy", 900, 700);
  TacceptanceRmin_b->SetGrid();
  gStyle->SetPalette(kBird);
  gStyle->SetOptStat(kFALSE);
  Tacc_Radmin->Draw("colz");

  //TCanvas *AAacceptanceRmin_a = new TCanvas("AAacceptanceRmin_a", "PADME #gamma#gamma acceptance as a function of R_{min} and energy", 900, 700);
  //AAacceptanceRmin_a->SetGrid();
  //gStyle->SetPalette(kBird);
  //gStyle->SetOptStat(kFALSE);
  //AAacc_Radmin->Draw("LEGO2z");

  TCanvas *AAacceptanceRmin_b = new TCanvas("AAacceptanceRmin_b", "PADME #gamma#gamma acceptance as a function of R_{min} and energy", 900, 700);
  AAacceptanceRmin_b->SetGrid();
  gStyle->SetPalette(kBird);
  gStyle->SetOptStat(kFALSE);
  AAacc_Radmin->Draw("colz");

  //TCanvas *SacceptanceRad_a = new TCanvas("SacceptanceRad_a", "PADME S-channel acceptance as a function of R_{max} and R_{min}", 900, 700);
  //SacceptanceRad_a->SetGrid();
  //gStyle->SetPalette(kBird);
  //gStyle->SetOptStat(kFALSE);
  ///SaccRad->Draw("LEGO2z");

  //TCanvas *SacceptanceRad_b = new TCanvas("SacceptanceRad_b", "PADME S-channel acceptance as a function of R_{max} and R_{min}", 900, 700);
  //SacceptanceRad_b->SetGrid();
  //gStyle->SetPalette(kBird);
  //gStyle->SetOptStat(kFALSE);
  //SaccRad->Draw("colz");

  //TCanvas *TacceptanceRad_a = new TCanvas("TacceptanceRad_a", "PADME T-channel acceptance as a function of R_{max} and R_{min}", 900, 700);
  //TacceptanceRad_a->SetGrid();
  //gStyle->SetPalette(kBird);
  //gStyle->SetOptStat(kFALSE);
  //TaccRad->Draw("LEGO2z");

  //TCanvas *TacceptanceRad_b = new TCanvas("TacceptanceRad_b", "PADME T-channel acceptance as a function of R_{max} and R_{min}", 900, 700);
  //TacceptanceRad_b->SetGrid();
  //gStyle->SetPalette(kBird);
  //gStyle->SetOptStat(kFALSE);
  //TaccRad->Draw("colz");

  //TCanvas *AAacceptanceRad_a = new TCanvas("AAacceptanceRad_a", "PADME #gamma#gamma acceptance as a function of R_{max} and R_{min}", 900, 700);
  //AAacceptanceRad_a->SetGrid();
  //gStyle->SetPalette(kBird);
  //gStyle->SetOptStat(kFALSE);
  //AAaccRad->Draw("LEGO2z");

  //TCanvas *AAacceptanceRad_b = new TCanvas("AAacceptanceRad_b", "PADME #gamma#gamma acceptance as a function of R_{max} and R_{min}", 900, 700);
  //AAacceptanceRad_b->SetGrid();
  //gStyle->SetPalette(kBird);
  //gStyle->SetOptStat(kFALSE);
  //AAaccRad->Draw("colz");

  /*TCanvas *Acc1D_S = new TCanvas("Acc1D_S", "PADME S-Channel Acceptance as a function of R_{max} and E>30 MeV", 900, 700);
  Acc1D_S->SetGrid();
  gStyle->SetOptStat(kFALSE);
  ACC_S->Draw("HIST");

  TCanvas *Acc1D_T = new TCanvas("Acc1D_T", "PADME T-Channel Acceptance as a function of R_{max} and E>30 MeV", 900, 700);
  Acc1D_T->SetGrid();
  gStyle->SetOptStat(kFALSE);
  ACC_T->Draw("HIST");

  TCanvas *Acc1D_AA = new TCanvas("Acc1D_AA", "PADME #gamma#gamma-production Acceptance as a function of R_{max} and E>30 MeV", 900, 700);
  Acc1D_AA->SetGrid();
  gStyle->SetOptStat(kFALSE);
  ACC_AA->Draw("HIST");*/

  //TCanvas *Acc_rat1 = new TCanvas("Acc_rat1", "Acceptance ratio as a function of R_{max} and energy", 900, 700);
  //Acc_rat1->SetGrid();
  //gStyle->SetOptStat(kFALSE);
  //gStyle->SetPalette(kBird);
  //ACC_RATIO1->Draw("colz");

  TCanvas *Acc_rat2 = new TCanvas("Acc_rat2", "Acceptance ratio as a function of R_{min} and energy", 900, 700);
  Acc_rat2->SetGrid();
  gStyle->SetOptStat(kFALSE);
  gStyle->SetPalette(kBird);
  ACC_RATIO2->Draw("colz");

  //TCanvas *Acc_rat3 = new TCanvas("Acc_rat3", "Acceptance ratio as a function of R_{max} and R_{min}", 900, 700);
  //Acc_rat3->SetGrid();
  //gStyle->SetOptStat(kFALSE);
  //gStyle->SetPalette(kBird);
  //ACC_RATIO3->Draw("colz");

  /*TCanvas *Sens3D = new TCanvas("Sens3D", "PADME X17 Sensitivity as a function of N-PoT and epsilon", 900, 700);
  Sens3D->SetGrid();
  gStyle->SetOptStat(kFALSE);
  SENS->Draw("colz");

  TCanvas *CL90_1D = new TCanvas("CL90_1D", "PADME X17 90% Confidence level as a function of N-PoT", 900, 700);
  CL90_1D->SetGrid();
  gStyle->SetOptStat(kFALSE);
  CL90->Draw("HIST");

  TCanvas *exclusion_X17_1D = new TCanvas("exclusion_X17_1D", "Exclusion plot #epsilon_{X17}^{2} as a function of N-PoT", 900, 700);
  //exclusion_X17_1D->SetGrid();
  gStyle->SetOptStat(kFALSE);
  gPad->SetFrameFillColor(kGreen-10);
  exclusion_X17->Draw("HIST");
  NA64_limit->Draw("HIST SAME");
  gPad->SetLogy();*/

  
  //OUTPUT FILES
  /*C5->SaveAs("Occupancy.png");
  C6->SaveAs("geom_occupancy.png");
  C7->SaveAs("geom_energy_Occupancy.png");
  Acc3D_a->SaveAs("ACC_Scan_a.png");
  Acc3D_b->SaveAs("ACC_Scan_b.png");
  Acc1D_S->SaveAs("ACC_Schan_radius_Scan.png");
  Acc1D_T->SaveAs("ACC_Tchan_radius_Scan.png");
  Acc1D_AA->SaveAs("ACC_AA_radius_Scan.png");
  Sens3D->SaveAs("SensitivityX17.png");
  CL90_1D->SaveAs("CL90X17_stat.png");
  exclusion_X17_1D->SaveAs("gve_exclusion.png");*/
  }


