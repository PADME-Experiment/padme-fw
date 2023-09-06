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

  Int_t numRow = 1000000;
  Int_t numCol = 11;
  Double_t Positron_InitialP = 0.282;
  Double_t Electron_initialEnergy = 0.;
  Double_t ElectronMass = 0.000511;
  
  TLorentzVector InitialPositron, InitialElectron, SPositron_4P, SElectron_4P, TPositron_4P, TElectron_4P;

  Double_t InitialEnergy_positron[1000000];
  Double_t SFinalEn_positron[1000000], SFinalEn_electron[1000000], TFinalEn_positron[1000000], TFinalEn_electron[1000000];
  Double_t SFinal_PosMomentum[1000000], SFinal_ElecMomentum[1000000];
  Double_t TFinal_PosMomentum[1000000], TFinal_ElecMomentum[1000000];
  Double_t SE_tot[1000000], TE_tot[1000000];
  Double_t Stheta_scattering[1000000], Ttheta_scattering[1000000];
  Double_t Stheta_test[1000000], Ttheta_test[1000000];

  Double_t Pos_Stheta_x[1000000], Pos_Sphi_y[1000000], Pos_Ttheta_x[1000000], Pos_Tphi_y[1000000]; //angoli di deviazione dei prodotti di reazione rispetto alla direzione x e y
  Double_t Elec_Stheta_x[1000000], Elec_Sphi_y[1000000], Elec_Ttheta_x[1000000], Elec_Tphi_y[1000000];
  Double_t TargetCalo = 3500; //in mm
  //Double_t semiL = 50; //lato mezzi del quadrato centrale
  //Double_t geometry_radius = 300; //raggio geometrico del calorimetro

  Double_t Pos_Sx[1000000], Pos_Sy[1000000], Elec_Sx[1000000], Elec_Sy[1000000]; //coordinate sul calorimentro nel piano trasverso al fascio xy
  Double_t Pos_Tx[1000000], Pos_Ty[1000000], Elec_Tx[1000000], Elec_Ty[1000000];
   
  TFile *f1 = new TFile("/home/marco1/Documenti/SM_analysis/BHABHA_T_1e6.root");
  TFile *f2 = new TFile("/home/marco1/Documenti/SM_analysis/BHABHA_S_1e6.root");
  TTree *t1 = (TTree*)f1->Get("t1");
  TTree *t2 = (TTree*)f2->Get("t2");
  Double_t InitialP3_positron, InitialP3_electron, TFinalP1_electron, TFinalP2_electron, TFinalP3_electron, TFinalP1_positron, TFinalP2_positron, TFinalP3_positron;
  Double_t SFinalP1_electron, SFinalP2_electron, SFinalP3_electron, SFinalP1_positron, SFinalP2_positron, SFinalP3_positron;
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

  Long64_t nentries1 = t1->GetEntries();
  //cout<<"il numero elementi nel tree: "<<nentries1<<endl;
  
  for(Long64_t k=0; k<nentries1; k++){
    t1->GetEntry(k);
    t2->GetEntry(k);

    //INITIAL STATE -> T AND S CHANNEL HAVE SAME INITIAL STATE
    InitialEnergy_positron[k] = TMath::Sqrt(ElectronMass*ElectronMass + InitialP3_positron*InitialP3_positron);
    InitialPositron.SetPxPyPzE(0, 0, InitialP3_positron, InitialEnergy_positron[k]);
    InitialElectron.SetPxPyPzE(0, 0, 0, ElectronMass);

    //cout<<"initial energy "<<(InitialEnergy_positron[k] + ElectronMass)<<endl;  
       
    SFinal_PosMomentum[k] = TMath::Sqrt(SFinalP1_positron*SFinalP1_positron + SFinalP2_positron*SFinalP2_positron + SFinalP3_positron*SFinalP3_positron);
    if(TMath::IsNaN(SFinal_PosMomentum[k])){ SFinal_PosMomentum[k] = -0.025;}
       
    SFinal_ElecMomentum[k] = TMath::Sqrt(SFinalP1_electron*SFinalP1_electron + SFinalP2_electron*SFinalP2_electron + SFinalP3_electron*SFinalP3_electron);
    if(TMath::IsNaN(SFinal_ElecMomentum[k])) {SFinal_ElecMomentum[k] = -0.025;}
    
    TFinal_PosMomentum[k] = TMath::Sqrt(TFinalP1_positron*TFinalP1_positron + TFinalP2_positron*TFinalP2_positron + TFinalP3_positron*TFinalP3_positron);
    if(TMath::IsNaN(TFinal_PosMomentum[k])) { TFinal_PosMomentum[k] = -0.025;}
    
    TFinal_ElecMomentum[k] = TMath::Sqrt(TFinalP1_electron*TFinalP1_electron + TFinalP2_electron*TFinalP2_electron + TFinalP3_electron*TFinalP3_electron);
    if(TMath::IsNaN(TFinal_ElecMomentum[k])) {TFinal_ElecMomentum[k] = -0.025;}
    ////////////////// THE OUTPUT HAS NOT NaN!! //////////////////////////


    SFinalEn_positron[k] = TMath::Sqrt(ElectronMass*ElectronMass + SFinal_PosMomentum[k]*SFinal_PosMomentum[k]);
    SFinalEn_electron[k] = TMath::Sqrt(ElectronMass*ElectronMass + SFinal_ElecMomentum[k]*SFinal_ElecMomentum[k]);
    TFinalEn_positron[k] = TMath::Sqrt(ElectronMass*ElectronMass + TFinal_PosMomentum[k]*TFinal_PosMomentum[k]);
    TFinalEn_electron[k] = TMath::Sqrt(ElectronMass*ElectronMass + TFinal_ElecMomentum[k]*TFinal_ElecMomentum[k]);

    SE_tot[k] = SFinalEn_positron[k] + SFinalEn_electron[k];
    TE_tot[k] = TFinalEn_positron[k] + TFinalEn_electron[k];
    
    SPositron_4P.SetPxPyPzE(SFinalP1_positron, SFinalP2_positron, SFinalP3_positron, SFinalEn_positron[k]);
    SElectron_4P.SetPxPyPzE(SFinalP1_electron, SFinalP2_electron, SFinalP3_electron, SFinalEn_electron[k]);
    TPositron_4P.SetPxPyPzE(TFinalP1_positron, TFinalP2_positron, TFinalP3_positron, TFinalEn_positron[k]);
    TElectron_4P.SetPxPyPzE(TFinalP1_electron, TFinalP2_electron, TFinalP3_electron, TFinalEn_electron[k]);
    
    //PRINTING ENERGY CONSERVATION
    //cout<<"final energy S-Channel "<<SE_tot[k]<<endl;
    //cout<<"final energy T_Channel "<<TE_tot[k]<<endl;

    Double_t angle1, angle2;
    Stheta_scattering[k] = TMath::ACos((SFinalEn_positron[k]*SFinalEn_electron[k] - ElectronMass*InitialEnergy_positron[k]) /(SFinal_PosMomentum[k]*SFinal_ElecMomentum[k]));
    if(TMath::IsNaN(Stheta_scattering[k])) {Stheta_scattering[k] = -0.05;}
   
    Ttheta_scattering[k] = TMath::ACos((TFinalEn_positron[k]*TFinalEn_electron[k] - ElectronMass*InitialEnergy_positron[k]) /(TFinal_PosMomentum[k]*TFinal_ElecMomentum[k]));
    if(TMath::IsNaN(Ttheta_scattering[k])) {Ttheta_scattering[k] = -0.07;}
    

    Pos_Stheta_x[k] = TMath::ATan(SFinalP1_positron/SFinalP3_positron);
    Pos_Sphi_y[k] = TMath::ATan(SFinalP2_positron/SFinalP3_positron);
    Elec_Stheta_x[k] = TMath::ATan(SFinalP1_electron/SFinalP3_electron);
    Elec_Sphi_y[k] = TMath::ATan(SFinalP2_electron/SFinalP3_electron);

    Pos_Ttheta_x[k] = TMath::ATan(TFinalP1_positron/TFinalP3_positron);
    Pos_Tphi_y[k] = TMath::ATan(TFinalP2_positron/TFinalP3_positron);
    Elec_Ttheta_x[k] = TMath::ATan(TFinalP1_electron/TFinalP3_electron);
    Elec_Tphi_y[k] = TMath::ATan(TFinalP2_electron/TFinalP3_electron);

    //le posizioni sul piano XY trasverso al fascio a 3.5 m dal bersaglio. 
    Pos_Sx[k] = TMath::Tan(Pos_Stheta_x[k]) * TargetCalo;
    Pos_Sy[k] = TMath::Tan(Pos_Sphi_y[k]) * TargetCalo;
    Elec_Sx[k] = TMath::Tan(Elec_Stheta_x[k]) * TargetCalo;
    Elec_Sy[k] = TMath::Tan(Elec_Sphi_y[k]) * TargetCalo;

    Pos_Tx[k] = TMath::Tan(Pos_Ttheta_x[k]) * TargetCalo;
    Pos_Ty[k] = TMath::Tan(Pos_Tphi_y[k]) * TargetCalo;
    Elec_Tx[k] = TMath::Tan(Elec_Ttheta_x[k]) * TargetCalo;
    Elec_Ty[k] = TMath::Tan(Elec_Tphi_y[k]) * TargetCalo;

    
 }

  double pos_min, pos_max;
  pos_min = TMath::Tan(0.025) * TargetCalo;
  pos_max = TMath::Tan(0.075) * TargetCalo;
  
   
  ofstream file;
  file.open("BHABHA_counter_new.txt");
  file<<"-----TAGLI GEOMETRICI ED ENERGITICI BHABHA SCATTERING-----"<<"\n";

  //definisco le variabili utili a fare i tagli geometrici. Le M sono Int_termedie; #Calo#X/Y sono le posizioni che sopravvivranno ai tagli. tutti gli altri sono contatori
  Double_t M1[1000000], M2[1000000], M3[1000000], M4[1000000], M5[1000000], M6[1000000], M7[1000000], M8[1000000];
  Double_t Radius_SPos[1000000], Radius_SElec[1000000], Radius_TPos[1000000], Radius_TElec[1000000];
  double HoleCut_SPos = 0, HoleCut_SElec = 0, HoleCut_TPos = 0, HoleCut_TElec = 0;
  double GeomCut_SPos = 0, GeomCut_SElec = 0, GeomCut_TPos = 0, GeomCut_TElec = 0;
  double OutCut_SPos = 0, OutCut_SElec = 0, OutCut_TPos = 0, OutCut_TElec = 0;
  
  //MAKE A COUNTER - GEOMETRY CUTS

  TH2F *h9 = new TH2F("h9", "S-Channel positron", 1000, -300, 300, 10000, -300, 300); //istogramma 2d per tutti gli eventi con soli tagli geometrici
  TH2F *h10 = new TH2F("h10", "S-Channel electron", 1000, -300, 300, 10000, -300, 300);

  TH2F *h11 = new TH2F("h11", "T-Channel positron", 1000, -300, 300, 10000, -300, 300);
  TH2F *h12 = new TH2F("h12", "T-Channel electron", 1000, -300, 300, 10000, -300, 300);

  
  for(Int_t k=0; k<numRow; k++){
    M1[k] = TMath::Abs(Pos_Sx[k]); //traslo il rivelatore nell'origine degli assi e lo studio solo nel primo quadrante
    M2[k] = TMath::Abs(Pos_Sy[k]);
    M3[k] = TMath::Abs(Elec_Sx[k]);
    M4[k] = TMath::Abs(Elec_Sy[k]);
    M5[k] = TMath::Abs(Pos_Tx[k]);
    M6[k] = TMath::Abs(Pos_Ty[k]);
    M7[k] = TMath::Abs(Elec_Tx[k]);
    M8[k] = TMath::Abs(Elec_Ty[k]);

    Radius_SPos[k] = TMath::Sqrt(Pos_Sx[k]*Pos_Sx[k] + Pos_Sy[k]*Pos_Sy[k]);
    Radius_SElec[k] = TMath::Sqrt(Elec_Sx[k]*Elec_Sx[k] + Elec_Sy[k]*Elec_Sy[k]);
    Radius_TPos[k] = TMath::Sqrt(Pos_Tx[k]*Pos_Tx[k] + Pos_Ty[k]*Pos_Ty[k]);
    Radius_TElec[k] = TMath::Sqrt(Elec_Tx[k]*Elec_Tx[k] + Elec_Ty[k]*Elec_Ty[k]);
    
    //questo taglio elimina gli eventi che sono nel buco centrale quadrato (10cmX10cm) del calorimetro 
    if(M1[k]>50. || M2[k]>50.){HoleCut_SPos++;
      if(Radius_SPos[k]<300.){GeomCut_SPos++; h9->Fill(Pos_Sx[k], Pos_Sy[k]);};};
   
    if(M3[k]>50. || M4[k]>50.){HoleCut_SElec++;
      if(Radius_SElec[k]<300.){GeomCut_SElec++; h10->Fill(Elec_Sx[k], Elec_Sy[k]);};};

    if(M5[k]>50. || M6[k]>50.){HoleCut_SPos++;
      if(Radius_TPos[k]<300.){GeomCut_TPos++; h11->Fill(Pos_Tx[k], Pos_Ty[k]);};};
   
    if(M7[k]>50. || M8[k]>50.){HoleCut_TElec++;
      if(Radius_TElec[k]<300.){GeomCut_TElec++; h12->Fill(Elec_Tx[k], Elec_Ty[k]);};};

    
  }

    OutCut_SPos = - GeomCut_SPos + HoleCut_SPos ; //conto quante tracce perdo nel buco centrale, quante fuori dal calorimetro e quante in totale
    OutCut_SElec = - GeomCut_SElec + HoleCut_SElec;
    OutCut_TPos = - GeomCut_TPos + HoleCut_TPos ; //conto quante tracce perdo nel buco centrale, quante fuori dal calorimetro e quante in totale
    OutCut_TElec = - GeomCut_TElec + HoleCut_TElec;
   
    file<<"Il numero positroni nel canale S che sono fuori dal quadrato centrale diviso il numero di eventi totali: "<<(HoleCut_SPos/numRow)<<"\n";
    file<<"Il numero elettroni nel canale S che sono fuori dal quadrato centrale diviso il numero di eventi totali: "<<(HoleCut_SElec/numRow)<<"\n";
    file<<"Il numero positroni nel canale T che sono fuori dal quadrato centrale diviso il numero di eventi totali: "<<(HoleCut_TPos/numRow)<<"\n";
    file<<"Il numero elettroni nel canale T che sono fuori dal quadrato centrale diviso il numero di eventi totali: "<<(HoleCut_TElec/numRow)<<"\n \n";
    file<<"Il numero positroni nel canale S che sono fuori dalla circonferenza diviso il numero di eventi totali: "<<(OutCut_SPos/numRow)<<"\n";
    file<<"Il numero elettroni nel canale S che sono fuori dalla circonferenza diviso il numero di eventi totali: "<<(OutCut_SElec/numRow)<<"\n";
    file<<"Il numero positroni nel canale T che sono fuori dalla circonferenza diviso il numero di eventi totali: "<<(OutCut_TPos/numRow)<<"\n";
    file<<"Il numero elettroni nel canale T che sono fuori dalla circonferenza diviso il numero di eventi totali: "<<(OutCut_TElec/numRow)<<"\n \n"; 
    file<<"Il numero positroni nel canale S che sono dentro il calorimetro diviso il numero di eventi totali: "<<(GeomCut_SPos/numRow)<<"\n";
    file<<"Il numero elettroni nel canale S che sono dentro il calorimetro diviso il numero di eventi totali: "<<(GeomCut_SElec/numRow)<<"\n";
    file<<"Il numero positroni nel canale T che sono dentro il calorimetro diviso il numero di eventi totali: "<<(GeomCut_TPos/numRow)<<"\n";
    file<<"Il numero elettroni nel canale T che sono dentro il calorimetro diviso il numero di eventi totali: "<<(GeomCut_TElec/numRow)<<"\n \n";  


    TH2F *h13 = new TH2F("h13", "S-Channel positron detected", 1000, -300, 300, 10000, -300, 300);
    TH2F *h14 = new TH2F("h14", "S-Channel electron detected", 1000, -300, 300, 10000, -300, 300);

    TH2F *h15 = new TH2F("h15", "T-Channel positron detected", 1000, -300, 300, 10000, -300, 300);
    TH2F *h16 = new TH2F("h16", "T-Channel electron detected", 1000, -300, 300, 10000, -300, 300);
    
    
    //tagli per selezione di eventi con 2 tracce -> genero un istogramma con i soli eventi a doppia traccia nello stato finale.
    double CoupleCut_SPos = 0, CoupleCut_SElec = 0, CoupleCut_TPos = 0, CoupleCut_TElec = 0;
    for(Int_t k = 0; k<numRow; k++){

      if(M1[k]>50. || M2[k]>50.){
	if(M3[k]>50. || M4[k]>50.){
	  if(Radius_SPos[k]<300. && Radius_SElec[k]<300.){h13->Fill(Pos_Sx[k], Pos_Sy[k]); h14->Fill(Elec_Sx[k], Elec_Sy[k]); CoupleCut_SPos++; CoupleCut_SElec++;};};};
    

      if(M5[k]>50. || M6[k]>50.){
	if(M7[k]>50. || M8[k]>50.){
	  if(Radius_TPos[k]<300. && Radius_TElec[k]<300.){h15->Fill(Pos_Tx[k], Pos_Ty[k]); h16->Fill(Elec_Tx[k], Elec_Ty[k]); CoupleCut_TPos++; CoupleCut_TElec++;};};};
     
    }

    file<<"___dopo la selezione dei soli eventi con due tracce nel calorimetro___"<<"\n";
    file<<"Il numero positroni nel canale S che sono dentro il calorimetro diviso il numero di eventi totali: "<<(CoupleCut_SPos/numRow)<<"\n";
    file<<"Il numero elettroni nel canale S che sono dentro il calorimetro diviso il numero di eventi totali: "<<(CoupleCut_SElec/numRow)<<"\n";
    file<<"Il numero positroni nel canale T che sono dentro il calorimetro diviso il numero di eventi totali: "<<(CoupleCut_TPos/numRow)<<"\n";
    file<<"Il numero elettroni nel canale T che sono dentro il calorimetro diviso il numero di eventi totali: "<<(CoupleCut_TElec/numRow)<<"\n";
    file<<"Il numero di tracce positrone-elettrone nel canale S che sono dentro il calorimetro diviso il numero di eventi totali: "<<(CoupleCut_SPos/numRow)<<"\n";
    file<<"Il numero di tracce positrone-elettrone nel canale T che sono dentro il calorimetro diviso il numero di eventi totali: "<<(CoupleCut_TPos/numRow)<<"\n \n";

    
    //tagli in energia per le coppie selezionate. ENTRAMBE DEVONO AVERE ENERGIA ALMENO SUPERIORE A 50 MeV
    double EnergyCut_SPos = 0, EnergyCut_SElec = 0, EnergyCut_TPos = 0, EnergyCut_TElec = 0;

    TH2F *h17 = new TH2F("h17", "S-Channel positron with E>50 MeV", 1000, -300, 300, 10000, -300, 300);
    TH2F *h18 = new TH2F("h18", "S-Channel electron with E>50 MeV", 1000, -300, 300, 10000, -300, 300);

    TH2F *h19 = new TH2F("h19", "T-Channel positron with E>50 MeV", 1000, -300, 300, 10000, -300, 300);
    TH2F *h20 = new TH2F("h20", "T-Channel electron with E>50 MeV", 1000, -300, 300, 10000, -300, 300);

    for(Int_t k = 0; k<numRow; k++){

      if(M1[k]>50. || M2[k]>50.){
	if(M3[k]>50. || M4[k]>50.){
	  if(Radius_SPos[k]<300. && Radius_SElec[k]<300.){
	    if(SFinalEn_positron[k]>0.05 && SFinalEn_electron[k]>0.05){h17->Fill(Pos_Sx[k], Pos_Sy[k]); h18->Fill(Elec_Sx[k], Elec_Sy[k]); EnergyCut_SPos++; EnergyCut_SElec++;};};};};
      

      if(M5[k]>50. || M6[k]>50.){
	if(M7[k]>50. || M8[k]>50.){
	  if(Radius_TPos[k]<300. && Radius_TElec[k]<300.){
	    if(TFinalEn_positron[k]>0.05 && TFinalEn_electron[k]>0.05){h19->Fill(Pos_Tx[k], Pos_Ty[k]); h20->Fill(Elec_Tx[k], Elec_Ty[k]); EnergyCut_TPos++; EnergyCut_TElec++;};};};};
      
 
      
    }
    

    file<<"___GLI EVENTI CHE VENGONO RIVELATI DAL CALORIMETRO DOPO TUTTI I TAGLI GEOMETRICI ED ENERGETICI___"<<"\n";
    file<<"Il numero positroni nel canale S con energia superiore a 50 MeV diviso il numero di eventi totali: "<<(EnergyCut_SPos/numRow)<<"\n";
    file<<"Il numero elettroni nel canale S con energia superiore a 50 MeV diviso il numero di eventi totali: "<<(EnergyCut_SElec/numRow)<<"\n";
    file<<"Il numero positroni nel canale T con energia superiore a 50 MeV diviso il numero di eventi totali: "<<(EnergyCut_TPos/numRow)<<"\n";
    file<<"Il numero elettroni nel canale T con energia superiore a 50 MeV diviso il numero di eventi totali: "<<(EnergyCut_TElec/numRow)<<"\n";
    file<<"Il numero di tracce positrone-elettrone nel canale S rivelate dal calorimetro diviso il numero di eventi totali: "<<(EnergyCut_SPos/numRow)<<"\n";
    file<<"Il numero di tracce positrone-elettrone nel canale T rivelate dal calorimetro diviso il numero di eventi totali: "<<(EnergyCut_TPos/numRow)<<"\n \n";

    //STESSO TIPO DI SELEZIONE DI EVENTI CON TAGLI FIDUCIARIO PER CORONA CIRCOLARE (10cm, 25 cm)
    double FidCut_SPos = 0, FidCut_SElec = 0, FidCut_TPos = 0, FidCut_TElec = 0;

  TH2F *h21 = new TH2F("h21", "S-Channel positron with E>50 MeV in confidence range (10 cm, 25 cm)", 1000, -300, 300, 10000, -300, 300);
  TH2F *h22 = new TH2F("h22", "S-Channel electron with E>50 MeV in confidence range (10 cm, 25 cm)", 1000, -300, 300, 10000, -300, 300);

  TH2F *h23 = new TH2F("h23", "T-Channel positron with E>50 MeV in confidence range (10 cm, 25 cm)", 1000, -300, 300, 10000, -300, 300);
  TH2F *h24 = new TH2F("h24", "T-Channel electron with E>50 MeV in confidence range (10 cm, 25 cm)", 1000, -300, 300, 10000, -300, 300);
  
  Int_t c1 = 0, c2 = 0, c3 = 0, c4 = 0;
  for(Int_t k=0; k<numRow; k++){
   
    if(Radius_SPos[k]>100. && Radius_SPos[k]<250.){
      if(Radius_SElec[k]>100. && Radius_SElec[k]<250.){
	if(SFinalEn_positron[k] > 0.05 && SFinalEn_electron[k] > 0.05){h21->Fill(Pos_Sx[k], Pos_Sy[k]); h22->Fill(Elec_Sx[k], Elec_Sy[k]); FidCut_SPos++; FidCut_SElec++;};};};
      

    if(Radius_TPos[k]>100. && Radius_TPos[k]<250.){
      if(Radius_TElec[k]>100. && Radius_TElec[k]<250.){
	if(TFinalEn_positron[k] > 0.05 && TFinalEn_electron[k] > 0.05){h23->Fill(Pos_Tx[k], Pos_Ty[k]); h24->Fill(Elec_Tx[k], Elec_Ty[k]); FidCut_TPos++; FidCut_TElec++;};};};
      
    
  }


  file<<"___GLI EVENTI CHE VENGONO RIVELATI DAL CALORIMETRO NELLA CORONA CIRCOLARE (10cm, 25cm)___"<<"\n";
  file<<"Il numero positroni nel canale S con energia superiore a 50 MeV diviso il numero di eventi totali: "<<(FidCut_SPos/numRow)<<"\n";
  file<<"Il numero elettroni nel canale S con energia superiore a 50 MeV diviso il numero di eventi totali: "<<(FidCut_SElec/numRow)<<"\n";
  file<<"Il numero positroni nel canale T con energia superiore a 50 MeV diviso il numero di eventi totali: "<<(FidCut_TPos/numRow)<<"\n";
  file<<"Il numero elettroni nel canale T con energia superiore a 50 MeV diviso il numero di eventi totali: "<<(FidCut_TElec/numRow)<<"\n";
  file<<"Il numero di tracce positrone-elettrone nel canale S rivelate dal calorimetro nella corona circolare (10cm, 25cm) diviso il numero di eventi totali: "<<(FidCut_SPos/numRow)<<"\n";
  file<<"Il numero di tracce positrone-elettrone nel canale T rivelate dal calorimetro nella corona circolare (10cm, 25cm) diviso il numero di eventi totali: "<<(FidCut_TPos/numRow)<<"\n \n";
    
  file.close();
  

  //DEFINITION OF HISTOGRAMS
  TH1F *h1 = new TH1F ("h1", "Positron P-distribution in S-Channel", 10000,-0.05, 0.30);
  TH1F *h2 = new TH1F ("h2", "Electron P-distribution in S-Channel", 10000,-0.05, 0.30);
  TH1F *h3 = new TH1F ("h3", "Positron P-distribution in T-Channel", 10000,-0.05, 0.30);
  TH1F *h4 = new TH1F ("h4", "Electron P-distribution in T-Channel", 10000,-0.05, 0.30);

  TH1F *h5 = new TH1F ("h5", "Positron energy distribution in S-Channel", 10000,-0.05, 0.30);
  TH1F *h6 = new TH1F ("h6", "Electron energy distribution in S-Channel", 10000,-0.05, 0.30);
  TH1F *h7 = new TH1F ("h7", "Positron energy distribution in T-Channel", 10000,-0.05, 0.30);
  TH1F *h8 = new TH1F ("h8", "Electron energy distribution in T-Channel", 10000,-0.05, 0.30);

  TH1F *check1 = new TH1F("check1", "Final energy sum in S-Channel", 100000, 0.27, 0.29); 
  TH1F *check2 = new TH1F("check2", "Final energy sum in T-Channel", 100000, 0.27, 0.29); 

  TH1F *htheta1 = new TH1F("htheta1", "S-Channel Angular distribution", 10000, -0.1, 1); 
  TH1F *htheta2 = new TH1F("htheta2", "T-Channel Angular distribution", 10000, -0.1, 1);

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
  h21->SetXTitle("y [mm]"); h21->SetYTitle("x [mm]");
  h22->SetXTitle("y [mm]"); h22->SetYTitle("x [mm]");
  h23->SetXTitle("y [mm]"); h23->SetYTitle("x [mm]");
  h24->SetXTitle("y [mm]"); h24->SetYTitle("x [mm]");
  
  h1->SetLineColor(kRed);
  h2->SetLineColor(kRed+3);
  h3->SetLineColor(kCyan);
  h4->SetLineColor(kCyan+2);
  h5->SetLineColor(kRed);
  h6->SetLineColor(kRed+3);
  h7->SetLineColor(kCyan);
  h8->SetLineColor(kCyan+2);
  check1->SetLineColor(kRed+4);
  check2->SetLineColor(kCyan+4);
  htheta1->SetLineColor(kRed+4);
  htheta2->SetLineColor(kCyan+4);
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
  h21->SetMarkerStyle(6); h21->SetMarkerColor(kRed-4);
  h22->SetMarkerStyle(6); h22->SetMarkerColor(kGreen-1);
  h23->SetMarkerStyle(6); h23->SetMarkerColor(kRed-4);
  h24->SetMarkerStyle(6); h24->SetMarkerColor(kGreen-1);
  
  //SCATTER PLOT DEFINITION
  TGraph *G1S = new TGraph(numRow, Pos_Sx, Pos_Sy);
  G1S->SetMarkerStyle(6); G1S->SetMarkerColor(kRed-4);
  TGraph *G2S = new TGraph(numRow, Elec_Sx, Elec_Sy);
  G2S->SetMarkerStyle(6); G2S->SetMarkerColor(kGreen-1);

  TMultiGraph *MG1 = new TMultiGraph(); //scatter plot con TGraph per vedere la distribuzione di tutti gli eventi ad una distanza di 3,5 mettri dal bersaglio.
  MG1->SetTitle("Calorimeter Position S-Channel; y[mm]; x[mm]");
  MG1->Add(G1S, "AP"); MG1->Add(G2S, "AP");
  MG1->GetXaxis()->SetLimits(-500, 500);
  MG1->SetMinimum(-500); MG1->SetMaximum(500);
  
  
  TGraph *G1T = new TGraph(numRow, Pos_Tx, Pos_Ty); 
  G1T->SetMarkerStyle(6); G1T->SetMarkerColor(kRed-4);
  TGraph *G2T = new TGraph(numRow, Elec_Tx, Elec_Ty);
  G2T->SetMarkerStyle(6); G2T->SetMarkerColor(kGreen-1);
 
  TMultiGraph *MG2 = new TMultiGraph(); //scatter plot con TGraph per vedere la distribuzione di tutti gli eventi ad una distanza di 3,5 mettri dal bersaglio.
  MG2->SetTitle("Calorimeter Position T-Channel; y[mm]; x[mm]");
  MG2->Add(G1T, "AP"); MG2->Add(G2T, "AP");
  MG2->GetXaxis()->SetLimits(-500, 500);
  MG2->SetMinimum(-500); MG2->SetMaximum(500);
   
  
  //CREATION OF CANVAS
  TCanvas *C1 = new TCanvas("C1", "3-Momentum BhaBha Scattering distribution", 900, 700);
  h1->Draw();
  h2->Draw("SAME");
  h3->Draw("SAME");
  h4->Draw("SAME");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  gPad->SetLogy();
    
  TCanvas *C2 = new TCanvas("C2", "3-Momentum BhaBha Scattering->S-Channel distribution", 900, 700);
  h1->Draw();
  h2->Draw("SAME");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  gPad->SetLogy();

  TCanvas *C3 = new TCanvas("C3", "3-Momentum BhaBha Scattering->T-Channel distribution", 900, 700);
  h3->Draw();
  h4->Draw("SAME");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  gPad->SetLogy();

  TCanvas *CHECK1 = new TCanvas("CHECK1", "Energy conservation check", 900, 700);
  check1->Draw();
  check2->Draw("SAME");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);

  TCanvas *C4 = new TCanvas("C4", "Scattering distribution S&T Channel", 900, 700);
  C4->Divide(1,2);
  C4->cd(1);
  htheta1->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  gPad->SetLogy();
  C4->cd(2);
  htheta2->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  gPad->SetLogy();

  TCanvas *C5 = new TCanvas("C5", "Energy BhaBha Scattering distribution", 900, 700);
  h5->Draw();
  h6->Draw("SAME");
  h7->Draw("SAME");
  h8->Draw("SAME");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  gPad->SetLogy();

  TCanvas *C6 = new TCanvas("C6", "Energy BhaBha Scattering->S-Channel distribution", 900, 700);
  h5->Draw();
  h6->Draw("SAME");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  gPad->SetLogy();

  TCanvas *C7 = new TCanvas("C7", "Energy BhaBha Scattering->T-Channel distribution", 900, 700);
  h7->Draw();
  h8->Draw("SAME");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  gPad->SetLogy();

  TCanvas *C8 = new TCanvas("C8", "Final position - S-Channel", 900, 700);
  MG1->Draw("AP");
  TLegend leg8(0.07, 0.8, 0.25, 0.95);
  leg8.AddEntry(G1S, "positron");
  leg8.AddEntry(G2S, "electron");
  leg8.Draw();

  TCanvas *C8a = new TCanvas("C8a", "Final position - S-Channel positron-electron", 600, 700);
  C8a->Divide(1,2);
  C8a->cd(1);
  G1S->SetTitle("Final Positron position S-Channel; y[mm]; x[mm]");
  G1S->GetXaxis()->SetLimits(-500, 500);
  G1S->SetMinimum(-500); G1S->SetMaximum(500);
  G1S->Draw("AP");
  TLegend leg8apos(0.07, 0.8, 0.25, 0.95);
  leg8apos.AddEntry(G1S, "positron");
  leg8apos.Draw();
  C8a->cd(2);
  G2S->SetTitle("Final Electron position S-Channel; y[mm]; x[mm]");
  G2S->GetXaxis()->SetLimits(-500, 500);
  G2S->SetMinimum(-500); G2S->SetMaximum(500);
  G2S->Draw("AP");
  TLegend leg8aelec(0.07, 0.8, 0.25, 0.95);
  leg8aelec.AddEntry(G2S, "electron");
  leg8aelec.Draw();  
 
  TCanvas *C9 = new TCanvas("C9", "Final position - T-Channel", 900, 700);
  MG2->Draw("AP");
  TLegend leg9(0.07, 0.8, 0.25, 0.95);
  leg9.AddEntry(G1T, "positron");
  leg9.AddEntry(G2T, "electron");
  leg9.Draw();

  TCanvas *C9a = new TCanvas("C9a", "Final position - T-Channel positron-electron", 600, 700);
  C9a->Divide(1,2);
  C9a->cd(1);
  G1T->SetTitle("Final Positron position T-Channel; y[mm]; x[mm]");
  G1T->GetXaxis()->SetLimits(-500, 500);
  G1T->SetMinimum(-500); G1T->SetMaximum(500);
  G1T->Draw("AP");
  TLegend leg9apos(0.07, 0.8, 0.25, 0.95);
  leg9apos.AddEntry(G1T, "positron");
  leg9apos.Draw();
  C9a->cd(2);
  G2T->SetTitle("Final Electron position T-Channel; y[mm]; x[mm]");
  G2T->GetXaxis()->SetLimits(-500, 500);
  G2T->SetMinimum(-500); G2T->SetMaximum(500);
  G2T->Draw("AP");
  TLegend leg9aelec(0.07, 0.8, 0.25, 0.95);
  leg9aelec.AddEntry(G2T, "electron");
  leg9aelec.Draw();  

  TCanvas *C10 = new TCanvas("C10", "Events in S & T Channel detected by Calorimeter", 600, 700);
  C10->Divide(1,2);
  C10->cd(1);
  h9->Draw();
  h10->Draw("SAME");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  C10->cd(2);
  h11->Draw();
  h12->Draw("SAME");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);

  TCanvas *C10a = new TCanvas("C10a", "Detected events S and T Channel positron-electron", 900, 700);
  C10a->Divide(2,2);
  C10a->cd(1);
  h9->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  C10a->cd(2);
  h10->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  C10a->cd(3);
  h11->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  C10a->cd(4);
  h12->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  
  TCanvas *C11 = new TCanvas("C11", "S-Channel", 600, 700);
  C11->Divide(1,2);
  C11->cd(1);
  h13->Draw();
  h14->Draw("SAME");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  C11->cd(2);
  h17->Draw();
  h18->Draw("SAME");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);

  TCanvas *C11a = new TCanvas("C11a", "S-Channel positron", 600, 700);
  C11a->Divide(1,2);
  C11a->cd(1);
  h13->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  C11a->cd(2);
  h17->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);

  TCanvas *C11b = new TCanvas("C11b", "S-Channel electron", 600, 700);
  C11b->Divide(1,2);
  C11b->cd(1);
  h14->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  C11b->cd(2);
  h18->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);

  TCanvas *C12 = new TCanvas("C12", "T-Channel", 600, 700);
  C12->Divide(1,2);
  C12->cd(1);
  h15->Draw();
  h16->Draw("SAME");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  C12->cd(2);
  h19->Draw();
  h20->Draw("SAME");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);

  TCanvas *C12a = new TCanvas("C12a", "T-Channel positron", 600, 700);
  C12a->Divide(1,2);
  C12a->cd(1);
  h15->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  C12a->cd(2);
  h19->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);

  TCanvas *C12b = new TCanvas("C12b", "T-Channel electron", 600, 700);
  C12b->Divide(1,2);
  C12b->cd(1);
  h16->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  C12b->cd(2);
  h20->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);

  TCanvas *C12c = new TCanvas("C13c", "Positron-Electron events detected by calorimeter (E>50 MeV)", 900, 700);
  C12c->Divide(2,2);
  C12c->cd(1);
  h17->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  C12c->cd(2);
  h18->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  C12c->cd(3);
  h19->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  C12c->cd(4);
  h20->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);

  TCanvas *C13 = new TCanvas("C13", "Confindence range calorimeter S and T Channel", 600, 700);
  C13->Divide(1,2);
  C13->cd(1);
  h21->Draw();
  h22->Draw("SAME");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  C13->cd(2);
  h23->Draw();
  h24->Draw("SAME");
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);

  TCanvas *C13a = new TCanvas("C13a", "Confindence range calorimeter S and T Channel positron-electron", 900, 700);
  C13a->Divide(2,2);
  C13a->cd(1);
  h21->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  C13a->cd(2);
  h22->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  C13a->cd(3);
  h23->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  C13a->cd(4);
  h24->Draw();
  gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
  
  //OUTPUT FILES
  C1->SaveAs("S_T_momentum_distribution.png");
  C2->SaveAs("S_3P_distribution.png");
  C3->SaveAs("T_3P_distribution.png");
  CHECK1->SaveAs("CHECK_energycons.png");
  C4->SaveAs("Angular_distribution.png");
  C5->SaveAs("S_T_energy_distribution.png");
  C6->SaveAs("S_energy_distribution.png");
  C7->SaveAs("T_energy_distribution.png");
  C8->SaveAs("S_finalXY.png");
  C8a->SaveAs("S_finalXY_eE.png");
  C9->SaveAs("T_finalXY.png");
  C9a->SaveAs("T_finalXY_eE.png");
  C10->SaveAs("S_T_events.png");
  C10a->SaveAs("S_T_events_divided.png");
  C11->SaveAs("S_Geometry_events.png");
  C11a->SaveAs("geom_S_positron.png");
  C11b->SaveAs("geom_S_electron.png");
  C12->SaveAs("T_Geometry_events.png");
  C12a->SaveAs("geom_T_positron.png");
  C12b->SaveAs("geom_T_electron.png");
  C12c->SaveAs("S_T_E50MeV_divided.png");
  C13->SaveAs("Confidence_ST.png");
  C13a->SaveAs("S_T_confidence_divided.png");

  
  }


