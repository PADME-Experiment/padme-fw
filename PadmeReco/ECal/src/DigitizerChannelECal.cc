// Written By M. Raggi 6/12/2018
#include "DigitizerChannelECal.hh"
#include "TMath.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TSpectrum.h"
#include "TRandom.h"

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>

void DigitizerChannelECal::PrintConfig(){
  std::cout << "Hi I'm the ECal: " << std::endl;
  std::cout << "Signal width: "   << fSignalWidth << " samples" << std::endl;
  std::cout << "fUseAbsSignals: " << fUseAbsSignals << std::endl; 
  std::cout << "fPedestalMode:   " << fPedestalMode <<std::endl;
  std::cout << "fUseOverSample: " << fUseOverSample << std::endl;  

//  Int_t NBD=8;
//  for(int i=0;i<32;i++){
//    std::cout<<"Ped "<<i<<" "<<fPedMap[std::make_pair(NBD,i)]<<" "<<std::endl;
//  }  

 if(fGlobalMode->GetGlobalDebugMode()!=0){
   std::cout<<"General Config Flags ECal Digi***************** "<<std::endl;
   std::cout<<"fIsPed          "<<fGlobalMode->IsPedestalMode()<< std::endl;
   std::cout<<"fIsReco         "<<fGlobalMode->IsRecoMode()<< std::endl;
   std::cout<<"fIsMonitor      "<<fGlobalMode->IsMonitorMode()<< std::endl;
   std::cout<<"fIsCosmic       "<<fGlobalMode->IsCosmicsMode()<< std::endl;
   std::cout<<"fIsDebug  ECAL  "<<fGlobalMode->GetGlobalDebugMode()<< std::endl;
  }
}

void DigitizerChannelECal::Init(GlobalRecoConfigOptions *gOptions,
				PadmeVRecoConfig *cfg)
{
// Setting flags for running modes.
  fGlobalMode = gOptions;

  
  fTimeBin        = cfg->GetParOrDefault("ADC","TimeBin",1.);
  fVoltageBin     = cfg->GetParOrDefault("ADC","VoltageBin",0.000244);
  fImpedance      = cfg->GetParOrDefault("ADC","InputImpedance",50.);

  fSignalWidth    = cfg->GetParOrDefault("RECO","SignalWindow",1024);
  fPedOffset      = cfg->GetParOrDefault("RECO","PedestalOffset",100);
  
  fPreSamples     = cfg->GetParOrDefault("RECO","SignalPreSamples",1024);
  fPostSamples    = cfg->GetParOrDefault("RECO","SignalPostSamples",1024);
  fPedMaxNSamples = cfg->GetParOrDefault("RECO","NumberOfSamplesPedestal",100);  
  fPedestalMode   = cfg->GetParOrDefault("RECO","PedestalMode",0);  

  fMinAmplitude    = cfg->GetParOrDefault("RECO","MinAmplitude",10);
  fAmpThresholdLow = cfg->GetParOrDefault("RECO","AmplThrLow",10.);
  fAmpThresholdHigh= cfg->GetParOrDefault("RECO","AmplThrHigh",20.);

  fMultihit       = cfg->GetParOrDefault("RECO","Multihit",0);
  fUseAbsSignals  = cfg->GetParOrDefault("RECO","UseAbsSignals",0);
  fUseOverSample  = cfg->GetParOrDefault("RECO","UseOverSample",0); //M. Raggi: 03/05/2019  
  fIntCorrection  = cfg->GetParOrDefault("RECO","UseIntegralCorrection",0); //M. Raggi: 15/05/2019  
  fSaturatioCorrection = cfg->GetParOrDefault("RECO","UseSaturationCorrection",0); //M. Raggi: 15/05/2019  
  fZeroSuppression= cfg->GetParOrDefault("RECO","UseZeroSuppression",5.); //M. Raggi: 07/06/2019  
  fSaveAnalog = cfg->GetParOrDefault("Output","Analog",0); //M. Raggi: 15/05/2019  

  fUsePulseProcessing  = cfg->GetParOrDefault("RECO","UsePulseProcessing",0);
  fDPParameterR1       = cfg->GetParOrDefault("RECO","fDPParameterR1",650.);
  fDPParameterR2       = cfg->GetParOrDefault("RECO","fDPParameterR2",100.);
  fDPParameterC        = cfg->GetParOrDefault("RECO","fDPParameterC",0.30e-9);


  std::cout << cfg->GetName() << "*******************************" <<  std::endl;

  if (!pedestalsFromFirstSamples()) SetAnalogOffSets();  //M. Raggi: 21/01/2019 read fixed anaolg values from files
  PrintConfig();
  PrepareTmpHistos();  //Temp histo servono anche in non debug mode
  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode()){
    PrepareDebugHistos(); //debugging mode histos
  }

  Int_t i=0;
  ifstream myfile;
  myfile.open ("template.txt");;
  if (myfile.is_open()){
   while (!myfile.eof()) {
   myfile >> fTemplate[i];
  if(fGlobalMode->GetGlobalDebugMode()) hTemplate->SetBinContent(i,fTemplate[i]);
   //std::cout<< Template[i] << std::endl;
   i++;
   }
  }
}

void DigitizerChannelECal::PrepareTmpHistos(){
  hListTmp    = new TList();
  if(!fUseOverSample){
    hListTmp->Add(hdxdt   = new TH1F("hdxdt","hdxdt",1000,0.,1000.));
    hListTmp->Add(hSignal = new TH1F("hSignal","hSignal",1000,0.,1000.));
 hListTmp->Add(hSignalShifted = new TH1F("hSignalShifted","hSignalShifted",1000,0.,1000.));
    hListTmp->Add(hSignalShifted2 = new TH1F("hSignalShifted2","hSignalShifted2",1000,0.,1000.));
    hListTmp->Add(hSignalShifted3 = new TH1F("hSignalShifted3","hSignalShifted3",1000,0.,1000.));
    hListTmp->Add(hDiffSignal = new TH1F("hDiffSignal","hDiffSignal",1000,0.,1000.));
    hListTmp->Add(hSat    = new TH1F("hSat","hSat",1000,0.,1000.));
  }
  // over sampled histograms
  Int_t nbinx=4000;
  if(fUseOverSample){
    hListTmp->Add(hSigOv       = new TH1F("hSigOv","hSigOv",nbinx,0.,nbinx/4));
    hListTmp->Add(hSigOvSm     = new TH1F("hSigOvSm","hSigOvSm",nbinx,0.,nbinx/4));
    hListTmp->Add(hdxdtSigOvSm = new TH1F("hdxdtSigOvSm","hdxdtSigOvSm",nbinx,0.,nbinx/4));
  }
}

void DigitizerChannelECal::PrepareDebugHistos(){
  fileOut    = new TFile("ECalAn.root", "RECREATE");
  hListCal   = new TList();  
  hListEv    = new TList();  
  ECal = new TTree("ECAL","ECAL");

  ECal->Branch("ElCh",&ElCh);
  ECal->Branch("Row",&Row);
  ECal->Branch("Col",&Col);
  ECal->Branch("Zsup",&Zsup);
  ECal->Branch("Avg200",&fAvg200);
  ECal->Branch("HitE",&HitE);
  ECal->Branch("HitEHyb",&HitEHyb);
  ECal->Branch("Hit200E",&HitE200);
  ECal->Branch("HitT",&HitT);
  ECal->Branch("Trig",&fTrig); // 0 reco 1 ped 2 cosmic
  ECal->Branch("IsSat",&IsSat);
  ECal->Branch("VMax",&fVMax);

  hPedCalo = new TH1D*[32];
  hAvgCalo = new TH1D*[32];
  hPedMean = new TH1D*[32];
  hVMax    = new TH1D*[32];
  h200QCh  = new TH1D*[32]; //CT
  hQCh     = new TH1D*[32]; //CT

  //  hListTmp->Add(hDiff    = new TH1F("hDiff","hDiff",4000,0.,1000.));
  hListCal->Add(hTime= new TH1F("hTime","hTime",1000,0.,1000.));
  hListCal->Add(hTimeCut= new TH1F("hTimeCut","hTimeCut",1000,0.,1000.));
  hListCal->Add(hTimeOv= new TH1F("hTimeOv","hTimeOv",1000,0.,1000.));
  hListCal->Add(hdxdtMax= new TH1F("hdxdtMax","hdxdtMax",1600,-200.,3000.));
  hListCal->Add(hdxdtRMS= new TH1F("hdxdtRMS","hdxdtRMS",1000,0.,200.));
  hListCal->Add(hTIntCorr= new TH1F("hTIntCorr","hTIntCorr",500,0.,1.));

 // histo control charge
  hListCal->Add(hAmplitude= new TH1F("hAmplitude","hAmplitude",1000,0.,500.)); 
  hListCal->Add(hCharge= new TH1F("hCharge","hCharge",1500,-500.,1000.));  
  hListCal->Add(hAmplitudeVSCharge= new TH2F("hAmplitudeVSCharge","hAmplitudeVSCharge",1000,0.,500.,1500,-500.,1000.)); 
  hListCal->Add(hDiffWavetemplate = new TH1F("hDiffWavetemplate", "hDiffWavetemplate", 500, -100,200));
  hListCal->Add(hChargeFirstHit = new TH1F("hChargeFirstHit", "hChargeFirstHit", 1500, -100, 1500));
  hListCal->Add(hEnergyFirstHit = new TH1F("hEnergyFirstHit", "hEnergyFirstHit", 1500, -100, 1500));
  hListCal->Add(hChargeSecondHit = new TH1F("hChargeSecondHit", "hChargeSecondHit", 1500, -100, 1500));
  hListCal->Add(hEnergySecondHit = new TH1F("hEnergySecondHit", "hEnergySecondHit", 1500, -100, 1500));
  hListCal->Add(hCharge3Hit = new TH1F("hCharge3Hit", "hCharge3Hit", 1500, -100, 1500));
  hListCal->Add(hEnergy3Hit = new TH1F("hEnergy3Hit", "hEnergy3Hit", 1500, -100, 1500));
  hListCal->Add(hECALsecondHitE      = new TH1F("ECAsecondLHitE","ECALsecondHitE",550,0,550));
  hListCal->Add(hDiffTimeHitWaveform = new TH1F("DiffTimeHitWaveform","DiffTimeHitWaveform",550,-500,500));
 hListCal->Add(hECALfirsthitEnergy = new TH1F("hECALfirsthitEnergy","hECALfirsthitEnergy",1500,-100,1500));
  hListCal->Add(hECALfirsthitEnergyCorrected = new TH1F("hECALfirsthitEnergyCorrected","hECALfirsthitEnergyCorrected",1500,-100,1550));
  hListCal->Add(hECALsecondhitEnergyCorrected = new TH1F("hECALsecondhitEnergyCorrected","hECALsecondhitEnergyCorrected",1500,-100,1550));
  hListCal->Add(hECALthirdhitEnergyCorrected = new TH1F("hECALthirdhitEnergyCorrected","hECALthirdhitEnergyCorrected",1500,-100,1550));

  //histo ECAL template
  hListCal->Add(hTemplate= new TH1F("hTemplate","hTemplate",1000,0.,1000.));
  
  for(int kk=0;kk<32;kk++){
    hPedCalo[kk] = new TH1D(Form("hPedCalo%d",kk),Form("hPedCalo%d",kk),1200,3300.,3900);
    hAvgCalo[kk] = new TH1D(Form("hAvgCalo%d",kk),Form("hAvgCalo%d",kk),1200,3300.,3900);
    hPedMean[kk] = new TH1D(Form("hSig%d",kk),Form("hSig%d",kk),1000,0.,1000.);
    hVMax[kk]    = new TH1D(Form("hVMax%d",kk),Form("hVMax%d",kk),1000,0.,1000.); // in mV
    h200QCh[kk]  = new TH1D(Form("h200QCh%d",kk),Form("h200QCh%d",kk),600,-200,400); //CT
    hQCh[kk]     = new TH1D(Form("hQCh%d",kk),Form("hQCh%d",kk),600,-200,400); //CT
    hListCal->Add(hPedCalo[kk]);
    hListCal->Add(hAvgCalo[kk]);
    hListCal->Add(hPedMean[kk]);
    hListCal->Add(hVMax[kk]);
    hListCal->Add(h200QCh[kk]); //CT
    hListCal->Add(hQCh[kk]); //CT
  }

}
void DigitizerChannelECal::DigitalProcessingRRC(Double_t *uin, Double_t *uout,int NPOINTS, Double_t timebin) {

  Double_t R1=1300;//ohms
  Double_t R2=100.; //ohms
  Double_t C=0.15e-9; //nF

  // Double_t R1=fDPParameterR1;//ohms
  // Double_t R2=fDPParameterR2; //ohms
  // Double_t C=fDPParameterC; //nF

  
  //Calculating the output pulse:
  Double_t integr=0;
  
  static Double_t ic[1024];

  Double_t bin_width=timebin;

  integr=0;
  ic[0]= uin[0]/R2;

  for(int i=1;i<NPOINTS;i++) {
    integr+=ic[i-1]*bin_width;
    ic[i]= uin[i]/R2 - ((R1+R2)/(C*R1*R2))* integr;
    uout[i] = uin[i] - integr/(C);
  }  
}

void DigitizerChannelECal::SaveDebugHistos(){
  fileOut->cd();
  if(fSaveAnalog) hListCal->Write(); //use it in monitor mode only  
  ECal->Write();
  // fileOut->Write();
  //  hListCal->ls();
  fileOut->Close();
}

void DigitizerChannelECal::SetAbsSignals(){
  for(UShort_t i = 0;i<fNSamples;i++){
    if (fSamples[i] < 2048) {
      fSamples[i] = 4096 - fSamples[i];
    }
  }
}
 
// Aho pero' questo trova il minimo (il segnale e'a scendere).
// perche' i counts sono discendenti.
Short_t DigitizerChannelECal::CalcMaximum() {

  fMax = 32767; // 2^15 - 1
  for(UShort_t i = 0;i<fNSamples;i++){
    if (fMax > fSamples[i]) { 
      fMax = fSamples[i];
      fIMax = i;
    }
  }
  return fMax;
}

// Computes pedestals as average of 1000 samples; store in structures the average of the observed pedestals
Double_t DigitizerChannelECal::CalcPedestal() {
  Int_t Ch   = GetElChID();
  Int_t BID  = GetBdID();
  TempPedMap[std::make_pair(BID,Ch)]   = TempPedMap[std::make_pair(BID,Ch)] + TMath::Mean(1000,&fSamples[0]);
  TempMapCount[std::make_pair(BID,Ch)] = TempMapCount[std::make_pair(BID,Ch)]+1;
  fPedMap[std::make_pair(BID,Ch)] = TempPedMap[std::make_pair(BID,Ch)]/TempMapCount[std::make_pair(BID,Ch)];
  return fPed;
}

// Cleans the pedestals map structure
void DigitizerChannelECal::ResetPedestal() {
  std::cout<<"Cleaning Ecal pedestal map structure "<<std::endl;
  TempPedMap.clear();
  TempMapCount.clear();
  fPedMap.clear();
}

// Compute zero suppression: returns 1 if the events has to be suppressed
Double_t DigitizerChannelECal::ZSupHit(Float_t Thr, UShort_t NAvg) {
  fRMS1000  = TMath::RMS(NAvg,&fSamples[0]);
  Double_t ZSupHit=-1;

  if(fRMS1000>Thr){
    ZSupHit=0;
  }else{
    ZSupHit=1;
    //    std::cout<<"compute zero supp "<<rms1000<<" Zsup "<<ZSupHit<<std::endl;
  }
  //  std::cout<<"compute zero supp "<<fRMS1000<<" Thr "<<Thr<<" Zsup "<<ZSupHit<<std::endl;
  return ZSupHit;
}

Double_t DigitizerChannelECal::CalcChargeSin(UShort_t iStart) {
  
  //  Short_t begin = iStart;  //to become iStart
  Short_t end   = 1000;

  Double_t Charge=0;
  Double_t ChargeSin=0;
  Double_t Charge200=0;
  Double_t ChargeHyb=0;
  Int_t Ch=0;
  Int_t BID=0;
  //  std::cout<<"Pedestal modes in charge "<<pedestalsFromFirstSamples()<<" "<< hybridPedestals()<<std::endl;
  if (pedestalsFromFirstSamples() || hybridPedestals() || fGlobalMode->GetGlobalDebugMode()!=0){
    //      Int_t Ch   = GetElChID();
    //      Int_t BID  = GetBdID();
//    Ch   = GetElChID();
//    BID  = GetBdID();      
    
    fAvg200  = TMath::Mean(iStart,&fSamples[0]);
    fRMS200  = TMath::RMS(iStart,&fSamples[0]);
    Zsup  = fRMS1000;  

    for(Short_t s=0;s<end;s++){
      AbsSamRec200[s] = (Double_t) (-1.*fSamples[s]+fAvg200)/4096*1000.; //in mV positivi using first Istart samples
      if(s>iStart && s<1000) {
	Charge200 += 1*AbsSamRec200[s]*1e-3/fImpedance*fTimeBin*1e-9/1E-12; 
      }
    }
    fVMax = TMath::MaxElement(1000,&AbsSamRec200[0]);
    //    Double_t ZeromV=TMath::Mean(iStart,&AbsSamRec200[0]);
    //if(abs(ZeromV)>0.0001 )  std::cout<<"BAD PEDESTAL!!! "<<ZeromV<<std::endl;
    if(pedestalsFromFirstSamples()) Charge = Charge200; 
  }

  fCharge=Charge;
  return fCharge;
  if(pedestalsFromAutoTrgEvents() || hybridPedestals() || fGlobalMode->GetGlobalDebugMode()!=0){
    Ch   = GetElChID();
    BID  = GetBdID();
    
    for(Short_t s=0;s<end;s++){
      AbsSamRec[s]    = (Double_t) (-1.*fSamples[s]+fPedMap[std::make_pair(BID,Ch)])/4096*1000.; //in mV positivi
      if(s>iStart && s<1000) ChargeSin += 1*AbsSamRec[s]   *1e-3/fImpedance*fTimeBin*1e-9/1E-12; 
    }
    if (pedestalsFromAutoTrgEvents()) Charge = ChargeSin;
  }
  if (hybridPedestals() || fGlobalMode->GetGlobalDebugMode()!=0){
    Int_t Ch   = GetElChID();
    Int_t BID  = GetBdID();
    
    Zsup  = fRMS1000;  
    for(Short_t s=0;s<end;s++){
      // Hybrid calculation just an attempt most probably useless
      if(fRMS200 < 5){ 
	AbsSamRecHyb[s] = (Double_t) (-1.*fSamples[s]+fAvg200)/4096*1000.; //in mV positivi
      }else{
	AbsSamRecHyb[s] = (Double_t) (-1.*fSamples[s]+fPedMap[std::make_pair(BID,Ch)])/4096*1000.; //in mV positivi     
      }
      if(s>iStart && s<1000) ChargeHyb += 1*AbsSamRecHyb[s]*1e-3/fImpedance*fTimeBin*1e-9/1E-12;     
    }
    if (hybridPedestals()) Charge = ChargeHyb;
  }
  if(fGlobalMode->GetGlobalDebugMode()!=0){
    //    std::cout<<"Pedestal first 200 in charge "<<pedestalsFromFirstSamples()<<" Charge "<<Charge<<" Charge 200 "<<Charge200<<std::endl;
    //filling NTU Variables
    HitE    = ChargeSin/15.;
    HitE200 = Charge200/15.;
    HitEHyb = ChargeHyb/15.;
    Int_t code=BID*100+Ch;
    ElCh=code;
    Row=GetChID()%100;
    Col=GetChID()/100;
    //    ECal->Fill();
  }

  if(BID==14 && fGlobalMode->GetGlobalDebugMode()!=0){
    char name[50];
    //     std::cout<<Ch<<" "<<BID<<" Charge "<<ChargeSin<<std::endl;
    sprintf(name,"hPedCalo%d",Ch);
    // std::cout<<Ch<<" "<<BID<<" Charge "<<name<<std::endl;
    histo =(TH1D*) hListCal->FindObject(name);
    histo->Fill(fAvg200);

    sprintf(name,"hAvgCalo%d",Ch);
    histo =(TH1D*) hListCal->FindObject(name);
    double pedAvg = fPedMap[std::make_pair(BID,Ch)];
    histo->Fill(pedAvg);
    
    sprintf(name,"hQCh%d",Ch);
    histo =(TH1D*) hListCal->FindObject(name);
    histo->Fill(ChargeSin/15.);

    sprintf(name,"h200QCh%d",Ch);
    histo =(TH1D*) hListCal->FindObject(name);
    histo->Fill(Charge200/15.);
  }

//  if(ChargeSin/15.<-15) {
//    if(fIsGlobalDebug){
//      Int_t code=BID*100+Ch;
//      H1 = new TH1D(Form("hV%d",code),Form("hV%d",code),end,0.,end);
//      H1->SetContent(AbsSamRec);
//      //     H2 = new TH1D(Form("h2V%d",m),Form("h2V%d",m),end,0.,end);
//      //      H2->SetContent(AbsSamRec200);
//      //  
//      fileOut->cd();
//      H1->Write();
//      //      H2->Write();
//      //      H1->Reset();
//      //      H2->Reset();
//      //      m++;
//    }
//  }

  fCharge=Charge;
  return Charge;
}

Double_t DigitizerChannelECal::CalcCharge(UShort_t iMax) {
  
  Short_t begin = iMax-fPreSamples > 0? iMax-fPreSamples:0;
  Short_t end   = iMax+fPostSamples < fNSamples? iMax+fPostSamples:fNSamples;
 
  //  std::cout << "Begin: "<< begin << "  end: " << end << std::endl;
  fCharge=0.;
  for(Short_t i = begin;i<end;++i) {
    fCharge+=1.* fSamples[i];
  }
  
//  std::cout << "Integral: " << fCharge << "  Ped:  " << fPed << " Base: " << (end-begin) * fPed << std::endl;
//  std::cout << "Pedestal: " << fPed << "  from: " << fNPedSamples << " samples"<< std::endl;
//  std::cout << "Maximum: " << fMax << "  at position: "<< fIMax << std::endl;

  fCharge = ((1.*end-1.*begin) * fPed) - fCharge;  //subtract events pedestral
  fCharge *= (fVoltageBin*fTimeBin/fImpedance);    // charge in nC
  return fCharge;
}

// try using oversampled histograms to improve time resolution
Double_t DigitizerChannelECal::CalcTimeOver(UShort_t iDer) {
  Double_t dxdt[4096];
  Double_t Over4[4096];
  Double_t Temp[4096];
  Double_t Temp1[4096];
  Int_t MaxSam = 4000;

  // oversample the signal by factor 4 with linear interpolation
  //  if(fGlobalMode->GetGlobalDebugMode()){
  histo   = (TH1D*)  hListTmp->FindObject("hdxdtSigOvSm");
  histo1  = (TH1D*)  hListTmp->FindObject("hSigOvSm");
  histoOv = (TH1D*)  hListTmp->FindObject("hSigOv");
  OverSample4(fSamples,Over4,1001);    
  for(Int_t kk=0;kk<=MaxSam;kk++){
    histoOv->SetBinContent(kk,Over4[kk]);
  }
    //  }
  
  //  Int_t npeaks=4;
  Int_t nsmooth=5*4;
  // Smooth the signal by averaging nsmooth samples 
  for(Int_t ll=0;ll<=MaxSam-nsmooth/2;ll++){
    if(ll>nsmooth/2){
      Temp[ll] =TMath::Mean(nsmooth,&Over4[ll-nsmooth/2]); // averaging over ±nsmooth/2 samples on the oversampled histogram 
      Temp1[ll]=(-1.*Temp[ll]+fAvg200)/4096*1000.;  // transform in postive mV using first Istart samples for pedestal
    }else{
      Temp[ll]=0;
      Temp1[ll]=0;
    }
    histo1->SetBinContent(ll,Temp1[ll]);
    // compute raw derivative subracting samples
    if(ll>iDer+nsmooth/2 && ll<2200){ 
      dxdt[ll]=-(Temp[ll]-Temp[ll-iDer]);
    }else{
      dxdt[ll]=0;
    }
    histo->SetBinContent(ll,dxdt[ll]);
    //  std::cout<<ll<<" sam "<<Temp[ll]<<" "<<Temp1[ll]<<" "<<dxdt[ll]<<std::endl;
  }
//  Int_t Ch   = GetElChID();
//  Int_t BID  = GetBdID();

  Int_t MaxBin = histo->GetMaximumBin();
  Int_t Max    = histo->GetMaximum();
  fTimeOv = (Double_t)MaxBin/4*fTimeBin;   //convert the time back in ns

  if(fGlobalMode->GetGlobalDebugMode()){  
    hdxdtMax->Fill(Max);
    hdxdtRMS->Fill(TMath::RMS(1000,&dxdt[0]));
    if(Max>200) hTimeCut->Fill(fTimeOv);
    hTimeOv->Fill(fTimeOv);
  }

  //  std::cout<<" MaxBin "<<MaxBin<<" Max "<<Max<<" "<<fTimeOv<<" "<<std::endl;


  if(fGlobalMode->GetGlobalDebugMode()){
    Double_t rnd=((double) rand() / (RAND_MAX));
    //    if(rnd<0.01){ 
    if(rnd<0.02){    /////CAMBIARE
      //      hListEv->Write();
      if(fSaveAnalog) hListTmp->Write();
    }
  }
  //  Int_t TOver4=GetStartTime(Over4,150);
  histo->Reset();
  histo1->Reset();
  histoOv->Reset();
  return fTimeOv;
}

// first approximation timing algorithm to be optimized  M. Raggi
Double_t DigitizerChannelECal::CalcTimeSing(UShort_t iDer) {
  fCountEventControl++;
  Int_t ll;
  fAmplitude=0.;

  static Double_t dxdt[1001];
  static Double_t Temp[1001];
  static Double_t Temp1[1001];

  if(fUsePulseProcessing ) {
    static Double_t DPP[1001];
    
    static Double_t AbsSamRec[1024];
    static Double_t AbsSamRecDP[1024];


    for(UShort_t s=0;s < 1000 ;s++){
      AbsSamRec[s] = (Double_t) (-1.*fSamples[s]+fAvg200)/4096.*1000.; //in mV positivo
      //std::cout<< s << "     "  << fSamples[s]  <<" V "<< AbsSamRec[s]  <<std::endl;
    }
    for(UShort_t s=1000;s<1024;s++){
      AbsSamRec[s] = 0;
    }
    DigitalProcessingRRC(AbsSamRec,AbsSamRecDP,999 ,fTimeBin*1e-9);
  }
  
  // H1->Reset();

  //  if (fUsePulseProcessing) {
  //DigitalProcessingRRC(AbsSamRec,AbsSamRecDP,iMax-1 ,fTimeBin*1e-9);



  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode()){
    histo   = (TH1D*)  hListTmp->FindObject("hdxdt");
    histo1  = (TH1D*)  hListTmp->FindObject("hSignal");
  }else{
    histo   = (TH1D*)  hListTmp->FindObject("hdxdt");
    histo1  = (TH1D*)  hListTmp->FindObject("hSignal");
  }
  
  Int_t nsmooth=5;
  // Smooth the signal by averaging nsmooth samples 
  //  for(ll=1;ll<1001;ll++){
  for(ll=nsmooth/2;ll<1001;ll++){
    if(ll>nsmooth/2){
      Temp[ll] =TMath::Mean(nsmooth,&fSamples[ll-nsmooth/2]); // averaging over ±nsmooth/2 samples 
      Temp1[ll]=(-1.*Temp[ll]+fAvg200)/4096*1000.;  // transform in postive mV using first Istart samples for pedestal
    }else{
      Temp[ll]=0;
      Temp1[ll]=0;
    }
    histo1->SetBinContent(ll,Temp1[ll]);
    // compute raw derivative subracting samples
    if(ll>iDer+nsmooth/2 && ll<550){ 
      dxdt[ll]=-(Temp[ll]-Temp[ll-iDer]);
    }else{
      dxdt[ll]=0;
    }
    histo->SetBinContent(ll,dxdt[ll]);
    //    std::cout<<ll<<" sam "<<Temp[ll]<<" "<<Temp1[ll]<<" "<<dxdt[ll]<<std::endl;
  }
  histo->SetBinContent(ll,dxdt[ll]);

  //  Int_t Ch   = GetElChID();
  //  Int_t BID  = GetBdID();
  
  Int_t MaxBin = histo->GetMaximumBin();
  Int_t Max    = histo->GetMaximum();
  fTimeSin = (Double_t)MaxBin*fTimeBin; 
  fAmplitude = histo1->GetBinContent(MaxBin);
  //  if(Max>30) 
  //  std::cout<<Ch<<" "<<BID<<" Max "<<Max<<" M bin "<<MaxBin<<" ftime "<<fTimeSin<<" "<<fCharge/15.<<std::endl;
  
  //  //// TSPECTRUM search is too time consuming.
  // double XBig=10000.;
  //  Int_t npeaks=4;
  // fTimeSin=XBig;
  // if(Max>100){
  //   TSpectrum *s = new TSpectrum(npeaks);
  //   //  Double_t peak_thr  = fAmpThresholdLow/Max;   //minimum peak height allowed.
  //   Int_t nfound = s->Search(histo,6,"",0.3);   //corrected for 2.5GHz cannot be less then 0.05
  //   // std::cout<<"found Npeaks "<<nfound<<""<<std::endl;
  //   // ROOT 6 version
  //   //    Double_t *xpeaks = s->GetPositionX();
  //   //    Double_t *ypeaks = s->GetPositionY();
  //   // ROOT 5 version
  //   Float_t *xpeaks = s->GetPositionX();
  //   Float_t *ypeaks = s->GetPositionY();
  //   //    std::cout<<"found Npeaks "<<nfound<<""<<std::endl;
  //   for(Int_t ll=0;ll<nfound;ll++){ //peak loop per channel
  //     // ROOT 6 version
  //     //      Double_t xp   = xpeaks[ll];
  //     //      Double_t yp   = ypeaks[ll];
  //     // ROOT 5 version
  //     Float_t xp   = xpeaks[ll];
  //     Float_t yp   = ypeaks[ll];
  //     if(xp<fTimeSin) fTimeSin = xp*fTimeBin; //convert time in ns get it from data
  //     //    fTimeSin = Max*fTimeBin; //convert time in ns get it from data
  //   }
  // }

  if(fGlobalMode->GetGlobalDebugMode()){  
    hdxdtMax->Fill(Max);
    hdxdtRMS->Fill(TMath::RMS(1000,&dxdt[0]));
    if(Max>100) hTimeCut->Fill(fTimeSin);
    hTime->Fill(fTimeSin);
  }
  /*  IO commented 
  if(fGlobalMode->GetGlobalDebugMode()){
    Double_t rnd=((double) rand() / (RAND_MAX));
    if(rnd<0.2){ 
      //      hListEv ->Write();
      if(fSaveAnalog) hListTmp->Write();
    }
    } end IO comment*/

  //  PeakSearch();

//  hTime->Fill(fTimeSin);
  //  histo->Fit("gaus");
  // std::cout<<"fTime "<<fTimeSin<<std::endl;
  //  hTime->Write();
 
  histo->Reset();
  histo1->Reset();
  return fTimeSin;
}

Double_t DigitizerChannelECal::CalcTime(UShort_t iMax) {
  fTime = 0.;
  //currently looking only at the signal rising edge
  
  float t1=0.;
  float t2=0.;

  float t3=0.;
  float t4=0.;

  
  float val1=0.;
  float val2=0.;

  int t1_ok=0;
  int t2_ok=0;

  int t3_ok=0;
  int t4_ok=0;

  float max = ( fPed - fMax);

  Short_t begin = fIMax - fPreSamples > 0? fIMax - fPreSamples:0;
  for(Short_t i = begin ;i < fIMax;i++) {
    val1 = fPed - fSamples[i];
    val2 = fPed - fSamples[i+1];
    
    // if( t1_ok == 0 && val1 < TH_RATIO_1*max && val2 > TH_RATIO_1*max) {
    //   t1 = i +  (TH_RATIO_1*max - val1)/(val2 - val1)  ;
    //   t1_ok = 1;
    // }
    // if( t1_ok = 1 && t2_ok == 0 && val1 < TH_RATIO_2*max && val2 > TH_RATIO_2*max) {
    //   t2 = i+ (TH_RATIO_2*max - val1)/(val2 - val1)   ;
    //   t2_ok = 1;
    // }

    if( t3_ok == 0 && val1 <= fAmpThresholdLow && val2 >fAmpThresholdLow) {
      t3 = 1.*i + (fAmpThresholdLow - val1)/(val2 - val1);
      t3_ok = 1;
    }
    if( t3_ok == 1 && t4_ok == 0 && val1 <= fAmpThresholdHigh && val2 > fAmpThresholdHigh) {
      t4 = 1.*i + (fAmpThresholdHigh - val1)/(val2 - val1);
      t4_ok = 1;
    }
  }
  
  //  fTime = (t1 + t2)/2.;
  fTime = t3 - (t4-t3)*fAmpThresholdLow/(fAmpThresholdHigh - fAmpThresholdLow);
  return fTime = fTime*fTimeBin;
}

void DigitizerChannelECal::ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray){
  IsSat=0;
  //  std::cout<<"Zsupp "<<fZeroSuppression<<std::endl;
  Double_t IsZeroSup = ZSupHit(fZeroSuppression,1000.);
  //  Double_t IsZeroSup = ZSupHit(5,1000.);
  // if(fZeroSuppression>0) IsZeroSup = ZSupHit(fZeroSuppression,1000.);
  // IsSaturated(); //check if the event is saturated M. Raggi 03/2019
  
  if(IsZeroSup==1 && !fGlobalMode->IsPedestalMode()) return; //perform zero suppression unless you are doing pedestals
 //***********************************************
  // Fix a broken chip in digitizer 8 durign 2019 run
  //***********************************************
  Int_t Ch     = GetElChID();
  Int_t BID    = GetBdID();
  if(BID==8 && Ch>=16 && Ch<=23){
    if(GetBadInd()>0){
      //      std::cout<<"fixing pedestal issue BD "<<BID<<" Ch "<<Ch<<" ev "<<vDerdt.size()<<std::endl;
      std::cout<<"fixing pedestal issue BD "<<BID<<" Ch "<<Ch<<" ev "<<std::endl;
      Fix2019BrokenChip(GetBadInd()); //fix errors if there is a misalignement
    }
  }

  fTrig = GetTrigMask();

  if(fUseOverSample){
    //    std::cout<<" over sampled "<<std::endl;
    HitT = CalcTimeOver(40);
  }else{
    //    std::cout<<" NON over sampled "<<std::endl;
    HitT = CalcTimeSing(10);
  } 

  if(GetTrigMask()!=2) CalcChargeSin(HitT-10);  //Physics in ECAL starts ~250 ns
  if(GetTrigMask()==2) CalcChargeSin(40);   //Cosmics in ECal start  ~40 ns
  if(IsSaturated()) IsSat=1; //check if the event is saturated M. Raggi 03/2019
  // M. Raggi going to energy with Nominal Calibration
  Double_t fEnergy= fCharge/15.; //going from pC to MeV using 15pC/MeV
  // std::cout <<"At the the digi levevl Hit charge:  " << fCharge << "  Time: " << fEnergy <<" HitE200 "<<HitE200<<std::endl; 
  if (fEnergy < 1. && !fGlobalMode->IsPedestalMode()) return; //cut at 1 MeV nominal

  if(!fGlobalMode->IsPedestalMode()){
    //correct for saturation effects integrated charge M. Raggi 23/05/2019
    // do it before extrapolating to full integral
    if(IsSat && fSaturatioCorrection) {
      Double_t ESatCorr = CorrectSaturation();
      fEnergy += ESatCorr; 
      HitE200 += ESatCorr;     
    }
    
    //correct for non integrated charge M. Raggi 15/05/2019
    if(fIntCorrection){ 
      Double_t QIntCorr = CorrectIntegrationTime(HitT,1000.);
      if(fGlobalMode->GetGlobalDebugMode()) hTIntCorr->Fill(QIntCorr); ///HISTO FILL
      fEnergy /= QIntCorr; 
      HitE200 /= QIntCorr; 
    }
  }
  //Filling hit structure
  TRecoVHit *Hit = new TRecoVHit();
  Hit->SetTime(HitT);
  Hit->SetEnergy(fEnergy);
  hitArray.push_back(Hit);
  fFirstHit = true;
  std::cout<<"i'm compiled the single hit reconstruction " << std::endl;
  if(fGlobalMode->GetGlobalDebugMode()) {
    ECal->Fill();
    // hCharge->Fill(fCharge);
    // hAmplitude->Fill(fAmplitude);
    // hAmplitudeVSCharge->Fill(fAmplitude, fCharge);
  }
  // std::cout << "Hit charge:  " << fCharge << "  Time: " <<HitT << "Hit array size "<< hitArray.size()<<std::endl; 
}


//M. Raggi: 21/01/2018 
//reads board pedestal from file in config/BDped 
void DigitizerChannelECal::SetAnalogOffSets(){
  std::ifstream PrevPed; 
  char fname[50];
  // Reading first half of the pedestals
  for(Int_t NBD=0;NBD<10;NBD++){
    //    sprintf(fname,"config/BDPed/PedBD%d.ped",NBD);
    sprintf(fname,"config/BDPed/Ped%d.txt",NBD);
    PrevPed.open(fname);
    if(PrevPed.is_open()){
      double temp;
      for(int i=0;i<32;i++){
	PrevPed >> temp >> fPedCh[i] >> temp>> temp;//reads channel number and discards it
	fPedMap[std::make_pair(NBD,i)] = fPedCh[i];
      }
    PrevPed.close();
    }
    else{ 
      std::cout << "ERROR: No previous data available for board "<<NBD<<" resorting to default pedestal (3800)"<<std::endl;
      //exit(-1);
    } 
  }

  // reading second half of the pedestals
  for(Int_t NBD=14;NBD<24;NBD++){
    //sprintf(fname,"config/BDPed/PedBD%d.ped",NBD);
    sprintf(fname,"config/BDPed/Ped%d.txt",NBD);
    PrevPed.open(fname);
    if(PrevPed.is_open()){
      double temp;
      for(int i=0;i<32;i++){
	//      PrevPed >> temp >> fPedCh[i] >> temp>> temp >> fCalibCh[i];//reads channel number and discards it
	PrevPed >> temp >> fPedCh[i] >> temp>> temp;//reads channel number and discards it
	//	std::cout<<NBD<<" PEDCH "<<i<<" "<<fPedCh[i]<<std::endl;      
	fPedMap[std::make_pair(NBD,i)] = fPedCh[i];
      }
    PrevPed.close();
    }
    else{ 
      std::cout << "ERROR: No previous data available for board "<<NBD<<" resorting to default pedestal (3800)"<<std::endl;
      //exit(-1);
      
    } 
  }

}
/*
void DigitizerChannelECal::ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray){
  std::cout<<"************************ RUNNING ECAL MULTI HIT RECONSTRUCTION *****************"<<std::endl;   
  std::cout<<"************************  V1 M. Raggi 16/05/2019               *****************"<<std::endl;   

  Double_t IsZeroSup = ZSupHit(5.,1000.);
  IsSaturated(); //check if the event is saturated M. Raggi 03/2019
  //  if(IsSaturated()) return;  //remove is a test
  if(IsZeroSup==1 && !fGlobalMode->IsPedestalMode()) return; //perform zero suppression unless you are doing pedestals
  fTrig = GetTrigMask();

}*/

void DigitizerChannelECal::ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray){
  fFirstHit=false;
  ReconstructSingleHit(hitArray);
  // std::cout<<"ok 1 hit "<< fFirstHit<< std::endl;
  Double_t energySecondHit=0.;
  Double_t energyFirstHit=0.;
  if(fFirstHit){
    Bool_t SecondHit=false;
    Bool_t ThirdHit=false;
    Double_t SecondEnergy=0.;
    Double_t SecondTime=0.;
    Double_t ThirdEnergy=0.;
    Double_t ThirdTime=0.;
    DrawMeanWave(10, SecondEnergy, SecondTime, ThirdEnergy, ThirdTime, SecondHit, ThirdHit);
    std::cout<< "firsthit "<< fFirstHit <<"second Energy " << SecondEnergy <<" bool " << SecondHit << " third energy " << ThirdEnergy << " bool " << ThirdHit << std::endl; 
    if(SecondHit && SecondEnergy>0.){
      // std::cout<<"In Reconstruct, second hit " << SecondHit << std::endl;
      if(fIntCorrection){ 
        Double_t QIntCorr = CorrectIntegrationTime(SecondTime,1000.);
        SecondEnergy /= QIntCorr; 
      }
      unsigned int nHitsBefore = hitArray.size()-1; 
      Double_t EnergyHitBefore=hitArray.at(nHitsBefore)->GetEnergy();
      Double_t TimeHitBefore=hitArray.at(nHitsBefore)->GetTime();
      if(TimeHitBefore>SecondTime){
	energyFirstHit-= energyFirstHit*exp(-(SecondTime-TimeHitBefore)/300);
      }
      else  energyFirstHit=EnergyHitBefore-SecondEnergy;
      if(EnergyHitBefore<0.1)std::cout<<"FIRTS HIT with energy 0 !!!!!!!" << std::endl;
      std::cout<<"In reconstruct multi hit ..in SecondHit , energy first hit corrected " << energyFirstHit <<" secondhitenergy " << SecondEnergy<<  std::endl; 
      hitArray.at(nHitsBefore)->SetEnergy(energyFirstHit);
      
      if(fGlobalMode->GetGlobalDebugMode()) {  
	fDiffTimeWave=hitArray.at(nHitsBefore)->GetTime()-SecondTime; 
	fEnergySecondHit=SecondEnergy;
	hECALsecondHitE->Fill(SecondEnergy);
	hDiffTimeHitWaveform->Fill(fDiffTimeWave);
   	hECALfirsthitEnergy->Fill(EnergyHitBefore);
	ECal->Fill();
      }

      TRecoVHit *Hit = new TRecoVHit();
      Hit->SetTime(SecondTime);
      Hit->SetEnergy(SecondEnergy);
      hitArray.push_back(Hit);
   std::cout<<"ADDED HIT " << std::endl;
      if(fGlobalMode->GetGlobalDebugMode()) ECal->Fill();
    }
    if(ThirdHit && ThirdEnergy>0.){
      if(fIntCorrection){ 
        Double_t QIntCorr = CorrectIntegrationTime(ThirdTime,1000.);
        ThirdEnergy /= QIntCorr; 
      }
      unsigned int nHitsBefore = hitArray.size()-1;
      Double_t EnergyHitBefore=hitArray.at(nHitsBefore)->GetEnergy();
      Double_t TimeHitBefore=hitArray.at(nHitsBefore)->GetTime();
      if(TimeHitBefore>ThirdTime){
	energySecondHit-= energySecondHit*exp(-(ThirdTime-TimeHitBefore)/300);
      }
      else energySecondHit=EnergyHitBefore-ThirdEnergy;
      if(EnergyHitBefore<0.1)std::cout<<"SECOND HIT with energy 0 !!!!!!!" << std::endl;
      std::cout<<"In reconstruct multi hit ..in ThirdHit , energy second hit "<< SecondEnergy <<" corrected " << energySecondHit <<" thirdhitenergy " << ThirdEnergy<<  std::endl;
      // if(energySecondHit<20){
      //	hitArray.erase(nHitsBefore);
      //	SecondHit=false;
      //}
      // else{
      //	std::cout<<"in ThirdHit , energy second hit " << energySecondHit << std::endl; 
      hitArray.at(nHitsBefore)->SetEnergy(energySecondHit);
      TRecoVHit *Hit1 = new TRecoVHit();
      Hit1->SetTime(ThirdTime);
      Hit1->SetEnergy(ThirdEnergy);
      hitArray.push_back(Hit1);
   std::cout<<"ADD HIT " << std::endl;
    }
    if(fGlobalMode->GetGlobalDebugMode()) {
      hECALfirsthitEnergyCorrected->Fill(energyFirstHit);
      if(SecondHit) hECALsecondhitEnergyCorrected->Fill(energySecondHit);
      if(ThirdHit) hECALthirdhitEnergyCorrected->Fill(ThirdEnergy); 
    }  
  } 
}


void DigitizerChannelECal::Reconstruct(std::vector<TRecoVHit *> &hitArray){
  fTrigMask = GetTrigMask();
  if(fUseAbsSignals) {
    SetAbsSignals();
  }
  CalcMaximum();
  if(fGlobalMode->IsPedestalMode()) CalcPedestal();
  //  if(GetTrigMask()==0) CalcPedestal();
  // if(fPed - fMax < fMinAmplitude ) return; //altro taglio da capire fatto in che unita'? conteggi?

  if(fMultihit) {
    ReconstructMultiHit(hitArray);
  } else {
    ReconstructSingleHit(hitArray);
  }
  // std::cout << "Maximum: " << fMax << "  at position: "<< fIMax << std::endl;
  // std::cout << "Pedestal: " << fPed << "  from: " << fNPedSamples << " samples"<< std::endl;
}

DigitizerChannelECal::~DigitizerChannelECal(){
  if(fGlobalMode->IsPedestalMode()) std::cout<<"Saving ECal BD pedestals "<<std::endl;
  for(Int_t KK=0;KK<10;KK++){
    if(fTrigMask==128 && fGlobalMode->IsPedestalMode()) SaveBDPed(KK);  //save only if pedestal run type is selected
  }
  
  for(Int_t KK=14;KK<24;KK++){
    if(fTrigMask==128 && fGlobalMode->IsPedestalMode()) SaveBDPed(KK); //save only if pedestal run type is selected
  }
  if(fGlobalMode->IsPedestalMode() || fGlobalMode->GetGlobalDebugMode()){
    SaveDebugHistos();
  }
}

void DigitizerChannelECal::SaveBDPed(Int_t BID){
  TString fName;
  fName = Form("config/BDPed/Ped%d.txt",BID);
  std::cout<<"Saving Ecal Pedestal "<<fName<<std::endl;
  std::ofstream cout(fName);
  for(Int_t Ch=0;Ch<32;Ch++){
    cout<<Ch<<" "<<fPedMap[std::make_pair(BID,Ch)]<<std::endl;
  }
  cout.close();
}

// M. Raggi 15/05/2019
// Provides correct amount of deposited charge correcting for shorter integration 
// window in the digitizer due to different hit arrival time. 
// Be carefull if the arrival time is wrong you will get worst charge determination 
Double_t DigitizerChannelECal::CorrectIntegrationTime(Double_t TStart,Double_t TStop){
  Double_t IntWindowEnd=1000.; //ns 
  Double_t IntWindowWdt=0.;    //ns 
  Double_t tau=300.;                  //BGO decay time in ns need tuning
  IntWindowWdt=IntWindowEnd-TStart;   // compute the effective integration window
  // gives the fraction of integrated charge in the available window
  Double_t Correction = 1-exp(-IntWindowWdt/tau);
  return Correction;
}

// Increase the number of samples to 4Gs interpolating. M. Raggi preliminary needs tests 04/2019
//void DigitizerChannelECal::OverSample4(Double_t* v, Double_t* o, Int_t n) {
void DigitizerChannelECal::OverSample4(Short_t* v, Double_t* o, Int_t n) {
  int j,k;
  double v1[2048];
  for (j=0;j<n;j++)
  {
     v1[2*j]=v[j];
     v1[2*j+1]=(v[j]+v[j+1])/2.;
     // std::cout<<"Azz "<<j<<" "<<v[j]<<" "<<v1[j]<<std::endl;
  }
  v1[2*n]=v[n];
  for (j=0;j<2*n;j++){
     o[2*j]=v1[j];
     o[2*j+1]=(v1[j]+v1[j+1])/2.;
//     //     std::cout<<j<<" "<<v1[j]<<std::endl;
  }
  o[4*n]=v1[2*n];
  for(k=0;k<4*n;k++){
    //std::cout<<k<<" "<<v[k/4]<<" "<<o[k]<<std::endl;
  }
}

// Trying to implement zero crossing. M. Raggi preliminary needs tests 04/2019
Int_t DigitizerChannelECal::GetStartTime(Double_t* v, Int_t nshift) {
  //  Int_t nshift = 6;
  Int_t bins=950*4;
  histoDiff = (TH1D*) hListTmp->FindObject("hDiff");
  Double_t orig[4*1024];
  Double_t shift[4*1024];
  Double_t diff[4*1024];
  nshift=10;
  for (Int_t i=0;i<bins-nshift;i++){ 
    shift[i]=-1.;
  }
//  for (Int_t i=0;i<bins-nshift;i++) {
//    orig[i] = v[i];
//    shift[i+nshift] = 0.35*orig[i];
//  }

  for (Int_t i=nshift;i<bins;i++) {
    orig[i] = v[i];
    shift[i-nshift] = 0.35*orig[i];
  }

  for (Int_t i=0;i<bins-nshift;i++) {
    diff[i] = orig[i]-shift[i];
    //    std::cout<<i<<" "<<orig[i]<<" "<<shift[i]<<" "<<diff[i]<<" "<<std::endl;
    histoDiff->SetBinContent(i,diff[i]);
  }
  
  for (Int_t i=0;i<bins;i++) {
    Double_t diff = v[i];
    if (diff<0.) return i;
  }
  return 16384;
}

// checks if the signal is saturated or if there are strange samples
Bool_t DigitizerChannelECal::IsSaturated(){
  Bool_t IsSaturated=false;
  Short_t min  = TMath::MinElement(1000,&fSamples[0]); 
  Short_t max  = TMath::MaxElement(1000,&fSamples[0]); 
  fNSat = 0;
  Short_t SatThr = 15;
  Short_t  FirstSat = -1;
  Short_t  LastSat  = -1;
  Int_t Ch     = GetElChID();
  Int_t BID    = GetBdID();


  //  if(min < 5 || max>5050){ 
  if(min < SatThr){ 
    //    if(fGlobalMode->GetGlobalDebugMode()) histoSat = (TH1D*) hListTmp->FindObject("hSat"); // swt the debug flag.
    histoSat = (TH1D*) hListTmp->FindObject("hSat"); // swt the debug flag.
    
    //    std::cout<<"saturated!! "<<min<<" BID "<<BID<<" CH "<<Ch<<"fPed "<<fPedMap[std::make_pair(BID,Ch)]<<std::endl;
    for(int ll=0;ll<1001;ll++){
      if(fSamples[ll]<SatThr){
	if(FirstSat==-1) FirstSat = ll;
	LastSat=ll;
	fCountsLastSat=fSamples[ll];
	fNSat++;
      }
      //  if(fGlobalMode->GetGlobalDebugMode()) histoSat->SetBinContent(ll,fSamples[ll]);
      histoSat->SetBinContent(ll,(Double_t)fSamples[ll]);
      ///      std::cout<<"Filling "<<ll<<" fSample "<<fSamples[ll]<<std::endl;
    }
    if(fNSat>4) IsSaturated=true;
  }
  return IsSaturated;
};

// Return the energy to be added due to saturation of the signal.
// using first Istart samples to compute pedestal
Double_t DigitizerChannelECal::CorrectSaturation(){
  Double_t VLastSat = 0.;
  Double_t tau=150.;
  VLastSat = (Double_t) (-1.*fCountsLastSat+fAvg200)/4096*1000.; 
  fVMax                   = VLastSat/(exp(-fNSat/tau));
  Double_t ChargeCorrExp  = (fVMax-VLastSat)*tau-(fVMax-VLastSat)*tau*(exp(-fNSat/tau));
  Double_t ChargeCorrTri  = (fVMax-VLastSat)*fNSat/2;

  Double_t ChargeCorrpC     = ChargeCorrTri*1e-3/fImpedance*fTimeBin*1e-9/1E-12;
  // Double_t AdditionalEnergy = ChargeCorrpC/15.;  

  ChargeCorrExp     = ChargeCorrExp*1e-3/fImpedance*fTimeBin*1e-9/1E-12;
  Double_t AdditionalEnergy = ChargeCorrExp/15.;  
  //  Double_t AdditionalEnergy = ChargeCorrTri/15.;  

//  // Use the average correlation of Vmax to energy to compute the value of the energy given
//  Double_t EnergyExtrapolated=

  //  std::cout<<"saturated!! "<<min<<" BID "<<BID<<" CH "<<Ch<<" nsat "<<nsat<<" First "<<FirstSat<<" last "<<LastSat<<" VLast "<<VLastSat<<std::endl;
  //  std::cout<<"New Max "<<fVMax<<" charge corr "<<ChargeCorrpC<<" Charge corr exp "<<ChargeCorrExp<<std::endl;
  //    if(fGlobalMode->GetGlobalDebugMode()) histoSat->Write();
  //    if(fGlobalMode->GetGlobalDebugMode()) histoSat->Reset();
  return AdditionalEnergy;
}


// searches for peak in the signal derivative
Double_t DigitizerChannelECal::PeakSearch(){
  TH1D* histo     = (TH1D*)  hListTmp->FindObject("hdxdt");
  TH1D* Signal    = (TH1D*)  hListTmp->FindObject("hSignal");
  TH1D* histotmp  = (TH1D*) histo->Clone();
  TH1D* SignalTmp = (TH1D*) Signal->Clone();
  Int_t Max    = histotmp->GetMaximum();
  Int_t npeaks = 0.;
  Int_t NRise  = 20.;
  histotmp->Write();
  //  SignalTmp->Write();
  while(Max>50){
    Int_t MaxBin = histotmp->GetMaximumBin();
    for(Int_t kk=MaxBin-NRise;kk<MaxBin+NRise;kk++){
      histotmp->SetBinContent(kk,0.);
    }
    std::cout<<histotmp<<" Dtdt max "<<Max<<" maxbin "<<MaxBin<<std::endl;
    Max    = histotmp->GetMaximum();
    npeaks++;
  }
  return 0.;
  //  std::cout<<"changing histogram "<<npeaks<<std::endl;
}





//multi hit functionalities IO
void DigitizerChannelECal::DrawMeanWave(UShort_t iDer,Double_t& SecondEnergy,Double_t& SecondTime,Double_t& ThirdEnergy,Double_t& ThirdTime, Bool_t& SecondHit, Bool_t& ThirdHit) {
 
  std::cout<<"DrawMeanWave....................................Event N. " << fCountEvent <<std::endl;

 Int_t ll;
 fAmplitude=0.;
 static Double_t dxdt[1001];
 static Double_t dxdt1[1001];
  static Double_t Temp[1001];
  static Double_t Temp1[1001];
  std::vector<Double_t> TempWave;
  std::vector<Double_t> Wave;
    
  
  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode()){
    histo   = (TH1D*)  hListTmp->FindObject("hdxdt");
    histo1  = (TH1D*)  hListTmp->FindObject("hSignal");
    //histo1->SetTitle(Form("hSignal_%d", GetElChID() ));
    histo2  = (TH1D*)  hListTmp->FindObject("hDiffSignal");
    //histo2->SetTitle(Form("hDiffSignal_%d", GetElChID() )); 
    histo3 =  (TH1D*)  hListTmp->FindObject("hSignalShifted");
    //histo3->SetTitle(Form("hSignalShifted_%d", GetElChID() ));
    histo4 =  (TH1D*)  hListTmp->FindObject("hSignalShifted2");
    //histo4->SetTitle(Form("hSignalShifted2_%d", GetElChID() ));
    histo5 =  (TH1D*)  hListTmp->FindObject("hSignalShifted3");
    //histo5->SetTitle(Form("hSignalShifted3_%d", GetElChID() ));
  }else{
    histo   = (TH1D*)  hListTmp->FindObject("hdxdt");
    histo1  = (TH1D*)  hListTmp->FindObject("hSignal");
    //histo1->SetTitle(Form("hSignal_%d", GetElChID() ));
    histo2  = (TH1D*)  hListTmp->FindObject("hDiffSignal");
    //histo2->SetTitle(Form("hDiffSignal_%d", GetElChID() )); 
    histo3 =  (TH1D*)  hListTmp->FindObject("hSignalShifted");
    //histo3->SetTitle(Form("hSignalShifted_%d", GetElChID() ));
    histo4 =  (TH1D*)  hListTmp->FindObject("hSignalShifted2");
    //histo4->SetTitle(Form("hSignalShifted2_%d", GetElChID() ));
    histo5 =  (TH1D*)  hListTmp->FindObject("hSignalShifted3");
    //histo5->SetTitle(Form("hSignalShifted3_%d", GetElChID() ));
  }
  
  Int_t nsmooth=20;
  for(int i=0; i<1500; i++)Wave.push_back(0);
  // Smooth the signal by averaging nsmooth samples 
  //  for(ll=1;ll<1001;ll++){
  
  
  for(ll=nsmooth/2;ll<1001;ll++){
    if(ll>nsmooth/2){
      Temp[ll] =TMath::Mean(nsmooth,&fSamples[ll-nsmooth/2]); // averaging over ±nsmooth/2 samples 
      Temp1[ll]=(-1.*Temp[ll]+fAvg200)/4096*1000.;  // transform in postive mV using first Istart samples for pedestal
    }else{
      Temp[ll]=0;
      Temp1[ll]=0;
    }
    Wave.at(ll)=Temp1[ll];
    //Double_t pedestal=0.;
    //for(int i=0; i<250; i++){
    //  pedestal+=Wave.at(i);
    //}//accumulate(Wave.begin(), Wave.end(), 0.0)/200;
    //pedestal/=200;
    //if(fabs(pedestal)>1)std::cout<<"pedestal " << pedestal <<" channel " << GetElChID()<< std::endl;
   if(fGlobalMode->GetGlobalDebugMode()) histo1->SetBinContent(ll,Temp1[ll]);
    // compute raw derivative subracting samples
   nsmooth=5;//like before IO
    if(ll>iDer+nsmooth/2 && ll<900){ 
      dxdt[ll]=-(Temp[ll]-Temp[ll-iDer]);
      //dxdt1[ll]=-(Temp1[ll]-Temp1[ll-iDer]);
    }else{
      dxdt[ll]=0;
      dxdt[ll]=0;
    }
   if(fGlobalMode->GetGlobalDebugMode()) histo->SetBinContent(ll,dxdt[ll]);
  }
  
    // compute raw derivative subracting samples
  Double_t maxValDerivativeWaveform=0.;
  Int_t MaxBin=MakeDerivativeAndTakeMaxTime(iDer,nsmooth, Wave, maxValDerivativeWaveform);
   
   // std::cout<< "I have the fist time " << std::endl;
   /*
  Int_t MaxBin1 = 0;
  Double_t maxdxdt=0.;
  for(int ll=0;ll<1001;ll++){
    if(dxdt[ll]>maxdxdt){
      MaxBin1=ll;
      maxdxdt=dxdt[ll];
    }
  }
  std::cout<<" MaxBin  "<< MaxBin1 << std::endl;*/
  
  fTimeSin = (Double_t)MaxBin*fTimeBin; 
  //fAmplitude = histo1->GetBinContent(MaxBin);
  fAmplitude = Temp1[MaxBin];
  std::vector<Double_t> DiffVec;
  Bool_t OutRMS; 
  MakeDifferenceWaveformTeplate(Wave, MaxBin,TempWave, DiffVec, OutRMS);    
  Double_t chargeFirstHitFromTemplate= CalcChargeSin(250, TempWave);
  Double_t energyFirstHitFromTemplate= chargeFirstHitFromTemplate/15.;
  std::cout<<"energyFirstHitFromTemplate "<< energyFirstHitFromTemplate << std::endl;
  if(fGlobalMode->GetGlobalDebugMode()) {
      hChargeFirstHit->Fill(chargeFirstHitFromTemplate);
      hEnergyFirstHit->Fill(energyFirstHitFromTemplate);
    }
  //std::cout<<"In drawmean , outRMS " << OutRMS << std::endl;
  /* ////////// to save the template
  ofstream myfile;
  myfile.open ("template.txt");
  for(ll=0;ll<1001;ll++){
    if(ll>200){
      Double_t hContenth=isto1->GetBinContent(MaxBin-220+ll)/hIntegral; //max bin at 200
      Double_t hLastContent=fTemplate[ll];
      Double_t hWaveform=(hLastContent*fCountEvent+hContent)/(fCountEvent+1);
      fTemplate[ll]=hWaveform;
      histo2->SetBinContent(ll,hWaveform);
      myfile<<hWaveform<<std::endl;
      if(hWaveform>fTemplate[ll-1])saveMaxPos=ll; 
    }
   else fTemplate[ll]=0.;
  }
  myfile.close();*/
  
  /* 
  Int_t saveMaxPos=-1;
  Double_t maxValue=0.;
  Double_t maxValueWave=0.;
  for(int i=0; i<1001; i++){
    if(fTemplate[i]>maxValue){
      saveMaxPos=i;
      maxValue=fTemplate[i];
    }
    if(Temp1[i]>maxValueWave){
      maxValueWave=Temp1[i];
    }
  }
  
  //std::cout<< "MaxBin derivate " << MaxBin << " max template "<< maxValue << " max wave " << maxValueWave << std::endl;
  
  //Double_t NormFactor=fAmplitude/maxValue;
  Double_t NormFactor=maxValueWave/maxValue;
  std::cout << "NormFactor " << NormFactor << std::endl;
  for(int ll=0;ll<1001;ll++){
   if(ll-20<1001){
    TempWave[ll+MaxBin-20]=fTemplate[ll];
    //if(TempWave[ll]>3000)TempWave[ll]=0.;
    histo3->SetBinContent(ll, TempWave[ll]*NormFactor);
    //std::cout <<"I'm filling the shifted&Normalized template with: " <<  TempWave[ll] << std::endl;
    histo2->SetBinContent(ll,Temp1[ll]-TempWave[ll]*NormFactor);
    if(ll>400)hDiffWavetemplate->Fill(Temp1[ll]-TempWave[ll]*NormFactor);
   }
  }
  */
  //std::cout <<" I'm going to fill the histograms " << std::endl;
  if(fGlobalMode->GetGlobalDebugMode()){
    // std::cout<<"i'm in the if loop " << fGlobalMode->GetGlobalDebugMode() << std::endl;
    for(int ll=0;ll<1001;ll++){
      histo3->SetBinContent(ll, TempWave.at(ll));     
      histo2->SetBinContent(ll,Temp1[ll]-TempWave.at(ll));
      if(ll>400)hDiffWavetemplate->Fill(Temp1[ll]-TempWave.at(ll));
    }
  } 
  Double_t maxValDerivativeDiffForSecondHit=0.;
  Double_t TimeSecondHit=MakeDerivativeAndTakeMaxTime(iDer,nsmooth, DiffVec, maxValDerivativeDiffForSecondHit);
  //std::cout<<"finished to filling histos " << std::endl;
  if(maxValDerivativeDiffForSecondHit>30.){   //I'm cutting on the value of the waveform at the maximumderivative time of diffvec
    //if(OutRMS){
    //io comment 3/04 Double_t chargeSeconHit=CalcChargeSin(250, DiffVec);
    // io comment 2/04 .. if(EnergySecondHit>20.){
    //SecondEnergy=EnergySecondHit;
    std::cout<<"In Draw.....SeconhHitEnergy " << SecondEnergy << std::endl;
    //SecondTime=TimeSecondHit;
    SecondHit=true;
    std::vector<Double_t> DiffVec_SecondHit;
    Bool_t OutRMS_SecondHit;    
    TempWave.clear(); 
    MakeDifferenceWaveformTeplate(DiffVec, TimeSecondHit,TempWave, DiffVec_SecondHit, OutRMS_SecondHit);
    Double_t chargeSecondHitFromTemplate= CalcChargeSin(250, TempWave);
    Double_t energySecondHitFromTemplate= chargeSecondHitFromTemplate/15.; //going from pC to MeV using 15pC/MeV 
    SecondEnergy=energySecondHitFromTemplate;
    SecondTime=TimeSecondHit;
    std::cout<<"energySecondHitFromTemplate "<< energySecondHitFromTemplate << std::endl;
    if(fGlobalMode->GetGlobalDebugMode()) {
      for(int ll=0; ll<TempWave.size();ll++) histo4->SetBinContent(ll, TempWave.at(ll));
      hChargeSecondHit->Fill(chargeSecondHitFromTemplate);
      hEnergySecondHit->Fill(energySecondHitFromTemplate);
    }

    Double_t maxValDerivativeDiffForThirdHit=0.;
    Double_t TimeThirdHit=MakeDerivativeAndTakeMaxTime(iDer,nsmooth, DiffVec_SecondHit, maxValDerivativeDiffForThirdHit);
    //std::cout<<"finished to filling histos " << std::endl;
    if(maxValDerivativeDiffForThirdHit>30.){   //I'm cutting on the value of the waveform at the maximumderivative time of diffvec
      std::cout<<"In Draw.....ThirdHitEnergy " << SecondEnergy << std::endl;
      ThirdHit=true;
      std::vector<Double_t> DiffVec_ThirdHit;
      Bool_t OutRMS_ThirdHit;    
      TempWave.clear(); 
      MakeDifferenceWaveformTeplate(DiffVec_SecondHit, TimeThirdHit,TempWave, DiffVec_ThirdHit, OutRMS_ThirdHit);
      Double_t chargeThirdHitFromTemplate= CalcChargeSin(250, TempWave);
      Double_t energyThirdHitFromTemplate= chargeThirdHitFromTemplate/15.; //going from pC to MeV using 15pC/MeV 
      ThirdEnergy=energyThirdHitFromTemplate;
      ThirdTime=TimeThirdHit;
      std::cout<<"energyThirdHitFromTemplate "<< energyThirdHitFromTemplate << std::endl;
      if(fGlobalMode->GetGlobalDebugMode()) {
	for(int ll=0; ll<TempWave.size();ll++) histo5->SetBinContent(ll, TempWave.at(ll));
	hCharge3Hit->Fill(chargeThirdHitFromTemplate);
	hEnergy3Hit->Fill(energyThirdHitFromTemplate);
      }
    }
    /*  io comment 3/04
    if(OutRMS_SecondHit){
    Double_t charge3Hit=CalcChargeSin(250, DiffVec_SecondHit);
    if(fGlobalMode->GetGlobalDebugMode()) hCharge3Hit->Fill(charge3Hit);
    Double_t Energy3Hit= charge3Hit/15.; //going from pC to MeV using 15pC/MeV 
    if(fGlobalMode->GetGlobalDebugMode()) hEnergy3Hit->Fill(Energy3Hit);
    // io comment 2/04.... if(Energy3Hit>20.){
       if(fGlobalMode->GetGlobalDebugMode()){ for(int ll=0; ll<DiffVec_SecondHit.size();ll++)histo5->SetBinContent(ll,DiffVec_SecondHit.at(ll));}
       Double_t maxValDerivativeDiffForThirdHit=0.;
       Double_t Time3Hit=MakeDerivativeAndTakeMaxTime(iDer,nsmooth, DiffVec_SecondHit, maxValDerivativeDiffForThirdHit);
       ThirdEnergy=Energy3Hit;
       ThirdTime=Time3Hit;
       ThirdHit=true;
       // io comment 2/04 }
       }    end io comment 3/04*/
    /*TRecoVHit *Hit = new TRecoVHit();
    Hit->SetTime(TimeSecondHit);
    Hit->SetEnergy(EnergySecondHit);
    hitArray.push_back(Hit);*/
    //io comment 2/04 }
  }
  
  fCountEvent++;
  
  if(fGlobalMode->GetGlobalDebugMode() && fAmplitude>30. && ThirdHit){
    if(fSaveAnalog) hListTmp->Write();
  
  histo->Reset();
  histo1->Reset();
  histo2->Reset();
  histo3->Reset();
  histo4->Reset();
  histo5->Reset();
  ECal->Fill();
  }

}



void DigitizerChannelECal::MakeDifferenceWaveformTeplate(std::vector<Double_t> input, Int_t MaxBin,std::vector<Double_t>& TempWave,  std::vector<Double_t>& output, Bool_t& OutRMS){
  //std::cout<<"i'm in makedifference " << std::endl;
  OutRMS=false;
  Int_t saveMaxPos=-1;
  Double_t ThrDiff=7*1.95;//1.95 come from a fit on the difference between the waveform and the template distribution. 
  //Double_t ThrDiff=7*1.15;//2.18 is the RMSsigma of Diff distribution
  //Double_t ThrDiff=7*2.18;//2.18 is the RMS of Diff distribution in [-20,20]
  // Double_t TempWave[1501]={0.};
  Double_t maxValue=0.;
  Double_t maxValueWave=0.;
  for(int i=0; i<1001; i++){
    if(fTemplate[i]>maxValue){
      saveMaxPos=i;
      maxValue=fTemplate[i];
    }
    if(i> MaxBin-20 && i<MaxBin+20){
      if(input[i]>maxValueWave){
        maxValueWave=input[i];
      }
    }  
  }
  
  //std::cout<< "make difference....MaxBin derivate " << MaxBin << " max template "<< maxValue << " max wave " << maxValueWave << std::endl;
  
  // Double_t NormFactor=input[MaxBin]/maxValue;
   Double_t NormFactor=maxValueWave/maxValue;
  //std::cout << "NormFactor " << NormFactor << std::endl;
  Double_t DiffSignal=0.;
  for(int i=0; i<2000; i++){
    TempWave.push_back(0);
    output.push_back(0);
  }
  
  for(int ll=0;ll<1001;ll++){
   if(ll+MaxBin-20>= 0 && ll+MaxBin-20<1001){
     //std::cout << "ll " << ll << "  position tempwave " << ll+MaxBin-20 << std::endl;
     TempWave.at(ll+MaxBin-20)=fTemplate[ll]*NormFactor;
    //if(TempWave[ll]>3000)TempWave[ll]=0.;
    //histo3->SetBinContent(ll, TempWave[ll]*NormFactor);
     if(ll<input.size()){
       DiffSignal=input.at(ll)-TempWave.at(ll);
      //if(ll>400)hDiffWavetemplate->Fill(Temp1[ll]-TempWave[ll]*NormFactor);
     }
     else DiffSignal=0.;
     output.at(ll)=DiffSignal;
     //std::cout<<"diff in draw " << DiffSignal << std::endl;
    if(fabs(DiffSignal>ThrDiff))OutRMS=true;
   }
   }

 return;
}



Double_t DigitizerChannelECal::CalcChargeSin(UShort_t iStart, std::vector<Double_t> wave) {
  
  //  Short_t begin = iStart;  //to become iStart
  Short_t end   = 1000;

  Double_t Charge200=0;
  Double_t Charge=0;

  //  std::cout<<"Pedestal modes in charge "<<pedestalsFromFirstSamples()<<" "<< hybridPedestals()<<std::endl;
  if (pedestalsFromFirstSamples() || hybridPedestals() || fGlobalMode->GetGlobalDebugMode()!=0){
     for(Short_t s=0;s<end;s++){
      if(s>iStart && s<1000 && wave.at(s)>-100) {
	       Charge200 += 1*wave.at(s)*1e-3/fImpedance*fTimeBin*1e-9/1E-12; 
      }
    }
    
    if(pedestalsFromFirstSamples()) Charge = Charge200; 
  }
  fCharge=Charge;
  return fCharge;
  
}

Double_t DigitizerChannelECal::MakeDerivativeAndTakeMaxTime(Int_t iDer, Int_t nsmooth, std::vector<Double_t> wave, Double_t& maxValDerivative){
    
    Double_t dxdt[1001];
    Double_t dxdt1[1001];
    Double_t tempWaveDer[1001]={0.};
    Int_t ConversionFactor=4096/1000;
    for(int ll=0;ll<1001;ll++){
     if(ll< wave.size())tempWaveDer[ll]=-wave.at(ll)*ConversionFactor+fAvg200;
     else tempWaveDer[ll]=0.;
    }

  for(int ll=0;ll<1001;ll++){ 
     if(ll>iDer+nsmooth/2 && ll<900){ 
       dxdt1[ll]=(wave.at(ll)-wave.at(ll-iDer));
       dxdt[ll]=-(tempWaveDer[ll]-tempWaveDer[ll-iDer]);
      }else{
        dxdt[ll]=0;
    }
  // histo->SetBinContent(ll,dxdt[ll]);
  }

  
  
  Int_t MaxBin = 0;
  Double_t maxdxdt=0.;
  for(int ll=0;ll<1001;ll++){
    if(dxdt[ll]>maxdxdt){
      MaxBin=ll;
      maxdxdt=dxdt[ll];
    }
  }
  maxValDerivative=wave.at(MaxBin);
  return MaxBin;
}

Double_t DigitizerChannelECal::Fix2019BrokenChip(Int_t Flag){
  Int_t Board = Flag/100000;
  Int_t Chip  = (Flag%100000)/10000;
  Int_t FirstC = Flag-100000*Board-10000*Chip;
  Int_t NSamp=1024;

  //  std::vector<double> TempSamp;
  Double_t TempSamp[1024];

//  std::cout<<"*********Flag******************"<<std::endl; 
//  std::cout<<"Flag "<<Flag<<std::endl; 
//  std::cout<<"Board "<<Board<<" Chip "<<Chip<<" first Capacitor "<<FirstC<<std::endl; 
//  std::cout<<"*********END******************"<<std::endl; 
//  std::cout<<""<<std::endl; 
//
//  Int_t ii = vFixBoard8.size();
//  Int_t jj = vFixedBoard8.size();
//  TH1D* h;    
//  TH1D* h1; 
//  TH1D* hs; 
//  //If Debug mode with save analog is issued save debug histograms
//  if(fSaveAnalog &&  fGlobalMode->GetGlobalDebugMode()){
//    h  = new TH1D(Form("hFixAn%d",ii),Form("hFixAn%d",ii),NSamp,0.,NSamp);
//    h1 = new TH1D(Form("hFixedAn%d",ii),Form("hFixedAn%d",ii),NSamp,0.,NSamp);
//    hs = new TH1D(Form("hFixedfSam%d",ii),Form("hFixedfSam%d",ii),NSamp,0.,NSamp);
//    vFixBoard8.push_back(h);
//    vFixedBoard8.push_back(h1);
//    vFixedAnBoard8.push_back(hs);
//  }
//  
  for(Int_t kk=0;kk<1024;kk++){ 
    //     //    std::cout<<"First "<<FirstC<<" k "<<kk<<std::endl;
    //    //    std::cout<<"Sample "<<fSamples[kk]<<" k "<<kk<<std::endl;
    //    //    std::cout<<FirstC<<" Filling "<<(kk+FirstC)%1024<<" with sample "<<fSamples[kk]<<" k "<<kk<<std::endl;
    //    if(fSaveAnalog) vFixBoard8.at(ii)->SetBinContent((kk+FirstC)%1024,fSamples[kk]);
    if((kk+FirstC)%1024<512){ 
      TempSamp[(kk+FirstC)%1024]=fSamples[kk]+70.;
      //      if(fSaveAnalog)  vFixedBoard8.at(ii)->SetBinContent((kk+FirstC)%1024,fSamples[kk]+70.);
    }else{
      //      if(fSaveAnalog) vFixedBoard8.at(ii)->SetBinContent((kk+FirstC)%1024,fSamples[kk]);
      TempSamp[(kk+FirstC)%1024]=fSamples[kk];
    }
  }

  for(Int_t kk=0;kk<1024;kk++) {
    //    if(fSaveAnalog) vFixedAnBoard8.at(ii)->SetBinContent(kk,TempSamp[(kk+FirstC)%1024]); // put back the fSample Vector
    fSamples[kk]=TempSamp[(kk+FirstC)%1024];
  }
  //  //  for(Int_t kk=0;kk<1024;kk++) 
  //
  //  if(fSaveAnalog && fGlobalMode->GetGlobalDebugMode()){
  //    //    std::cout<<"Writing analog fix "<<vFixBoard8.size()-1<<std::endl;
  //    if(fSaveAnalog) vFixBoard8.at(vFixBoard8.size()-1)->Write();
  //    if(fSaveAnalog) vFixedBoard8.at(vFixedBoard8.size()-1)->Write();
  //    if(fSaveAnalog) vFixedAnBoard8.at(vFixedAnBoard8.size()-1)->Write();
  //    // delete histos to avoid Memory Leaks    
  //    h->Delete(); 
  //    h1->Delete(); 
  //    hs->Delete();   
  //  }
  //  delete[] TempSamp;
  return 1;                                                                  
}
