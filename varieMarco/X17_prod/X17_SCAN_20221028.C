//MANCINI MARCO 06-05-2022 X17 production
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
  
  TFile *f1 = new TFile("/home/marco1/Documenti/SM_analysis/BHABHA_T_1e6new.root");
  TFile *f2 = new TFile("/home/marco1/Documenti/SM_analysis/BHABHA_S_1e6.root");
  TFile *f3 = new TFile("/home/marco1/Documenti/SM_analysis/Ee_AA_1e6.root");
  TFile *f4 = new TFile("/home/marco1/Documenti/varie/NA64.root");

  TTree *t1 = (TTree*)f1->Get("T1");
  TTree *t2 = (TTree*)f2->Get("t2");
  TTree *t3 = (TTree*)f3->Get("t3");
  TTree *t4 = (TTree*)f4->Get("t4");
  Double_t InitialP3_positron, InitialP3_electron, TFinalP1_electron, TFinalP2_electron, TFinalP3_electron, TFinalP1_positron, TFinalP2_positron, TFinalP3_positron;
  Double_t SFinalP1_electron, SFinalP2_electron, SFinalP3_electron, SFinalP1_positron, SFinalP2_positron, SFinalP3_positron;
  Double_t FinalP1_G1, FinalP2_G1, FinalP3_G1, FinalP1_G2, FinalP2_G2, FinalP3_G2;
  Double_t mass, lower, upper;
  
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
  t4->SetBranchAddress("mass", &mass);
  t4->SetBranchAddress("lower", &lower);
  t4->SetBranchAddress("upper", &upper);

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

  Double_t PoT[20] = {1e10, 5e9, 3e10, 4e10, 5e10, 6e10, 7e10, 8e10, 9e10, 10e10, 11e10, 12e10, 13e10, 14e10, 15e10, 16e10, 17e10, 18e10, 19e10, 2e11}; //MODIFICATO IL [1] CON 5e9 ANZICHÉ 2e10
  Double_t Na = 6.022e23;
  Double_t Z_C = 6;
  Double_t D_C = 3.5;
  Double_t A_C = 12;
  Double_t TThickness = 0.01; //in cm
  Double_t CmtoPb = 1e-36;

  Double_t PreFactor_X17 = 3.8e-7; //DATAS FROM PROCEEDINGS DARME-NARDI
  Double_t g_ve[10] = {1e-4, 2e-4, 3e-4, 4e-4, 5e-4, 6e-4, 7e-4, 8e-4, 9e-4, 10e-4} ;

  Double_t Sigma_S = 3.020595e8; //pb
  Double_t Sigma_T = 5.1550e11; //pb -> FOCUS IR divergence saved by energy cut on CalcHEP (E_min = 0.005 GeV)
  Double_t Sigma_AA = 2.732152e9; //pb

  Double_t Luminosity_C;
  Double_t N_Schannel_C[20], N_Tchannel_C[20], N_AA_C[20], N_X17_C[20][10];

  Luminosity_C = CmtoPb * Na * Z_C * D_C * TThickness / A_C;
  cout<<Luminosity_C<<endl;
  
  for(int k = 0; k<20; k++){
  N_Schannel_C[k] = Luminosity_C * Sigma_S * PoT[k]; 
  N_Tchannel_C[k] = Luminosity_C * Sigma_T * PoT[k];
  N_AA_C[k] = Luminosity_C * Sigma_AA * PoT[k];
  }
    
  Double_t En_min[10] = {0., 0.03, 0.1, 0.11, 0.115, 0.12, 0.125, 0.13, 0.135, 0.14}; // fissata energia del secondo valore a 30 MeV - prima stava a 50
  Double_t Radius_min = 90.;
  Double_t Radius_max[21] = {180., 185., 190., 195., 200., 205., 210., 215., 220., 225., 230., 235., 240., 245., 250., 255., 260., 265., 270., 275., 280.};
  Double_t Radius_SPos[1000000], Radius_SElec[1000000], Radius_TPos[1000000], Radius_TElec[1000000], Radius_G1[1000000], Radius_G2[1000000];
  Double_t Scan_S[21][10], Scan_T[21][10], Scan_AA[21][10];
  Double_t S_ACC[21][10], T_ACC[21][10], AA_ACC[21][10];
  
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
  
  for(Int_t l = 0; l<21; l++){
    //file<<"---------------------R_{max}= "<<Radius_max[l]<<" mm "<<"-------------\n";
   for(Int_t j = 0; j<10; j++){
     //file<<"--------------DATA E> "<<En_min[j]<<" GeV "<<"-----------\n";
    for(Int_t k = 0; k<numRow; k++){
	
	if(Radius_SPos[k]>Radius_min && Radius_SElec[k]>Radius_min){
	  if(Radius_SPos[k]<Radius_max[l] && Radius_SElec[k]<Radius_max[l]){
	    if(SFinalEn_positron[k]>En_min[j] && SFinalEn_electron[k]>En_min[j]){Scan_S[l][j]++;};};};

	if(Radius_TPos[k]>Radius_min && Radius_TElec[k]>Radius_min){
	  if(Radius_TPos[k]<Radius_max[l] && Radius_TElec[k]<Radius_max[l]){
	    if(TFinalEn_positron[k]>En_min[j] && TFinalEn_electron[k]>En_min[j]){Scan_T[l][j]++;};};};

      
	if(Radius_G1[k]>Radius_min && Radius_G2[k]>Radius_min){
	  if(Radius_G1[k]<Radius_max[l] && Radius_G2[k]<Radius_max[l]){
	    if(FinalEn_G1[k]>En_min[j] && FinalEn_G2[k]>En_min[j]){Scan_AA[l][j]++;};};};
     
    }


    S_ACC[l][j] = Scan_S[l][j]/numRow;
    T_ACC[l][j] = Scan_T[l][j]/numRow;
    AA_ACC[l][j] = Scan_AA[l][j]/numRow; 
  }
  }

  
  Double_t prod_X17[31][1000], prod_X171[31][1000],  Squared_gve[31][1000], Squared_gve1[31][1000];
  Double_t Mass_X17[1000];
  Double_t E_beam[1000];
  Double_t E_res[18] = {269.5, 271, 272.5, 274, 275.5, 277, 278.5, 280, 281.5, 283, 284.5, 286, 287.5, 289, 290.5, 292, 293.5, 295};
  Double_t E_resHalf[13] = {273.25, 274.75, 276.25, 277.75, 279.25, 280.75, 282.25, 283.75, 285.25, 286.75, 288.25, 289.75, 291.25};
  Double_t E_ResTot[31] = {269.5, 271, 272.5, 273.25, 274, 274.75, 275.5, 276.25, 277, 277.75, 278.5, 279.25, 280, 280.75, 281.5, 282.25, 283, 283.75, 284.5, 285.25, 286, 286.75, 287.5, 288.25, 289, 289.75, 290.5, 291.25, 292, 293.5, 295};
  Double_t NC_PADME_S[20], NC_PADME_T[20],NC_PADME_AA[20], NC_PADME_X17[20][10];
  Double_t LowerLimitX17[20], X17_discover[20];


  for(Int_t l = 0; l<20; l++){ //POT
    NC_PADME_S[l] = S_ACC[19][1] * N_Schannel_C[l];
    NC_PADME_T[l] = T_ACC[19][1] * N_Tchannel_C[l];
    NC_PADME_AA[l] = AA_ACC[19][1] * N_AA_C[l];
    cout<<"Full Bg: "<<(NC_PADME_S[l] + NC_PADME_T[l])<<endl;
    LowerLimitX17[l]= 1.2*TMath::Sqrt(NC_PADME_S[l] + NC_PADME_T[l]); //check prefactor: is 1.2 equal to 90% confidency level?
    X17_discover[l] = LowerLimitX17[l] / S_ACC[19][1]; //il numero di candidati X17 entro il calorimetro -> 90CL * Accettanza S Channel

  }

  std::vector<std::tuple<Double_t,Double_t,Double_t>> DiscPoTRes;
  for(int i = 0; i<31; i++){
    if(i>=3 & i<=27){
      if (i%2 != 0) DiscPoTRes.push_back(std::make_tuple(X17_discover[1], PoT[1], E_ResTot[i]));
      else DiscPoTRes.push_back(std::make_tuple(X17_discover[0], PoT[0], E_ResTot[i]));
    } 
    else DiscPoTRes.push_back(std::make_tuple(X17_discover[0], PoT[0], E_ResTot[i]));
    cout<<"X17_Disc: "<<std::get<0>(DiscPoTRes[i])<<", PoT: "<<std::get<1>(DiscPoTRes[i])<<", E_Res: "<<std::get<2>(DiscPoTRes[i])<<endl;
    
  }
  
  for(int k = 0; k<1000; k++){
    E_beam[k] = 240 + (k*0.08);
    Mass_X17[k] = TMath::Sqrt(2*E_beam[k]*0.511);
  }

  ofstream file;
  file.open("gVe_MANCINI_1E10.lmt");
    
  for(int l = 0; l<31; l++){
    for(int k = 0; k<1000; k++){
      
      // prod_X17[l][k] = S_ACC[19][1]*(PoT[9] * PreFactor_X17 * ((g_ve[0]*g_ve[0])/(9e-8)) * ((Na * Z_C * D_C)/(A_C * 1e24)) * (1/0.7) * TMath::Gaus(E_beam[k], E_res[l], 0.7)); //accettanza: E>30 MeV, R_min 90mm, R_max 270 mm
      // prod_X171[l][k] = S_ACC[19][1]*(PoT[0] * PreFactor_X17 * ((g_ve[0]*g_ve[0])/(9e-8)) * ((Na * Z_C * D_C)/(A_C * 1e24)) * (1/0.7) * TMath::Gaus(E_beam[k], E_res[l], 0.7)); //accettanza: E>30 MeV, R_min 90mm, R_max 270 mm
      // Squared_gve[l][k] = ((X17_discover[9] * 0.7 * A_C * 1e24 * 9e-8)/( PoT[9] * PreFactor_X17 * Na * Z_C * D_C)) / (TMath::Gaus(E_res[l], E_beam[k], 0.7) + (TMath::Gaus(E_res[l+1], E_beam[k], 0.7)));
      // if(l>=2 & l<=27){
      // 	Squared_gve1[l][k] = ((X17_discover[0] * 0.7 * A_C * 1e24 * 9e-8)/( PoT[0] * PreFactor_X17 * Na * Z_C * D_C)) / (TMath::Gaus(E_ResTot[l], E_beam[k], 0.7) + (TMath::Gaus(E_ResTot[l+1], E_beam[k], 0.7)));}
      // else Squared_gve1[l][k] = 10;

      
      Squared_gve1[l][k] = ((0.7 * A_C * 1e24 * 9e-8)/(PreFactor_X17 * Na * Z_C * D_C))*((std::get<0>(DiscPoTRes[l])/(std::get<1>(DiscPoTRes[l])*TMath::Gaus(std::get<2>(DiscPoTRes[l]), E_beam[k], 0.7))));
      //+ (std::get<0>(DiscPoTRes[l+1])/(std::get<1>(DiscPoTRes[l+1])*TMath::Gaus(std::get<2>(DiscPoTRes[l+1]), E_beam[k], 0.7))) + (std::get<0>(DiscPoTRes[l+2])/(std::get<1>(DiscPoTRes[l+2])*TMath::Gaus(std::get<2>(DiscPoTRes[l+2]), E_beam[k], 0.7))));
	
      //if(Squared_gve[l][k] > 1e1){Squared_gve[l][k] = 1e1;};
      if(Squared_gve1[l][k] > 1e1){Squared_gve1[l][k] = 1e1;};
          
      }
    }

   
    double max_value[1000], max_value1[1000], min_value[1000], min_value1[1000], Prod550[1000], Brem550[1000];
    double check1[31], check2[31], check3[31], check4[31];
    double check31[31], check41[31];
    double A[1000], B[1000];
    for(int k = 0; k<1000; k++){
      //Prod550[k] = 2.5;
      //Brem550[k] = 4e-2;
      //cout<<Prod550[k]<< "  "<<Brem550[k]<<endl;
      for(int l = 0; l<31; l++){
	//check1[l] = prod_X17[l][k] + prod_X17[l+1][k]; 
	//check2[l] = prod_X171[l][k] + prod_X171[l+1][k];      
	//check3[l] = Squared_gve[l][k];
	//check31[l] = Squared_gve[l+1][k]; 
	check4[l] = Squared_gve1[l][k];
	//check41[l] = Squared_gve1[l+1][k]; 
  
      }

      //max_value[k] = TMath::MaxElement(17, check1);
      //max_value1[k] = TMath::MaxElement(17, check2);
    
      //min_value[k] = TMath::MinElement(17, check3);
      min_value1[k] = TMath::MinElement(30, check4);

      //A[k] = TMath::Sqrt(min_value[k]);
      B[k] = TMath::Sqrt(min_value1[k]);

      file<<Mass_X17[k]<<"  "<<B[k]<<"\n";
    
    }

    file.close();
    int nentries1 = t4->GetEntries();
    double_t NA64[90], massNA64[90];

    for(int i = 0; i<90; i++){
      t4->GetEntry(i);
      NA64[i] = upper*0.303;
      massNA64[i] = mass*1e3;
	}

    /*TGraph *Production = new TGraph(1000, Mass_X17, max_value);
  Production->SetTitle("NPoT = 1e11; M_{X17} [MeV]; N_{tot}^{X17}");
  Production->SetMaximum(2e4); Production->SetMinimum(1e-2);
  Production->GetXaxis()->SetRangeUser(15.50, 18.00);
  Production->SetLineColor(kTeal-6);
  Production->SetLineWidth(2);*/

  // TGraph *Production1 = new TGraph(1000, Mass_X17, max_value1);
  // Production1->SetTitle("NPoT = 1e10; M_{X17} [MeV]; N_{tot}^{X17}");
  // //Production1->SetMaximum(2e4); Production1->SetMinimum(1e-2);
  // //Production1->GetXaxis()->SetRangeUser(15.50, 18.00);
  // Production1->SetLineColor(kBlue-2);
  // Production1->SetLineWidth(2);
  
  /*TGraph *GgVe = new TGraph(1000, Mass_X17, A);
  GgVe->SetTitle("NPoT=1e11; M_{X17} [MeV]; g_{V_{e}}");
  GgVe->SetMaximum(4e-6); GgVe->SetMinimum(8e-10);
  GgVe->GetXaxis()->SetRangeUser(15.50, 18.00);
  GgVe->SetLineColor(kTeal-6);
  GgVe->SetLineWidth(2);*/

  TGraph *GgVe1 = new TGraph(1000, Mass_X17, B);
  GgVe1->SetTitle("NPoT=2.5 #times 10^{11}; M_{X17} [MeV]; g_{V_{e}}");
  GgVe1->SetMaximum(4e-6); GgVe1->SetMinimum(8e-10);
  GgVe1->GetXaxis()->SetRangeUser(15.50, 18.00);
  GgVe1->SetLineColor(kBlue-2);
  GgVe1->SetLineWidth(2);

  // TGraph *X17_550 = new TGraph(1000, Mass_X17, Prod550);
  // X17_550->SetTitle("e^{+}e^{-}->#gamma X17, E_{e^{+}}~550 MeV; M_{X17} [MeV]; N_{tot}^{X17}");
  // //X17_550->SetMaximum(2e4); X17_550->SetMinimum(1e-2);
  // //X17_550->GetXaxis()->SetRangeUser(15.50, 18.00);
  // X17_550->SetLineColor(kRed);
  // X17_550->SetLineWidth(2);
  // X17_550->SetLineStyle(2);

  // TGraph *BREM_550 = new TGraph(1000, Mass_X17, Brem550);
  // BREM_550->SetTitle("Bremm., E_{e^{+}}~550 MeV; M_{X17} [MeV]; N_{tot}^{X17}");
  // // BREM_550->SetMaximum(2e4); X17_550->SetMinimum(1e-2);
  // //BREM_550->GetXaxis()->SetRangeUser(15.50, 18.00);
  // BREM_550->SetLineColor(kMagenta+1);
  // BREM_550->SetLineWidth(2);
  // BREM_550->SetLineStyle(2);

  TGraph *NA64limit = new TGraph(nentries1, massNA64, NA64);
  NA64limit->SetTitle("NA64 upper limit; M_{X17} [MeV]; g_{V_{e}}");
  NA64limit->SetMaximum(4e-6); NA64limit->SetMinimum(8e-10);
  NA64limit->GetXaxis()->SetRangeUser(15.50, 18.00);
  NA64limit->SetLineColor(kRed);
  NA64limit->SetLineWidth(2);
  NA64limit->SetLineStyle(1);

  // TMultiGraph *MGProd = new TMultiGraph();
  // //MGProd->Add(Production);
  // MGProd->Add(Production1);
  // MGProd->Add(X17_550);
  // MGProd->Add(BREM_550);

  // MGProd->SetTitle("N_{tot}^{X17} Mass Scan - g_{V_{e}} = 1e-4; M_{X17} [MeV]; N_{tot}^{X17}");
  // //MGProd->SetMaximum(2e4); MGProd->SetMinimum(1e-2);
  // //MGProd->GetXaxis()->SetRangeUser(15.50, 18.00);


  TMultiGraph *MGCoupling = new TMultiGraph();
  //MGCoupling->Add(GgVe);
  MGCoupling->Add(GgVe1);
  MGCoupling->Add(NA64limit);


  MGCoupling->SetTitle("g_{V_{e}} Mass Scan; M_{X17} [MeV]; g_{V_{e}}");
  //MGCoupling->SetMaximum(4e-6); MGCoupling->SetMinimum(8e-10);


  // TCanvas *X17prod = new TCanvas("X17prod", "Production of X17 - Mass Scan", 900, 700);
  // X17prod->SetGrid();
  // MGProd->Draw("AC");
  // X17prod->BuildLegend();
  // gPad->SetLogy();
  
  TCanvas *CgVe = new TCanvas("CgVe", "g_{Ve}^{2}", 900, 700);
  CgVe->SetGrid();
  MGCoupling->Draw("AC");
  CgVe->BuildLegend();
  gPad->SetLogy();


//pippo

  }


