//MANCINI MARCO 04-28-2022 20 production
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
  Double_t Positron_InitialP      = 0.282;
  Double_t Electron_initialEnergy = 0.;
  Double_t ElectronMass           = 0.000511;
  Double_t GammaMass              = 0.;

  Double_t InitialEnergy_positron[1000000];
  Double_t SFinalEn_positron[1000000], SFinalEn_electron[1000000], TFinalEn_positron[1000000], TFinalEn_electron[1000000];
  Double_t SFinal_PosMomentum[1000000], SFinal_ElecMomentum[1000000];
  Double_t TFinal_PosMomentum[1000000], TFinal_ElecMomentum[1000000];
  Double_t SE_tot[1000000], TE_tot[1000000];
  Double_t Stheta_scattering[1000000], Ttheta_scattering[1000000];
  Double_t Stheta_test[1000000], Ttheta_test[1000000];

  Double_t FinalEn_Fp[1000000], FinalEn_Fe[1000000];
  Double_t Final_FpMomentum[1000000], Final_FeMomentum[1000000];
  Double_t FullE_tot[1000000];
  Double_t Fulltheta_scattering[1000000];
  Double_t Fulltheta_test[1000000];

  Double_t theta_test[1000000];
  
  Double_t Pos_Stheta_x[1000000], Pos_Sphi_y[1000000], Pos_Ttheta_x[1000000], Pos_Tphi_y[1000000]; //angoli di deviazione dei prodotti di reazione rispetto alla direzione x e y
  Double_t Elec_Stheta_x[1000000], Elec_Sphi_y[1000000], Elec_Ttheta_x[1000000], Elec_Tphi_y[1000000];
  Double_t Fp_theta_x[1000000], Fp_phi_y[1000000], Fe_theta_x[1000000], Fe_phi_y[1000000];
  Double_t TargetCalo = 3500; //in mm 20230213 DeltaZ target-ECal  
  //Double_t semiL = 50; //lato mezzi del quadrato centrale
  //Double_t geometry_radius = 300; //raggio geometrico del calorimetro

  Double_t Pos_Sx[1000000], Pos_Sy[1000000], Elec_Sx[1000000], Elec_Sy[1000000]; //coordinate sul calorimentro nel piano trasverso al fascio xy
  Double_t Pos_Tx[1000000], Pos_Ty[1000000], Elec_Tx[1000000], Elec_Ty[1000000];
  Double_t Fp_x[1000000], Fp_y[1000000], Fe_x[1000000], Fe_y[1000000];
  
  TFile *fBhabhaSCh  = new TFile("/home/marco/analysis/SM_analysis/CalcHepFiles/bhabhaSch_430MeV.root"); 
  TFile *fBhabhaFull = new TFile("/home/marco/analysis/SM_analysis/CalcHepFiles/bhabhaFull_430MeV.root"); 

  TTree *tBhabhaSCh  = (TTree*)fBhabhaSCh->Get("tBhabhaSCh"); 
  TTree *tBhabhaFull = (TTree*)fBhabhaFull->Get("tBhabhaFull"); 
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
  tBhabhaFull->SetBranchAddress("FullFinalP1_electron", &FinalP1_Fe);
  tBhabhaFull->SetBranchAddress("FullFinalP2_electron", &FinalP2_Fe);
  tBhabhaFull->SetBranchAddress("FullFinalP3_electron", &FinalP3_Fe);
  tBhabhaFull->SetBranchAddress("FullFinalP1_positron", &FinalP1_Fp);
  tBhabhaFull->SetBranchAddress("FullFinalP2_positron", &FinalP2_Fp);
  tBhabhaFull->SetBranchAddress("FullFinalP3_positron", &FinalP3_Fp);
  
  Long64_t nentries = tBhabhaFull->GetEntries();
  cout<<nentries<<endl;
  
  for(Long64_t k=0; k<nentries; k++){
    tBhabhaSCh->GetEntry(k);
    tBhabhaFull->GetEntry(k);

    //INITIAL STATE -> T,S CHANNEL and Ee->AA HAVE SAME INITIAL STATE
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
  Double_t PoT[20] = {1e10, 2e10, 3e10, 4e10, 5e10, 6e10, 7e10, 8e10, 9e10, 10e10, 11e10, 12e10, 13e10, 14e10, 15e10, 16e10, 17e10, 18e10, 19e10, 2e11};
  Double_t Na  = 6.022e23;
  Double_t Z_C = 6;
  Double_t D_C = 3.5;
  Double_t A_C = 12;
  Double_t TThickness = 0.01; //in cm
  Double_t CmtoPb = 1e-36;

  Double_t PreFactor_X20 = 3.8e-7; //DATAS FROM PROCEEDINGS DARME-NARDI
  Double_t g_ve[10]      = {1e-4, 2e-4, 3e-4, 4e-4, 5e-4, 6e-4, 7e-4, 8e-4, 9e-4, 10e-4} ;

  //to be checked with new CalcHep simulations 20230214
  Double_t Sigma_Full = 5.115879e11; //pb
  Double_t Sigma_S    = 3.014359e8;  //pb

  Double_t Luminosity_C;
  Double_t N_Full_C[20], N_Schannel_C[20], N_X20_C[20][10];

  Luminosity_C = CmtoPb * Na * Z_C * D_C * TThickness / A_C;

  for(int k = 0; k<20; k++){
    N_Full_C[k]     = Luminosity_C * Sigma_Full * PoT[k];
    N_Schannel_C[k] = Luminosity_C * Sigma_S * PoT[k]; 
  
    for(int i = 0; i<10; i++){
      N_X20_C[k][i] = PoT[k] * PreFactor_X20 * ((g_ve[i]*g_ve[i])/(9e-8)) * (1/0.7) * ((Na * Z_C * D_C)/(A_C * 1e24)); //EQUATION 13 Proceedings LUC  <- il primo indice sono i POT, il secondo il valore del COUPLING g_ve
    }
  }

  
  //ofstream file;
  //file.open("dataPaper.txt");
  
  cout<<"######################  PADME X20 SEARCH  ############################"<<endl;

  Double_t En_min[23]; // fissata energia del secondo valore a 0 MeV 
  Double_t Radius_min = 90.; 
  //Double_t MagnetShadow = 256.;//modified
  Double_t Radius_max[21] = {180., 185., 190., 195., 200., 205., 210., 215., 220., 225., 230., 235., 240., 245., 250., 255., 260., 265., 400., 275., 280.}; //modified
  Double_t Radius_SPos[Nev_CalcHep], Radius_SElec[Nev_CalcHep], Radius_TPos[Nev_CalcHep], Radius_TElec[Nev_CalcHep], Radius_Fp[Nev_CalcHep], Radius_Fe[Nev_CalcHep], Radius_G1[Nev_CalcHep], Radius_G2[Nev_CalcHep];
  Double_t N_PADME_S[21][28], N_PADME_X20[21][28];
  Double_t NC_PADME_S[20],NC_PADME_Full[20], NC_PADME_X20[20][10];
  Double_t Squared_gve[20];
  Double_t Scan_S[21][23] ={0.}{0.}, Scan_Full[21][23]={0.}{0.};
  Double_t S_ACC[21][23] = {0.}{0.}, Full_ACC[21][23]={0.}{0.};
  Double_t Sensitivity[20][10], LowerLimitX20[20], X20_produced[20];
  Double_t Acceptance_ratio[21][23];  
  double a = 0.;
  for(Int_t l= 0; l<21; l++){ //indice di raggio massimo
    for(Int_t j = 0; j<23; j++){ //indice di energia
      Scan_S[l][j] = 0.;
      Scan_T[l][j] = 0.;
      Scan_Full[l][j] = 0.;
      S_ACC[l][j] = 0.;
      T_ACC[l][j] = 0.;
      Full_ACC[l][j] = 0.;
    }
  }

  for(Int_t j = 0; j<23; j++){
    En_min[j] = a;
    a = a +0.0005; 
  }
			   
  for(Int_t k=0; k<numRow; k++){   
    Radius_SPos[k]  = TMath::Sqrt(Pos_Sx[k]*Pos_Sx[k] + Pos_Sy[k]*Pos_Sy[k]);
    Radius_SElec[k] = TMath::Sqrt(Elec_Sx[k]*Elec_Sx[k] + Elec_Sy[k]*Elec_Sy[k]);
    Radius_Fp[k]    = TMath::Sqrt(Fp_x[k]*Fp_x[k] + Fp_y[k]*Fp_y[k]);
    Radius_Fe[k]    = TMath::Sqrt(Fe_x[k]*Fe_x[k] + Fe_y[k]*Fe_y[k]);

  }
  
  //MAKE A COUNTER - GEOMETRY CUTS
  int CaloRes = 30; //modified
  int res_occ = 45; //modified
  TH2F *histS_Pos_tot = new TH2F("histS_Pos_tot", "S-Channel positron transverse position", res_occ, -450, 450, res_occ, -450, 450); //istogramma 2d per tutti gli eventi 
  TH2F *histS_Elec_tot = new TH2F("histS_Elec_tot", "S-Channel electron transverse position", res_occ, -450, 450, res_occ, -450, 450);  //istogramma 2d per tutti gli eventi 
  TH2F *histFull_Pos_tot = new TH2F("histFull_Pos_tot", "Bhabha full positron transverse position", res_occ, -450, 450, res_occ, -450, 450); //istogramma 2d con tagli geometrici e energetici
  TH2F *histFull_Elec_tot = new TH2F("histFull_Elec_tot", "Bhabha full electron  transverse position", res_occ, -450, 450, res_occ, -450, 450);
  
  TH2F *h9 = new TH2F("h9", "S-Channel positron R_{max} = 270 mm", res_occ-5, -410, 390, res_occ-5, -410, 390); //MODIFIED  //istogramma 2d per tutti gli eventi con soli tagli geometrici
  TH2F *h10 = new TH2F("h10", "S-Channel electron R_{max} = 270 mm", CaloRes, -300, 300, CaloRes, -300, 300);

  TH2F *h11 = new TH2F("h11", "S-Channel positron R_{max} = 270 mm and E>100 MeV", CaloRes, -300, 300, CaloRes, -300, 300);  //istogramma 2d con tagli geometrici e energetici
  TH2F *h12 = new TH2F("h12", "S-Channel electron R_{max} = 270 mm and E>100 MeV", CaloRes, -300, 300, CaloRes, -300, 300);
  
  TH2F *h17 = new TH2F("h17", "Bhabha full positron final position R_{max} = 270 mm", res_occ-5, -410, 390, res_occ-5, -410, 390); //MODIFIED//istogramma 2d per tutti gli eventi con soli tagli geometrici
  TH2F *h18 = new TH2F("h18", "Bhabha full electron final position R_{max} = 270 mm", CaloRes, -300, 300, CaloRes, -300, 300);

  TH2F *h19 = new TH2F("h19", "Bhabha full final position R_{max} = 270 mm and E>100 MeV", CaloRes, -300, 300, CaloRes, -300, 300);  //istogramma 2d con tagli geometrici e energetici
  TH2F *h20 = new TH2F("h20", "Bhabha full final position R_{max} = 270 mm and E>100 MeV", CaloRes, -300, 300, CaloRes, -300, 300);

  TH1F *ACC_S = new TH1F("ACC_S", "PADME S Channel Acceptance as a function of R_{max} and E > 100 MeV", 16, 202.5, 282.5);
  TH1F *ACC_Full = new TH1F("ACC_Full", "PADME Bhabha full Acceptance as a function of R_{max} and E > 100 MeV", 16, 202.5, 282.5);
  TH2F *ACC_RATIO = new TH2F("ACC_RATIO", "Ratio between S- and Full Acceptance as a function of R_{max} and energy", 21, 177.5, 282.5, 23, 0.0275, 0.1475);

  TH1F *HS = new TH1F("HS", "S-chanel e^{+} final position distribution", 60, 0, 300);
  TH1F *HFull = new TH1F("HFull", "Bhabha e^{+} final position distribution", 60, 0, 300);
  TH1F *HS1 = new TH1F("HS1", "S-channel e^{-} final position distribution", 60, 0, 300);
  TH1F *HFull1 = new TH1F("HFull1", "Bhabha e^{-} final position distribution", 60, 0, 300);

  ACC_S->SetTitle("PADME S Channel Acceptance as a function of R_{max} and E > 100 MeV; R_{max} [mm]; Acceptance");
  ACC_Full->SetTitle("PADME Bhabha full Acceptance as a function of R_{max} and E > 100 MeV; R_{max} [mm]; Acceptance");

  // TDatime date;
  // Int_t day = date.GetDate();
  // TFile *BhabhaOut = new TFile(Form("BhabhaOut_%i.root", day),"recreate");
  
  for(Int_t l = 0; l<21; l++){
    //file<<"---------------------R_{max}= "<<Radius_max[l]<<" mm "<<"-------------\n";
    for(Int_t j = 0; j<23; j++){
      //file<<"--------------DATA E> "<<En_min[j]<<" GeV "<<"-----------\n";
      for(Int_t k = 0; k<numRow; k++){
           
	if(l == 18 && j == 21){
	  histS_Pos_tot->Fill(Pos_Sx[k], Pos_Sy[k]);
	  histS_Elec_tot->Fill(Elec_Sx[k], Elec_Sy[k]);
	  histFull_Pos_tot->Fill(Fp_x[k], Fp_y[k]);
	  histFull_Elec_tot->Fill(Fe_x[k], Fe_y[k]);
	  HS->Fill(Radius_SPos[k]);
	  HFull->Fill(Radius_Fp[k]);
	  HS1->Fill(Radius_SElec[k]);
	  HFull1->Fill(Radius_Fe[k]);

	  if(Radius_SPos[k]>Radius_min && Radius_SElec[k]>Radius_min){
	    if(Radius_SPos[k]<Radius_max[l] && Radius_SElec[k]<Radius_max[l]){h9->Fill(Pos_Sx[k], Pos_Sy[k]); h10->Fill(Elec_Sx[k], Elec_Sy[k]);
	      if(SFinalEn_positron[k]>En_min[j] && SFinalEn_electron[k]>En_min[j]){h11->Fill(Pos_Sx[k], Pos_Sy[k]); h12->Fill(Elec_Sx[k], Elec_Sy[k]);
  		  Scan_S[l][j]++;}}}
      
	  if(Radius_Fp[k]>Radius_min && Radius_Fe[k]>Radius_min){
	    if(Radius_Fp[k]<Radius_max[l] && Radius_Fe[k]<Radius_max[l]){h17->Fill(Fp_x[k], Fp_y[k]); h18->Fill(Fe_x[k], Fe_y[k]);
	      if(FinalEn_Fp[k]>En_min[j] && FinalEn_Fe[k]>En_min[j]){h19->Fill(Fp_x[k], Fp_y[k]); h20->Fill(Fe_x[k], Fe_y[k]);
  		  Scan_Full[l][j]++;}}}

	else {

	  if(Radius_SPos[k]>Radius_min && Radius_SElec[k]>Radius_min){
	    if(Radius_SPos[k]<Radius_max[l] && Radius_SElec[k]<Radius_max[l]){
	      if(SFinalEn_positron[k]>En_min[j] && SFinalEn_electron[k]>En_min[j]){
		    Scan_S[l][j]++;}}}
      

	  if(Radius_Fp[k]>Radius_min && Radius_Fe[k]>Radius_min){
	    if(Radius_Fp[k]<Radius_max[l] && Radius_Fe[k]<Radius_max[l]){
	      if(FinalEn_Fp[k]>En_min[j] && FinalEn_Fe[k]>En_min[j]){
		    Scan_Full[l][j]++;}}}

	}


	S_ACC[l][j] = Scan_S[l][j]/numRow;
	Full_ACC[l][j] = Scan_Full[l][j]/numRow;
	
	//cout<<"SCAN S-channel ACCEPTANCE = "<<S_ACC[l][j]<<endl;
	//cout<<"SCAN T-channel ACCEPTANCE = "<<T_ACC[l][j]<<endl;
	//cout<<"SCAN Gamma-Gamma Production ACCEPTANCE = "<<AA_ACC[l][j]<<endl;
	//cout<<"N_tot S-channel : "<<N_PADME_S[l][j]<<endl;
	//cout<<"N_tot T-channel : "<<N_PADME_T[l][j]<<endl;
	//cout<<"N_tot Gamma-Gamma : "<<N_PADME_AA[l][j]<<endl;    
	//cout<<"N_tot X20 : "<<NC_PADME_X20[l][j]<<endl;
  
      }
    }
    ACC_S->Fill(Radius_max[l], S_ACC[l][20]);
    ACC_Full->Fill(Radius_max[l], Full_ACC[l][20]);
  }
  
  //running PoT adn coupling
  TH2F * SENS = new TH2F("SENS", "PADME X20 detected over sqrt(BG) as a function of N-PoT and g_{Ve}", 20, 0.5e10, 2.05e11, 10, 0.5e-4, 1.05e-3);
  TH1F * SENSrad = new TH1F("SENSrad", "PADME Sensitivity as a function of R_{max}", 21, 177.5, 282.5);
  TH1F * DPOT = new TH1F("DPOT", "PADME X20 discovery potential as a function of N-PoT", 20, 0.5e10, 2.05e11);
  TH1F * CL90 = new TH1F("CL90", "PADME 90% CL as a function of N-PoT", 20, 0.5e10, 2.05e11);
  TH1F * exclusion_X20 = new TH1F("exclusion_X20", "g_{V_{e}} exclusion plot as a function of N-PoT", 20, 0.5e10, 2.05e11);

  //NA64 epsilon limit
  // TH1F * NA64_limit = new TH1F("NA64_limit", "g_{e} NA64", 20, 0.5e10, 2.05e11);
  // Double_t ge_NA64[20], 
  Double_t Disc[20];


  cout<<"SChannel Acc : "<<S_ACC[18][20]<<endl;
  cout<<"Bhabha Full Acc : "<<Full_ACC[18][20]<<endl;

  double SensRad[21];
  
  for(Int_t l = 0; l<20; l++){ //POT
    cout<<"\n ---------------------PoT = "<<PoT[l]<<" -------------"<<endl;
    NC_PADME_Full[l] = Full_ACC[18][20] * N_Full_C[l];
    NC_PADME_S[l] = S_ACC[18][20] * N_Schannel_C[l];
    
    // ge_NA64[l] = 2e-4;

    LowerLimitX20[l]= 1.3*TMath::Sqrt(NC_PADME_Full[l]); //bhabha full con 90CL 1.3sigma
    X20_produced[l] = LowerLimitX20[l] / S_ACC[18][20]; //il numero di candidati X20 entro il calorimetro -> 90CL * Accettanza S Channel
    Squared_gve[l]= TMath::Sqrt((X20_produced[l] * 0.7)/(PoT[l]*PreFactor_X20)*(9e-8) * ((A_C * 1e24)/(Na * Z_C * D_C)));
    CL90->Fill(PoT[l], LowerLimitX20[l]);
    exclusion_X20->Fill(PoT[l], Squared_gve[l]);
    // NA64_limit->Fill(PoT[l], ge_NA64[l]);

    cout<<"N_tot S-channel : "<<NC_PADME_S[l]<<endl;
    cout<<"N_tot Bhabha full : "<<NC_PADME_Full[l]<<endl;
    // cout<<"90% Confidence level : "<<LowerLimitX20[l]<<endl; 
    // cout<<"Minimal g_Ve^{2}: "<<Squared_gve[l]<<endl; 
    
    for(Int_t j = 0; j<10; j++){ //EPSILON
      cout<<"--------------g_Ve = "<<g_ve[j]<<" -----------"<<endl;

      NC_PADME_X20[l][j] = S_ACC[18][20] * N_X20_C[l][j]; // questi sono il numero di X20 prodotti dalla formula 13_proceedings moltiplicati per S_Accettanza -> QUELLI RIVELATI DAL CALO
      Sensitivity[l][j] = NC_PADME_X20[l][j] / TMath::Sqrt(NC_PADME_X20[l][j] + NC_PADME_Full[l]); //tiene conto di tutte le accettanze
      if(Sensitivity[l][j]<5){Disc[l] = g_ve[j];}
      SENS->Fill(PoT[l], g_ve[j], Sensitivity[l][j]);
      
      cout<<"N_tot X20 : "<<NC_PADME_X20[l][j]<<endl;
      //file<<"Sensitivity : "<<Sensitivity[l][j]<<"\n";
      
    }
    DPOT->Fill(PoT[l],Disc[l]);
  }

  for(Int_t k=0; k<21; k++){
    SensRad[k] = (S_ACC[k][20] * N_X20_C[0][1]) / TMath::Sqrt((Full_ACC[k][20]*N_Full_C[0]) + (S_ACC[k][20] * N_X20_C[0][1]) ); //1e10 POT - g_Ve = 2e-4 - scan sensitivity in R_max
    if(TMath::IsNaN(SensRad[k])) {SensRad[k] = 0.;}
    SENSrad->Fill(Radius_max[k], SensRad[k]);
    cout<<"Sensitivity as a function of Rmax: "<<SensRad[k]<<endl;
  }
  
  //file.close();

  double num = 0.;
  double den = 0.;
  TH2D *hSensECal = new TH2D("hSensECal", "Sensitivity", res_occ-5, -400, 400, res_occ-5, -400, 400); //modified
  for(int xECal = 1; xECal<res_occ-5; xECal++){ //modified
    for(int yECal = 1; yECal<res_occ-5; yECal++){ //modified
      // if(xECal>17 && xECal<23){
      // 	if(yECal>17 && yECal<23){
      // 	  continue;
      // 	}
      // }
      num = h9->GetBinContent(xECal, yECal)*1e-6*N_X20_C[0][1];
      den = TMath::Sqrt(h17->GetBinContent(xECal,yECal)*1e-6*N_Full_C[0] + hg1->GetBinContent(xECal, yECal)*1e-6*N_AA_C[0]); //modified
      hSensECal->Fill((-400 + 20*xECal), (-400 + 20*yECal), (num/den));
      if(xECal==0) cout<<"num "<<num<<" - den "<<den<<" - sens: "<<num/den<<endl;
      // hSensECal->Fill(xECal, yECal, (num/den));
      num = 0.;
      den = 0.;
    }
  }
  hSensECal->Draw("colz");

  //DEFINITION OF HISTOGRAMS
  TH1F *h1 = new TH1F ("h1", "T-, S-channel and Bhabha-full tri-momentum distribution", 282, -0.0005, 0.2815);
  TH1F *h2 = new TH1F ("h2", "Electron 3P-distribution in S-Channel", 282, -0.0005, 0.2815);
  TH1F *h1Full = new TH1F ("h1Full", "Positron Full 3P-distribution", 282, -0.0005, 0.2815);
  TH1F *h2Full = new TH1F ("h2Full", "Electron Full 3P-distribution", 282, -0.0005, 0.2815);
  
  TH1F *h5 = new TH1F ("h5", "S-channel and Bhabha-full energy distribution", 282, -0.0005, 0.2815);
  TH1F *h6 = new TH1F ("h6", "Electron energy distribution in S-Channel", 282, -0.0005, 0.2815);
  TH1F *h3Full = new TH1F ("h3Full", "Positron Full energy distribution", 282, -0.0005, 0.2815);
  TH1F *h4Full = new TH1F ("h4Full", "Electron Full energy distribution", 282, -0.0005, 0.2815);
  
  TH1F *check1 = new TH1F("check1", "Final total energy S-Channel", 500, 0., 0.5); 
  TH1F *checkFull = new TH1F("checkFull", "Final energy Bhabha Full", 500, 0., 0.5);

  TH1F *htheta1 = new TH1F("htheta1", "S-Channel Angular distribution", 6280, -0.005, 6.285); 
  TH1F *hthetaFull = new TH1F("hthetaFull", "Angular distribution Bhabha Full", 6280, -0.005, 6.285);
  
  for(Int_t k=0; k<numRow; k++){
    h1->Fill(SFinal_PosMomentum[k]); //distribuzione del 3-impulso finale - positroni S
    h2->Fill(SFinal_ElecMomentum[k]); //distribuzione del 3-impulso finale - elettroni S
    h5->Fill(SFinalEn_positron[k]); //distribuzione energia finale - positroni S
    h6->Fill(SFinalEn_electron[k]); //distribuzione energia finale - elettroni S
    check1->Fill(SE_tot[k]); //conservazione dell'energia CHECK
    htheta1->Fill(Stheta_scattering[k]); //distribuzione angolare finale positrone-elettrone S
    h1Full->Fill(Final_FpMomentum[k]); 
    h2Full->Fill(Final_FeMomentum[k]); 
    h3Full->Fill(FinalEn_Fp[k]); 
    h4Full->Fill(FinalEn_Fe[k]); 
    checkFull->Fill(FullE_tot[k]); 
    hthetaFull->Fill(Fulltheta_scattering[k]);
    
  }
  
  h1->SetXTitle("3-P [GeV]");
  h2->SetXTitle("3-P [GeV]");
  h5->SetXTitle("E [GeV]");
  h6->SetXTitle("E [GeV]");
  htheta1->SetXTitle("#theta [rad]");
  h1Full->SetXTitle("3-P [GeV]");
  h2Full->SetXTitle("3-P [GeV]");
  h3Full->SetXTitle("E [GeV]");
  h4Full->SetXTitle("E [GeV]");
  hthetaFull->SetXTitle("#theta [rad]");
 

  histS_Pos_tot->SetXTitle("y [mm]"); histS_Pos_tot->SetYTitle("x [mm]");
  histS_Elec_tot->SetXTitle("y [mm]"); histS_Elec_tot->SetYTitle("x [mm]");
  histFull_Pos_tot->SetXTitle("y [mm]"); histFull_Pos_tot->SetYTitle("x [mm]");
  histFull_Elec_tot->SetXTitle("y [mm]"); histFull_Elec_tot->SetYTitle("x [mm]");
  HS->SetXTitle("R_{e^{+}} [mm]"); HS->SetYTitle("counts");
  HFull->SetXTitle("R_{e^{+}} [mm]"); HFull->SetYTitle("counts");
  HS1->SetXTitle("R_{e^{-}} [mm]"); HS1->SetYTitle("counts");
  HFull1->SetXTitle("R_{e^{-}} [mm]"); HFull1->SetYTitle("counts");
  h9->SetXTitle("y [mm]"); h9->SetYTitle("x [mm]");
  h10->SetXTitle("y [mm]"); h10->SetYTitle("x [mm]");
  h11->SetXTitle("y [mm]"); h11->SetYTitle("x [mm]");
  h12->SetXTitle("y [mm]"); h12->SetYTitle("x [mm]");
  h17->SetXTitle("y [mm]"); h17->SetYTitle("x [mm]");
  h18->SetXTitle("y [mm]"); h18->SetYTitle("x [mm]");
  h19->SetXTitle("y [mm]"); h19->SetYTitle("x [mm]");
  h20->SetXTitle("y [mm]"); h20->SetYTitle("x [mm]");

  histS_Pos_tot->GetYaxis()->SetTitleOffset(1.15);
  histS_Elec_tot->GetYaxis()->SetTitleOffset(1.15);
  histFull_Pos_tot->GetYaxis()->SetTitleOffset(1.15);
  histFull_Elec_tot->GetYaxis()->SetTitleOffset(1.15);
  h9->GetYaxis()->SetTitleOffset(1.15); 
  h10->GetYaxis()->SetTitleOffset(1.15); 
  h11->GetYaxis()->SetTitleOffset(1.15); 
  h12->GetYaxis()->SetTitleOffset(1.15); 
  h17->GetYaxis()->SetTitleOffset(1.15);
  h18->GetYaxis()->SetTitleOffset(1.15); 
  h19->GetYaxis()->SetTitleOffset(1.15); 
  h20->GetYaxis()->SetTitleOffset(1.15);


  ACC_S->SetXTitle("R_{max} [mm]"); ACC_S->SetYTitle("Acceptance");
  ACC_Full->SetXTitle("R_{max} [mm]"); ACC_Full->SetYTitle("Acceptance");
  ACC_RATIO->SetXTitle("R_{max} [mm]"); ACC_RATIO->SetYTitle("E_{min} [GeV]");
  SENS->SetXTitle("N-PoT"); SENS->SetYTitle("g_{Ve}");
  CL90->SetXTitle("N-PoT"); CL90->SetYTitle("X_{17}^{min}");
  exclusion_X20->SetXTitle("N-PoT"); exclusion_X20->SetYTitle("g_{V_{e}}");
  // NA64_limit->SetXTitle("N-PoT"); NA64_limit->SetYTitle("g_{V_{e}}");
  
  
  h1->Scale((1./(h1->Integral()))*Sigma_S);
  h2->Scale(1./(h2->Integral()));
  h5->Scale((1./(h5->Integral()))*Sigma_S);
  h6->Scale(1./(h6->Integral()));
  htheta1->Scale(1./(htheta1->Integral()));
  h1Full->Scale((1./(h1Full->Integral()))*Sigma_Full);
  h2Full->Scale(1./(h2Full->Integral()));
  h3Full->Scale((1./(h3Full->Integral()))*Sigma_Full);
  h4Full->Scale(1./(h4Full->Integral()));
  hthetaFull->Scale(1./(hthetaFull->Integral()));
  h9->Scale(1./(h9->Integral())*Sigma_S);
  h10->Scale(1./(h10->Integral())*Sigma_S);
  h11->Scale(1./(h11->Integral())*Sigma_S);
  h12->Scale(1./(h12->Integral())*Sigma_S);
  h17->Scale(1./(h17->Integral())*Sigma_Full);
  h18->Scale(1./(h18->Integral())*Sigma_Full);
  h19->Scale(1./(h19->Integral())*Sigma_Full);
  h20->Scale(1./(h20->Integral())*Sigma_Full);

  /*HS->Scale(1./(HS->Integral()));
    HT->Scale(1./(HT->Integral()));
    HA->Scale(1./(HA->Integral()));
    HS1->Scale(1./(HS1->Integral()));
    HT1->Scale(1./(HT1->Integral()));
    HA1->Scale(1./(HA1->Integral()));
    HScut->Scale(1./(HScut->Integral()));
    HTcut->Scale(1./(HTcut->Integral()));
    HAcut->Scale(1./(HAcut->Integral()));
    HS1cut->Scale(1./(HS1cut->Integral()));
    HT1cut->Scale(1./(HT1cut->Integral()));
    HA1cut->Scale(1./(HA1cut->Integral()));*/


  h1->SetMaximum(1); h1->SetMinimum(1e-7);
  h2->SetMaximum(1); h2->SetMinimum(1e-7);
  h5->SetMaximum(1); h5->SetMinimum(1e-7);
  h6->SetMaximum(1); h6->SetMinimum(1e-7);
  htheta1->SetMaximum(1); htheta1->SetMinimum(1e-7);
  h1Full->SetMaximum(1); h1Full->SetMinimum(1e-7);
  h2Full->SetMaximum(1); h2Full->SetMinimum(1e-7);
  h3Full->SetMaximum(1); h3Full->SetMinimum(1e-7);
  h4Full->SetMaximum(1); h4Full->SetMinimum(1e-7);
  hthetaFull->SetMaximum(1); hthetaFull->SetMinimum(1e-7);
  exclusion_X20->SetMaximum(4e-4); exclusion_X20->SetMinimum(9e-5);
  // NA64_limit->SetMaximum(4e-4); NA64_limit->SetMinimum(9e-5);


  h1->SetLineColor(kRed);
  h2->SetLineColor(kRed+3); 
  h5->SetLineColor(kRed); 
  h6->SetLineColor(kRed+3);
  h1->SetFillColor(kRed); h1->SetFillStyle(3003);
  h2->SetFillColor(kRed+3); h2->SetFillStyle(3003);
  h5->SetFillColor(kRed); h5->SetFillStyle(3003);
  h6->SetFillColor(kRed+3); h6->SetFillStyle(3003);
  check1->SetLineColor(kRed+4);
  htheta1->SetLineColor(kRed+4);
  h1Full->SetLineColor(kCyan); 
  h2Full->SetLineColor(kCyan+4); 
  h3Full->SetLineColor(kCyan); 
  h4Full->SetLineColor(kCyan+4);
  h1Full->SetFillColor(kCyan); h1Full->SetFillStyle(3003);
  h2Full->SetFillColor(kCyan+4); h2Full->SetFillStyle(3003);
  h3Full->SetFillColor(kCyan); h3Full->SetFillStyle(3003);
  h4Full->SetFillColor(kCyan+4); h4Full->SetFillStyle(3003);
  checkFull->SetLineColor(kCyan+4);
  hthetaFull->SetLineColor(kCyan+4);

  
  ACC_S->SetFillColor(kRed);
  ACC_Full->SetFillColor(kCyan);
  CL90->SetLineColor(kOrange+7);
  CL90->SetFillColor(kOrange+7); CL90->SetFillStyle(3003);
  exclusion_X20->SetLineColor(kGreen-10); exclusion_X20->SetFillColor(10); exclusion_X20->SetFillStyle(1001); 
  // NA64_limit->SetLineColor(kRed);

  HS->SetLineColor(kRed); 
  HFull->SetLineColor(kBlue);
  HS1->SetLineColor(kRed);
  HFull1->SetLineColor(kBlue);

  HS->SetMaximum(5e5); HS->SetMinimum(1);
  HFull->SetMaximum(5e5); HFull->SetMinimum(1);
  HS1->SetMaximum(5e5); HS1->SetMinimum(1);
  HFull1->SetMaximum(5e5); HFull1->SetMinimum(1);

  HS->SetFillColor(kRed); HS->SetFillStyle(3003); 
  HFull->SetFillColor(kCyan); HFull->SetFillStyle(3003);
  HS1->SetFillColor(kRed); HS1->SetFillStyle(3003); 
  HFull1->SetFillColor(kCyan); HFull1->SetFillStyle(3003);

  HS->SetLineStyle(7); HS->SetLineWidth(2);
  HFull->SetLineStyle(7); HFull->SetLineWidth(2);
  HS1->SetLineStyle(7); HS1->SetLineWidth(2);
  HFull1->SetLineStyle(7); HFull1->SetLineWidth(2);

  DPOT->SetLineColor(kRed);
  
  h9->SetMarkerStyle(6); h9->SetMarkerColor(kRed-4);
  h10->SetMarkerStyle(6); h10->SetMarkerColor(kGreen-1);
  h11->SetMarkerStyle(6); h11->SetMarkerColor(kRed-4);
  h12->SetMarkerStyle(6); h12->SetMarkerColor(kGreen-1);
  h17->SetMarkerStyle(6); h17->SetMarkerColor(kRed-4);
  h18->SetMarkerStyle(6); h18->SetMarkerColor(kGreen-1);
  h19->SetMarkerStyle(6); h19->SetMarkerColor(kRed-4);
  h20->SetMarkerStyle(6); h20->SetMarkerColor(kGreen-1);

  SENSrad->SetMarkerStyle(8);
  
  TCanvas *C1 = new TCanvas("C1", "3-Momentum BhaBha Scattering->S-Channel distribution", 600, 700);
  C1->Divide(1,2);
  C1->cd(1);
  h1->Draw("HIST");
  gPad->SetLogy();
  C1->cd(2);
  h2->Draw("HIST");
  gPad->SetLogy();

  TCanvas *C1Full = new TCanvas("C1Full", "3-Momentum Bhabha Full distribution", 600, 700);
  C1Full->Divide(1,2);
  C1Full->cd(1);
  h1Full->Draw("HIST");
  gPad->SetLogy();
  C1Full->cd(2);
  h2Full->Draw("HIST");
  gPad->SetLogy();


  TCanvas *C3P = new TCanvas("C3P", "Final 3-Momentum Positron S Channel vs Full ", 900, 700);
  // gStyle->SetOptStat(kFALSE);
  C3P->SetGrid();
  h1->Draw("HIST");
  h1Full->Draw("same HIST");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  gPad->SetLogy();

  TCanvas *CHECK1 = new TCanvas("CHECK1", "Energy conservation check", 900, 700);
  CHECK1->Divide(2,1);
  CHECK1->cd(1);
  check1->Draw("HIST");
  CHECK1->cd(2);
  checkFull->Draw("HIST");

  TCanvas *Ctheta = new TCanvas("Ctheta", "Angle between final particles in CoM frame", 900, 700);
  Ctheta->Divide(2,1);
  Ctheta->cd(1);
  htheta1->Draw("HIST");
  gPad->SetLogy();
  Ctheta->cd(2);
  hthetaFull->Draw("HIST");
  gPad->SetLogy();

  TCanvas *C3 = new TCanvas("C3", "Energy BhaBha Scattering-> S-Channel distribution", 600, 700);
  C3->Divide(1,2);
  C3->cd(1);
  h5->Draw("HIST");
  gPad->SetLogy();
  C3->cd(2);
  h6->Draw("HIST");
  gPad->SetLogy();

  TCanvas *C2Full = new TCanvas("C2Full", "Energy Bhabha Full distribution", 600, 700);
  C2Full->Divide(1,2);
  C2Full->cd(1);
  h3Full->Draw("HIST");
  gPad->SetLogy();
  C2Full->cd(2);
  h4Full->Draw("HIST");
  gPad->SetLogy();


  TCanvas *CEn = new TCanvas("CEn", "Final Energy Positron S Channe, T Channel, Full and #gamma#gamma", 900, 700);
  // gStyle->SetOptStat(kFALSE);
  CEn->SetGrid();
  h5->Draw();
  h3Full->Draw("same HIST");
  gPad->BuildLegend(0.07, 0.77, 0.27, 0.95);
  gPad->SetLogy();

  TCanvas *CHS = new TCanvas("CHS", "S-channel Final Positron distribution", 900, 700);
  gStyle->SetOptStat(kFALSE);
  CHS->SetGrid();
  HS->Draw("hist");
  //HScut->Draw("hist same");
  //gPad->BuildLegend();
  gPad->SetLogy();
 
  TCanvas *CHFull = new TCanvas("CHFull", "Bhabha Full Final Positron distribution", 900, 700);
  gStyle->SetOptStat(kFALSE);
  CHFull->SetGrid();
  HFull->Draw("hist");
  //HAcut->Draw("hist same");
  //gPad->BuildLegend();
  gPad->SetLogy();
 
  TCanvas *CHS1 = new TCanvas("CHS1", "S-channel Final Electron distribution", 900, 700);
  gStyle->SetOptStat(kFALSE);
  CHS1->SetGrid();
  HS1->Draw("hist");
  //HS1cut->Draw("hist same");
  //gPad->BuildLegend();
  gPad->SetLogy();
 
  TCanvas *CHFull1 = new TCanvas("CHFull1", "Bhabha Full Final Electron distribution", 900, 700);
  gStyle->SetOptStat(kFALSE);
  CHFull1->SetGrid();
  HFull1->Draw("hist");
  //HA1cut->Draw("hist same");
  //gPad->BuildLegend();
  gPad->SetLogy();

  TCanvas *C5 = new TCanvas("C5", "Final transverse position at 3500 mm from the target", 1250, 770);
  //gStyle->SetOptStat(kFALSE);
  C5->Divide(2,2);
  C5->cd(1);
  histS_Pos_tot->Draw("colz");
  C5->cd(2);
  histFull_Pos_tot->Draw("colz");
  C5->cd(3);
  histS_Elec_tot->Draw("colz");
  C5->cd(4);
  histFull_Elec_tot->Draw("colz");

  TCanvas *C6 = new TCanvas("C6", "Final transverse position at 3500 mm from the target - R_{max} = 270 mm", 1250, 770);
  //gStyle->SetOptStat(kFALSE);
  C6->Divide(2,2);
  C6->cd(1);
  h9->Draw("colz");
  C6->cd(2);
  h17->Draw("colz");
  C6->cd(3);
  h10->Draw("colz");
  C6->cd(4);
  h18->Draw("colz");
  
  TCanvas *C7 = new TCanvas("C7", "Final transverse position at 3720 mm from the target - R_{max} = 270 mm", 1250, 770);
  //gStyle->SetOptStat(kFALSE);
  C7->Divide(4,2);
  C7->cd(1);
  h11->Draw("colz");
  C7->cd(2);
  h19->Draw("colz");
  C7->cd(3);
  h12->Draw("colz");
  C7->cd(4);
  h20->Draw("colz");

  TCanvas *C8 = new TCanvas("C8", "Final transverse position at 3500 mm from the target - R_{max} = 270 mm & Magnet effect", 1250, 770);
  gStyle->SetOptStat(kTRUE);
  C8->Divide(1,2);
  C8->cd(1); //modified
  h25->Draw("colz");
  C8->cd(2); //modified
  h26->Draw("colz");
    
  TCanvas *Acc1D_S = new TCanvas("Acc1D_S", "PADME S-Channel Acceptance as a function of R_{max} and E>100 MeV", 900, 700);
  Acc1D_S->SetGrid();
  gStyle->SetOptStat(kFALSE);
  ACC_S->Draw("HIST");

  TCanvas *Acc1D_Full = new TCanvas("Acc1D_Full", "PADME Bhabha Full Acceptance as a function of R_{max} and E>100 MeV", 900, 700);
  Acc1D_Full->SetGrid();
  gStyle->SetOptStat(kFALSE);
  ACC_Full->Draw("HIST");

  // TCanvas *Acc_rat = new TCanvas("Acc_rat", "Acceptance ratio as a function of R_{max} and energy", 900, 700);
  // Acc_rat->SetGrid();
  // gStyle->SetOptStat(kFALSE);
  // gStyle->SetPalette(kBird);
  // ACC_RATIO->Draw("colz");

  TCanvas *Sens3D = new TCanvas("Sens3D", "PADME X20 Sensitivity as a function of N-PoT and epsilon", 900, 700);
  Sens3D->SetGrid();
  gStyle->SetOptStat(kFALSE);
  SENS->Draw("colz");
  DPOT->Draw("hist same");

  TCanvas *SENSRAD = new TCanvas("SENSRAD", "PADME X20 Sensitivity R_{max} scan", 900, 700);
  SENSRAD->SetGrid();
  gStyle->SetOptStat(kFALSE);
  SENSrad->Draw("HIST");
  
  TCanvas *CL90_1D = new TCanvas("CL90_1D", "PADME X20 90% Confidence level as a function of N-PoT", 900, 700);
  CL90_1D->SetGrid();
  gStyle->SetOptStat(kFALSE);
  CL90->Draw("HIST");

  TCanvas *exclusion_X20_1D = new TCanvas("exclusion_X20_1D", "Exclusion plot #epsilon_{X20}^{2} as a function of N-PoT", 900, 700);
  //exclusion_X20_1D->SetGrid();
  gStyle->SetOptStat(kFALSE);
  gPad->SetFrameFillColor(kGreen-10);
  exclusion_X20->Draw("HIST");
  NA64_limit->Draw("HIST SAME");
  gPad->SetLogy();


  // BhabhaOut->cd();
  // h1->Write();
  // h2->Write();
  // h3->Write();
  // h4->Write();
  // h5->Write();
  // h6->Write();
  // h7->Write();
  // h8->Write();
  // h1Full->Write();
  // h2Full->Write();
  // h3Full->Write();
  // h4Full->Write();
  // h1g->Write();
  // h2g->Write();
  // h3g->Write();
  // h4g->Write();
  // htheta1->Write();
  // htheta2->Write();
  // hthetaFull->Write();
  // hthetag->Write();
  


  
  // BhabhaOut->Close();


  
  //OUTPUT FILES
  /*C1->SaveAs("S_Final3P.png");
    C2->SaveAs("T_Final3P.png");
    C1g->SaveAs("AA_Final3P.png");
    C3P->SaveAs("Final3p_distribution.png");
    CHECK1->SaveAs("Energy_Conservation_Check.png");
    Ctheta->SaveAs("Theta_FinalParticles.png");
    C3->SaveAs("S_FinalEnergy.png");
    C4->SaveAs("T_FinalEnergy.png");
    C2g->SaveAs("AA_FinalEnergy.png");
    C5->SaveAs("Occupancy.png");
    C6->SaveAs("geom_occupancy.png");
    C7->SaveAs("geom_energy_Occupancy.png");
    Acc3D_a->SaveAs("ACC_Scan_a.png");
    Acc3D_b->SaveAs("ACC_Scan_b.png");
    Acc1D_S->SaveAs("ACC_Schan_radius_Scan.png");
    Acc1D_T->SaveAs("ACC_Tchan_radius_Scan.png");
    Acc1D_AA->SaveAs("ACC_AA_radius_Scan.png");
    Sens3D->SaveAs("SensitivityX20.png");
    CL90_1D->SaveAs("CL90X20_stat.png");
    exclusion_X20_1D->SaveAs("gve_exclusion.png");*/
}


