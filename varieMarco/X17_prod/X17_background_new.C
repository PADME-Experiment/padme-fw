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
  
  TFile *f1 = new TFile("/home/marco1/Documenti/background_SM/BHABHA_T_1e6.root");
  TFile *f2 = new TFile("/home/marco1/Documenti/background_SM/BHABHA_S_1e6.root");
  TFile *f3 = new TFile("/home/marco1/Documenti/background_SM/Ee_AA_1e6.root");

  TTree *t1 = (TTree*)f1->Get("t1");
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

  Double_t time = 86400; //seconds ->example = 1 day is equal to 86400 s
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
    N_X17_C[k][i] = PoT[k] * PreFactor_X17 * ((g_ve[i]*g_ve[i])/(9e-8)) * (1/0.7); //EQUATION 13 Proceedings LUC  <- il primo indice sono i POT, il secondo il valore del COUPLING g_ve
    }
  }

  
  ofstream file;
  file.open("X17_production_stat.txt");
  
  file<<"###################### X17 BACKGROUND ANALYSIS ############################"<<"\n";

  Double_t En_min[10] = {0., 0.03, 0.1, 0.11, 0.115, 0.12, 0.125, 0.13, 0.135, 0.14}; // fissata energia del secondo valore a 30 MeV - prima stava a 50
  Double_t Radius_min = 90.;
  Double_t Radius_max[21] = {180., 185., 190., 195., 200., 205., 210., 215., 220., 225., 230., 235., 240., 245., 250., 255., 260., 265., 270., 275., 280.};
  Double_t Radius_SPos[1000000], Radius_SElec[1000000], Radius_TPos[1000000], Radius_TElec[1000000], Radius_G1[1000000], Radius_G2[1000000];
  //Double_t NC_PADME_S[21][10], NC_PADME_T[21][10],NC_PADME_AA[21][10], NC_PADME_X17[21][10];
  Double_t NC_PADME_S[20], NC_PADME_T[20],NC_PADME_AA[20], NC_PADME_X17[20][10];
  Double_t Squared_gve[20];
  Double_t Scan_S[21][10], Scan_T[21][10], Scan_AA[21][10];
  Double_t S_ACC[21][10], T_ACC[21][10], AA_ACC[21][10];
  Double_t Sensitivity[20][10], LowerLimitX17[20], X17_produced[20];;
  
  for(Int_t l= 0; l<21; l++){
    for(Int_t j = 0; j<10; j++){
      Scan_S[l][j] = 0.;
      Scan_T[l][j] = 0.;
      Scan_AA[l][j] = 0.;
      S_ACC[l][j] = 0.;
      T_ACC[l][j] = 0.;
      AA_ACC[l][j] = 0.;
    }
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

   TH2F *histS_Pos_tot = new TH2F("histS_Pos_tot", "S-Channel positron transverse position", 1000, -450, 450, 1000, -450, 450);  //istogramma 2d per tutti gli eventi 
   TH2F *histS_Elec_tot = new TH2F("histS_Elec_tot", "S-Channel electron transverse position", 1000, -450, 450, 1000, -450, 450);  //istogramma 2d per tutti gli eventi 
   TH2F *histT_Pos_tot = new TH2F("histT_Pos_tot", "T-Channel positron transverse position", 1000, -450, 450, 1000, -450, 450);
   TH2F *histT_Elec_tot = new TH2F("histT_Elec_tot", "T-Channel electron transverse position", 1000, -450, 450, 1000, -450, 450);
   TH2F *histAA1_tot = new TH2F("histAA1_tot", "#gamma-1 transverse position", 1000, -450, 450, 1000, -450, 450);  //istogramma 2d con tagli geometrici e energetici
   TH2F *histAA2_tot = new TH2F("histAA2_tot", "#gamma-2 transverse position", 1000, -450, 450, 1000, -450, 450);
  
   TH2F *h9 = new TH2F("h9", "S-Channel positron R_{max}=262 mm", 1000, -300, 300, 10000, -300, 300);  //istogramma 2d per tutti gli eventi con soli tagli geometrici
   TH2F *h10 = new TH2F("h10", "S-Channel electron R_{max}=262 mm", 1000, -300, 300, 10000, -300, 300);

   TH2F *h11 = new TH2F("h11", "S-Channel positron R_{max}=262 mm and E>30 MeV", 1000, -300, 300, 10000, -300, 300);  //istogramma 2d con tagli geometrici e energetici
   TH2F *h12 = new TH2F("h12", "S-Channel electron R_{max}=262 mm and E>30 MeV ", 1000, -300, 300, 10000, -300, 300);

   TH2F *h13 = new TH2F("h13", "T-Channel positron R_{max}=262 mm", 1000, -300, 300, 10000, -300, 300);  //istogramma 2d per tutti gli eventi con soli tagli geometrici
   TH2F *h14 = new TH2F("h14", "T-Channel electron R_{max}=262 mm", 1000, -300, 300, 10000, -300, 300);

   TH2F *h15 = new TH2F("h15", "T-Channel positron R_{max}=262 mm and E>30 MeV", 1000, -300, 300, 10000, -300, 300);  //istogramma 2d con tagli geometrici e energetici
   TH2F *h16 = new TH2F("h16", "T-Channel electron R_{max}=262 mm and E>30 MeV ", 1000, -300, 300, 10000, -300, 300);

   TH2F *h17 = new TH2F("h17", "#gamma1 final position R_{max}=262 mm", 1000, -300, 300, 1000, -300, 300);  //istogramma 2d per tutti gli eventi con soli tagli geometrici
   TH2F *h18 = new TH2F("h18", "#gamma2 final position R_{max}=262 mm", 1000, -300, 300, 1000, -300, 300);

   TH2F *h19 = new TH2F("h19", "#gamma1 final position R_{max}=262 mm and E>30 MeV", 1000, -300, 300, 1000, -300, 300);  //istogramma 2d con tagli geometrici e energetici
   TH2F *h20 = new TH2F("h20", "#gamma2 final position R_{max}=262 mm and E>30 MeV", 1000, -300, 300, 1000, -300, 300);

   TH2F *SCAN2D = new TH2F("SCAN2D", "Scan Acceptance as a function of R_{max} and energy", 22, 172.5, 282.5, 15, -0.005, 0.145);
   TH1F *ACC_S = new TH1F("ACC_SD", "PADME S Channel Acceptance as a function of R_{max} and E > 30 MeV - CARBON TARGET", 16, 202.5, 282.5);
   TH1F *ACC_T = new TH1F("ACC_T", "PADME T Channel Acceptance as a function of R_{max} and E > 30 MeV - CARBON TARGET", 16, 202.5, 282.5);
   TH1F *ACC_AA = new TH1F("ACC_AA", "PADME #gamma#gamma-Production Acceptance as a function of R_{max} and E > 30 MeV - CARBON TARGET", 16, 202.5, 282.5);


   //ACC_S->SetTitle("PADME S Channel Acceptance as a function of R_{max} and E > 50 MeV - CARBON TARGET; R_{max} [mm]; Acceptance");
  //ACC_T->SetTitle("PADME T Channel Acceptance as a function of R_{max} and E > 50 MeV - CARBON TARGET; R_{max} [mm]; Acceptance");
  //ACC_AA->SetTitle("PADME #gamma#gamma-Production Acceptance as a function of R_{max} and E > 50 MeV - CARBON TARGET; R_{max} [mm]; Acceptance");
  
  for(Int_t l = 0; l<21; l++){
    //file<<"---------------------R_{max}= "<<Radius_max[l]<<" mm "<<"-------------\n";
   for(Int_t j = 0; j<10; j++){
     //file<<"--------------DATA E> "<<En_min[j]<<" GeV "<<"-----------\n";
    for(Int_t k = 0; k<numRow; k++){
           
      if(l == 19 && j == 1){
	histS_Pos_tot->Fill(Pos_Sx[k], Pos_Sy[k]);
	histS_Elec_tot->Fill(Elec_Sx[k], Elec_Sy[k]);
	histT_Pos_tot->Fill(Pos_Tx[k], Pos_Ty[k]);
	histT_Elec_tot->Fill(Elec_Tx[k], Elec_Ty[k]);
	histAA1_tot->Fill(G1_x[k], G1_y[k]);
	histAA2_tot->Fill(G2_x[k], G2_y[k]);
	
	if(Radius_SPos[k]>Radius_min && Radius_SElec[k]>Radius_min){
	  if(Radius_SPos[k]<Radius_max[l] && Radius_SElec[k]<Radius_max[l]){h9->Fill(Pos_Sx[k], Pos_Sy[k]); h10->Fill(Elec_Sx[k], Elec_Sy[k]);
	    if(SFinalEn_positron[k]>En_min[j] && SFinalEn_electron[k]>En_min[j]){h11->Fill(Pos_Sx[k], Pos_Sy[k]); h12->Fill(Elec_Sx[k], Elec_Sy[k]);
	      Scan_S[l][j]++;
	      SCAN2D->Fill(Radius_max[l], En_min[j]);};};};

	if(Radius_TPos[k]>Radius_min && Radius_TElec[k]>Radius_min){
	  if(Radius_TPos[k]<Radius_max[l] && Radius_TElec[k]<Radius_max[l]){h13->Fill(Pos_Tx[k], Pos_Ty[k]); h14->Fill(Elec_Tx[k], Elec_Ty[k]);
	    if(TFinalEn_positron[k]>En_min[j] && TFinalEn_electron[k]>En_min[j]){h15->Fill(Pos_Tx[k], Pos_Ty[k]); h16->Fill(Elec_Tx[k], Elec_Ty[k]);
	      Scan_T[l][j]++;
	      SCAN2D->Fill(Radius_max[l], En_min[j]);};};};

      
	if(Radius_G1[k]>Radius_min && Radius_G2[k]>Radius_min){
	  if(Radius_G1[k]<Radius_max[l] && Radius_G2[k]<Radius_max[l]){h17->Fill(G1_x[k], G1_y[k]); h18->Fill(G2_x[k], G2_y[k]);
	    if(FinalEn_G1[k]>En_min[j] && FinalEn_G2[k]>En_min[j]){h19->Fill(G1_x[k], G1_y[k]); h20->Fill(G2_x[k], G2_y[k]);
	      Scan_AA[l][j]++;};};};
      }
      else {

      if(Radius_SPos[k]>Radius_min && Radius_SElec[k]>Radius_min){
	if(Radius_SPos[k]<Radius_max[l] && Radius_SElec[k]<Radius_max[l]){
	  if(SFinalEn_positron[k]>En_min[j] && SFinalEn_electron[k]>En_min[j]){
	    Scan_S[l][j]++;
	    SCAN2D->Fill(Radius_max[l], En_min[j]);};};};
      
      if(Radius_TPos[k]>Radius_min && Radius_TElec[k]>Radius_min){
	if(Radius_TPos[k]<Radius_max[l] && Radius_TElec[k]<Radius_max[l]){
	  if(TFinalEn_positron[k]>En_min[j] && TFinalEn_electron[k]>En_min[j]){
	    Scan_T[l][j]++;
	    SCAN2D->Fill(Radius_max[l], En_min[j]);};};};

      if(Radius_G1[k]>Radius_min && Radius_G2[k]>Radius_min){
	  if(Radius_G1[k]<Radius_max[l] && Radius_G2[k]<Radius_max[l]){
	    if(FinalEn_G1[k]>En_min[j] && FinalEn_G2[k]>En_min[j]){
	      Scan_AA[l][j]++;};};};
      }
    }


    S_ACC[l][j] = Scan_S[l][j]/numRow;
    T_ACC[l][j] = Scan_T[l][j]/numRow;
    AA_ACC[l][j] = Scan_AA[l][j]/numRow;

    
    //NC_PADME_X17[l][j] = S_ACC[l][j] * N_X17_C;
   
    //file<<"SCAN S-channel ACCEPTANCE = "<<S_ACC[l][j]<<"\n";
    //file<<"SCAN T-channel ACCEPTANCE = "<<T_ACC[l][j]<<"\n";
    //file<<"SCAN Gamma-Gamma Production ACCEPTANCE = "<<AA_ACC[l][j]<<"\n\n";
    //file<<"N_tot S-channel : "<<NC_PADME_S[l][j]<<"\n";
    //file<<"N_tot T-channel : "<<NC_PADME_T[l][j]<<"\n";
    //file<<"N_tot Gamma-Gamma : "<<NC_PADME_AA[l][j]<<"\n";    
    //file<<"N_tot X17 : "<<NC_PADME_X17[l][j]<<"\n\n";
  
  }

   ACC_S->Fill(Radius_max[l], S_ACC[l][1]);
   ACC_T->Fill(Radius_max[l], T_ACC[l][1]);
   ACC_AA->Fill(Radius_max[l], AA_ACC[l][1]);
   
  }
  
  //running PoT adn coupling
  TH2F * SENS = new TH2F("SENS", "PADME X17 detected over sqrt(BG) as a function of N-PoT and #epsilon-X17", 20, 0.5e10, 2.05e11, 10, 0.5e-4, 1.05e-3);
  TH1F * CL90 = new TH1F("CL90", "PADME 90% CL as a function of N-PoT", 20, 0.5e10, 2.05e11);
  TH1F * exclusion_X17 = new TH1F("exclusion_X17", "g_{V_{e}}^{2} exclusion plot as a function of N-PoT", 20, 0.5e10, 2.05e11);

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
    Squared_gve[l]= (X17_produced[l] * 0.7)/(PoT[l]*PreFactor_X17)*(9e-8);
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
  }
  file.close();


  //produzione di X17 a partire da eq 42 PrecisionBeamDump Luc Darmè

  Double_t prod_X17[21][10000];
  Double_t Mass_X17[10000];
  Double_t E_res[10000];
  Double_t E_beam[21] = {260, 262, 264, 266, 268, 270, 272, 274, 276, 278, 280, 282, 284, 286, 288, 290, 292, 294, 296, 298, 300};
  TH1F *HistX17_res = new TH1F("HistX17_res", "N_{tot}^{X17} as a function of Resonant Mass = 282 MeV - eq 43 PrecisionBeamDump", 10000, 260, 300);
  TH1F *HistX17 = new TH1F("HistX17", "N_{tot}^{X17} as a function of Resonant Mass - eq 43 PrecisionBeamDump", 10000, 16, 18);

  for(int k = 0; k<10000; k++){
    E_res[k] = 260 + (k*0.004);
    Mass_X17[k] = TMath::Sqrt(2*E_res[k]*0.511);
  }
  for(int l = 0; l<21; l++){
    
    for(int k = 0; k<10000; k++){
      prod_X17[l][k] = PoT[9] * PreFactor_X17 * ((g_ve[4]*g_ve[4])/(9e-8)) * ((Na * Z_C * D_C)/(A_C * 1e24)) * (1/0.7) * TMath::Gaus(E_res[k], E_beam[l], 0.7);
      //if(TMath::IsNaN(prod_X17[k])){prod_X17[k] = 1;}
      if(l==11){HistX17->Fill(Mass_X17[k], prod_X17[l][k]); HistX17_res->Fill(Mass_X17[k], prod_X17[l][k]);}
      HistX17->Fill(Mass_X17[k], prod_X17[l][k]);
    }
  }
  

  //DEFINITION OF HISTOGRAMS
  TH1F *h1 = new TH1F ("h1", "Positron 3P-distribution in S-Channel", 10000,-0.05, 0.30);
  TH1F *h2 = new TH1F ("h2", "Electron 3P-distribution in S-Channel", 10000,-0.05, 0.30);
  TH1F *h3 = new TH1F ("h3", "Positron 3P-distribution in T-Channel", 10000,-0.05, 0.30);
  TH1F *h4 = new TH1F ("h4", "Electron 3P-distribution in T-Channel", 10000,-0.05, 0.30);
  TH1F *h1g = new TH1F ("h1g", "#gamma-1 3P-distribution", 1000,-0.05, 0.30);
  TH1F *h2g = new TH1F ("h2g", "#gamma-2 3P-distribution", 1000,-0.05, 0.30);

  TH1F *h5 = new TH1F ("h5", "Positron energy distribution in S-Channel", 10000,-0.05, 0.30);
  TH1F *h6 = new TH1F ("h6", "Electron energy distribution in S-Channel", 10000,-0.05, 0.30);
  TH1F *h7 = new TH1F ("h7", "Positron energy distribution in T-Channel", 10000,-0.05, 0.30);
  TH1F *h8 = new TH1F ("h8", "Electron energy distribution in T-Channel", 10000,-0.05, 0.30);
  TH1F *h3g = new TH1F ("h3g", "#gamma-1 energy distribution", 1000,-0.05, 0.30);
  TH1F *h4g = new TH1F ("h4g", "#gamma-2 energy distribution", 1000,-0.05, 0.30);
  
  TH1F *check1 = new TH1F("check1", "Final total energy S-Channel", 1000, 0.27, 0.29); 
  TH1F *check2 = new TH1F("check2", "Final total energy T-Channel", 1000, 0.27, 0.29); 
  TH1F *checkg = new TH1F("checkg", "Final energy for #gamma#gamma-Production", 1000, 0.27, 0.29);

  TH1F *htheta1 = new TH1F("htheta1", "S-Channel Angular distribution", 1000, -0.1, 1); 
  TH1F *htheta2 = new TH1F("htheta2", "T-Channel Angular distribution", 1000, -0.1, 1);
  TH1F *hthetag = new TH1F("hthetag", "Angular distribution #gamma#gamma-Production", 1000, -0.1, 1);
  
  
  for(Int_t k=0; k<numRow; k++){
    h1->Fill(SFinal_PosMomentum[k]); //distribuzione del 3-impulso finale - positroni S
    h2->Fill(SFinal_ElecMomentum[k]); //distribuzione del 3-impulso finale - elettroni S
    h3->Fill(TFinal_PosMomentum[k]); //distribuzione del 3-impulso finale - positroni T
    h4->Fill(TFinal_ElecMomentum[k]); //distribuzione del 3-impulso finale - elettroni T
    h5->Fill(SFinalEn_positron[k]); //distribuzione energia finale - positroni S
    h6->Fill(SFinalEn_electron[k]); //distribuzione energia finale - elettroni S
    h7->Fill(TFinalEn_positron[k]); //distribuzione energia finale - positroni T
    h8->Fill(TFinalEn_electron[k]); //distribuzione energia finale - elettroni T
    check1->Fill(SE_tot[k]); //conservazione dell'energia CHECK
    check2->Fill(TE_tot[k]);
    htheta1->Fill(Stheta_scattering[k]); //distribuzione angolare finale positrone-elettrone S
    htheta2->Fill(Ttheta_scattering[k]); //distribuzione angolare finale positrone-elettrone T
    h1g->Fill(Final_G1Momentum[k]); //distribuzione del 3-impulso finale del primo fotone
    h2g->Fill(Final_G2Momentum[k]); //distribuzione del 3-impulso finale del secondo fotone
    h3g->Fill(FinalEn_G1[k]); //distribuzione energia finale - primo fotone
    h4g->Fill(FinalEn_G2[k]); //distribuzione energia finale - secondo fotone
    checkg->Fill(GGE_tot[k]); //conservazione dell'energia CHECK
    hthetag->Fill(AAtheta_scattering[k]); //distribuzione angolare finale fotone1-fotone2
   }
  

  
  h1->SetXTitle("3-P [GeV]");
  h2->SetXTitle("3-P [GeV]");
  h3->SetXTitle("3-P [GeV]");
  h4->SetXTitle("3-P [GeV]");
  h5->SetXTitle("E [GeV]");
  h6->SetXTitle("E [GeV]");
  h7->SetXTitle("E [GeV]");
  h8->SetXTitle("E [GeV]");
  htheta1->SetXTitle("theta [rad]");
  htheta2->SetXTitle("theta [rad]");
  h1g->SetXTitle("3-P [GeV]");
  h2g->SetXTitle("3-P [GeV]");
  h3g->SetXTitle("E [GeV]");
  h4g->SetXTitle("E [GeV]");
  hthetag->SetXTitle("theta [rad]");
  
  h9->SetXTitle("y [mm]"); h9->SetYTitle("x [mm]");
  h10->SetXTitle("y [mm]"); h10->SetYTitle("x [mm]");
  h11->SetXTitle("y [mm]"); h11->SetYTitle("x [mm]");
  h12->SetXTitle("y [mm]"); h12->SetYTitle("x [mm]");
  h13->SetXTitle("y [mm]"); h13->SetYTitle("x [mm]");
  h14->SetXTitle("y [mm]"); h14->SetYTitle("x [mm]");
  h15->SetXTitle("y [mm]"); h15->SetYTitle("x [mm]");
  h16->SetXTitle("y [mm]"); h16->SetYTitle("x [mm]");
  h17->SetXTitle("y [mm]"); h17->SetYTitle("x [mm]");
  h18->SetXTitle("y [mm]"); h18->SetYTitle("x [mm]");
  h19->SetXTitle("y [mm]"); h19->SetYTitle("x [mm]");
  h20->SetXTitle("y [mm]"); h20->SetYTitle("x [mm]");

  SCAN2D->SetXTitle("R_{max} [mm]"); SCAN2D->SetYTitle("E_{min} [GeV]");
  ACC_S->SetXTitle("R_{max} [mm]"); ACC_S->SetYTitle("Acceptance");
  ACC_T->SetXTitle("R_{max} [mm]"); ACC_T->SetYTitle("Acceptance");
  ACC_AA->SetXTitle("R_{max} [mm]"); ACC_AA->SetYTitle("Acceptance");
  SENS->SetXTitle("N-PoT"); SENS->SetYTitle("#epsilon");
  CL90->SetXTitle("N-PoT"); CL90->SetYTitle("X_{17}^{min}");
  exclusion_X17->SetXTitle("N-PoT"); exclusion_X17->SetYTitle("g_{V_{e}}^{2}");
  NA64_limit->SetXTitle("N-PoT"); NA64_limit->SetYTitle("g_{V_{e}}^{2}");
  HistX17->SetXTitle("M_{X17} [MeV]"); HistX17->SetYTitle("N_{X17}^{tot}(NPoT=1e10)");
  HistX17_res->SetXTitle("M_{X17} [MeV]"); HistX17_res->SetYTitle("N_{X17}^{tot}(NPoT=1e10)");
  
  
  h1->Scale(1./(h1->Integral()));
  h2->Scale(1./(h2->Integral()));
  h3->Scale(1./(h3->Integral()));
  h4->Scale(1./(h4->Integral()));
  h5->Scale(1./(h5->Integral()));
  h6->Scale(1./(h6->Integral()));
  h7->Scale(1./(h7->Integral()));
  h8->Scale(1./(h8->Integral()));
  htheta1->Scale(1./(htheta1->Integral()));
  htheta2->Scale(1./(htheta2->Integral()));
  h1g->Scale(1./(h1g->Integral()));
  h2g->Scale(1./(h2g->Integral()));
  h3g->Scale(1./(h3g->Integral()));
  h4g->Scale(1./(h4g->Integral()));
  hthetag->Scale(1./(hthetag->Integral()));
  h9->Scale(1./(h9->Integral()));
  h10->Scale(1./(h10->Integral()));
  h11->Scale(1./(h11->Integral()));
  h12->Scale(1./(h12->Integral()));
  h13->Scale(1./(h13->Integral()));
  h14->Scale(1./(h14->Integral()));
  h15->Scale(1./(h15->Integral()));
  h16->Scale(1./(h16->Integral()));
  h17->Scale(1./(h17->Integral()));
  h18->Scale(1./(h18->Integral()));
  h19->Scale(1./(h19->Integral()));
  h20->Scale(1./(h20->Integral()));
   

  h1->SetMaximum(1); h1->SetMinimum(1e-7);
  h2->SetMaximum(1); h2->SetMinimum(1e-7);
  h3->SetMaximum(1); h3->SetMinimum(1e-7);
  h4->SetMaximum(1); h4->SetMinimum(1e-7);
  h5->SetMaximum(1); h5->SetMinimum(1e-7);
  h6->SetMaximum(1); h6->SetMinimum(1e-7);
  h7->SetMaximum(1); h7->SetMinimum(1e-7);
  h8->SetMaximum(1); h8->SetMinimum(1e-7);
  htheta1->SetMaximum(1); htheta1->SetMinimum(1e-7);
  htheta2->SetMaximum(1); htheta2->SetMinimum(1e-7);
  h1g->SetMaximum(1); h1g->SetMinimum(1e-7);
  h2g->SetMaximum(1); h2g->SetMinimum(1e-7);
  h3g->SetMaximum(1); h3g->SetMinimum(1e-7);
  h4g->SetMaximum(1); h4g->SetMinimum(1e-7);
  hthetag->SetMaximum(1); hthetag->SetMinimum(1e-7);
  exclusion_X17->SetMaximum(5e-8); exclusion_X17->SetMinimum(5e-9);
  NA64_limit->SetMaximum(5e-8); NA64_limit->SetMinimum(5e-9);
  HistX17->SetMaximum(1e6); HistX17->SetMinimum(1);
  HistX17_res->SetMaximum(1e6); HistX17_res->SetMinimum(1);


  h1->SetLineColor(kRed);
  h2->SetLineColor(kRed+3); 
  h3->SetLineColor(kGreen);
  h4->SetLineColor(kGreen+2);
  h5->SetLineColor(kRed); 
  h6->SetLineColor(kRed+3);
  h7->SetLineColor(kGreen); 
  h8->SetLineColor(kGreen+2); 
  h1->SetFillColor(kRed); h1->SetFillStyle(3003);
  h2->SetFillColor(kRed+3); h2->SetFillStyle(3003);
  h3->SetFillColor(kGreen); h3->SetFillStyle(3003);
  h4->SetFillColor(kGreen+2); h4->SetFillStyle(3003);
  h5->SetFillColor(kRed); h5->SetFillStyle(3003);
  h6->SetFillColor(kRed+3); h6->SetFillStyle(3003);
  h7->SetFillColor(kGreen); h7->SetFillStyle(3003);
  h8->SetFillColor(kGreen+2); h8->SetFillStyle(3003);
  check1->SetLineColor(kRed+4);
  check2->SetLineColor(kGreen+4);
  htheta1->SetLineColor(kRed+4);
  htheta2->SetLineColor(kGreen+4);
  h1g->SetLineColor(kCyan); 
  h2g->SetLineColor(kCyan+4); 
  h3g->SetLineColor(kCyan); 
  h4g->SetLineColor(kCyan+4);
  h1g->SetFillColor(kCyan); h1g->SetFillStyle(3003);
  h2g->SetFillColor(kCyan+4); h2g->SetFillStyle(3003);
  h3g->SetFillColor(kCyan); h3g->SetFillStyle(3003);
  h4g->SetFillColor(kCyan+4); h4g->SetFillStyle(3003);
  checkg->SetLineColor(kCyan+4);
  hthetag->SetLineColor(kCyan+4);
  ACC_S->SetFillColor(kRed);
  ACC_T->SetFillColor(kGreen);
  ACC_AA->SetFillColor(kCyan);
  CL90->SetFillColor(kOrange+7);
  exclusion_X17->SetLineColor(kGreen-10); exclusion_X17->SetFillColor(10); exclusion_X17->SetFillStyle(1001); 
  NA64_limit->SetLineColor(kRed);
  HistX17_res->SetLineColor(kOrange+7);
  
  h9->SetMarkerStyle(6); h9->SetMarkerColor(kRed-4);
  h10->SetMarkerStyle(6); h10->SetMarkerColor(kGreen-1);
  h11->SetMarkerStyle(6); h11->SetMarkerColor(kRed-4);
  h12->SetMarkerStyle(6); h12->SetMarkerColor(kGreen-1);
  h13->SetMarkerStyle(6); h13->SetMarkerColor(kRed-4);
  h14->SetMarkerStyle(6); h14->SetMarkerColor(kGreen-1);
  h15->SetMarkerStyle(6); h15->SetMarkerColor(kRed-4);
  h16->SetMarkerStyle(6); h16->SetMarkerColor(kGreen-1);
  h17->SetMarkerStyle(6); h17->SetMarkerColor(kRed-4);
  h18->SetMarkerStyle(6); h18->SetMarkerColor(kGreen-1);
  h19->SetMarkerStyle(6); h19->SetMarkerColor(kRed-4);
  h20->SetMarkerStyle(6); h20->SetMarkerColor(kGreen-1);
  HistX17->SetMarkerStyle(6); HistX17->SetMarkerColor(kOrange+7);
   
  
  
  TCanvas *C1 = new TCanvas("C1", "3-Momentum BhaBha Scattering->S-Channel distribution", 600, 700);
  C1->Divide(1,2);
  C1->cd(1);
  h1->Draw("HIST");
  gPad->SetLogy();
  C1->cd(2);
  h2->Draw("HIST");
  gPad->SetLogy();

  TCanvas *C2 = new TCanvas("C2", "3-Momentum BhaBha Scattering-> T-Channel distribution", 600, 700);
  C2->Divide(1,2);
  C2->cd(1);
  h3->Draw("HIST");
  gPad->SetLogy();
  C2->cd(2);
  h4->Draw("HIST");
  gPad->SetLogy();

  TCanvas *C1g = new TCanvas("C1g", "3-Momentum #gamma#gamma distribution", 600, 700);
  C1g->Divide(1,2);
  C1g->cd(1);
  h1g->Draw("HIST");
  gPad->SetLogy();
  C1g->cd(2);
  h2g->Draw("HIST");
  gPad->SetLogy();

  TCanvas *C3P = new TCanvas("C3P", "Final 3-Momentum Positron S Channel, T Channel and Gamma", 900, 700);
  h1->Draw("HIST");
  h3->Draw("same HIST");
  h1g->Draw("same HIST");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  gPad->SetLogy();

  TCanvas *CHECK1 = new TCanvas("CHECK1", "Energy conservation check", 1200, 700);
  CHECK1->Divide(3,1);
  CHECK1->cd(1);
  check1->Draw("HIST");
  CHECK1->cd(2);
  check2->Draw("HIST");
  CHECK1->cd(3);
  checkg->Draw("HIST");

  TCanvas *Ctheta = new TCanvas("Ctheta", "Angle between final particles in CoM fram", 1200, 700);
  Ctheta->Divide(3,1);
  Ctheta->cd(1);
  htheta1->Draw("HIST");
  gPad->SetLogy();
  Ctheta->cd(2);
  htheta2->Draw("HIST");
  gPad->SetLogy();
  Ctheta->cd(3);
  hthetag->Draw("HIST");
  gPad->SetLogy();

  TCanvas *C3 = new TCanvas("C3", "Energy BhaBha Scattering-> S-Channel distribution", 600, 700);
  C3->Divide(1,2);
  C3->cd(1);
  h5->Draw("HIST");
  gPad->SetLogy();
  C3->cd(2);
  h6->Draw("HIST");
  gPad->SetLogy();

  TCanvas *C4 = new TCanvas("C4", "Energy BhaBha Scattering->T-Channel distribution", 600, 700);
  C4->Divide(1,2);
  C4->cd(1);
  h7->Draw("HIST");
  gPad->SetLogy();
  C4->cd(2);
  h8->Draw("HIST");
  gPad->SetLogy();

  TCanvas *C2g = new TCanvas("C2g", "Energy #gamma#gamma distribution", 600, 700);
  C2g->Divide(1,2);
  C2g->cd(1);
  h3g->Draw("HIST");
  gPad->SetLogy();
  C2g->cd(2);
  h4g->Draw("HIST");
  gPad->SetLogy();

  TCanvas *CEn = new TCanvas("CEn", "Final Energy Positron S Channe, T Channel and Gamma", 900, 700);
  h5->Draw();
  h7->Draw("same HIST");
  h3g->Draw("same HIST");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  gPad->SetLogy();

  TCanvas *C5 = new TCanvas("C5", "Final transverse position at 3500 mm from the target", 900, 700);
  C5->Divide(3,2);
  C5->cd(1);
  histS_Pos_tot->Draw("colz");
  C5->cd(2);
  histT_Pos_tot->Draw("colz");
  C5->cd(3);
  histAA1_tot->Draw("colz");
  C5->cd(4);
  histS_Elec_tot->Draw("colz");
  C5->cd(5);
  histT_Elec_tot->Draw("colz");
  C5->cd(6);
  histAA2_tot->Draw("colz");

  TCanvas *C6 = new TCanvas("C6", "Final transverse position at 3500 mm from the target - R_{max} = 262 mm", 900, 700);
  C6->Divide(3,2);
  C6->cd(1);
  h9->Draw("colz");
  C6->cd(2);
  h13->Draw("colz");
  C6->cd(3);
  h17->Draw("colz");
  C6->cd(4);
  h10->Draw("colz");
  C6->cd(5);
  h14->Draw("colz");
  C6->cd(6);
  h18->Draw("colz");

  TCanvas *C7 = new TCanvas("C7", "Final transverse position at 3500 mm from the target - R_{max} = 262 mm and E> 30 MeV", 900, 700);
  C7->Divide(3,2);
  C7->cd(1);
  h11->Draw("colz");
  C7->cd(2);
  h15->Draw("colz");
  C7->cd(3);
  h19->Draw("colz");
  C7->cd(4);
  h12->Draw("colz");
  C7->cd(5);
  h16->Draw("colz");
  C7->cd(6);
  h20->Draw("colz");
    
  TCanvas *Acc3D_a = new TCanvas("Acc3D_a", "PADME Acceptance as a function of R_{max} and energy", 900, 700);
  Acc3D_a->SetGrid();
  gStyle->SetPalette(kBird);
  SCAN2D->Draw("LEGO2z");

  TCanvas *Acc3D_b = new TCanvas("Acc3D_b", "PADME Acceptance as a function of R_{max} and energy", 900, 700);
  Acc3D_b->SetGrid();
  gStyle->SetPalette(kBird);
  SCAN2D->Draw("colz");

  TCanvas *Acc1D_S = new TCanvas("Acc1D_S", "PADME S-Channel Acceptance as a function of R_{max} and E>30 MeV", 900, 700);
  Acc1D_S->SetGrid();
  ACC_S->Draw("HIST");

  TCanvas *Acc1D_T = new TCanvas("Acc1D_T", "PADME T-Channel Acceptance as a function of R_{max} and E>30 MeV", 900, 700);
  Acc1D_T->SetGrid();
  ACC_T->Draw("HIST");

  TCanvas *Acc1D_AA = new TCanvas("Acc1D_AA", "PADME #gamma#gamma-production Acceptance as a function of R_{max} and E>30 MeV", 900, 700);
  Acc1D_AA->SetGrid();
  ACC_AA->Draw("HIST");

  TCanvas *Sens3D = new TCanvas("Sens3D", "PADME X17 Sensitivity as a function of N-PoT and epsilon", 900, 700);
  Sens3D->SetGrid();
  SENS->Draw("colz");

  TCanvas *CL90_1D = new TCanvas("CL90_1D", "PADME X17 90% Confidence level as a function of N-PoT", 900, 700);
  CL90_1D->SetGrid();
  CL90->Draw("HIST");

  TCanvas *exclusion_X17_1D = new TCanvas("exclusion_X17_1D", "Exclusion plot #epsilon_{X17}^{2} as a function of N-PoT", 900, 700);
  //exclusion_X17_1D->SetGrid();
  gPad->SetFrameFillColor(kGreen-10);
  exclusion_X17->Draw("HIST");
  NA64_limit->Draw("HIST SAME");
  gPad->SetLogy();

  TCanvas *X17_Canvas = new TCanvas("X17_Canvas", "X17 production at PADME - S-Channel Acceptance", 900, 700);
  X17_Canvas->SetGrid();
  HistX17->Draw("HIST");
  gPad->SetLogy();

  TCanvas *X17res_Canvas = new TCanvas("X17res_Canvas", "X17 production at PADME at resonance M=282MeV - S-Channel Acceptance", 900, 700);
  X17_Canvas->SetGrid();
  HistX17_res->Draw("HIST");
  gPad->SetLogy();
  
  //OUTPUT FILES
  //C1->SaveAs("S_Final3P.png");
  //C2->SaveAs("T_Final3P.png");
  //C1g->SaveAs("AA_Final3P.png");
  //C3P->SaveAs("Final3p_distribution.png");
  //CHECK1->SaveAs("Energy_Conservation_Check.png");
  //Ctheta->SaveAs("Theta_FinalParticles.png");
  //C3->SaveAs("S_FinalEnergy.png");
  //C4->SaveAs("T_FinalEnergy.png");
  //C2g->SaveAs("AA_FinalEnergy.png");
  C5->SaveAs("Occupancy.png");
  C6->SaveAs("geom_occupancy.png");
  C7->SaveAs("geom_energy_Occupancy.png");
  Acc3D_a->SaveAs("ACC_Scan_a.png");
  Acc3D_b->SaveAs("ACC_Scan_b.png");
  Acc1D_S->SaveAs("ACC_Schan_radius_Scan.png");
  Acc1D_T->SaveAs("ACC_Tchan_radius_Scan.png");
  Acc1D_AA->SaveAs("ACC_AA_radius_Scan.png");
  Sens3D->SaveAs("SensitivityX17.png");
  CL90_1D->SaveAs("CL90X17_stat.png");
  exclusion_X17_1D->SaveAs("gve_exclusion.png");
  X17_Canvas->SaveAs("X17_production_mass.png");
  }


