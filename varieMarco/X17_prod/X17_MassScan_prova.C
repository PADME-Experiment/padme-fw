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
#include <THStack.h>


  
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

  Double_t PoT[20] = {1e10, 2e10, 3e10, 4e10, 5e10, 6e10, 7e10, 8e10, 9e10, 10e10, 11e10, 12e10, 13e10, 14e10, 15e10, 16e10, 17e10, 18e10, 19e10, 2e11};
  Double_t Na = 6.022e23;
  Double_t Z_C = 6;
  Double_t D_C = 3.5;
  Double_t A_C = 12;
  Double_t TThickness = 0.01; //in cm
  Double_t CmtoPb = 1e-36;

  Double_t PreFactor_X17 = 3.8e-7; //DATAS FROM PROCEEDINGS DARME-NARDI
  Double_t g_ve[10] = {1e-4, 2e-4, 3e-4, 4e-4, 5e-4, 6e-4, 7e-4, 8e-4, 9e-4, 10e-4} ;

  Double_t Sigma_S = 3.020595e8; //pb
  Double_t Sigma_T = 5.261278e10; //pb -> FOCUS IR divergence saved by energy cut on CalcHEP (E_min = 0.005 GeV)
  Double_t Sigma_AA = 2.732152e9; //pb

  Double_t Luminosity_C;
  Double_t N_Schannel_C[20], N_Tchannel_C[20], N_AA_C[20], N_X17_C[20][10];

  Luminosity_C = CmtoPb * Na * Z_C * D_C * TThickness / A_C;

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

 
  Double_t prod_X17[21][20000], Squared_gve[21][20000];
  Double_t Mass_X17[20000];
  Double_t E_beam[20000];
  Double_t E_res[21] = {260, 262, 264, 266, 268, 270, 272, 274, 276, 278, 280, 282, 284, 286, 288, 290, 292, 294, 296, 298, 300};
  Double_t NC_PADME_S[20], NC_PADME_T[20],NC_PADME_AA[20], NC_PADME_X17[20][10];
  Double_t Sensitivity[20][10], LowerLimitX17[20], X17_discover[20];


  for(Int_t l = 0; l<20; l++){ //POT
    NC_PADME_S[l] = S_ACC[18][1] * N_Schannel_C[l];
    NC_PADME_T[l] = T_ACC[18][1] * N_Tchannel_C[l];
    NC_PADME_AA[l] = AA_ACC[18][1] * N_AA_C[l];
    LowerLimitX17[l]= 1.2*TMath::Sqrt(NC_PADME_S[l] + NC_PADME_T[l]); //check prefactor: is 1.2 equal to 90% confidency level?
    X17_discover[l] = LowerLimitX17[l] / S_ACC[18][1]; //il numero di candidati X17 entro il calorimetro -> 90CL * Accettanza S Channel

  }


  for(int k = 0; k<20000; k++){
    E_beam[k] = 240 + (k*0.004);
    Mass_X17[k] = TMath::Sqrt(2*E_res[k]*0.511);
  }
  
  //TH1F *h0 = new TH1F("h0", "N_{tot}^{X17}, 260 MeV < E_{beam} < 300 MeV, NPoT = 1e10, g_{Ve} = 1e-4 - eq 43 PrecisionBeamDump", 5000, 16, 18);
  TH1F **h0;
  h0 = new TH1F*[21];
  TH1F **g0;
  g0 = new TH1F*[21];


  //TH1F *MASS = new TH1F(Form("histo"), Form("MASS"), 5000, 16, 18);
  //THStack *HStack = new THStack("Stack", "Stack");
  //for(int l = 0; l<21; l++){
  //HStack->Add(h0[1],"");
    //}
  
  for(int l = 0; l<21; l++){

    h0[l] = new TH1F(Form("histo"), Form("N_{tot}^{X17}, 260 MeV < E_{beam} < 300 MeV, NPoT = 1e10, g_{Ve} = 1e-4 - eq 43 PrecisionBeamDump"), 5000, 16, 18);
    g0[l] = new TH1F(Form("histo"), Form("g_{V_{e}}^{2}, 260 MeV < E_{beam} < 300 MeV, NPoT = 1e10 - eq 43 PrecisionBeamDump"), 5000, 16, 18);

    h0[l]->SetXTitle("M_{X17} [MeV]"); h0[l]->SetYTitle("N^{tot}_{X17}");
    g0[l]->SetXTitle("M_{X17} [MeV]"); g0[l]->SetYTitle("g_{Ve}^{2}");

    h0[l]->SetMaximum(1e5); h0[l]->SetMinimum(1);
    
    h0[l]->SetLineColor(kOrange+7);
    g0[l]->SetLineColor(kGreen-6);


  }


  for(int l = 0; l<21; l++){
    for(int k = 0; k<20000; k++){
      
      prod_X17[l][k] = S_ACC[18][1]*(PoT[9] * PreFactor_X17 * ((g_ve[0]*g_ve[0])/(9e-8)) * ((Na * Z_C * D_C)/(A_C * 1e24)) * (1/0.7) * TMath::Gaus(E_beam[k], E_res[l], 0.7)); //accettanza: E>30 MeV, R_min 90mm, R_max 270 mm
      Squared_gve[l][k] = ((X17_discover[9] * 0.7 * A_C * 1e24 * 9e-8)/( PoT[9] * PreFactor_X17 * Na * Z_C * D_C)) / (TMath::Gaus(E_res[k], E_beam[l], 0.7));
      if(Squared_gve[l][k] > 1e1){Squared_gve[l][k] = 1e1;};
      
      h0[l]->Fill(Mass_X17[k], prod_X17[l][k]);
      g0[l]->Fill(Mass_X17[k], Squared_gve[l][k]);
      
      
    }   
    
  }

  TH1F *Sum_H = (TH1F*)h0[0]->Clone();
  
  
  for(int l = 1; l<21 ; l++){
    Sum_H->Add(h0[l]);
  }
  
  //double y[20000];

  //for(k = 0; k<20000; k++){
  //y[k] = Sum->GetMinimum();
  //G->Fill(Mass_X17[k], y[k]);
  //}
  
  /*h19->Add(h20);
  h18->Add(h19);
  h17->Add(h18);
  h16->Add(h17);
  h15->Add(h16);
  h14->Add(h15);
  h13->Add(h14);
  h12->Add(h13);
  h11->Add(h12);
  h10->Add(h11);
  h9->Add(h10);
  h8->Add(h9);
  h7->Add(h8);
  h6->Add(h7);
  h5->Add(h6);
  h4->Add(h5);
  h3->Add(h4);
  h2->Add(h3);
  h1->Add(h2);
  h0->Add(h1);

  g19->Add(g20);
  g18->Add(g19);
  g17->Add(g18);
  g16->Add(g17);
  g15->Add(g16);
  g14->Add(g15);
  g13->Add(g14);
  g12->Add(g13);
  g11->Add(g12);
  g10->Add(g11);
  g9->Add(g10);
  g8->Add(g9);
  g7->Add(g8);
  g6->Add(g7);
  g5->Add(g6);
  g4->Add(g5);
  g3->Add(g4);
  g2->Add(g3);
  g1->Add(g2);
  g0->Add(g1);*/

  
  /*h0->SetXTitle("M_{X17} [MeV]"); h0->SetYTitle("N^{tot}_{X17}");
  h1->SetXTitle("M_{X17} [MeV]"); h1->SetYTitle("N^{tot}_{X17}");
  h2->SetXTitle("M_{X17} [MeV]"); h2->SetYTitle("N^{tot}_{X17}");
  h3->SetXTitle("M_{X17} [MeV]"); h3->SetYTitle("N^{tot}_{X17}");
  h4->SetXTitle("M_{X17} [MeV]"); h4->SetYTitle("N^{tot}_{X17}");
  h5->SetXTitle("M_{X17} [MeV]"); h5->SetYTitle("N^{tot}_{X17}");
  h6->SetXTitle("M_{X17} [MeV]"); h6->SetYTitle("N^{tot}_{X17}");
  h7->SetXTitle("M_{X17} [MeV]"); h7->SetYTitle("N^{tot}_{X17}");
  h8->SetXTitle("M_{X17} [MeV]"); h8->SetYTitle("N^{tot}_{X17}");
  h9->SetXTitle("M_{X17} [MeV]"); h9->SetYTitle("N^{tot}_{X17}");
  h10->SetXTitle("M_{X17} [MeV]"); h10->SetYTitle("N^{tot}_{X17}");
  h11->SetXTitle("M_{X17} [MeV]"); h11->SetYTitle("N^{tot}_{X17}");
  h12->SetXTitle("M_{X17} [MeV]"); h12->SetYTitle("N^{tot}_{X17}");
  h13->SetXTitle("M_{X17} [MeV]"); h13->SetYTitle("N^{tot}_{X17}");
  h14->SetXTitle("M_{X17} [MeV]"); h14->SetYTitle("N^{tot}_{X17}");
  h15->SetXTitle("M_{X17} [MeV]"); h15->SetYTitle("N^{tot}_{X17}");
  h16->SetXTitle("M_{X17} [MeV]"); h16->SetYTitle("N^{tot}_{X17}");
  h17->SetXTitle("M_{X17} [MeV]"); h17->SetYTitle("N^{tot}_{X17}");
  h18->SetXTitle("M_{X17} [MeV]"); h18->SetYTitle("N^{tot}_{X17}");
  h19->SetXTitle("M_{X17} [MeV]"); h19->SetYTitle("N^{tot}_{X17}");
  h20->SetXTitle("M_{X17} [MeV]"); h20->SetYTitle("N^{tot}_{X17}");

  g0->SetXTitle("M_{X17} [MeV]"); g0->SetYTitle("g_{Ve}^{2}");
  g1->SetXTitle("M_{X17} [MeV]"); g1->SetYTitle("g_{Ve}^{2}");
  g2->SetXTitle("M_{X17} [MeV]"); g2->SetYTitle("g_{Ve}^{2}");
  g3->SetXTitle("M_{X17} [MeV]"); g3->SetYTitle("g_{Ve}^{2}");
  g4->SetXTitle("M_{X17} [MeV]"); g4->SetYTitle("g_{Ve}^{2}");
  g5->SetXTitle("M_{X17} [MeV]"); g5->SetYTitle("g_{Ve}^{2}");
  g6->SetXTitle("M_{X17} [MeV]"); g6->SetYTitle("g_{Ve}^{2}");
  g7->SetXTitle("M_{X17} [MeV]"); g7->SetYTitle("g_{Ve}^{2}");
  g8->SetXTitle("M_{X17} [MeV]"); g8->SetYTitle("g_{Ve}^{2}");
  g9->SetXTitle("M_{X17} [MeV]"); g9->SetYTitle("g_{Ve}^{2}");
  g10->SetXTitle("M_{X17} [MeV]"); g10->SetYTitle("g_{Ve}^{2}");
  g11->SetXTitle("M_{X17} [MeV]"); g11->SetYTitle("g_{Ve}^{2}");
  g12->SetXTitle("M_{X17} [MeV]"); g12->SetYTitle("g_{Ve}^{2}");
  g13->SetXTitle("M_{X17} [MeV]"); g13->SetYTitle("g_{Ve}^{2}");
  g14->SetXTitle("M_{X17} [MeV]"); g14->SetYTitle("g_{Ve}^{2}");
  g15->SetXTitle("M_{X17} [MeV]"); g15->SetYTitle("g_{Ve}^{2}");
  g16->SetXTitle("M_{X17} [MeV]"); g16->SetYTitle("g_{Ve}^{2}");
  g17->SetXTitle("M_{X17} [MeV]"); g17->SetYTitle("g_{Ve}^{2}");
  g18->SetXTitle("M_{X17} [MeV]"); g18->SetYTitle("g_{Ve}^{2}");
  g19->SetXTitle("M_{X17} [MeV]"); g19->SetYTitle("g_{Ve}^{2}");
  g20->SetXTitle("M_{X17} [MeV]"); g20->SetYTitle("g_{Ve}^{2}");*/


  /*h0->SetMaximum(1e5); h0->SetMinimum(1);
  h1->SetMaximum(1e5); h1->SetMinimum(1);
  h2->SetMaximum(1e5); h2->SetMinimum(1);
  h3->SetMaximum(1e5); h3->SetMinimum(1);
  h4->SetMaximum(1e5); h4->SetMinimum(1);
  h5->SetMaximum(1e5); h5->SetMinimum(1);
  h6->SetMaximum(1e5); h6->SetMinimum(1);
  h7->SetMaximum(1e5); h7->SetMinimum(1);
  h8->SetMaximum(1e5); h8->SetMinimum(1);
  h9->SetMaximum(1e5); h9->SetMinimum(1);
  h10->SetMaximum(1e5); h10->SetMinimum(1);
  h11->SetMaximum(1e5); h11->SetMinimum(1);
  h12->SetMaximum(1e5); h12->SetMinimum(1);
  h13->SetMaximum(1e5); h13->SetMinimum(1);
  h14->SetMaximum(1e5); h14->SetMinimum(1);
  h15->SetMaximum(1e5); h15->SetMinimum(1);
  h16->SetMaximum(1e5); h16->SetMinimum(1);
  h17->SetMaximum(1e5); h17->SetMinimum(1);
  h18->SetMaximum(1e5); h18->SetMinimum(1);
  h19->SetMaximum(1e5); h19->SetMinimum(1);
  h20->SetMaximum(1e5); h20->SetMinimum(1);

  g0->SetMaximum(1e-3); g0->SetMinimum(1e-10);
  g1->SetMaximum(1e-3); g1->SetMinimum(1e-10);
  g2->SetMaximum(1e-3); g2->SetMinimum(1e-10);
  g3->SetMaximum(1e-3); g3->SetMinimum(1e-10);
  g4->SetMaximum(1e-3); g4->SetMinimum(1e-10);
  g5->SetMaximum(1e-3); g5->SetMinimum(1e-10);
  g6->SetMaximum(1e-3); g6->SetMinimum(1e-10);
  g7->SetMaximum(1e-3); g7->SetMinimum(1e-10);
  g8->SetMaximum(1e-3); g8->SetMinimum(1e-10);
  g9->SetMaximum(1e-3); g9->SetMinimum(1e-10);
  g10->SetMaximum(1e-3); g10->SetMinimum(1e-10);
  g11->SetMaximum(1e-3); g11->SetMinimum(1e-10);
  g12->SetMaximum(1e-3); g12->SetMinimum(1e-10);
  g13->SetMaximum(1e-3); g13->SetMinimum(1e-10);
  g14->SetMaximum(1e-3); g14->SetMinimum(1e-10);
  g15->SetMaximum(1e-3); g15->SetMinimum(1e-10);
  g16->SetMaximum(1e-3); g16->SetMinimum(1e-10);
  g17->SetMaximum(1e-3); g17->SetMinimum(1e-10);
  g18->SetMaximum(1e-3); g18->SetMinimum(1e-10);
  g19->SetMaximum(1e-3); g19->SetMinimum(1e-10);
  g20->SetMaximum(1e-3); g20->SetMinimum(1e-10);*/

  /*h0->SetLineColor(kOrange+7);
  h1->SetLineColor(kOrange+7);
  h2->SetLineColor(kOrange+7);
  h3->SetLineColor(kOrange+7);
  h4->SetLineColor(kOrange+7);
  h5->SetLineColor(kOrange+7);
  h6->SetLineColor(kOrange+7);
  h7->SetLineColor(kOrange+7);
  h8->SetLineColor(kOrange+7);
  h9->SetLineColor(kOrange+7);
  h10->SetLineColor(kOrange+7);
  h11->SetLineColor(kOrange+7);
  h12->SetLineColor(kOrange+7);
  h13->SetLineColor(kOrange+7);
  h14->SetLineColor(kOrange+7);
  h15->SetLineColor(kOrange+7);
  h16->SetLineColor(kOrange+7);
  h17->SetLineColor(kOrange+7);
  h18->SetLineColor(kOrange+7);
  h19->SetLineColor(kOrange+7);
  h20->SetLineColor(kOrange+7);


  g0->SetLineColor(kGreen-6);
  g1->SetLineColor(kGreen-6);
  g2->SetLineColor(kGreen-6);
  g3->SetLineColor(kGreen-6);
  g4->SetLineColor(kGreen-6);
  g5->SetLineColor(kGreen-6);
  g6->SetLineColor(kGreen-6);
  g7->SetLineColor(kGreen-6);
  g8->SetLineColor(kGreen-6);
  g9->SetLineColor(kGreen-6);
  g10->SetLineColor(kGreen-6);
  g11->SetLineColor(kGreen-6);
  g12->SetLineColor(kGreen-6);
  g13->SetLineColor(kGreen-6);
  g14->SetLineColor(kGreen-6);
  g15->SetLineColor(kGreen-6);
  g16->SetLineColor(kGreen-6);
  g17->SetLineColor(kGreen-6);
  g18->SetLineColor(kGreen-6);
  g19->SetLineColor(kGreen-6);
  g20->SetLineColor(kGreen-6);*/

    
  
  TCanvas *X17_MassScan = new TCanvas("X17_MassScan", "X17 production", 900, 700);
  X17_MassScan->SetGrid();
  //for(int l = 0; l<21; l++){
  //h0[l]->Draw("HIST SAME");
  //}
  Sum_H->Draw("HIST");
  //HStack->Draw("NOSTACK");
  gPad->Update();
  gPad->SetLogy();
  
  TCanvas *gVe_MassScan = new TCanvas("GVe_MassScan", "g_{Ve}^{2}", 900, 700);
  gVe_MassScan->SetGrid();
  for(int l = 0; l<21; l++){
    g0[l]->Draw("HIST SAME");
  }
  //G->Draw("HIST");
  gPad->SetLogy();

  

  }


