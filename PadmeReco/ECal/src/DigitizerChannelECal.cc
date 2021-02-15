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
  std::cout <<"**************************** DigitizerChannelECal config {*********************"<<std::endl;
  std::cout << "Hi I'm the ECal: " << std::endl;
  std::cout << "Signal width: "   << fSignalWidth << " samples" << std::endl;
  std::cout << "fUseAbsSignals: " << fUseAbsSignals << std::endl; 
  std::cout << "fPedestalMode:   " << fPedestalMode <<std::endl;
  std::cout << "fUseOverSample: " << fUseOverSample << std::endl;  
  std::cout << "fMultiHit: " << fMultihit << std::endl;
  std::cout <<"**************************** DigitizerChannelECal config }*********************"<<std::endl;

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

  //MH parameters
  fEnThrToDefineHit  = cfg->GetParOrDefault("RECO","EnThrToDefineTheExistenceOf2ndOr3dhHits",2.);
  fEnThrToDefineHit_sat  = cfg->GetParOrDefault("RECO","EnThrToDefineTheExistenceOf2ndOr3dhHits_saturatedWave",2.);
  fVThrToDefineHit  = cfg->GetParOrDefault("RECO","VoltageThrToDefineTheExistenceOf2ndOr3dhHits",5.);
  fVThrToDefineHit_sat  = cfg->GetParOrDefault("RECO","VoltageThrToDefineTheExistenceOf2ndOr3dhHits_saturatedWave",50.);
  fDTimeForHitInSameCh  = cfg->GetParOrDefault("RECO","DTimeForHitInSameCh",25.);


  std::cout << cfg->GetName() << "*******************************" <<  std::endl;

  if (!pedestalsFromFirstSamples()) SetAnalogOffSets();  //M. Raggi: 21/01/2019 read fixed anaolg values from files
  PrintConfig();
  PrepareTmpHistos();  //Temp histo servono anche in non debug mode
  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode()){
    PrepareDebugHistos(); //debugging mode histos
  }
  fCountSavedEvent=0;
  //take the template from the external file for MH reconstruction (the template is extended up to 5k ns using an exponential)
  for(int i=0; i<5000; i++) fTemplate[i]=0.;
  if (fMultihit) {
    ifstream myfile;
    std::string templateFileName = "./config/BGOwaveformTemplate.txt";
    std::cout<<"DigitizerChannelECal: reading BGO waveform template from file <"<<templateFileName<<"> "<<std::endl;
    myfile.open (templateFileName.c_str());;
    Int_t i=0;
    Double_t readValue=0.;
    if (myfile.is_open()){
      std::cout<<"DigitizerChannelECal: file <"<<templateFileName<<"> open"<<std::endl;
      while (!myfile.eof()) {
	myfile >> readValue;
	if (i<5000) {
	  fTemplate[i]=readValue;
	  if(fGlobalMode->GetGlobalDebugMode()) hTemplate->SetBinContent(i,fTemplate[i]);
	  //std::cout<< " Filling template ... "<<i<<" "<< fTemplate[i] << std::endl;
	  i++;
	}
      }
      std::cout<<"DigitizerChannelECal: "<<i<<" lines read from file "<<templateFileName<<std::endl;
    }
    else 
      {
	std::cout<<"DigitizerChannelECal - Template file requested <"<<templateFileName<<"> is not found ---- terminate here "<<std::endl;
	std::cerr<<"DigitizerChannelECal - Template file requested is not found ---- terminate here "<<std::endl;
	exit(1);
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
  hListCal->Add(hDiffWavetemplate_0_10 = new TH1F("hDiffWavetemplate_0_10", "hDiffWavetemplate_0_10", 500, -100,200));
  hListCal->Add(hDiffWavetemplate_10_30 = new TH1F("hDiffWavetemplate_10_30", "hDiffWavetemplate_10_30", 500, -100,200));
  hListCal->Add(hDiffWavetemplate_30_50 = new TH1F("hDiffWavetemplate_30_50", "hDiffWavetemplate_30_50", 500, -100,200));
  hListCal->Add(hDiffWavetemplate_50_100 = new TH1F("hDiffWavetemplate_50_100", "hDiffWavetemplate_50_100", 500, -100,200));
  hListCal->Add(hDiffWavetemplate_100_150 = new TH1F("hDiffWavetemplate_100_150", "hDiffWavetemplate_100_150", 500, -100,200));
  hListCal->Add(hDiffWavetemplate_150_200 = new TH1F("hDiffWavetemplate_150_200", "hDiffWavetemplate_150_200", 500, -100,200));
  hListCal->Add(hDiffWavetemplate_200_250 = new TH1F("hDiffWavetemplate_200_250", "hDiffWavetemplate_200_250", 500, -100,200));
  hListCal->Add(hDiffWavetemplate_250_300 = new TH1F("hDiffWavetemplate_250_300", "hDiffWavetemplate_250_300", 500, -100,200));

  hListCal->Add(hDiffFirstHitSeconHitTime = new TH1F("hDiffFirstHitSeconHitTime", "hDiffFirstHitSeconHitTime", 400,-200, 200));
  hListCal->Add(hChargeFirstHit = new TH1F("hChargeFirstHit", "hChargeFirstHit", 1500, -100, 1500));
  hListCal->Add(hEnergyFirstHit = new TH1F("hEnergyFirstHit", "hEnergyFirstHit", 1500, -100, 1500));
  hListCal->Add(hChargeSecondHit = new TH1F("hChargeSecondHit", "hChargeSecondHit", 1500, -100, 1500));
  hListCal->Add(hEnergySecondHit = new TH1F("hEnergySecondHit", "hEnergySecondHit", 1500, -100, 1500));
  hListCal->Add(hCharge3Hit = new TH1F("hCharge3Hit", "hCharge3Hit", 1500, -100, 1500));
  hListCal->Add(hEnergy3Hit = new TH1F("hEnergy3Hit", "hEnergy3Hit", 1500, -100, 1500));
  hListCal->Add(hECALsecondHitE      = new TH1F("ECAsecondLHitE","ECALsecondHitE",550,0,550));
  // hListCal->Add(hDiffTimeHitWaveform = new TH1F("DiffTimeHitWaveform","DiffTimeHitWaveform",550,-500,500));
  hListCal->Add(hDMaxDerivativeMaxWaveVSEnergy_First= new TH2F("hDMaxDerivativeMaxWaveVSEnergy_First","hDMaxDerivativeMaxWaveVSEnergy_First",600,0.,600.,400,-200.,200.)); 
  hListCal->Add(hDMaxDerivativeMaxWaveVSEnergy_Second= new TH2F("hDMaxDerivativeMaxWaveVSEnergy_Second","hDMaxDerivativeMaxWaveVSEnergy_Second",600,0.,600.,400,-200.,200.));
  hListCal->Add(hDMaxDerivativeMaxWaveVSEnergy_Third= new TH2F("hDMaxDerivativeMaxWaveVSEnergy_Third","hDMaxDerivativeMaxWaveVSEnergy_Third",600,0.,600.,400,-200.,200.));
  hListCal->Add(hDiffTimeFirstSecondHit = new TH1F("DiffTimeFirstSecondHit","DiffTimeFirstSecondHit",1000,-500,500));
  hListCal->Add(hDiffTimeFirstThirdHit = new TH1F("DiffTimeFirstThirdHit","DiffTimeFirstThirdHit",1000,-500,500));
  hListCal->Add(hDiffTimeSecondThirdHit = new TH1F("DiffTimeSecondThirdHit","DiffTimeSecondThirdHit",1000,-500,500));
  hListCal->Add(hECALfirsthitEnergy = new TH1F("hECALfirsthitEnergy_SH","hECALfirsthitEnergy_SH",3500,-100,3500));
  hListCal->Add(hECALfirsthitEnergyCorrected = new TH1F("hECALfirsthitEnergyCorrected","hECALfirsthitEnergyCorrected",3500,-10,3500));
  hListCal->Add(hECALsecondhitEnergyCorrected = new TH1F("hECALsecondhitEnergyCorrected","hECALsecondhitEnergyCorrected",3500,-10,3500));
  hListCal->Add(hECALthirdhitEnergyCorrected = new TH1F("hECALthirdhitEnergyCorrected","hECALthirdhitEnergyCorrected",3500,-10,3500));
  hListCal->Add(hAmplitudeVSEnergyTemplate= new TH2F("hAmplitudeVSEnergyTemplate","hAmplitudeVSEnergyTemplate",600,0.,600.,400,0.,500.));
  hListCal->Add(hECALsecondhitEnergy_Saved = new TH1F("hECALsecondhitEnergyCorrected_saved","hECALsecondhitEnergyCorrected_saved",1500,-10,1550));
  hListCal->Add(hECALthirdhitEnergy_Saved = new TH1F("hECALthirdhitEnergyCorrected_saved","hECALthirdhitEnergyCorrected_saved",1500,-10,1550));
  //histo ECAL template
  hListCal->Add(hTemplate= new TH1F("hTemplate","hTemplate",2000,0.,2000.));
  hListCal->Add(hCoeffAngularSaturation=new TH1F("hCoeffAngularSaturation","hCoeffAngularSaturation", 500, -6, 6));
  hListCal->Add(hConstant=new TH1F("hConstant","hConstant", 1000, -10000, 10000));
  hListCal->Add(hdiffSatMaxWave = new TH1F("hdiffSatMaxWave", "hdiffSatMaxWave", 202, -50.5, 50.5));
  hListCal->Add(hdiffSat1Wave = new TH1F("hdiffSat1Wave", "hdiffSat1Wave", 101, -50.5, 50.5));  
  hListCal->Add(hdiffSatContiguosWave = new TH1F("hdiffSatContiguosWave", "hdiffSatContiguosWave", 101, -50.5, 50.5)); 
  hListCal->Add( hdiffSat_firstLastFunction=new TH1F( "hdiffSat_firstLastFunction", "hdiffSat_firstLastFunction",100, -50, 50));
  // hListCal->Add(hsatDerivative = new TH1F ("hsatDerivative", "hsatDerivative", 200, -50,150));
  hListCal->Add(hNsat = new TH1F ("hNsat", "hNsat", 1001, -0.5,1000.5));
  hListCal->Add(hNsat_OnlyOneRecognise = new TH1F ("hNsat_OnlyOneRecognised", "hNsat_OnlyOneRecognise", 1001, -0.5,1000.5));
  hListCal->Add(hNsat_TwoRecognise = new TH1F ("hNsat_TwoRecognise", "hNsat_TwoRecognise", 1001, -0.5,1000.5));
  hListCal->Add(hTemplateVsMaxVSEnergy= new TH2F("hTemplateVsMaxVSEnergy","hTemplateVsMaxVSEnergy",2000,0.,2000.,2000,0.,2000.));
  hListCal->Add(hTemplateVMaxVSEnergy_= new TH2F("hTemplateVMaxVSEnergy_","hTemplateVMaxVSEnergy_",2000,0.,5000.,2000,0.,2000.));
  hListCal->Add(hDiffRMSVSEnergyTemplate= new TH2F("hDiffRMSVSEnergyTemplate","hDiffRMSVSEnergyTemplate",1000,0.,1000.,400,-200.,200.));
  hListCal->Add(hZSupFromSw= new TH1F("ZSupFromSw","ZSupFromSw",3,-0.5,2.5));


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
  //std::cout<<"fRMS1000 " << fRMS1000<<std::endl;
  Double_t ZSupHit=-1;
  
  if(fRMS1000>Thr){
    ZSupHit=0;
  }else{
    ZSupHit=1;
    //    std::cout<<"compute zero supp "<<rms1000<<" Zsup "<<ZSupHit<<std::endl;
  }
  //  std::cout<<"compute zero supp "<<fRMS1000<<" Thr "<<Thr<<" Zsup "<<ZSupHit<<std::endl;
  
  /*
  //Hardware implementation
  ZSupHit=-1;
   Double_t tot=0;
  Double_t tot2=0;
  Int_t nSamples=994;
  for(int i=0; i<nSamples; i++){
    tot+=fSamples[i];
    tot2+=fSamples[i]*fSamples[i];
  }
  Double_t Dev=sqrt((tot2-(tot*tot/nSamples))/(nSamples-1));
  std::cout<<"----------------------------------------------ZSupHit values , RMS: " << fRMS1000 << " stdDev " << Dev << " ";
  if (Dev<Thr)ZSupHit=1;
  else ZSupHit=0;
  std::cout << "ZSup flag " << ZSupHit << std::endl;
  */
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
    
    //fAvg200  = TMath::Mean(iStart,&fSamples[0]);
    fAvg200  = TMath::Mean(200,&fSamples[0]);   // io 23/06
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
  //  Double_t IsZeroSup = ZSupHit(fZeroSuppression,994.);   //io 8/06 to try
  Double_t IsZeroSup = ZSupHit(fZeroSuppression,1000.);   //io 8/06 to try
  //  Double_t IsZeroSup = ZSupHit(5,1000.);
  // if(fZeroSuppression>0) IsZeroSup = ZSupHit(fZeroSuppression,1000.);
  // IsSaturated(); //check if the event is saturated M. Raggi 03/2019
  if(fGlobalMode->GetGlobalDebugMode()) hZSupFromSw->Fill(IsZeroSup);
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
  if(GetTrigMask()!=2) CalcChargeSin(250); // io comment 22/06 (HitT-10);  //Physics in ECAL starts ~250 ns
  if(GetTrigMask()==2) CalcChargeSin(40);   //Cosmics in ECal start  ~40 ns
  if(IsSaturated()) IsSat=1; //check if the event is saturated M. Raggi 03/2019
  // M. Raggi going to energy with Nominal Calibration
  Double_t fEnergy= fCharge/15.; //going from pC to MeV using 15pC/MeV
  //std::cout <<"At the the digi levevl Hit charge:  " << fCharge << "  Time: " << fEnergy <<" HitE200 "<<HitE200 << "   isSat " << IsSat <<std::endl; 
  if (fEnergy < 1. && !fGlobalMode->IsPedestalMode()) return; //cut at 1 MeV nominal

  if(!fGlobalMode->IsPedestalMode()){
    //correct for saturation effects integrated charge M. Raggi 23/05/2019
    // do it before extrapolating to full integral
    //if(!IsSat)return;  //io 24/06
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
  //std::cout<<"In single hit recon fFirstHit: " << fFirstHit <<" isSat "<< IsSat<< std::endl;
  if(IsSat==1)fSaturatedHit=true;
  if(fGlobalMode->GetGlobalDebugMode()) {
    ECal->Fill();
    // hCharge->Fill(fCharge);
    // hAmplitude->Fill(fAmplitude);
    // hAmplitudeVSCharge->Fill(fAmplitude, fCharge);
  }
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

  Double_t IsZeroSup =Z SupHit(5.,1000.);
  IsSaturated(); //check if the event is saturated M. Raggi 03/2019
  //  if(IsSaturated()) return;  //remove is a test
  if(IsZeroSup==1 && !fGlobalMode->IsPedestalMode()) return; //perform zero suppression unless you are doing pedestals
  fTrig = GetTrigMask();

}*/

void DigitizerChannelECal::ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray){
  fFirstHit=false;
  fSaturatedHit=false;
  //std::cout<<"multi hit reconstruction ... " << std::endl;
 
 //to establish if there is saturated waveform or if the wave is ZSup
  ReconstructSingleHit(hitArray);
  if(!fFirstHit) return;
  //there is a single hit reconstructed and saved; if there is a second (third) hit I redefine this reconstructed hit 
  if (fGlobalMode->GetGlobalDebugMode())
    std::cout<< "-----------------------------------------------------------------------------ev " << fCountEvent << " saved: " << fCountSavedEvent << std::endl;
  Double_t energySecondHit=0.;
  Double_t energyFirstHit=0.;
  if(!fSaturatedHit){
    //std::cout<< "I'm in not saturated reconstruction " << std::endl;
    Bool_t SecondHit=false;
    Bool_t ThirdHit=false;
    Double_t FirstEnergy=0.;
    Double_t FirstTime=0.;
    Double_t SecondEnergy=0.;
    Double_t SecondTime=0.;
    Double_t ThirdEnergy=0.;
    Double_t ThirdTime=0.;
    Bool_t saveSecondHit=false;
    DrawMeanWave(10, FirstEnergy, FirstTime, SecondEnergy, SecondTime, ThirdEnergy, ThirdTime, SecondHit, ThirdHit);
    //std::cout<<"SecondHit Bool " << SecondHit << "First Time " << FirstTime << " second Time " << SecondTime << std::endl;
    //std::cout<< "firsthit "<< fFirstHit <<"second Energy " << SecondEnergy <<" bool " << SecondHit << " third energy " << ThirdEnergy << " bool " << ThirdHit << std::endl; 
    if(fGlobalMode->GetGlobalDebugMode()) {  
      if(SecondHit)hDiffTimeFirstSecondHit->Fill(FirstTime-SecondTime);
      if(ThirdHit)hDiffTimeFirstThirdHit->Fill(FirstTime-ThirdTime);
      if(SecondHit && ThirdHit)hDiffTimeSecondThirdHit->Fill(SecondTime-ThirdTime);
    }
    if(SecondHit && FirstEnergy>0.001 ){
      if(SecondEnergy>2){
        saveSecondHit=true;
        //std::cout<<"I'm saving the second hit " << std::endl;
        unsigned int nHitsBefore = hitArray.size()-1;
	if(fGlobalMode->GetGlobalDebugMode()) {  
	  hECALfirsthitEnergy->Fill(hitArray.at(nHitsBefore)->GetEnergy());
        } 
        hitArray.at(nHitsBefore)->SetEnergy(FirstEnergy);
        hitArray.at(nHitsBefore)->SetTime(FirstTime);
        TRecoVHit *Hit = new TRecoVHit();
        Hit->SetTime(SecondTime);
        Hit->SetEnergy(SecondEnergy);
        hitArray.push_back(Hit);
	if(fGlobalMode->GetGlobalDebugMode()) {  
	  hECALsecondhitEnergy_Saved->Fill(SecondEnergy);
        }
      }
    }
    if(ThirdHit && FirstEnergy>0.001){
      if(ThirdEnergy>2){
        //std::cout<<"I'm saving the third hit " << std::endl;
        if(!saveSecondHit){
          unsigned int nHitsBefore = hitArray.size()-1; 
           if(fGlobalMode->GetGlobalDebugMode()) {  
    	      hECALfirsthitEnergy->Fill(hitArray.at(nHitsBefore)->GetEnergy());
          }
          hitArray.at(nHitsBefore)->SetEnergy(FirstEnergy);
          hitArray.at(nHitsBefore)->SetTime(FirstTime);
         
        }
        TRecoVHit *Hit = new TRecoVHit();
        Hit->SetTime(ThirdTime);
        Hit->SetEnergy(ThirdEnergy);
        hitArray.push_back(Hit);
	if(fGlobalMode->GetGlobalDebugMode()) {  
	  hECALthirdhitEnergy_Saved->Fill(ThirdEnergy);
        }
      }
    }    
  } 
  
  else if(fSaturatedHit){
    Bool_t SecondHit=false;
    Bool_t ThirdHit=false;
    Double_t FirstEnergy=0.;
    Double_t FirstTime=0.;
    Double_t SecondEnergy=0.;
    Double_t SecondTime=0.;
    Double_t ThirdEnergy=0.;
    Double_t ThirdTime=0.;
    Bool_t saveSecondHit=false;
    DrawMeanSaturatedWave(10, FirstEnergy, FirstTime, SecondEnergy, SecondTime, ThirdEnergy, ThirdTime, SecondHit, ThirdHit);
    //std::cout<<"SecondHit Bool " << SecondHit << "First Time " << FirstTime << " second Time " << SecondTime << std::endl;
    if(fGlobalMode->GetGlobalDebugMode()) {  
      if(SecondHit)hDiffTimeFirstSecondHit->Fill(FirstTime-SecondTime);
    }
    if(SecondHit && FirstEnergy>0.001 ){
      if(SecondEnergy>2){
        saveSecondHit=true;
        //std::cout<<"I'm saving the second hit " << std::endl;
        unsigned int nHitsBefore = hitArray.size()-1;
         if(fGlobalMode->GetGlobalDebugMode()) {  
   	      hECALfirsthitEnergy->Fill(hitArray.at(nHitsBefore)->GetEnergy());
        } 
        hitArray.at(nHitsBefore)->SetEnergy(FirstEnergy);
        hitArray.at(nHitsBefore)->SetTime(FirstTime);
        TRecoVHit *Hit = new TRecoVHit();
        Hit->SetTime(SecondTime);
        Hit->SetEnergy(SecondEnergy);
        hitArray.push_back(Hit);
	if(fGlobalMode->GetGlobalDebugMode()) hECALsecondhitEnergy_Saved->Fill(SecondEnergy);
      }
    }
    if(ThirdHit && FirstEnergy>0.001){
      if(ThirdEnergy>2){
        //std::cout<<"I'm saving the third hit " << std::endl;
        if(!saveSecondHit){
          unsigned int nHitsBefore = hitArray.size()-1; 
	  if(fGlobalMode->GetGlobalDebugMode()) {  
	    hECALfirsthitEnergy->Fill(hitArray.at(nHitsBefore)->GetEnergy());
          }
          hitArray.at(nHitsBefore)->SetEnergy(FirstEnergy);
          hitArray.at(nHitsBefore)->SetTime(FirstTime);
         
        }
        TRecoVHit *Hit = new TRecoVHit();
        Hit->SetTime(ThirdTime);
        Hit->SetEnergy(ThirdEnergy);
        hitArray.push_back(Hit);
	if(fGlobalMode->GetGlobalDebugMode()) hECALthirdhitEnergy_Saved->Fill(ThirdEnergy);
      }
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
      fFirstSat=FirstSat;
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
  //std::cout<<"VLastSat " << VLastSat << std::endl;
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
void DigitizerChannelECal::DrawMeanWave(UShort_t iDer, Double_t& FirstEnergy, Double_t& FirstTime, Double_t& SecondEnergy,Double_t& SecondTime,Double_t& ThirdEnergy,Double_t& ThirdTime, Bool_t& SecondHit, Bool_t& ThirdHit) {
 
  //std::cout<<"DrawMeanWave....................................Event N. " << fCountEvent <<std::endl;

 Int_t ll;
 fAmplitude=0.;
 Double_t dxdt[1001];
 Double_t dxdt1[1001];
 Double_t Temp[1001];
 Double_t Temp1[1001];
  std::vector<Double_t> TempWave;
  std::vector<Double_t> Wave;
  Double_t chargeFirstHitFromTemplate=0.;
  Double_t energyFirstHitFromTemplate=0.;
  Double_t chargeSecondHitFromTemplate=0.;
  Double_t energySecondHitFromTemplate=0.;
  Double_t chargeThirdHitFromTemplate=0.;
  Double_t energyThirdHitFromTemplate=0.;
  Double_t enCorrection12=0.;
  Double_t enCorrection13=0.;
  Double_t enCorrection23=0.;
    
  
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
  Wave.clear();
  for(int i=0; i<1500; i++)Wave.push_back(0);
  // Smooth the signal by averaging nsmooth samples 
  //  for(ll=1;ll<1001;ll++){
  // Double_t myfAvg200=0.;
  
  for(ll=nsmooth/2;ll<1001;ll++){
    if(ll>nsmooth/2){
      Temp[ll] =TMath::Mean(nsmooth,&fSamples[ll-nsmooth/2]); // averaging over ±nsmooth/2 samples 
      Temp1[ll]=(-1.*Temp[ll]+fAvg200)/4096*1000.;  // transform in postive mV using first Istart samples for pedestal
    }else{
      Temp[ll]=0;
      Temp1[ll]=0;
    }
    Wave.at(ll)=Temp1[ll];
    //if(Wave.at(ll)<-10.)std::cout<< "In DrawMean !!! negative waveform " << Wave.at(ll) << " at time " << ll << " favg " << fAvg200 << " origin " << Temp[ll] << std::endl;
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
  //myfAvg200=TMath::Mean(200,&fSamples[0]);
  //std::cout << "myfavg200 " << myfAvg200<< std::endl;
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
  //FirstTime=fTimeSin;
  //fAmplitude = histo1->GetBinContent(MaxBin);
  // fAmplitude = Temp1[MaxBin];
  std::vector<Double_t> DiffVec;
  Bool_t OutRMS; 
  //MakeDifferenceWaveformTeplate functionalities: take the maximum of the waveform close (20ns) to the time of maximum derivative; Shift the template so that its maximum,VTemplateMax, falls in the bin where the waveform height is vmax
  MakeDifferenceWaveformTeplate(Wave, MaxBin,TempWave, DiffVec, OutRMS);    
  chargeFirstHitFromTemplate= CalcChargeSin(250, TempWave);
  energyFirstHitFromTemplate= chargeFirstHitFromTemplate/15.;
  if(fGlobalMode->GetGlobalDebugMode()) hAmplitudeVSEnergyTemplate->Fill(energyFirstHitFromTemplate, fAmplitude);
  if(fGlobalMode->GetGlobalDebugMode()){
    hDiffRMSVSEnergyTemplate->Fill(energyFirstHitFromTemplate,TMath::RMS(1000,&DiffVec[0]));
    if(energyFirstHitFromTemplate<10.){for(int jj=0; jj< DiffVec.size(); jj++)hDiffWavetemplate_0_10->Fill(DiffVec.at(jj));}
    if(energyFirstHitFromTemplate>10. && energyFirstHitFromTemplate<30. ){for(int jj=0; jj< DiffVec.size(); jj++)hDiffWavetemplate_10_30->Fill(DiffVec.at(jj));}
    if(energyFirstHitFromTemplate>30. && energyFirstHitFromTemplate<50. ){for(int jj=0; jj< DiffVec.size(); jj++)hDiffWavetemplate_30_50->Fill(DiffVec.at(jj));}
    if(energyFirstHitFromTemplate>50. && energyFirstHitFromTemplate<100. ){for(int jj=0; jj< DiffVec.size(); jj++)hDiffWavetemplate_50_100->Fill(DiffVec.at(jj));}
    if(energyFirstHitFromTemplate>100. && energyFirstHitFromTemplate<150. ){for(int jj=0; jj< DiffVec.size(); jj++)hDiffWavetemplate_100_150->Fill(DiffVec.at(jj));}
    if(energyFirstHitFromTemplate>150. && energyFirstHitFromTemplate<200. ){for(int jj=0; jj< DiffVec.size(); jj++)hDiffWavetemplate_150_200->Fill(DiffVec.at(jj));}
    if(energyFirstHitFromTemplate>200. && energyFirstHitFromTemplate<250. ){for(int jj=0; jj< DiffVec.size(); jj++)hDiffWavetemplate_200_250->Fill(DiffVec.at(jj));}
    if(energyFirstHitFromTemplate>250. && energyFirstHitFromTemplate<300. ){for(int jj=0; jj< DiffVec.size(); jj++)hDiffWavetemplate_250_300->Fill(DiffVec.at(jj));}

  }
  Int_t maxWave1= GetMaximumPosition(Wave);
  if(fGlobalMode->GetGlobalDebugMode()) hDMaxDerivativeMaxWaveVSEnergy_First->Fill(energyFirstHitFromTemplate, MaxBin-maxWave1);
  //Double_t TimeFirstHit=MaxBin*fTimeBin;
  FirstTime=fTimeSin;
  //std::cout<<"energyFirstHitFromTemplate "<< energyFirstHitFromTemplate << std::endl;
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
  Double_t SecondBinMax=MakeDerivativeAndTakeMaxTime(iDer,nsmooth, DiffVec, maxValDerivativeDiffForSecondHit);
  Double_t TimeSecondHit= SecondBinMax*fTimeBin;  
//std::cout<<"finished to filling histos " << std::endl;
  Double_t DiffFirstSecondHit= fTimeSin-TimeSecondHit;
  //if(DiffFirstSecondHit>0.1 && maxValDerivativeDiffForSecondHit>50 ){for(int kk=0; kk<1000 ; kk++)std::cout <<"In DrawMeanWave time "<< kk <<" diffVec : " <<DiffVec.at(kk)<< " wave: "<< Wave.at(kk) << " template "<< TempWave.at(kk)<< std::endl;}
  if(maxValDerivativeDiffForSecondHit>fVThrToDefineHit && fabs(DiffFirstSecondHit)>fDTimeForHitInSameCh){   //I'm cutting on the value of the waveform at the maximumderivative time of diffvec
    //if(OutRMS){
    //io comment 3/04 Double_t chargeSeconHit=CalcChargeSin(250, DiffVec);
    // io comment 2/04 .. if(EnergySecondHit>20.){
    //SecondEnergy=EnergySecondHit;
    //std::cout<<"In Draw.....SeconhHitEnergy " << SecondEnergy << std::endl;
    if(fGlobalMode->GetGlobalDebugMode()) hDiffFirstHitSeconHitTime->Fill(fTimeSin-TimeSecondHit);
    //SecondTime=TimeSecondHit;
    std::vector<Double_t> DiffVec_SecondHit;
    Bool_t OutRMS_SecondHit;    
    TempWave.clear(); 
    MakeDifferenceWaveformTeplate(DiffVec,SecondBinMax ,TempWave, DiffVec_SecondHit, OutRMS_SecondHit);
    chargeSecondHitFromTemplate= CalcChargeSin(250, TempWave);
    energySecondHitFromTemplate= chargeSecondHitFromTemplate/15.; //going from pC to MeV using 15pC/MeV 
    if(energySecondHitFromTemplate>fEnThrToDefineHit)SecondHit=true;
    //if(SecondHit)std::cout << "DrawMeanWave.....ftimesin " << fTimeSin << "Time First Hit " << fTimeSin << " second Time " << TimeSecondHit << std::endl;
    Int_t maxWave2= GetMaximumPosition(DiffVec);
    if(fGlobalMode->GetGlobalDebugMode()) hDMaxDerivativeMaxWaveVSEnergy_Second->Fill(energySecondHitFromTemplate, SecondBinMax-maxWave2);
    if(SecondHit && fTimeSin> TimeSecondHit){
      enCorrection12 =  CalcChargeSin(MaxBin, TempWave)/15.;
    }
    else enCorrection12=0.;
    
    SecondTime=TimeSecondHit;
    //<<"energySecondHitFromTemplate "<< energySecondHitFromTemplate << std::endl;
    if(fGlobalMode->GetGlobalDebugMode()) {
      for(int ll=0; ll<TempWave.size();ll++) histo4->SetBinContent(ll, TempWave.at(ll));
      hChargeSecondHit->Fill(chargeSecondHitFromTemplate);
      hEnergySecondHit->Fill(energySecondHitFromTemplate);
    }
       
    Double_t maxValDerivativeDiffForThirdHit=0.;
    Double_t ThirdBinMax=MakeDerivativeAndTakeMaxTime(iDer,nsmooth, DiffVec_SecondHit, maxValDerivativeDiffForThirdHit);
    Double_t TimeThirdHit=  ThirdBinMax*fTimeBin;  
    Double_t DiffFirstThirdHit= fTimeSin-TimeThirdHit;
    Double_t DiffSecondThirdHit= TimeSecondHit-TimeThirdHit;
  //std::cout<<"finished to filling histos " << std::endl;
    if(maxValDerivativeDiffForThirdHit>fVThrToDefineHit &&  fabs(DiffFirstThirdHit)>fDTimeForHitInSameCh &&  fabs(DiffSecondThirdHit)>fDTimeForHitInSameCh ){   //I'm cutting on the value of the waveform at the maximumderivative time of diffvec
      //std::cout<<"In Draw.....ThirdHitEnergy " << SecondEnergy << std::endl;
      std::vector<Double_t> DiffVec_ThirdHit;
      Bool_t OutRMS_ThirdHit;    
      TempWave.clear(); 
      MakeDifferenceWaveformTeplate(DiffVec_SecondHit, TimeThirdHit,TempWave, DiffVec_ThirdHit, OutRMS_ThirdHit);
      chargeThirdHitFromTemplate= CalcChargeSin(250, TempWave);
      energyThirdHitFromTemplate= chargeThirdHitFromTemplate/15.; //going from pC to MeV using 15pC/MeV
      if(energyThirdHitFromTemplate> fEnThrToDefineHit )  ThirdHit=true; 
      Int_t maxWave3= GetMaximumPosition(DiffVec_SecondHit);
      if(fGlobalMode->GetGlobalDebugMode()) hDMaxDerivativeMaxWaveVSEnergy_Third->Fill(energyThirdHitFromTemplate, ThirdBinMax-maxWave3);
      if(ThirdHit && fTimeSin> TimeThirdHit){
	enCorrection13 =  CalcChargeSin(MaxBin, TempWave)/15.;
      }
      else enCorrection13=0.;
      if( ThirdHit && TimeSecondHit> TimeThirdHit){
	enCorrection23 =  CalcChargeSin(SecondBinMax, TempWave)/15.;
      }
      else enCorrection23=0.;
      ThirdTime=TimeThirdHit;
      //std::cout<<"energyThirdHitFromTemplate "<< energyThirdHitFromTemplate << std::endl;
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
  FirstEnergy=energyFirstHitFromTemplate-enCorrection12-enCorrection13;
  SecondEnergy=energySecondHitFromTemplate-enCorrection23;
  ThirdEnergy=energyThirdHitFromTemplate;
  //  if(SecondHit){
  //    std::cout<<"DrawMeanWave...energy 1 template " << energyFirstHitFromTemplate << " correction12 " << enCorrection12 << " correction 13 " << enCorrection13 << " final energy " << FirstEnergy << std::endl;
  //    std::cout<< "DrawMeanWave...energy 2 template " << energySecondHitFromTemplate << " correction23 " << enCorrection23 << " final energy " << SecondEnergy << std::endl; 
    //if(ThirdHit) std::cout<<"DrawMeanWave...energy 3 template " << energyThirdHitFromTemplate << std::endl;
  //  }
  if(fGlobalMode->GetGlobalDebugMode()){
      if(SecondHit)hECALfirsthitEnergyCorrected->Fill(FirstEnergy);
      if(SecondHit) hECALsecondhitEnergyCorrected->Fill(SecondEnergy);
      if(ThirdHit) hECALthirdhitEnergyCorrected->Fill(ThirdEnergy); 
  }
 fCountEvent++;
 //if(fGlobalMode->GetGlobalDebugMode() && fAmplitude>30. && SecondHit/* && ThirdHit*/){
 /* if(fSaveAnalog) hListTmp->Write();
    fCountSavedEvent++;
    histo->Reset();
    histo1->Reset();
    histo2->Reset();
    histo3->Reset();
    histo4->Reset();
    histo5->Reset();
    ECal->Fill();
  }*/

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
   fAmplitude =maxValueWave;
  //std::cout << "NormFactor " << NormFactor << std::endl;
  Double_t DiffSignal=0.;
  for(int i=0; i<5000; i++){
    TempWave.push_back(0);
    output.push_back(0);
  }
 
  for(int ll=0;ll<5000;ll++){
   if(ll+MaxBin-20>= 0 && ll+MaxBin-20<5000){
     TempWave.at(ll+MaxBin-20)=fTemplate[ll]*NormFactor;
     if(ll<input.size()){
       DiffSignal=input.at(ll)-TempWave.at(ll);
     }
     else DiffSignal=0.;
     output.at(ll)=DiffSignal;
    if(fabs(DiffSignal>ThrDiff))OutRMS=true;
   }
   }
  if(fGlobalMode->GetGlobalDebugMode()){
    Double_t tempEnergy= CalcChargeSin(250, TempWave)/15.;
    hTemplateVMaxVSEnergy_->Fill(tempEnergy, maxValueWave);
  }
 return;
}



Double_t DigitizerChannelECal::CalcChargeSin(UShort_t iStart, std::vector<Double_t> wave) {
  
  //  Short_t begin = iStart;  //to become iStart
  Short_t end   = wave.size(); // 1000;

  Double_t Charge200=0;
  Double_t Charge=0;

  //  std::cout<<"Pedestal modes in charge "<<pedestalsFromFirstSamples()<<" "<< hybridPedestals()<<std::endl;
  if (pedestalsFromFirstSamples() || hybridPedestals() || fGlobalMode->GetGlobalDebugMode()!=0){
     for(Short_t s=0;s<end;s++){
       if(s>iStart/* && s<1000*/ && wave.at(s)>-100) {
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
    Int_t end=wave.size();
    if(end>900)end=900;
    for(int ll=0;ll<1001;ll++){
     if(ll< wave.size())tempWaveDer[ll]=-wave.at(ll)*ConversionFactor+fAvg200;
     else tempWaveDer[ll]=0.;
    }

  for(int ll=0;ll<1001;ll++){ 
     if(ll>iDer+nsmooth/2 && ll<end){ 
       dxdt1[ll]=(wave.at(ll)-wave.at(ll-iDer));
       dxdt[ll]=-(tempWaveDer[ll]-tempWaveDer[ll-iDer]);
      }else{
        dxdt[ll]=0;
    }
  // histo->SetBinContent(ll,dxdt[ll]);
  }

  
  
  Int_t MaxBin = 0;
  Double_t maxdxdt=0.;
  for(int ll=0;ll<end;ll++){
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

Int_t DigitizerChannelECal::GetMaximumPosition(std::vector<Double_t> wave){
  Int_t MaxBin = 0;
  Double_t maxvalue=0.;
  for(int ll=0;ll<wave.size();ll++){
    if(wave.at(ll)>maxvalue){
      MaxBin=ll;
      maxvalue=wave.at(ll);
    }
  }
  return MaxBin;
}






void DigitizerChannelECal::DrawMeanSaturatedWave(UShort_t iDer, Double_t& FirstEnergy, Double_t& FirstTime, Double_t& SecondEnergy,Double_t& SecondTime,Double_t& ThirdEnergy,Double_t& ThirdTime, Bool_t& SecondHit, Bool_t& ThirdHit) {
 
  //std::cout<<"DrawMeanSaturatedWave....................................Event N. " << fCountEvent <<std::endl;

 Int_t ll;
 fAmplitude=0.;
 Double_t dxdt[1001];
 Double_t dxdt1[1001];
 Double_t Temp[1001];
  Double_t Temp1[1001];
  std::vector<Double_t> TempWave;
  std::vector<Double_t> Wave;
  Double_t chargeFirstHitFromTemplate=0.;
  Double_t energyFirstHitFromTemplate=0.;
  Double_t chargeSecondHitFromTemplate=0.;
  Double_t energySecondHitFromTemplate=0.;
  Double_t chargeThirdHitFromTemplate=0.;
  Double_t energyThirdHitFromTemplate=0.;
  Double_t enCorrection12=0.;
  Double_t enCorrection13=0.;
  Double_t enCorrection23=0.;
    
  
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
  for(int i=0; i<5000; i++)Wave.push_back(0);
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
  Double_t maxValDerivativeWaveform2=0.;
  Double_t maxWave=0.;
  for(int i=0; i< Wave.size(); i++){
    if(Wave.at(i)>maxWave)maxWave=Wave.at(i);
  }
  fmyNSat=0;
  
  for(int i=0; i< Wave.size(); i++){
    if(fNSat>20){
      if(fabs(Wave.at(i)-maxWave)<15)fmyNSat++;
    }
    else { if(fabs(Wave.at(i)-maxWave)<10)fmyNSat++;}
    }

  Bool_t firstHitSaturated=false;
  Bool_t secondHitSaturated=false;
  Double_t positionFirstSat=0.;
  Double_t positionSecondSat=0.;
  Double_t satThr=10.;
  Int_t myNSatSecond=0;
  Int_t myNSat=0;
  FindDoubleHitSaturated(Wave,maxWave, firstHitSaturated, positionFirstSat, myNSat, secondHitSaturated, positionSecondSat,myNSatSecond);
  if(!secondHitSaturated) FindDoubleHitSaturatedAngularCoefficient(Wave,maxWave, firstHitSaturated, positionFirstSat, myNSat, secondHitSaturated, positionSecondSat,myNSatSecond);
  if(fGlobalMode->GetGlobalDebugMode()){
    hNsat->Fill(myNSat);
    if(!secondHitSaturated)hNsat_OnlyOneRecognise->Fill(myNSat);
    if(secondHitSaturated){
      hNsat_TwoRecognise->Fill(myNSatSecond);
      hNsat_TwoRecognise->Fill(myNSat);
      hNsat->Fill(myNSatSecond);
    }
  }
  fCountEvent++;
  //std::cout<<"DrawMeanSaturatedWave...Second saturated hit " << secondHitSaturated << std::endl;
  //if(positionSecondSat>0. || fabs(fmyNSat-myNSat)>1.)std::cout<<"First saturated at " << positionFirstSat << " dev. first: " << fFirstSat <<" for " << myNSat << " ( previous count " << fmyNSat <<  ")  second position " << positionSecondSat <<" for "<< myNSatSecond  << "---ev" << fCountEvent << std::endl;
  // std::cout<<"MaxWave "<< maxWave<< " myNSat " <<fmyNSat<< std::endl;
  //IO comment 1/05 Int_t MaxBin=MakeDerivativeAndTakeMaxTime(iDer,nsmooth, Wave, maxValDerivativeWaveform);
  if(!secondHitSaturated){
  Int_t MaxBin=fFirstSat;
  fTimeSin = (Double_t)MaxBin*fTimeBin; 
  std::vector<Double_t> DiffVec;
  Bool_t OutRMS; 

  Double_t MaxRelativeDerivative=0.;
  Double_t maxRelativeValue=0;
  //std::cout<<"------Max Bin " << MaxBin << std::endl;
  for(int i=(MaxBin-5); i< (MaxBin); i++){
    if(dxdt[i]>maxRelativeValue){maxRelativeValue=dxdt[i];MaxRelativeDerivative=i;}
  }
  //if(fmyNSat<20) std::cout<<"Timerelative " << MaxRelativeDerivative <<" previous time " << fFirstSat <<std::endl;
  //Double_t satTime=fCountsLastSat-fNSat;
  //std::cout<<"Sat Dtime " << fNSat << " countlastSat " << fCountsLastSat <<" fFistSat " << fFirstSat << " VMax " <<fVMax <<std::endl;
  if(fmyNSat<40){
    TempWave.clear();
    MakeDifferenceSaturatedWaveformTeplate(Wave,MaxRelativeDerivative ,TempWave, DiffVec, OutRMS,maxWave ); 
     MaxBin=MaxRelativeDerivative; 
  }  
  else{TempWave.clear(); MakeDifferenceSaturatedWaveformTeplate(Wave, fFirstSat ,TempWave, DiffVec, OutRMS,maxWave ); }
  // MaxBin= MakeDerivativeAndTakeMaxTime(iDer,nsmooth, TempWave, maxValDerivativeWaveform);   // io comment 12/05
  //MaxBin=fFirstSat;  //io comment 21/05
  chargeFirstHitFromTemplate= CalcChargeSin(250, TempWave);
  energyFirstHitFromTemplate= chargeFirstHitFromTemplate/15.;
  if(fGlobalMode->GetGlobalDebugMode()) hAmplitudeVSEnergyTemplate->Fill(energyFirstHitFromTemplate, fAmplitude);
  Int_t maxWave1= GetMaximumPosition(Wave);
  if(fGlobalMode->GetGlobalDebugMode()) hDMaxDerivativeMaxWaveVSEnergy_First->Fill(energyFirstHitFromTemplate, MaxBin-maxWave1);
  Int_t maxTemp= GetMaximumPosition(TempWave);
  if(fGlobalMode->GetGlobalDebugMode())hTemplateVsMaxVSEnergy->Fill(energyFirstHitFromTemplate, maxTemp);
  Double_t TimeFirstHit=MaxBin*fTimeBin;
  FirstTime=TimeFirstHit;
  //std::cout<<"energyFirstHitFromTemplate "<< energyFirstHitFromTemplate << std::endl;
  if(fGlobalMode->GetGlobalDebugMode()) {
      hChargeFirstHit->Fill(chargeFirstHitFromTemplate);
      hEnergyFirstHit->Fill(energyFirstHitFromTemplate);
    }
  /*
  for(int i=0; i<DiffVec.size(); i++){
    //if(i>fFirstSat-20 && i<fmyNSat+20) DiffVec.at(i)=0;
    std::cout<<"diffVec at " << i << " is " << DiffVec.at(i) << "....time first sat " << fFirstSat << " and nsat " << fmyNSat << std::endl;
    }*/
  if(fGlobalMode->GetGlobalDebugMode()){
    // std::cout<<"i'm in the if loop " << fGlobalMode->GetGlobalDebugMode() << std::endl;
    for(int ll=0;ll<1001;ll++){
      histo3->SetBinContent(ll, TempWave.at(ll));     
      histo2->SetBinContent(ll,/*Temp1[ll]-TempWave.at(ll)*/DiffVec.at(ll));
      if(ll>400)hDiffWavetemplate->Fill(Temp1[ll]-TempWave.at(ll));
    }
  } 
  Double_t maxValDerivativeDiffForSecondHit=0.;
  Double_t SecondBinMax=MakeDerivativeAndTakeMaxTime(iDer,nsmooth, DiffVec, maxValDerivativeDiffForSecondHit);
  Double_t TimeSecondHit= SecondBinMax*fTimeBin;  
  //std::cout <<"DrawMeanSaturatedWave... DiffVec at maxDerivative2 " << maxValDerivativeDiffForSecondHit << " from vec " << DiffVec.at(SecondBinMax) << " second bin max" << SecondBinMax << std::endl;
  Double_t DiffFirstSecondHit= TimeFirstHit-TimeSecondHit;
  //std::cout << "maxValDerivativeDiffForSecondHit " << maxValDerivativeDiffForSecondHit << " diff time 12 " << DiffFirstSecondHit << " first time " << TimeFirstHit << " secondTime " << TimeSecondHit <<std::endl;
  if(maxValDerivativeDiffForSecondHit>fVThrToDefineHit_sat && fabs(DiffFirstSecondHit)>fDTimeForHitInSameCh){   //I'm cutting on the value of the waveform at the maximumderivative time of diffvec
    //std::cout<<"DrawMeanSaturatedWave...second hit passed "<<std::endl;
    if(fGlobalMode->GetGlobalDebugMode()) hDiffFirstHitSeconHitTime->Fill(TimeFirstHit-TimeSecondHit);
    std::vector<Double_t> DiffVec_SecondHit;
    Bool_t OutRMS_SecondHit;    
    TempWave.clear(); 
    if(DiffFirstSecondHit>0. ) MakeDifferenceWaveformTeplate(Wave/*DiffVec*/,SecondBinMax ,TempWave, DiffVec_SecondHit, OutRMS_SecondHit);
    else  MakeDifferenceWaveformTeplate(DiffVec,SecondBinMax ,TempWave, DiffVec_SecondHit, OutRMS_SecondHit);
    Double_t maxBin2= MakeDerivativeAndTakeMaxTime(iDer,nsmooth, TempWave, maxValDerivativeDiffForSecondHit);
    //  TimeSecondHit=maxBin2;    //io comment 12/05
    chargeSecondHitFromTemplate= CalcChargeSin(250, TempWave);
    energySecondHitFromTemplate= chargeSecondHitFromTemplate/15.; //going from pC to MeV using 15pC/MeV 
    if(energySecondHitFromTemplate>fEnThrToDefineHit)SecondHit=true;
    //if(SecondHit)std::cout << "DrawMeanSaturatedWave..ftimesin " << fTimeSin << "Time First Hit " << TimeFirstHit << " second Time " << TimeSecondHit << std::endl;
    Int_t maxWave2= GetMaximumPosition(DiffVec);
    if(fGlobalMode->GetGlobalDebugMode()) hDMaxDerivativeMaxWaveVSEnergy_Second->Fill(energySecondHitFromTemplate, SecondBinMax-maxWave2);
    if(SecondHit && TimeFirstHit> TimeSecondHit){
      enCorrection12 =  CalcChargeSin(MaxBin, TempWave)/15.;
    }
    else enCorrection12=0.;
    
    SecondTime=TimeSecondHit;
    //<<"energySecondHitFromTemplate "<< energySecondHitFromTemplate << std::endl;
    if(fGlobalMode->GetGlobalDebugMode()) {
      for(int ll=0; ll<TempWave.size();ll++) histo4->SetBinContent(ll, TempWave.at(ll));
      hChargeSecondHit->Fill(chargeSecondHitFromTemplate);
      hEnergySecondHit->Fill(energySecondHitFromTemplate);
    }
    
    for(int i=0; i< DiffVec_SecondHit.size(); i++){
      if(i>SecondBinMax-25 ) DiffVec_SecondHit.at(i)=0.;
      if(i>fFirstSat-25) DiffVec_SecondHit.at(i)=0.;
    } 
    Double_t maxValDerivativeDiffForThirdHit=0.;
    Double_t ThirdBinMax=MakeDerivativeAndTakeMaxTime(iDer,nsmooth, DiffVec_SecondHit, maxValDerivativeDiffForThirdHit);
    Double_t TimeThirdHit=  ThirdBinMax*fTimeBin;  
    Double_t DiffFirstThirdHit= TimeFirstHit-TimeThirdHit;
    Double_t DiffSecondThirdHit= TimeSecondHit-TimeThirdHit;
    //std::cout << "maxValDerivativeDiffForThirdHit " << maxValDerivativeDiffForThirdHit << " diff time 13 " << DiffFirstThirdHit << " -23 " << DiffSecondThirdHit << " secondTime " << TimeSecondHit << " third time " << TimeThirdHit <<std::endl;
    if(maxValDerivativeDiffForThirdHit> fVThrToDefineHit_sat &&  fabs(DiffFirstThirdHit)>fDTimeForHitInSameCh &&  fabs(DiffSecondThirdHit)>fDTimeForHitInSameCh ){   //I'm cutting on the value of the waveform at the maximumderivative time of diffvec
      //std::cout<<"In Draw.....ThirdHitEnergy " << SecondEnergy << std::endl;
      //std::cout<<"DrawMeanSaturatedWave...third hit passed " << std::endl;
      std::vector<Double_t> DiffVec_ThirdHit;
      Bool_t OutRMS_ThirdHit;    
      TempWave.clear(); 
      MakeDifferenceWaveformTeplate(DiffVec_SecondHit, TimeThirdHit,TempWave, DiffVec_ThirdHit, OutRMS_ThirdHit);
      chargeThirdHitFromTemplate= CalcChargeSin(250, TempWave);
      energyThirdHitFromTemplate= chargeThirdHitFromTemplate/15.; //going from pC to MeV using 15pC/MeV
      if(energyThirdHitFromTemplate> fEnThrToDefineHit )  ThirdHit=true; 
      Int_t maxWave3= GetMaximumPosition(DiffVec_SecondHit);
      if(fGlobalMode->GetGlobalDebugMode()) hDMaxDerivativeMaxWaveVSEnergy_Third->Fill(energyThirdHitFromTemplate, ThirdBinMax-maxWave3);
      if(ThirdHit && TimeFirstHit> TimeThirdHit){
	enCorrection13 =  CalcChargeSin(MaxBin, TempWave)/15.;
      }
      else enCorrection13=0.;
      if( ThirdHit && TimeSecondHit> TimeThirdHit){
	enCorrection23 =  CalcChargeSin(SecondBinMax, TempWave)/15.;
      }
      else enCorrection23=0.;
      ThirdTime=TimeThirdHit;
      //std::cout<<"energyThirdHitFromTemplate "<< energyThirdHitFromTemplate << std::endl;
      if(fGlobalMode->GetGlobalDebugMode()) {
	for(int ll=0; ll<TempWave.size();ll++) histo5->SetBinContent(ll, TempWave.at(ll));
	hCharge3Hit->Fill(chargeThirdHitFromTemplate);
	hEnergy3Hit->Fill(energyThirdHitFromTemplate);
      }
    }
  }  
  FirstEnergy=energyFirstHitFromTemplate-enCorrection12-enCorrection13;
  SecondEnergy=energySecondHitFromTemplate-enCorrection23;
  ThirdEnergy=energyThirdHitFromTemplate;
  /*if(SecondHit){
  std::cout<<"energy 1 template " << energyFirstHitFromTemplate << " correction12 " << enCorrection12 << " final energy " << FirstEnergy << std::endl;
  std::cout<< "energy 2 template " << energySecondHitFromTemplate << " final energy " << SecondEnergy << std::endl; 
  }*/
  }
  else{
  Int_t MaxBin=fFirstSat;
  fTimeSin = (Double_t)MaxBin*fTimeBin; 
  std::vector<Double_t> DiffVec;
  std::vector<Double_t> TempWave2;
  Bool_t OutRMS; 

  Double_t MaxRelativeDerivative=0.;
  Double_t maxRelativeValue=0;
  for(int i=(positionFirstSat-25); i< (positionFirstSat); i++){
    if(dxdt[i]>maxRelativeValue){maxRelativeValue=dxdt[i];MaxRelativeDerivative=i;}
  }
  MaxBin=positionFirstSat;
  //std::cout<<"DrawMean2SaturatedWave..fFirstSat " << fFirstSat << " my position " << positionFirstSat <<" maxRelative "<< MaxRelativeDerivative<<" myNSat " << myNSat << std::endl;
  // for(int jj=fFirstSat-40; jj<fFirstSat+25; jj++)std::cout << "Wave at " << jj << " value: " << Wave.at(jj) << std::endl;
  Double_t MaxRelativeDerivative2=positionSecondSat;
  Double_t maxRelativeValue2=0;
  for(int i=(positionSecondSat-25); i< (positionSecondSat); i++){
    if(dxdt[i]>maxRelativeValue2){maxRelativeValue2=dxdt[i];MaxRelativeDerivative2=i;}
  }
  // std::cout<<"maxrelative 1 " << MaxRelativeDerivative << " max relative2 " << MaxRelativeDerivative2 <<std::endl;
  if(myNSat<40){
    MakeDifferenceSaturatedWaveformTeplate(Wave,myNSat, MaxRelativeDerivative ,TempWave, DiffVec, OutRMS,maxWave ); 
    MaxBin=MaxRelativeDerivative;
    //std::cout<<"DrawMean2SaturatedWave..I'm passing the max time: " << MaxRelativeDerivative << std::endl;
  } 
  else {MakeDifferenceSaturatedWaveformTeplate(Wave,myNSat, fFirstSat ,TempWave, DiffVec, OutRMS,maxWave );
    MaxBin=fFirstSat;   
  }
  //std::cout<<"Afetr filled diffvec size" << DiffVec.size()<< " and value position max " << DiffVec.at(MaxBin) << " at second time " << DiffVec.at(375)<< " ---ev " <<fCountEvent << std::endl; 
  //for(int i=200; i<1000; i++) std::cout<<"Pos " <<i <<" Waveform "<< Wave.at(i) << " temp1 " <<Temp1[i] << " template " <<TempWave.at(i) << " DiffVec " << " value " << DiffVec.at(i) << std::endl;

  Double_t MaxBinDerivative= MakeDerivativeAndTakeMaxTime(iDer,nsmooth, TempWave, maxValDerivativeWaveform);
  chargeFirstHitFromTemplate= CalcChargeSin(250, TempWave);
  energyFirstHitFromTemplate= chargeFirstHitFromTemplate/15.;
  if(fGlobalMode->GetGlobalDebugMode()) hAmplitudeVSEnergyTemplate->Fill(energyFirstHitFromTemplate, fAmplitude);
  Int_t maxWave1= GetMaximumPosition(Wave);
  if(fGlobalMode->GetGlobalDebugMode()) hDMaxDerivativeMaxWaveVSEnergy_First->Fill(energyFirstHitFromTemplate, MaxBinDerivative-maxWave1);
  Int_t maxTemp= GetMaximumPosition(TempWave);
  if(fGlobalMode->GetGlobalDebugMode())hTemplateVsMaxVSEnergy->Fill(energyFirstHitFromTemplate, maxTemp);
  Double_t TimeFirstHit=MaxBin*fTimeBin;
  FirstTime=TimeFirstHit;
  //std::cout<<"energyFirstHitFromTemplate "<< energyFirstHitFromTemplate << std::endl;
  if(fGlobalMode->GetGlobalDebugMode()) {
    hChargeFirstHit->Fill(chargeFirstHitFromTemplate);
    hEnergyFirstHit->Fill(energyFirstHitFromTemplate);
  }
 
  if(fGlobalMode->GetGlobalDebugMode()){
    // std::cout<<"i'm in the if loop " << fGlobalMode->GetGlobalDebugMode() << std::endl;
    for(int ll=0;ll<1001;ll++){
      histo3->SetBinContent(ll, TempWave.at(ll));     
      histo2->SetBinContent(ll,Temp1[ll]-TempWave.at(ll));
      if(ll>400)hDiffWavetemplate->Fill(Temp1[ll]-TempWave.at(ll));
    }
  }
   
  if(myNSatSecond<40){ MakeDifferenceSaturatedWaveformTeplate(Wave,myNSatSecond,MaxRelativeDerivative2 ,TempWave2, DiffVec, OutRMS,maxWave ); positionSecondSat=MaxRelativeDerivative2;}
  else MakeDifferenceSaturatedWaveformTeplate(Wave,myNSatSecond, positionSecondSat ,TempWave2, DiffVec, OutRMS,maxWave );
  Double_t TimeSecondHit= positionSecondSat*fTimeBin; 
  // Double_t TimeSecondHit= MakeDerivativeAndTakeMaxTime(iDer,nsmooth, TempWave2, maxValDerivativeWaveform2);   //io comment 12/05
  Double_t DiffFirstSecondHit= TimeFirstHit-TimeSecondHit; 
 chargeSecondHitFromTemplate= CalcChargeSin(250, TempWave2);
    energySecondHitFromTemplate= chargeSecondHitFromTemplate/15.; //going from pC to MeV using 15pC/MeV 
    if(energySecondHitFromTemplate>fEnThrToDefineHit)SecondHit=true;
    //if(SecondHit)std::cout << "DrawMean2SaturatedWave...ftimesin " << fTimeSin << "Time First Hit " << TimeFirstHit << " second Time " << TimeSecondHit << std::endl;
    enCorrection12 =  CalcChargeSin(positionSecondSat, TempWave)/15.;
    SecondTime=TimeSecondHit;
    //<<"energySecondHitFromTemplate "<< energySecondHitFromTemplate << std::endl;
    if(fGlobalMode->GetGlobalDebugMode()) {
      for(int ll=0; ll<TempWave.size();ll++) histo4->SetBinContent(ll, TempWave2.at(ll));
      hChargeSecondHit->Fill(chargeSecondHitFromTemplate);
      hEnergySecondHit->Fill(energySecondHitFromTemplate);
    }

    std::vector<Double_t> waveNoSaturated;
    for(int i=0; i<fFirstSat-25; i++){
      waveNoSaturated.push_back(Wave.at(i));
    }
    Double_t maxValDerivativeDiffForThirdHit=0.;
    Double_t ThirdBinMax=MakeDerivativeAndTakeMaxTime(iDer,nsmooth, waveNoSaturated, maxValDerivativeDiffForThirdHit);
    Double_t TimeThirdHit=  ThirdBinMax*fTimeBin;  
    Double_t DiffFirstThirdHit= TimeFirstHit-TimeThirdHit;
    Double_t DiffSecondThirdHit= TimeSecondHit-TimeThirdHit;
    //std::cout<<"third bin value " << maxValDerivativeDiffForThirdHit <<" waveNoSat at third time " << waveNoSaturated.at(TimeThirdHit) <<" wave at third time " << Wave.at(TimeThirdHit) << " diff13 time " << DiffFirstThirdHit << " diff23 time " << DiffSecondThirdHit << std::endl;
    if(maxValDerivativeDiffForThirdHit> fVThrToDefineHit_sat &&  fabs(DiffFirstThirdHit)>fDTimeForHitInSameCh &&  fabs(DiffSecondThirdHit)>fDTimeForHitInSameCh ){   //I'm cutting on the value of the waveform at the maximumderivative time of diffvec
      //std::cout<<"DrawMeanSaturatedWave...third hit passed " << std::endl;
      std::vector<Double_t> DiffVec_ThirdHit;
      Bool_t OutRMS_ThirdHit;    
      TempWave.clear(); 
      MakeDifferenceWaveformTeplate(/*waveNoSaturated*/Wave, TimeThirdHit,TempWave, DiffVec_ThirdHit, OutRMS_ThirdHit);
      chargeThirdHitFromTemplate= CalcChargeSin(250, TempWave);
      energyThirdHitFromTemplate= chargeThirdHitFromTemplate/15.; //going from pC to MeV using 15pC/MeV
      if(energyThirdHitFromTemplate> fEnThrToDefineHit )  ThirdHit=true; 
      Int_t maxWave3= GetMaximumPosition(waveNoSaturated);
      if(fGlobalMode->GetGlobalDebugMode()) hDMaxDerivativeMaxWaveVSEnergy_Third->Fill(energyThirdHitFromTemplate, ThirdBinMax-maxWave3);
      if(ThirdHit && TimeFirstHit> TimeThirdHit){
	enCorrection13 =  CalcChargeSin(TimeFirstHit, TempWave)/15.;
      }
      else enCorrection13=0.;
      if( ThirdHit && TimeSecondHit> TimeThirdHit){
	enCorrection23 =  CalcChargeSin(positionSecondSat, TempWave)/15.;
      }
      else enCorrection23=0.;
      ThirdTime=TimeThirdHit;
      //std::cout<<"energyThirdHitFromTemplate "<< energyThirdHitFromTemplate << std::endl;
      if(fGlobalMode->GetGlobalDebugMode()) {
	for(int ll=0; ll<TempWave.size();ll++) histo5->SetBinContent(ll, TempWave.at(ll));
	hCharge3Hit->Fill(chargeThirdHitFromTemplate);
	hEnergy3Hit->Fill(energyThirdHitFromTemplate);
      }
    }
   
  FirstEnergy=energyFirstHitFromTemplate-enCorrection12-enCorrection13;
  SecondEnergy=energySecondHitFromTemplate-enCorrection23;
  ThirdEnergy=energyThirdHitFromTemplate;
  }


 
  if(fGlobalMode->GetGlobalDebugMode()){
      if(SecondHit)hECALfirsthitEnergyCorrected->Fill(FirstEnergy);
      if(SecondHit) hECALsecondhitEnergyCorrected->Fill(SecondEnergy); 
  }

  if(fGlobalMode->GetGlobalDebugMode() /*&& fAmplitude>30. && SecondHit  && ThirdHit*/){
    if(fSaveAnalog) hListTmp->Write();
    fCountSavedEvent++;
    histo->Reset();
    histo1->Reset();
    histo2->Reset();
    histo3->Reset();
    histo4->Reset();
    histo5->Reset();
    ECal->Fill();
  }

}


void DigitizerChannelECal::MakeDifferenceSaturatedWaveformTeplate(std::vector<Double_t> input, Int_t MaxBin,std::vector<Double_t>& TempWave,std::vector<Double_t>& output, Bool_t& OutRMS, Double_t maxWave){
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
   //io comment 29/04    Double_t NormFactor=maxValueWave/maxValue;
  Double_t p0=0.;
  Double_t p1=0.;
  Double_t p2=0.;
 if(maxWave<=875&& maxWave>825){
    p0=865.19;
    p1=3.17536;
    p2=0.0145792;
  }
  else if(maxWave<=825 && maxWave>780){
    p0 =      837.721;
    p1 =      2.63729; 
    p2 =    0.0145538;
  }
  else if(maxWave<=780){
    p0 =      815.644;
    p1 =     2.04881 ; 
    p2 =   0.0148075 ;
  }
  else{   
    p0=904.438;
    p1=3.63858;
    p2=0.0140253;                  
   
  }
  Double_t NormFactor=0.; 
  /*if(fmyNSat>40)*/ NormFactor=(p0+p1*fmyNSat+p2*fmyNSat*fmyNSat)/maxValue;
  //else NormFactor=(p0+p1*fNSat+p2*fNSat*fNSat)/maxValue; // fVMax/maxValue;
  //std::cout<< "MyMax " << NormFactor << std::endl;
   //io comment 29/04    fAmplitude =maxValueWave;
  //std::cout << "NormFactor " << NormFactor << std::endl;
  Double_t DiffSignal=0.;
  for(int i=0; i<5000; i++){
    TempWave.push_back(0);
    output.push_back(0);
  }
  
  Double_t deltaBin=20;
  if(fmyNSat>200)deltaBin=8;
  // if(fmyNSat<20)deltaBin=0;
  for(int ll=0;ll<5000;ll++){
   if(ll+MaxBin-deltaBin>= 0 && ll+MaxBin-deltaBin<5000){
     //std::cout << "ll " << ll << "  position tempwave " << ll+MaxBin-20 << std::endl;
     TempWave.at(ll+MaxBin-deltaBin)=fTemplate[ll]*NormFactor;
    //if(TempWave[ll]>3000)TempWave[ll]=0.;
    //histo3->SetBinContent(ll, TempWave[ll]*NormFactor);
     if(ll<input.size()){
       DiffSignal=input.at(ll)-TempWave.at(ll);
      //if(ll>400)hDiffWavetemplate->Fill(Temp1[ll]-TempWave[ll]*NormFactor);
     }
     else DiffSignal=0.;
     if(ll>MaxBin-30 && ll<MaxBin+fmyNSat){
       DiffSignal=0.;
       // std::cout<<"in draw  " << ll <<" value " << output.at(ll) << " at first sat "<< MaxBin << " for nsat " <<  fmyNSat << std::endl;
     }
     output.at(ll)=DiffSignal;
     //std::cout<<"diff in draw " << DiffSignal << std::endl;
    if(fabs(DiffSignal>ThrDiff))OutRMS=true;
   }
   }
  if(fGlobalMode->GetGlobalDebugMode()){
    Double_t tempEnergy= CalcChargeSin(250, TempWave)/15.;
    hTemplateVMaxVSEnergy_->Fill(tempEnergy, NormFactor*maxValue);
  }
 return;
}

void DigitizerChannelECal::FindDoubleHitSaturated(std::vector<Double_t> Wave,Double_t maxWave, Bool_t& firstHitSaturated,Double_t& positionFirstSat,Int_t& myNSat,Bool_t& secondHitSaturated, Double_t& positionSecondSat,Int_t& myNSatSecond){
 Bool_t firstContiguos=false;
 Bool_t secondContiguos=false;
 Int_t NnoSat=0;
 Double_t satThr=10;
 if(fNSat>20)satThr=20;
 for(int i=fFirstSat-10; i<Wave.size(); i++ ){
   if(fabs(Wave.at(i)-maxWave)<satThr){
     if(!firstHitSaturated){
       positionFirstSat=i;
       firstHitSaturated=true;
       firstContiguos=true;
       myNSat++;
     }
     else if(firstContiguos)myNSat++;
   }
   else if(fabs(Wave.at(i)-maxWave)>satThr && i>fFirstSat){
     NnoSat++;
     firstContiguos=false;
   }
   if(NnoSat>6 && fabs(Wave.at(i)-maxWave)<satThr){   //before nnosat>6
     if(!secondHitSaturated){
       positionSecondSat=i;
       secondHitSaturated=true;
       secondContiguos=true;
       myNSatSecond++;
     }
     else if(secondContiguos)myNSatSecond++;
   }
 }
 if( myNSat<4  ||myNSatSecond < 6 )secondHitSaturated = false; //the <4 like develop (on first hit sat.) ; <6 to eliminate fluctuation 
 return;
}

void DigitizerChannelECal::FindDoubleHitSaturatedAngularCoefficient(std::vector<Double_t> Wave,Double_t maxWave, Bool_t& firstHitSaturated,Double_t& positionFirstSat,Int_t& myNSat,Bool_t& secondHitSaturated, Double_t& positionSecondSat,Int_t& myNSatSecond){
 Bool_t firstContiguos=false;
 Bool_t secondContiguos=false;
 Int_t NnoSat=0;
 Double_t satThr=20;
 Int_t iCoeff=1;
 Double_t sigma=0.8;
 Double_t sigmaDiff=2.077;
 Double_t meanDiff= -2.397;
 std::vector<Double_t> timeOut;
 std::vector<Double_t> AngCoeffOut;
 // std::vector<Double_t> dxdt;
 //if(fNSat<20){return;}
 //Int_t nsmooth=5;
 Int_t j=0;
 for(int i=fFirstSat-10; i<Wave.size(); i+=iCoeff ){
   if(fabs(Wave.at(i)-maxWave)<20){
     if(fGlobalMode->GetGlobalDebugMode()) hdiffSatMaxWave->Fill(Wave.at(i)-maxWave);
     if(fGlobalMode->GetGlobalDebugMode()) hdiffSatContiguosWave->Fill(Wave.at(i)-Wave.at(i-1));
     if(fGlobalMode->GetGlobalDebugMode()) hdiffSat1Wave->Fill(Wave.at(i)-Wave.at(fFirstSat));
     /* if(i>3+nsmooth/2){ 
       dxdt.push_back(Wave.at(i)-Wave.at(i-3));
       if(dxdt.at(j)>5*1.715)std::cout<<"Out of the derivative range... time " << i << " derivative " << dxdt.at(j) << std::endl;
       j++;
      //dxdt1[ll]=-(Temp1[ll]-Temp1[ll-iDer]);
     }else{
       dxdt.push_back(0);
       j++;
       }*/
   }
 }
 // if(fGlobalMode->GetGlobalDebugMode()){ for(int i=0; i<dxdt.size(); i++) hsatDerivative->Fill(dxdt.at(i));}
 /*
 Int_t NnoSatEnd=0;
 Int_t NnoFirstSat=0;
 Bool_t stopSat=false;
 std::cout<<"No sat beginning " << NnoSat <<std::endl;
 for(int i=fFirstSat-10; i<Wave.size(); i++ ){
   if(fabs(Wave.at(i)-maxWave-meanDiff)<3*sigmaDiff){
     if(!firstHitSaturated){
       positionFirstSat=i;
       firstHitSaturated=true;
       firstContiguos=true;
       myNSat++;
     }
     else if(firstContiguos){
       if((fabs(Wave.at(i-1)-maxWave-meanDiff)<3*sigmaDiff) && (fabs(Wave.at(i+1)-maxWave-meanDiff)<3*sigmaDiff) )myNSat++;
       else{firstContiguos=false; NnoFirstSat++;}
     }
   }
   else if(fabs(Wave.at(i)-maxWave-meanDiff)>3*sigmaDiff && i>fFirstSat && stopSat==false ){
     NnoSat++;
     // std::cout<<"no sat in condition " << NnoSat << " at position " << i << std::endl;
   }
   if(NnoSat>2 && fabs(Wave.at(i)-maxWave-meanDiff)<3*sigmaDiff){   //before nnosat>6
     if(!secondHitSaturated){
       positionSecondSat=i;
       secondHitSaturated=true;
       secondContiguos=true;
       myNSatSecond++;
     }
     else if(secondContiguos){
       if((fabs(Wave.at(i-1)-maxWave-meanDiff)<3*sigmaDiff)   && (fabs(Wave.at(i+1)-maxWave-meanDiff)<3*sigmaDiff) )myNSatSecond++;
       else{secondContiguos=false; NnoSatEnd++; stopSat=true;}
     }
   }
 }
 if(secondHitSaturated)std::cout<<" first hit for nsat " <<myNSat <<" nnosatFirst "<< NnoFirstSat <<" nnosat " <<NnoSat <<" second saturated hit ---time " << positionSecondSat << " for n2sat " <<myNSatSecond << std::endl;
 */  
 for(int i=fFirstSat-10; i<Wave.size(); i+=iCoeff ){
   if(fabs(Wave.at(i)-maxWave)<satThr){
     Double_t angularCoeff = (Wave.at(i+iCoeff)-Wave.at(i))/iCoeff;
     Double_t Const= (Wave.at(i)*(i+iCoeff)-Wave.at(i+iCoeff)*i)/iCoeff;
     if(fGlobalMode->GetGlobalDebugMode()){
       hCoeffAngularSaturation->Fill(angularCoeff);
       hConstant->Fill(Const);
     }
     if(fabs(angularCoeff)>3*sigma){
       timeOut.push_back(i);
       AngCoeffOut.push_back(angularCoeff);
       //std::cout<<"I'm out at time " << i << " while the first sat is " << fFirstSat << " my angular coefficient is " << angularCoeff << " constant " << Const << std::endl;
     }
   }
 }
 Int_t left=0;
 Int_t right=0;
 Bool_t changePendence=false;
 Int_t DeltaLast=5;
 if(fNSat>20 && fNSat<=40)DeltaLast=10;
 if(fNSat>40)DeltaLast=15;
 std::vector<Int_t> separatingTime;
 if(AngCoeffOut.size()>3){
   for(int i=1; i<AngCoeffOut.size()-2; i++){
     if(AngCoeffOut.at(i)*AngCoeffOut.at(i+1)<0 && fabs(timeOut.at(i)-timeOut.at(i+1))<5 && timeOut.at(i)>fFirstSat && (AngCoeffOut.at(i)*AngCoeffOut.at(i-1))>0 && (AngCoeffOut.at(i+2)*AngCoeffOut.at(i+1))>0 && fabs(timeOut.at(i)-timeOut.at(i-1))<3 && fabs(timeOut.at(i+2)-timeOut.at(i+1))<3 &&  timeOut.at(i)<timeOut.at(timeOut.size()-1)-DeltaLast ){ 
       changePendence=true;
       separatingTime.push_back(i);
       //std::cout<<"starTime " << i << std::endl;
     }
   }
 }

 if(changePendence && separatingTime.size()<2 ){
   // std::cout<<"time size " << timeOut.size() << "ang size " << AngCoeffOut.size() << std::endl;
     for(int j=0; j<AngCoeffOut.size()-1; j++){
       // std::cout<<"j " << j << std::endl;
       if( (timeOut.at(j)-timeOut.at(j+1))<3 && timeOut.at(j)<=timeOut.at(separatingTime.at(0))  && timeOut.at(j)>=timeOut.at(separatingTime.at(0))-8 && (AngCoeffOut.at(j)*AngCoeffOut.at(j+1))>0 )left++;
       else if((timeOut.at(j)-timeOut.at(j+1))<3 && timeOut.at(j)>=timeOut.at(separatingTime.at(0)+1)  && timeOut.at(j)<=timeOut.at(separatingTime.at(0)+1)+8  && (AngCoeffOut.at(j)*AngCoeffOut.at(j+1))>0)right++;
     }
   }
 //std::cout<<"left " << left << " right " << right << std::endl;
 // if(left+right>3){
 if(left>1 || right>1){
   Double_t oldNSat=myNSat;
   myNSat=timeOut.at(separatingTime.at(0))-positionFirstSat;
   positionSecondSat=timeOut.at(separatingTime.at(0)+1);
   myNSatSecond=oldNSat-myNSat-(timeOut.at(separatingTime.at(0)+1)-timeOut.at(separatingTime.at(0)));
   if(myNSatSecond>10 && myNSat>10) secondHitSaturated=true;
   //std::cout<<" .........I've find  a second saturated bin??...at i "<< AngCoeffOut.at(separatingTime.at(0))<<" ( time " << timeOut.at(separatingTime.at(0))<<" )  at i+1 " << AngCoeffOut.at(separatingTime.at(0)+1) <<" (time " <<timeOut.at(separatingTime.at(0)+1) << " myNSat " << myNSat << " MynSatSecond "<<myNSatSecond <<" so the boolean is " << secondHitSaturated << std::endl;

   }
 return;
}


void DigitizerChannelECal::MakeDifferenceSaturatedWaveformTeplate(std::vector<Double_t> input,Int_t DeltaSat, Int_t MaxBin,std::vector<Double_t>& TempWave,std::vector<Double_t>& output, Bool_t& OutRMS, Double_t maxWave){
  //std::cout<<"i'm in makedifferenceSaturated....deltasat " << std::endl;
  //std::cout<<"the max time that i see is " << MaxBin << std::endl;
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
   //io comment 29/04    Double_t NormFactor=maxValueWave/maxValue;
  Double_t p0=0.;
  Double_t p1=0.;
  Double_t p2=0.;
  if(maxWave<=875&& maxWave>825){
    p0=865.19;
    p1=3.17536;
    p2=0.0145792;
  }
  else if(maxWave<=825 && maxWave>780){
    p0 =      837.721;
    p1 =      2.63729; 
    p2 =    0.0145538;
  }
  else if(maxWave<=780){
    p0 =      815.644;
    p1 =     2.04881 ; 
    p2 =   0.0148075 ;
  }
  else{   
    p0=904.438;
    p1=3.63858;
    p2=0.0140253;                  
   
  }
  Double_t NormFactor=0.; 
  /*if(fmyNSat>40)*/ NormFactor=(p0+p1*DeltaSat+p2*DeltaSat*DeltaSat)/maxValue;
  //else NormFactor=(p0+p1*fNSat+p2*fNSat*fNSat)/maxValue; // fVMax/maxValue;
  //std::cout<< "MyMax " << NormFactor << std::endl;
   //io comment 29/04    fAmplitude =maxValueWave;
  //std::cout << "NormFactor " << NormFactor << std::endl;
  Double_t DiffSignal=0.;
  for(int i=0; i<5000; i++){
    TempWave.push_back(0);
    output.push_back(0);
  }
  
  /*Double_t deltaBin=20;    // io comment 23/06
  if(fmyNSat>200)deltaBin=8;
  if(fmyNSat<20)deltaBin=0;*/
   
  Double_t deltaBin=15;
  if(DeltaSat>200)deltaBin=8;
  //if(DeltaSat<40)deltaBin=0; // io comment 24/06 for MC 
   for(int ll=0;ll<5000;ll++){
   if(ll+MaxBin-deltaBin>= 0 && ll+MaxBin-deltaBin<5000){
     //std::cout << "ll " << ll << "  position tempwave " << ll+MaxBin-20 << std::endl;
     TempWave.at(ll+MaxBin-deltaBin)=fTemplate[ll]*NormFactor;
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
  if(fGlobalMode->GetGlobalDebugMode()){
    Double_t tempEnergy= CalcChargeSin(250, TempWave)/15.;
    hTemplateVMaxVSEnergy_->Fill(tempEnergy, NormFactor*maxValue);
  }
 return;
}
