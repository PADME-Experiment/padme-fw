//MANCINI MARCO 06-05-2022 X20 production
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
#include <TFile.h>
#include <TMath.h>
#include <TTree.h>

using namespace std; 
// void X20_fullBG_20230213(){

  static const int Nev_CalcHep = 1000000;
  
  Int_t numCol    = 11;
  Double_t numRow = 1000000;
  Double_t Positron_InitialP      = 0.43;
  Double_t Electron_initialEnergy = 0.;
  Double_t ElectronMass           = 0.000511;
  Double_t GammaMass              = 0.;

  Double_t InitialEnergy_positron[1000000];
  Double_t SFinalEn_positron[1000000], SFinalEn_electron[1000000];
  Double_t SFinal_PosMomentum[1000000], SFinal_ElecMomentum[1000000];
  Double_t SE_tot[1000000], TE_tot[1000000];
  Double_t Stheta_scattering[1000000], Ttheta_scattering[1000000];
  Double_t Stheta_test[1000000], Ttheta_test[1000000];

  Double_t FinalEn_Fp[1000000], FinalEn_Fe[1000000];
  Double_t Final_FpMomentum[1000000], Final_FeMomentum[1000000];
  Double_t FullE_tot[1000000];
  Double_t Fulltheta_scattering[1000000];
  Double_t Fulltheta_test[1000000];

  Double_t theta_test[1000000];
  
  Double_t Pos_Stheta_x[1000000], Pos_Sphi_y[1000000]; //angoli di deviazione dei prodotti di reazione rispetto alla direzione x e y
  Double_t Elec_Stheta_x[1000000], Elec_Sphi_y[1000000];
  Double_t Fp_theta_x[1000000], Fp_phi_y[1000000], Fe_theta_x[1000000], Fe_phi_y[1000000];
  Double_t TargetCalo = 3500; //in mm 20230213 DeltaZ target-ECal  
  //Double_t semiL = 50; //lato mezzi del quadrato centrale
  //Double_t geometry_radius = 300; //raggio geometrico del calorimetro

  Double_t Pos_Sx[1000000], Pos_Sy[1000000], Elec_Sx[1000000], Elec_Sy[1000000]; //coordinate sul calorimentro nel piano trasverso al fascio xy
  Double_t Fp_x[1000000], Fp_y[1000000], Fe_x[1000000], Fe_y[1000000];
  
  TFile *fBhabhaSCh  = new TFile("/home/marco/analysis/SM_analysis/CalcHepFiles/Schannel/bhabhaSch_430MeV.root"); 
  TFile *fBhabhaFull = new TFile("/home/marco/analysis/SM_analysis/CalcHepFiles/BhabhaFull/bhabhaFull_430MeV_thetaSel.root"); 

  TTree *tBhabhaSCh  = (TTree*)fBhabhaSCh->Get("tbhabhaSch_430MeV"); 
  TTree *tBhabhaFull = (TTree*)fBhabhaFull->Get("tbhabhaFull_430MeV"); 
  Double_t InitialP3_positron, InitialP3_electron;
  Double_t SFinalP1_electron, SFinalP2_electron, SFinalP3_electron, SFinalP1_positron, SFinalP2_positron, SFinalP3_positron;
  Double_t FinalP1_Fp, FinalP2_Fp, FinalP3_Fp, FinalP1_Fe, FinalP2_Fe, FinalP3_Fe;

  tBhabhaSCh->SetBranchAddress("InitialP3_positron", &InitialP3_positron);
  tBhabhaSCh->SetBranchAddress("InitialP3_electron", &InitialP3_electron);
  tBhabhaSCh->SetBranchAddress("SFinalP1_electron", &SFinalP1_electron);
  tBhabhaSCh->SetBranchAddress("SFinalP2_electron", &SFinalP2_electron);
  tBhabhaSCh->SetBranchAddress("SFinalP3_electron", &SFinalP3_electron);
  tBhabhaSCh->SetBranchAddress("SFinalP1_positron", &SFinalP1_positron);
  tBhabhaSCh->SetBranchAddress("SFinalP2_positron", &SFinalP2_positron);
  tBhabhaSCh->SetBranchAddress("SFinalP3_positron", &SFinalP3_positron);
  tBhabhaFull->SetBranchAddress("FinalP1_Fe", &FinalP1_Fe);
  tBhabhaFull->SetBranchAddress("FinalP2_Fe", &FinalP2_Fe);
  tBhabhaFull->SetBranchAddress("FinalP3_Fe", &FinalP3_Fe);
  tBhabhaFull->SetBranchAddress("FinalP1_Fp", &FinalP1_Fp);
  tBhabhaFull->SetBranchAddress("FinalP2_Fp", &FinalP2_Fp);
  tBhabhaFull->SetBranchAddress("FinalP3_Fp", &FinalP3_Fp);
  
  Long64_t nentries = tBhabhaFull->GetEntries();
  cout<<nentries<<endl;
  
  for(Long64_t k=0; k<nentries; k++){
    tBhabhaSCh->GetEntry(k);
    tBhabhaFull->GetEntry(k);

    //INITIAL STATE -> S CHANNEL and bhabhaFull HAVE SAME INITIAL STATE
    InitialEnergy_positron[k] = TMath::Sqrt(ElectronMass*ElectronMass + InitialP3_positron*InitialP3_positron);

    //FINAL STATE 3MOMENTUM DISTRIBUTION - T, S CHANNEL and Ee->AA
    SFinal_PosMomentum[k]     = TMath::Sqrt(SFinalP1_positron*SFinalP1_positron + SFinalP2_positron*SFinalP2_positron + SFinalP3_positron*SFinalP3_positron);
    if(TMath::IsNaN(SFinal_PosMomentum[k])){ SFinal_PosMomentum[k] = -0.025;}

    SFinal_ElecMomentum[k]    = TMath::Sqrt(SFinalP1_electron*SFinalP1_electron + SFinalP2_electron*SFinalP2_electron + SFinalP3_electron*SFinalP3_electron);
    if(TMath::IsNaN(SFinal_ElecMomentum[k])) {SFinal_ElecMomentum[k] = -0.025;}

    Final_FpMomentum[k]       = TMath::Sqrt(FinalP1_Fp*FinalP1_Fp + FinalP2_Fp*FinalP2_Fp + FinalP3_Fp*FinalP3_Fp);
    if(TMath::IsNaN(Final_FpMomentum[k])) { Final_FpMomentum[k] = -0.025;}

    Final_FeMomentum[k]       = TMath::Sqrt(FinalP1_Fe*FinalP1_Fe + FinalP2_Fe*FinalP2_Fe + FinalP3_Fe*FinalP3_Fe);
    if(TMath::IsNaN(Final_FeMomentum[k])) { Final_FeMomentum[k] = -0.025;}

    ////////////////// THE OUTPUT HAS NOT NaN!! //////////////////////////

    //FINAL STATE ENERGY DISTRIBUTION - S,T CHANNEL and Ee->AA
    SFinalEn_positron[k] = TMath::Sqrt(ElectronMass*ElectronMass + SFinal_PosMomentum[k]*SFinal_PosMomentum[k]);
    SFinalEn_electron[k] = TMath::Sqrt(ElectronMass*ElectronMass + SFinal_ElecMomentum[k]*SFinal_ElecMomentum[k]);
    FinalEn_Fp[k]        = TMath::Sqrt(ElectronMass*ElectronMass + Final_FpMomentum[k]*Final_FpMomentum[k]);
    FinalEn_Fe[k]        = TMath::Sqrt(ElectronMass*ElectronMass + Final_FeMomentum[k]*Final_FeMomentum[k]);

    //ENERGY CONSERVATION CHECK
    SE_tot[k]    = SFinalEn_positron[k] + SFinalEn_electron[k];
    FullE_tot[k] = FinalEn_Fp[k] + FinalEn_Fe[k];

    
    //FINAL STATE ANGULAR DISTRIBUTION BETWEEN FINAL PARTICLES - S,T CHANNEL and Ee->AA
    Stheta_scattering[k]    = TMath::ACos((SFinalEn_positron[k]*SFinalEn_electron[k] - ElectronMass*InitialEnergy_positron[k]) /(SFinal_PosMomentum[k]*SFinal_ElecMomentum[k]));
    if(TMath::IsNaN(Stheta_scattering[k])) {Stheta_scattering[k] = -0.05;}

    Fulltheta_scattering[k] = TMath::ACos((FinalEn_Fp[k]*FinalEn_Fe[k] - ElectronMass*InitialEnergy_positron[k] - ElectronMass*ElectronMass) / (Final_FpMomentum[k]*Final_FeMomentum[k]));
    if(TMath::IsNaN(Fulltheta_scattering[k])) {Fulltheta_scattering[k] = -0.05;}

    //TRANSVERSE FINAL POSITION AT 3000 MM FROM TARGET - THETA PHI DEF
    Pos_Stheta_x[k]  = TMath::ATan(SFinalP1_positron/SFinalP3_positron);
    Pos_Sphi_y[k]    = TMath::ATan(SFinalP2_positron/SFinalP3_positron);
    Elec_Stheta_x[k] = TMath::ATan(SFinalP1_electron/SFinalP3_electron);
    Elec_Sphi_y[k]   = TMath::ATan(SFinalP2_electron/SFinalP3_electron);

    Fp_theta_x[k] = TMath::ATan(FinalP1_Fp/FinalP3_Fp);
    Fp_phi_y[k]   = TMath::ATan(FinalP2_Fp/FinalP3_Fp);
    Fe_theta_x[k] = TMath::ATan(FinalP1_Fe/FinalP3_Fe);
    Fe_phi_y[k]   = TMath::ATan(FinalP2_Fe/FinalP3_Fe);

    //POSITION
    Pos_Sx[k]  = TMath::Tan(Pos_Stheta_x[k]) * TargetCalo;
    Pos_Sy[k]  = TMath::Tan(Pos_Sphi_y[k]) * TargetCalo;
    Elec_Sx[k] = TMath::Tan(Elec_Stheta_x[k]) * TargetCalo;
    Elec_Sy[k] = TMath::Tan(Elec_Sphi_y[k]) * TargetCalo;

    Fp_x[k] = TMath::Tan(Fp_theta_x[k]) * TargetCalo;
    Fp_y[k] = TMath::Tan(Fp_phi_y[k]) * TargetCalo;
    Fe_x[k] = TMath::Tan(Fe_theta_x[k]) * TargetCalo;
    Fe_y[k] = TMath::Tan(Fe_phi_y[k]) * TargetCalo; 
    
  }
//Double_t time = 86400; //seconds -> example = 1 day is equal to 86400 s
  // Double_t PoT[20] = {1e10, 2e10, 3e10, 4e10, 5e10, 6e10, 7e10, 8e10, 9e10, 10e10, 11e10, 12e10, 13e10, 14e10, 15e10, 16e10, 17e10, 18e10, 19e10, 2e11};
  Double_t PoT = 1e10;
  Double_t Na  = 6.022e23;
  Double_t Z_C = 6.;
  Double_t D_C = 3.5;
  Double_t A_C = 12.;
  Double_t TThickness = 0.01; //in cm
  Double_t CmtoPb = 1e-36;

  Double_t PreFactor_X20 = 3.8e-7; //DATAS FROM PROCEEDINGS DARME-NARDI
  Double_t g_ve[10]      = {1e-4, 2e-4, 3e-4, 4e-4, 5e-4, 6e-4, 7e-4, 8e-4, 9e-4, 10e-4} ;

  //to be checked with new CalcHep simulations 20230214
  Double_t Sigma_Full = 4.132900e11; //pb 
  Double_t Sigma_FullP5 = Sigma_Full*1.05;
  Double_t Sigma_FullM5 = Sigma_Full*0.95; //pb
  Double_t Sigma_S    = 1.978156e8;  //pb 

  Double_t Luminosity_C;
  Double_t N_Full_C, N_FullP5_C, N_FullM5_C, N_Schannel_C, N_X20_C[10];

  Luminosity_C = CmtoPb * Na * Z_C * D_C * TThickness / A_C;

  N_Full_C     = Luminosity_C * Sigma_Full * PoT;
  N_FullP5_C   = Luminosity_C * Sigma_FullP5 * PoT;
  N_FullM5_C   = Luminosity_C * Sigma_FullM5 * PoT;

  N_Schannel_C = Luminosity_C * Sigma_S * PoT; 
  
  for(int i = 0; i<10; i++){
    N_X20_C[i] = PoT * PreFactor_X20 * ((g_ve[i]*g_ve[i])/(9e-8)) * (1/2.) * ((Na * Z_C * D_C)/(A_C * 1e24)); //EQUATION 13 Proceedings LUC  <- il primo indice sono i POT, il secondo il valore del COUPLING g_ve
  }
  

  Double_t En_min = 0.; 
  Double_t Radius_min = 90; 
  //Double_t MagnetShadow = 256.;//modified
  Double_t Radius_max = 270.; //modified
  Double_t Radius_SPos[1000000], Radius_SElec[1000000], Radius_Fp[1000000], Radius_Fe[1000000];
  Double_t Scan_S = 0.;
  Double_t Scan_Full = 0.;
  Double_t S_ACC = 0.;
  Double_t Full_ACC = 0.;
      
  for(Int_t k=0; k<numRow; k++){   
    Radius_SPos[k] = TMath::Sqrt(Pos_Sx[k]*Pos_Sx[k] + Pos_Sy[k]*Pos_Sy[k]);
    Radius_SElec[k] = TMath::Sqrt(Elec_Sx[k]*Elec_Sx[k] + Elec_Sy[k]*Elec_Sy[k]);
    Radius_Fp[k]    = TMath::Sqrt(Fp_x[k]*Fp_x[k] + Fp_y[k]*Fp_y[k]);
    Radius_Fe[k]    = TMath::Sqrt(Fe_x[k]*Fe_x[k] + Fe_y[k]*Fe_y[k]);
  }
  
  for(Int_t k = 0; k<numRow; k++){
    if(Radius_SPos[k]>Radius_min && Radius_SElec[k]>Radius_min){
      if(Radius_SPos[k]<Radius_max && Radius_SElec[k]<Radius_max){
        if(SFinalEn_positron[k]>En_min && SFinalEn_electron[k]>En_min){
          Scan_S++;
          // cout<<"Scan S "<<Scan_S<<endl;
        }
      }
    }
    
    if(Radius_Fp[k]>Radius_min && Radius_Fe[k]>Radius_min){
      if(Radius_Fp[k]<Radius_max && Radius_Fe[k]<Radius_max){
        if(FinalEn_Fp[k]>En_min && FinalEn_Fe[k]>En_min){
          Scan_Full++;
          // cout<<"Scan Full "<<Scan_Full<<endl;
        }
      }
    }
   }

  // S_ACC = Scan_S/numRow;
  S_ACC = 0.154; //HARDCODED
  Full_ACC = Scan_Full/numRow;
  cout<<"accettanza full bbhabha: "<<Full_ACC<<endl;
  
  Double_t prod_X20[1000];
  Double_t prod_X20P5[1000];
  Double_t prod_X20M5[1000];
  Double_t Squared_gve_stat[1000], gve_stat[1000];
  Double_t Squared_gve[1000], gve[1000];
  Double_t Squared_gveP5_stat[1000], gveP5_stat[1000];
  Double_t Squared_gveP5[1000], gveP5[1000];
  Double_t Squared_gveM5_stat[1000], gveM5_stat[1000];
  Double_t Squared_gveM5[1000], gveM5[1000];
  Double_t Mass_X20[1000];
  Double_t E_beam[1000];
  Double_t E_res = 430.;
  Double_t NC_PADME_S, NC_PADME_X20[10];
  Double_t LowerLimitX20, LowerLimitX20_P5, LowerLimitX20_M5;
  Double_t X20_discover_stat, X20_discover_P5_stat, X20_discover_M5_stat;
  Double_t X20_discover, X20_discover_P5, X20_discover_M5;
  Double_t NC_PADME_Full, NC_PADME_FullP5, NC_PADME_FullM5;

  NC_PADME_S = S_ACC * N_Schannel_C; // #S-channel events in Acceptance
  // NC_PADME_Full = Full_ACC * N_Full_C; // #Bhabha events in acceptance (BG)
  NC_PADME_Full = 46000; // #Bhabha events HARDCODED
  // NC_PADME_FullP5 = Full_ACC * N_FullP5_C;
  NC_PADME_FullP5 = NC_PADME_Full * 1.05; //HARDCODED
  // NC_PADME_FullM5 = Full_ACC * N_FullM5_C; 
  NC_PADME_FullM5 = NC_PADME_Full * 0.95; //HARDCODED

  cout<<"N full: "<<N_Full_C<<endl;
  cout<<"S-channel ACC: "<<S_ACC<<" HARDCODED"<<endl;
  cout<<"Full Bhabha ACC: "<<Full_ACC<<endl;
  cout<<"Number of S-channel in acceptance: "<<NC_PADME_S<<endl; 
  cout<<"Number of Bhabha events in acceptance: "<<NC_PADME_Full<<endl;
  cout<<"Number of BhabhaP5 events in acceptance: "<<NC_PADME_FullP5<<endl;
  cout<<"Number of BhabhaM5 events in acceptance: "<<NC_PADME_FullM5<<endl;

  LowerLimitX20_stat = 1.3*TMath::Sqrt(NC_PADME_Full); //maximal value of a fluctuation to exclude an exceed.
  LowerLimitX20_P5_stat = 1.3*TMath::Sqrt(NC_PADME_FullP5); 
  LowerLimitX20_M5_stat = 1.3*TMath::Sqrt(NC_PADME_FullM5); 
  LowerLimitX20 = 1.3*NC_PADME_Full*0.05; //maximal value of a fluctuation to exclude an exceed.
  LowerLimitX20_P5 = 1.3*NC_PADME_FullP5*0.05; 
  LowerLimitX20_M5 = 1.3*NC_PADME_FullM5*0.05; 

  cout<<"90CL exclusion: "<<LowerLimitX20<<endl;
  cout<<"90CL exclusion P5: "<<LowerLimitX20_P5<<endl;
  cout<<"90CL esclusion M5: "<<LowerLimitX20_M5<<endl;

  X20_discover_stat = LowerLimitX20_stat / S_ACC; // #signal events produced to exclude
  X20_discover_P5_stat = LowerLimitX20_P5_stat / S_ACC;
  X20_discover_M5_stat = LowerLimitX20_M5_stat / S_ACC; 
  X20_discover = LowerLimitX20 / S_ACC; // #5% discrepancy
  X20_discover_P5 = LowerLimitX20_P5 / S_ACC;
  X20_discover_M5 = LowerLimitX20_M5 / S_ACC; 

  cout<<"Number of X20: "<<X20_discover<<endl;
  cout<<"Number of X20 P5: "<<X20_discover_P5<<endl;
  cout<<"Number of X20 M5: "<<X20_discover_M5<<endl;

  // ofstream file;
  // file.open("gVe_MANCINI_X21.lmt");

  // ofstream file1;
  // file1.open("gVe_MANCINI_X21stat.lmt");

  for(int k = 0; k<1000; k++){
    E_beam[k] = 410 + (k*0.04);
    Mass_X20[k] = TMath::Sqrt(2*E_beam[k]*0.511);
  }
    
  for(int k = 0; k<1000; k++){
    Squared_gve_stat[k] = ((X20_discover_stat * 2 * A_C * 1e24 * 9e-8)/(PoT * PreFactor_X20 * Na * Z_C * D_C)) / TMath::Gaus(E_res, E_beam[k], 2);
    if(Squared_gve_stat[k] > 1e0) Squared_gve_stat[k] = 1e0;
    gve_stat[k] = TMath::Sqrt(Squared_gve_stat[k]);
    // cout<<"E_beam: "<<E_beam[k]<<" gve: "<<gve[k]<<endl;
    Squared_gveP5_stat[k] = ((X20_discover_P5_stat * 2 * A_C * 1e24 * 9e-8)/(PoT * PreFactor_X20 * Na * Z_C * D_C)) / TMath::Gaus(E_res, E_beam[k], 2);
    if(Squared_gveP5_stat[k] > 1e0) Squared_gveP5_stat[k] = 1e0;
    gveP5_stat[k] = TMath::Sqrt(Squared_gveP5_stat[k]);
    // cout<<"E_beam: "<<E_beam[k]<<" gveP5: "<<gveP5[k]<<endl;
    Squared_gveM5_stat[k] = ((X20_discover_M5_stat * 2 * A_C * 1e24 * 9e-8)/(PoT * PreFactor_X20 * Na * Z_C * D_C)) / TMath::Gaus(E_res, E_beam[k], 2);
    if(Squared_gveM5_stat[k] > 1e0) Squared_gveM5_stat[k] = 1e0;
    gveM5_stat[k] = TMath::Sqrt(Squared_gveM5_stat[k]);
    // cout<<"E_beam: "<<E_beam[k]<<" gveM5: "<<gveM5[k]<<endl;

    Squared_gve[k] = ((X20_discover * 2 * A_C * 1e24 * 9e-8)/(PoT * PreFactor_X20 * Na * Z_C * D_C)) / TMath::Gaus(E_res, E_beam[k], 2);
    if(Squared_gve[k] > 1e0) Squared_gve[k] = 1e0;
    gve[k] = TMath::Sqrt(Squared_gve[k]);
    // cout<<"E_beam: "<<E_beam[k]<<" gve: "<<gve[k]<<endl;
    Squared_gveP5[k] = ((X20_discover_P5 * 2 * A_C * 1e24 * 9e-8)/(PoT * PreFactor_X20 * Na * Z_C * D_C)) / TMath::Gaus(E_res, E_beam[k], 2);
    if(Squared_gveP5[k] > 1e0) Squared_gveP5[k] = 1e0;
    gveP5[k] = TMath::Sqrt(Squared_gveP5[k]);
    // cout<<"E_beam: "<<E_beam[k]<<" gveP5: "<<gveP5[k]<<endl;
    Squared_gveM5[k] = ((X20_discover_M5 * 2 * A_C * 1e24 * 9e-8)/(PoT * PreFactor_X20 * Na * Z_C * D_C)) / TMath::Gaus(E_res, E_beam[k], 2);
    if(Squared_gveM5[k] > 1e0) Squared_gveM5[k] = 1e0;
    gveM5[k] = TMath::Sqrt(Squared_gveM5[k]);
    // cout<<"E_beam: "<<E_beam[k]<<" gveM5: "<<gveM5[k]<<endl;

    // file<<Mass_X20[k]<<"  "<<gve[k]<<"\n";

  }

  Double_t min_gve_stat, min_gveP5_stat, min_gveM5_stat;
  Double_t min_gve, min_gveP5, min_gveM5;
  min_gve_stat = TMath::MinElement(1000, gve_stat);
  min_gveP5_stat = TMath::MinElement(1000, gveP5_stat);
  min_gveM5_stat = TMath::MinElement(1000, gveM5_stat);
  min_gve = TMath::MinElement(1000, gve);
  min_gveP5 = TMath::MinElement(1000, gveP5);
  min_gveM5 = TMath::MinElement(1000, gveM5);

  cout<<"gve min_stat: "<<min_gve_stat<<endl;
  cout<<"gveP5 min_stat "<<min_gveP5_stat<<endl;
  cout<<"gveM5 min_stat: "<<min_gveM5_stat<<endl;
  cout<<"gve min: "<<min_gve<<endl;
  cout<<"gveP5 min: "<<min_gveP5<<endl;
  cout<<"gveM5 min: "<<min_gveM5<<endl;

  // file.close();
  // file1.close();


  for(int k = 0; k<1000; k++){
      prod_X20[k] = PoT * PreFactor_X20 * ((min_gve*min_gve)/(9e-8)) * ((Na * Z_C * D_C)/(A_C * 1e24)) * (1/2.) * TMath::Gaus(E_res, E_beam[k], 2); 
      // cout<<"E_beam: "<<E_beam[k]<<" N_X20: "<<prod_X20[k]<<" with g_ve:"<<min_gve<<endl;

      prod_X20P5[k] = PoT * PreFactor_X20 * ((min_gveP5*min_gveP5)/(9e-8)) * ((Na * Z_C * D_C)/(A_C * 1e24)) * (1/2.) * TMath::Gaus(E_res, E_beam[k], 2); 
      // cout<<"E_beam: "<<E_beam[k]<<" N_X20 P5: "<<prod_X20P5[k]<<" with g_ve:"<<min_gveP5<<endl;

      prod_X20M5[k] = PoT * PreFactor_X20 * ((min_gveM5*min_gveM5)/(9e-8)) * ((Na * Z_C * D_C)/(A_C * 1e24)) * (1/2.) * TMath::Gaus(E_res, E_beam[k], 2); 
      // cout<<"E_beam: "<<E_beam[k]<<" N_X20 M5: "<<prod_X20M5[k]<<" with g_ve:"<<min_gveM5<<endl;
    }

  TGraph *Production = new TGraph(1000, Mass_X20, prod_X20);
  Production->SetTitle("NPoT = 1e10; M_{X20} [MeV]; N_{A'}^{tot}");
  Production->SetMaximum(1e4); Production->SetMinimum(1e0);
  Production->GetXaxis()->SetRangeUser(20.5, 21.5);
  Production->SetLineColor(kTeal-6);
  Production->SetLineWidth(2);

  TGraph *ProductionP5 = new TGraph(1000, Mass_X20, prod_X20P5);
  ProductionP5->SetTitle("NPoT = 1e10; M_{X20} [MeV]; N_{A'}^{tot}");
  ProductionP5->SetMaximum(1e4); ProductionP5->SetMinimum(1e0);
  ProductionP5->GetXaxis()->SetRangeUser(20.5, 21.5);
  ProductionP5->SetLineColor(kTeal-3);
  ProductionP5->SetLineWidth(2);

  TGraph *ProductionM5 = new TGraph(1000, Mass_X20, prod_X20M5);
  ProductionM5->SetTitle("NPoT = 1e10; M_{X20} [MeV]; N_{A'}^{tot}");
  ProductionM5->SetMaximum(1e4); ProductionM5->SetMinimum(1e0);
  ProductionM5->GetXaxis()->SetRangeUser(20.5, 21.5);
  ProductionM5->SetLineColor(kTeal-3);
  ProductionM5->SetLineWidth(2);
  
  TGraph *GgVe_stat = new TGraph(1000, Mass_X20, gve_stat);
  GgVe_stat->SetTitle("NPoT=1e10; M_{X20} [MeV]; g_{V_{e}}");
  GgVe_stat->SetMaximum(1e-1); GgVe_stat->SetMinimum(1e-4);
  GgVe_stat->GetXaxis()->SetRangeUser(20.5, 21.5);
  GgVe_stat->SetLineColor(kTeal-6);
  GgVe_stat->SetLineWidth(1);

  TGraph *GgVeP5_stat = new TGraph(1000, Mass_X20, gveP5_stat);
  GgVeP5_stat->SetTitle("NPoT=1e10; M_{X20} [MeV]; g_{V_{e}}");
  GgVeP5_stat->SetMaximum(1e-1); GgVeP5_stat->SetMinimum(1e-4);
  GgVeP5_stat->GetXaxis()->SetRangeUser(20.5, 21.5);
  GgVeP5_stat->SetLineColor(kTeal-3);
  GgVeP5_stat->SetLineWidth(1);

  TGraph *GgVeM5_stat = new TGraph(1000, Mass_X20, gveM5_stat);
  GgVeM5_stat->SetTitle("NPoT=1e10; M_{X20} [MeV]; g_{V_{e}}");
  GgVeM5_stat->SetMaximum(1e-1); GgVeM5_stat->SetMinimum(1e-4);
  GgVeM5_stat->GetXaxis()->SetRangeUser(20.5, 21.5);
  GgVeM5_stat->SetLineColor(kTeal-3);
  GgVeM5_stat->SetLineWidth(1);

  TGraph *GgVe = new TGraph(1000, Mass_X20, gve);
  GgVe->SetTitle("NPoT=1e10; M_{X20} [MeV]; g_{V_{e}}");
  GgVe->SetMaximum(1e-1); GgVe->SetMinimum(1e-4);
  GgVe->GetXaxis()->SetRangeUser(20.5, 21.5);
  GgVe->SetLineColor(kRed+3);
  GgVe->SetLineWidth(1);

  TGraph *GgVeP5 = new TGraph(1000, Mass_X20, gveP5);
  GgVeP5->SetTitle("NPoT=1e10; M_{X20} [MeV]; g_{V_{e}}");
  GgVeP5->SetMaximum(1e-1); GgVeP5->SetMinimum(1e-4);
  GgVeP5->GetXaxis()->SetRangeUser(20.5, 21.5);
  GgVeP5->SetLineColor(kRed-7);
  GgVeP5->SetLineWidth(1);

  TGraph *GgVeM5 = new TGraph(1000, Mass_X20, gveM5);
  GgVeM5->SetTitle("NPoT=1e10; M_{X20} [MeV]; g_{V_{e}}");
  GgVeM5->SetMaximum(1e-1); GgVeM5->SetMinimum(1e-4);
  GgVeM5->GetXaxis()->SetRangeUser(20.5, 21.5);
  GgVeM5->SetLineColor(kRed-7);
  GgVeM5->SetLineWidth(1);

  TMultiGraph *MGProduction = new TMultiGraph();
  MGProduction->Add(Production);
  MGProduction->Add(ProductionP5);
  MGProduction->Add(ProductionM5);
  MGProduction->SetTitle("Vector boson production at E_{beam}=430 MeV; M_{X20} [MeV]; N_{A'}^{tot}");
  
  TMultiGraph *MGCoupling = new TMultiGraph();
  MGCoupling->Add(GgVe_stat);
  MGCoupling->Add(GgVeP5_stat);
  MGCoupling->Add(GgVeM5_stat);
  MGCoupling->Add(GgVe);
  MGCoupling->Add(GgVeP5);
  MGCoupling->Add(GgVeM5);
  MGCoupling->SetTitle("g_{Ve} Exclusion  ; M_{X20} [MeV]; g_{V_{e}}");

  TCanvas *X20prod = new TCanvas("X20prod", "Production of X20 - Mass Scan", 900, 700);
  X20prod->SetGrid();
  MGProduction->Draw("AC");
  X20prod->BuildLegend();
  gPad->SetLogy();
  
  TCanvas *CgVe = new TCanvas("CgVe", "g_{Ve}", 900, 700);
  CgVe->SetGrid();
  MGCoupling->Draw("AC");
  // CgVe->BuildLegend();
  gPad->SetLogy();


  }


