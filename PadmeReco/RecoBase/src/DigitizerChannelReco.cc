#include "DigitizerChannelReco.hh"
#include "TObject.h"
#include "TVector3.h"
#include "TSpectrum.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TTree.h"

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>

static int waveformhit=0;
int waveformhitmax=96;
char name[256];
char newname[256];

void DigitizerChannelReco::PrintConfig(){
  std::cout << "Signal width: " << fSignalWidth << " samples" << std::endl;
  std::cout << "fUseAbsSignals: " << fUseAbsSignals << std::endl;  
}

DigitizerChannelReco::~DigitizerChannelReco(){
  if(fGlobalMode->IsPedestalMode() || fGlobalMode->GetGlobalDebugMode()){
    SaveDebugHistos();
  }
;}

void DigitizerChannelReco::Init(GlobalRecoConfigOptions *gMode, PadmeVRecoConfig *cfg){

  fGlobalMode = gMode;
  std::string name;
  name = cfg->GetParOrDefault("DETECTOR","NAME","DIGI");
  name += "DIGI";
  H1 = new TH1D(name.c_str(),name.c_str(),1000,0.,1000.);
  name = cfg->GetParOrDefault("DETECTOR","NAME","DIGI");
  name += "2";
  H2 = new TH1D(name.c_str(),name.c_str(),990,0.,990.);
    
  fTimeBin        = cfg->GetParOrDefault("ADC","TimeBin",1.);
  fVoltageBin     = cfg->GetParOrDefault("ADC","VoltageBin",0.000244);
  fImpedance      = cfg->GetParOrDefault("ADC","InputImpedance",50.);

  fSignalWidth    = cfg->GetParOrDefault("RECO","SignalWindow",1024);
  fPedOffset      = cfg->GetParOrDefault("RECO","PedestalOffset",100);
  
  fPreSamples     = cfg->GetParOrDefault("RECO","SignalPreSamples",1024);
  fPostSamples    = cfg->GetParOrDefault("RECO","SignalPostSamples",1024);
  fPedMaxNSamples = cfg->GetParOrDefault("RECO","NumberOfSamplesPedestal",100);  

  fMinAmplitude    = cfg->GetParOrDefault("RECO","MinAmplitude",10);
  fAmpThresholdLow = cfg->GetParOrDefault("RECO","AmplThrLow",10.);
  fAmpThresholdHigh= cfg->GetParOrDefault("RECO","AmplThrHigh",20.);


  fProcessing     = cfg->GetParOrDefault("RECO","Processing",0); //if fProcessing=1, Peaks finding with TSpectrum, if fProcessing=2, RRC processing followed by TSPectrum peak finding
  fUseAbsSignals  = cfg->GetParOrDefault("RECO","UseAbsSignals",0);

  fNewPed  = cfg->GetParOrDefault("RECO","NewPed",1);
  fTimeCut  = cfg->GetParOrDefault("RECO","TimeCut",0);

  // fUsePulseProcessing  = cfg->GetParOrDefault("RECO","UsePulseProcessing",1);
 fPeakSearchWidth     = cfg->GetParOrDefault("RECO","PeakSearchWidth",6);
 fZeroSuppression     = cfg->GetParOrDefault("RECO","ZeroSuppression",5.);
 fChargeCut           = cfg->GetParOrDefault("RECO","ChargeCut",0.01);
 fDPParameterR1       = cfg->GetParOrDefault("RECO","fDPParameterR1",650.);
 fDPParameterR2       = cfg->GetParOrDefault("RECO","fDPParameterR2",100.);
 fDPParameterC        = cfg->GetParOrDefault("RECO","fDPParameterC",0.030e-9);

  std::cout << cfg->GetName() << "*******************************" <<  std::endl;
  PrintConfig();
  PrepareTmpHistos();  //Temp histo servono anche in non debug mode
  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode()){
    std::cout<<"preparing debug histos"<<std::endl;
    PrepareDebugHistos(); //debugging mode histos
  }
  if (fProcessing==0)
    {
      if (fabs(fAmpThresholdLow-fAmpThresholdHigh)<0.001)
	{
	  std::cout<<cfg->GetName()<<" Digitizer config. ERROR: inconsistent confuguration .... fAmpThresholdLow and fAmpThresholdHigh cannot be ~equal if singleHit reco is selected"<<std::endl;
	}
    }
}

void DigitizerChannelReco::PrepareDebugHistos(){ //Beth 7/6/19 copied from Mauro's DigitizerChannelECal structure to create a set of debug histograms that are produced only in debug mode
  
  static char* fileoutname;
  char name[256];
  char newname[256];

  if(fProcessing==0&&fNewPed==0&&fTimeCut==0) fileoutname="RecoAnSingleHitOldPedNoTimeCut.root";
  if(fProcessing==0&&fNewPed==1&&fTimeCut==0) fileoutname="RecoAnSingleHitNewPedNoTimeCut.root";

  if(fProcessing==0&&fNewPed==0&&fTimeCut==1) fileoutname="RecoAnSingleHitOldPedTimeCut.root";
  if(fProcessing==0&&fNewPed==1&&fTimeCut==1) fileoutname="RecoAnSingleHitNewPedTimeCut.root";

  if(fProcessing==1) fileoutname="RecoAnMultiHit.root";
  if(fProcessing==2) fileoutname="RecoAnRCProcessing.root";

  std::cout<<"Created "<<fileoutname<<std::endl;

  fileOut    = new TFile(fileoutname, "RECREATE");
  
  fileOut->mkdir("Waveforms");
  fileOut->mkdir("AmpSpectra");
  fileOut->mkdir("PedSpectra");
  fileOut->mkdir("AmpChargeSpectra");
  fileOut->mkdir("AmpCharge2D");
  
  if(fProcessing==2){
    fileOut->cd("AmpSpectra");
    gDirectory->mkdir("MultiHit");
    gDirectory->mkdir("RCProcessed");

    fileOut->cd("AmpChargeSpectra");
    gDirectory->mkdir("MultiHit");
    gDirectory->mkdir("RCProcessed");
    
    fileOut->cd("AmpCharge2D");
    gDirectory->mkdir("MultiHit");
    gDirectory->mkdir("RCProcessed");
  }
  
  hListWaveform       = new TList(); //Waveform histograms
  hListAmp            = new TList(); //sAmplitude spectra
  hListPed            = new TList(); //Pedestal spectra
  hListAmpChargeRatio = new TList(); //Spectra of ratio of amplitude to charge
  hListAmpCharge2D    = new TList(); //2D amplitude/charge plots
  
  //these RC lists are used to store the RC plots. If fProcessing==2 then lists without RC will be multihit.
  if(fProcessing==2){
    hListAmpRC            = new TList(); //Amplitude spectra
    hListAmpChargeRatioRC = new TList(); //Spectra of ratio of amplitude to charge
    hListAmpCharge2DRC    = new TList(); //2D amplitude/charge plots
  }
  
  Reco = new TTree("RECO","RECO");//Tree variables:
  Reco->Branch("ElCh",&ElCh);
  hListTmp->Add(hSat    = new TH1F("hSat","hSat",1000,0.,1000.));
  
  //Waveform analysis histograms
  for(int i =0;i<waveformhitmax;i++){
    sprintf(name, "hSignal%d", i);
    hListWaveform->Add(hSignal[i] = new TH1F(name,"Waveform",1024,0.,1024.)); //Signal waveform in digitizer counts

    sprintf(name, "hRCVoltage%d", i);
    hListWaveform->Add(hRCVoltage[i]= new TH1F(name,"hRCVoltage",1024,0.,1024.));

    sprintf(name, "HitAmplitudeChannel%d",i); //This histogram stores the the spectrum of signal amplitudes
    
    if(fProcessing==0&&i<82)       hListAmp->Add(hAmpSpectrum[i] = new TH1F(name,"HitAmplitude",100,0.0,2000.0));
    else if(fProcessing==0&&i>81)  hListAmp->Add(hAmpSpectrum[i] = new TH1F(name,"HitAmplitude",100,0.0,4000.0));

    else if (fProcessing!=0&&i<77) hListAmp->Add(hAmpSpectrum[i] = new TH1F(name,"HitAmplitude",100,0.0,1000.0));
    else if (fProcessing!=0&&i<82) hListAmp->Add(hAmpSpectrum[i] = new TH1F(name,"HitAmplitude",100,0.0,1500.0));
    else if (fProcessing!=0&&i>81) hListAmp->Add(hAmpSpectrum[i] = new TH1F(name,"HitAmplitude",100,0.0,4000.0));
    
    if(fProcessing==2){
      sprintf(name, "RCHitAmplitudeChannel%d",i); //This histogram stores the the spectrum of signal amplitudes
      if(i<77)      hListAmpRC->Add(hAmpSpectrumRC[i] = new TH1F(name,"RCHitAmplitude",100,0.0,300.0));
      else if(i<87) hListAmpRC->Add(hAmpSpectrumRC[i] = new TH1F(name,"RCHitAmplitude",100,0.0,450.0));
      else          hListAmpRC->Add(hAmpSpectrumRC[i] = new TH1F(name,"RCHitAmplitude",100,0.0,900.0));
    }
    
    sprintf(newname, "HitPedestalChannel%d",i); //This histogram stores the spectrum of hit pedestals
    hListPed->Add(hPedSpectrum[i] = new TH1F(newname,"HitPedestal",100,1500.0,4000.0));

    sprintf(name, "AmplitudetoChargeRatioChannel%d",i);
    
    if(fProcessing==0&&fNewPed==0)        hListAmpChargeRatio->Add(hAmpChargeRatio[i] = new TH1F(name,"AmplitudetoChargeRatio",100,0.0,25000.0));
    else if(fProcessing==0&&fNewPed==1)   hListAmpChargeRatio->Add(hAmpChargeRatio[i] = new TH1F(name,"AmplitudetoChargeRatio",100,0.0,25000.0));
    else if (fProcessing!=0){
      if(i<85) hListAmpChargeRatio->Add(hAmpChargeRatio[i] = new TH1F(name,"AmplitudetoChargeRatio",100,0.0,5.0));
      else     hListAmpChargeRatio->Add(hAmpChargeRatio[i] = new TH1F(name,"AmplitudetoChargeRatio",100,0.0,3.0));
      if(fProcessing==2){
   	sprintf(name, "RCAmplitudetoChargeRatioChannel%d",i);
   	if (i<83)  hListAmpChargeRatioRC->Add(hAmpChargeRatioRC[i] = new TH1F(name,"RCAmplitudetoChargeRatio",100,0.0,10.0));
   	else       hListAmpChargeRatioRC->Add(hAmpChargeRatioRC[i] = new TH1F(name,"RCAmplitudetoChargeRatio",100,0.0,6.0));
      }
    }
    
    sprintf(name, "AmplitudeVsChargeChannel%d",i);
    if(fProcessing ==0&&i<72)        hListAmpCharge2D->Add(hAmpCharge2D[i] = new TH2F(name,"AmplitudeVsCharge",500,0.,800.,1000.,0.,0.2));
    else if(fProcessing ==0&&i<83)        hListAmpCharge2D->Add(hAmpCharge2D[i] = new TH2F(name,"AmplitudeVsCharge",500,0.,1500.,1000.,0.,0.2));
    else if(fProcessing ==0&&i>82)        hListAmpCharge2D->Add(hAmpCharge2D[i] = new TH2F(name,"AmplitudeVsCharge",500,0.,3500.,1000.,0.,0.8));
    
    else if(fProcessing!=0&&i<61)    hListAmpCharge2D->Add(hAmpCharge2D[i] = new TH2F(name,"AmplitudeVsCharge",500,0.,450.,1000.,0.,300.));
    else if(fProcessing!=0&&i<73)    hListAmpCharge2D->Add(hAmpCharge2D[i] = new TH2F(name,"AmplitudeVsCharge",500,0.,650.,1000.,0.,500.));
    else if(fProcessing!=0&&i<82)    hListAmpCharge2D->Add(hAmpCharge2D[i] = new TH2F(name,"AmplitudeVsCharge",500,0.,1100.,1000.,0.,900.));
    else if(fProcessing!=0&&i>81)    hListAmpCharge2D->Add(hAmpCharge2D[i] = new TH2F(name,"AmplitudeVsCharge",500,0.,4000.,1000.,0.,3500.));
    if(fProcessing==2){
      sprintf(name, "RCAmplitudeVsChargeChannel%d",i);
      if(i<72)  hListAmpCharge2DRC->Add(hAmpCharge2DRC[i] = new TH2F(name,"RCAmplitudeVsCharge",500,0.,200.,1000.,0.,70.));
      else if(i<81)  hListAmpCharge2DRC->Add(hAmpCharge2DRC[i] = new TH2F(name,"RCAmplitudeVsCharge",500,0.,250.,1000.,0.,90.));
      else           hListAmpCharge2DRC->Add(hAmpCharge2DRC[i] = new TH2F(name,"RCAmplitudeVsCharge",500,0.,900.,1000.,0.,300.));
    }
  }
    

  hListWaveform->Add(hSigFinal = new TH1F("hSigFinal","hSigFinal",1000,0.,1000.));
  hListWaveform->Add(hSigMax = new TH1F("hSigMax","hSigMax",1000,0.,1000.));
  hListWaveform->Add(hdxdtFinal = new TH1F("hdxdtFinal","hdxdtFinal",1000,0.,1000.));
  hListWaveform->Add(hdxdtMaxTime= new TH1F("hdxdtMaxTime","hdxdtMaxTime",400,0.,400.));//Time of max derivative
 
}

void DigitizerChannelReco::PrepareTmpHistos(){ //Copied from DigitizerChannelEcal.cc Beth 7/6/2019
  hListTmp    = new TList();
  if(!fUseOverSample){
    hListTmp->Add(hdxdtTmp   = new TH1F("hdxdt","hdxdt",1000,0.,1000.));
    hListTmp->Add(hSignalTmp = new TH1F("hSignal","hSignal",1000,0.,1000.));
  }
  // over sampled histograms
  Int_t nbinx=4000;
  if(fUseOverSample){
    hListTmp->Add(hSigOv       = new TH1F("hSigOv","hSigOv",nbinx,0.,nbinx/4));
    hListTmp->Add(hSigOvSm     = new TH1F("hSigOvSm","hSigOvSm",nbinx,0.,nbinx/4));
    hListTmp->Add(hdxdtSigOvSm = new TH1F("hdxdtSigOvSm","hdxdtSigOvSm",nbinx,0.,nbinx/4));
  } 
}

void DigitizerChannelReco::SaveDebugHistos(){ //Beth 7/6/19
  std::cout<<"Save Debug"<<std::endl;
  fileOut->cd();
  Reco->Write();
  
  fileOut->cd("Waveforms");
  hListWaveform->Write();
  
  fileOut->cd("AmpSpectra");

  if(fProcessing!=2)  hListAmp->Write();
  
  if(fProcessing==2){
    gDirectory->cd("MultiHit");
    hListAmp->Write();
    gDirectory->cd("../RCProcessed");
    hListAmpRC->Write();
  }
  
  fileOut->cd("PedSpectra");
  hListPed->Write();
  
  fileOut->cd("AmpChargeSpectra");
  if(fProcessing!=2)  hListAmpChargeRatio->Write();
  
  if(fProcessing==2){
    gDirectory->cd("MultiHit");
    hListAmpChargeRatio->Write();
    gDirectory->cd("../RCProcessed");
    hListAmpChargeRatioRC->Write();
  }
  
  fileOut->cd("AmpCharge2D");
  if(fProcessing!=2)  hListAmpCharge2D->Write();
  
  if(fProcessing==2){
    gDirectory->cd("MultiHit");
    hListAmpCharge2D->Write();
    gDirectory->cd("../RCProcessed");
    hListAmpCharge2DRC->Write();
  }

  // fileOut->Write();
  fileOut->Close();
}

void DigitizerChannelReco::SetAbsSignals(){
  for(UShort_t i = 0;i<fNSamples;i++){
    if (fSamples[i] < 2048) {
      fSamples[i] = 4096 - fSamples[i];
    }
  }
}
 
Short_t DigitizerChannelReco::CalcMaximum() {

  fMax = 32767; // 2^15 - 1
  
  for(UShort_t i = 0;i<fNSamples;i++){
    if (fMax > fSamples[i]) {
      fMax = fSamples[i];
      fIMax = i;
    }
  }
  return fMax;
}


Double_t DigitizerChannelReco::CalcPedestal() { //Beth 30/7/19 - one method contains both the "old" and the "new" pedestal calculation so that the pedestal reconstruction method can be chosen by use of the fNewPed flag
  //"old" pedestal calculation
  if(fNewPed==0){
    fPed = 0.;
    fNPedSamples = 0;
    //Call this only after the maximum is set
    if(fIMax - fPedOffset < 0) {
      //What to do if not enough samples for pedestal calculation... 
      //Use the first few samples
      fPed = fSamples[0];
      fNPedSamples = 1;
    } 
    
    for(Short_t i = fIMax - fPedOffset; 
	i >= 0 &&  fNPedSamples <  fPedMaxNSamples; 
	--i,++fNPedSamples) {
      //    std::cout << i << "  " << fNPedSamples << std::endl;
      fPed+= fSamples[i];
    }
       
    fPed /= fNPedSamples;
  }
 
  //"new" pedestal calculation - the pedestal is calculated using the first 200 samples
  if(fNewPed==1){
    fPed = 0.;
    fNPedSamples = 0;
    fPedMaxNSamples=200;
    
    // average of first fPedMaxNSamples
    for(Short_t i = 0 ; i  <   fPedMaxNSamples; i++) {
      fNPedSamples ++;
      fPed+= fSamples[i];
    }
    //std::cout << " fNPedSamples " << fNPedSamples <<" fPed Veto"<< std::endl;
    fPed /= fNPedSamples;
    //std::cout <<  fPed << " fNPedSamples " << fNPedSamples << std::endl;
  }
    return fPed;
}

Double_t DigitizerChannelReco::ZSupHit(Float_t Thr, UShort_t NAvg) {
  Double_t rms1000  = TMath::RMS(NAvg,&fSamples[0]);
  Double_t ZSupHit=-1;
  //  if(rms1000>Thr){
  //  if(rms1000>4.){
  // come back to a veto setup 
  if(rms1000>Thr){
    ZSupHit=0;
  }else{
    ZSupHit=1;
  }
  //std::cout<<"compute zero supp "<<rms1000<<" Zsup "<<ZSupHit<<std::endl;
  return ZSupHit;
}

Double_t DigitizerChannelReco::CalcCharge(UShort_t iMax) { //Only called for single hit
  
  Short_t begin = iMax-fPreSamples > 0? iMax-fPreSamples:0; //Pre = 30 samples = 12ns
  Short_t end = iMax+fPostSamples < fNSamples? iMax+fPostSamples:fNSamples; //Post = 120 samples = 48ns
 
  // M. Raggi for testing
  //begin = 180;
  //end =  1000;
  // come back to a veto setup

  //  std::cout << "Begin: "<< begin << "  end: " << end << std::endl;
  fCharge=0.;
  for(Short_t i = begin;i<end;++i) {
    fCharge+=1.* fSamples[i];
  }
  
  // std::cout << "Integral: " << fCharge << "  Ped:  " << fPed << " Base: " << (end-begin) * fPed << std::endl;
  // std::cout << "Pedestal: " << fPed << "  from: " << fNPedSamples << " samples"<< std::endl;
  // std::cout << "Maximum: " << fMax << "  at position: "<< fIMax << std::endl;

  fCharge = ((1.*end-1.*begin) * fPed) - fCharge;  //subtract events pedestral
  fCharge *= (fVoltageBin*fTimeBin/fImpedance);
  return fCharge;
}

Double_t DigitizerChannelReco::CalcTime(UShort_t iMax) {
  double Time = 0.;
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

  Short_t begin =  - fPreSamples > 0? fIMax - fPreSamples:0;
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
  if (fabs(fAmpThresholdHigh - fAmpThresholdLow)<0.001)
    {
      std::cout<<" Digitizer config. ERROR: inconsistent confuguration .... fAmpThresholdLow and fAmpThresholdHigh cannot be ~equal if singleHit reco is selected"<<std::endl; 
      Time = t3*fTimeBin;
      return Time;
    }
  Time = t3 - (t4-t3)*fAmpThresholdLow/(fAmpThresholdHigh - fAmpThresholdLow);
  Time = Time*fTimeBin;
  return Time;
}


void DigitizerChannelReco::DigitalProcessingRRC(Double_t *uin, Double_t *uout,int NPOINTS, Double_t timebin) { //Beth, implemented from Venelin's idea 06/2019
  // Double_t R1=1300;//ohms
  // Double_t R2=100.; //ohms
  // Double_t C=0.015e-9; //nF


  //simulating RRC circuit. Circuit diagram in presentation from Beth 19/06/2019 
  //approximating voltage output to:
  //dQ/dt=(uin(t)/R2)-((R1+R2)/CR1R2)*Q(t)
  //uout(t)=uin(t)-Q(t)/C


  Double_t R1=fDPParameterR1;//ohms
  Double_t R2=fDPParameterR2; //ohms
  Double_t C=fDPParameterC; //nF

  
  //Calculating the output pulse:
  Double_t integr=0;
  
  static Double_t ic[1024];

  Double_t bin_width=timebin;

  integr=0;
  ic[0]= uin[0]/R2;

  for(int i=1;i<NPOINTS;i++) {
    integr+=ic[i-1]*bin_width; //integr = intgrated charge = charge of this bin + charge of previous bin + bin before...
    ic[i]= uin[i]/R2 - ((R1+R2)/(C*R1*R2))* integr; //ic = current through capacitor = dQ/dt
    uout[i] = uin[i] - integr/(C);
  }  

}

Double_t DigitizerChannelReco::CalcChaTime(std::vector<TRecoVHit *> &hitArray,UShort_t iMax, UShort_t ped) {
  Int_t npeaks = 10;
  static TSpectrum SpectrumProcessor(20);// = new TSpectrum(20);
  double Time   = 0.;
  fCharge = 0.;

  //currently looking for peaks with TSpectrum to obtain multi hit times
  //Int_t npeaks =25;
  static Double_t AbsSamRec[1024];
  static Double_t AbsSamRecDP[1024];


  for(UShort_t s=0;s<iMax;s++){
    AbsSamRec[s] = (Double_t) (-1.*fSamples[s]+ped)/4096.*1000.; //in mV positivo
    //std::cout<< s << "     "  << fSamples[s]  <<" V "<< AbsSamRec[s]  <<std::endl;
  }

  if(fProcessing==1)       H1->SetContent(AbsSamRec); //+ve mV
  else if(fProcessing==2){
    DigitalProcessingRRC(AbsSamRec,AbsSamRecDP,iMax-1 ,fTimeBin*1e-9);
    H1->SetContent(AbsSamRecDP);
  }/* else {
    H1->SetContent(AbsSamRec);
    }*/

  for(UShort_t s=iMax;s<1024;s++){
    AbsSamRec[s] = 0;
  }

  // H1->Reset();

  // for(UShort_t s = 0;s<256;s++) {
  //   AbsSamRecRebin[s]=0;
  //   for(int i = 0;i<4;i++) {
  //     AbsSamRecRebin[s]+=AbsSamRecDP[4*s + i];
  //   }
  // }

  
  //  H1->Rebin(4);

  Double_t VMax = H1->GetMaximum(); //mV
  Double_t VMin = H1->GetMinimum(); //mV

  //   std::cout<<"Get Maximum     "<<VMax<<"   Get Minimum    "<<VMin<<std::endl;
  
  //if (VMax>fAmpThresholdHigh) std::cout<<VMax<<" VMax "<<std::endl;

  //  fMinAmplitude
  if(VMax>fAmpThresholdHigh){
    TSpectrum *s = &SpectrumProcessor;// new TSpectrum(2*npeaks);  //npeaks max number of peaks
    Double_t peak_thr  = fAmpThresholdLow /VMax; //minimum peak height allowed. 
    Int_t nfound = s->Search(H1,fPeakSearchWidth,"",peak_thr);     

    for(Int_t ll=0;ll<nfound;ll++){ //peak loop per channel
      fCharge = 0.;
      Double_t xp = (s->GetPositionX())[ll];
      Double_t yp = (s->GetPositionY())[ll];
      fAmpli1=yp;
      Time=xp*fTimeBin;
      Int_t bin    = H1->GetXaxis()->FindBin(xp);

      TRecoVHit *Hit = new TRecoVHit();  
      Hit->SetTime(Time);
      Hit->SetEnergy(fAmpli1);
      hitArray.push_back(Hit);
    }
    //    delete ;
  }

  Time = Time*fTimeBin;
  return Time;

}

void DigitizerChannelReco::ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray){
  //  Double_t IsZeroSup = ZSupHit(5.,1000.);
  CalcCharge(fIMax);
  //if (fCharge < .01) return;
  // come back to a Veto setup
  if (fCharge < fChargeCut) return;
  double time = CalcTime(fIMax);
  // if (fTime < 20.) return;
  
  TRecoVHit *Hit = new TRecoVHit();
  Hit->SetTime(time);
  // M. Raggi going to charge
  // Double_t fEnergy= fCharge*1000./15; //going from nC to MeV using 15pC/MeV
  // Hit->SetEnergy(fCharge);
  // come back to a Veto setup
  Hit->SetEnergy(fCharge);
  hitArray.push_back(Hit);

  // std::cout << "Hit charge:  " << fCharge << "  Time: " << fTime << std::endl;
  
}

void DigitizerChannelReco::ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray){  //using TSpectrum
  
  Double_t ped=CalcPedestal();
  //std::cout<<"Pedestal    "<<ped<<std::endl;
  //ped=3650;
  CalcChaTime(hitArray,1000,ped);

}


void DigitizerChannelReco::Reconstruct(std::vector<TRecoVHit *> &hitArray){
  //  std::cout<<"Reconstruct "<<hPedSpectrum[GetChID()]->GetName()<<std::endl;
   if(fZeroSuppression > 1 ) {
     if (ZSupHit(fZeroSuppression,1000.) < 0) {
       std::cout<<"zero suppressing"<<std::endl;
       return; 
     }
    }

  if(fUseAbsSignals) {
    SetAbsSignals();
  }

  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode()){ //only do histofill in debug mode (therefore only create histograms in debug mode)
    static int noskipped = 0;
    CalcMaximum(); //returns maximum amplitude of signal (not position of maximum)
    CalcPedestal();
    if(fPed - fMax < fMinAmplitude ){
      //std::cout<<"GetChID() "<<GetChID()<<"fPed-fMax<fMinAmplitude"<<std::endl;
      noskipped++;
      return; //altro taglio da capire fatto in che unita'? conteggi?
    }
    //std::cout<<"ped = "<<ped<<std::endl;
    HistoFill();
    //    std::cout<<"noskipped = "<<noskipped<<std::endl;
  }

  if(fProcessing!=0) {                //if not in single hit mode, run multihit analysis. ReconstructMultiHit includes the RC processing reconstruction code - so if the fProcessing flag ==2 then the RC processing is done there
    ReconstructMultiHit(hitArray);
  } 
  else {                              //if in single hit mode run single hit analysis
    CalcMaximum();
    //std::cout<<" fIMax =  "<<fIMax<<std::endl;
    CalcPedestal();
    if(fPed - fMax < fMinAmplitude ) return; //altro taglio da capire fatto in che unita'? conteggi?
    ReconstructSingleHit(hitArray);
  }
}


void DigitizerChannelReco::HistoFill(){  

  static Double_t Signal[1024];

  const Int_t npeaks =50;//maximum number of peaks to be found
  static Int_t nfound1=-1;
  static Int_t nfound2=0;

  static Int_t iMax = 1000; //Parsed to DigitalProcessingRRC
  
  static Double_t MultiSignal[npeaks];//amplitude of peaks found with TSpectrum
  static Double_t RCSignal[npeaks];//amplitude of peaks of RC processed signal found with TSpectrum

  static Double_t MultiCharge[npeaks];//amplitude of peaks found with TSpectrum
  static Double_t RCCharge[npeaks];//amplitude of peaks of RC processed signal found with TSpectrum

  static TSpectrum spMulti(20);
  static TSpectrum spRC(20);
  TSpectrum *sMulti = &spMulti; 
  TSpectrum *sRC = &spRC; 
    //= new TSpectrum(2*npeaks);  //npeaks max number of peaks

  Int_t    dxdtMaxbin; //bin containing maximum dxdt
  Double_t sigmax; //maximum signal
  Int_t    sigmaxbin;  //bin containing maximum signal

  int derivbins = 10;
  int nsmoothing =1;//Number of samples either side used to smooth the signal

  //  std::cout<<"fPed = "<<fPed<<std::endl;

  for(Int_t SignalSample = 0;SignalSample<fNSamples;SignalSample++){
    Signal[SignalSample]=fPed-(1.*fSamples[SignalSample]);//in digitizer counts
    hSigFinal->SetBinContent(SignalSample,Signal[SignalSample]);//used to fill sigmax histo. Overwritten on every loop so will eventually hold the last hit in the last channel
    hSigMax->Fill(hSigFinal->GetMaximum()/4096*1e3);//in mV
  }
  
  Double_t ProcessedAmp[1024]= {TMath::Infinity()};
  
  if (fProcessing!=0){ //Beth 11/7/19 - include TSpectrum processing in Histofill so that you have direct access to variables here, then you can plot things directly, meaning that you can directly compare pre- and post-RC processing 
    
    double Time1   = 0.; //1 refers to multihi
    double Time2   = 0.; //2 refers to RC processing
    Double_t pCMeV= 3.2E5*2*1.67E-7; 
    Int_t NIntSamp= fSignalWidth/fTimeBin; 
    
    H1->SetContent(Signal);
    DigitalProcessingRRC(Signal, ProcessedAmp,iMax-1 ,fTimeBin*1e-9);
    H2->SetContent(ProcessedAmp);
    
    Double_t VMax1 = H1->GetMaximum();
    Double_t VMin1 = H1->GetMinimum();
    Double_t VMax2 = H2->GetMaximum();
    Double_t VMin2 = H2->GetMinimum();
    
    //if (VMax1>fAmpThresholdHigh) std::cout<<VMax1<<" VMax1 "<<std::endl;
    //if (VMax2>fAmpThresholdHigh) std::cout<<VMax2<<" VMax2 "<<std::endl;
    
    if(VMax1>fAmpThresholdHigh){
      Double_t peak_thr1  = fAmpThresholdLow/VMax1; //minimum peak height allowed. 
      nfound1 = sMulti->Search(H1,10,"",peak_thr1);     //10 = sigma
    }
    
    if(fProcessing==2&&(VMax2>fAmpThresholdHigh)){
      Double_t peak_thr2  = fAmpThresholdLow/VMax2; //minimum peak height allowed. 
      nfound2 = sRC->Search(H2,10,"",peak_thr2);     //10 = sigma
    }
    
    for(Int_t ll=0;ll<nfound1;ll++){ //peak loop per channel
      fCharge = 0.;
      //Float_t xp   = xpeaks[ll];
      Double_t xp = (sMulti->GetPositionX())[ll];
      //Float_t yp   = ypeaks[ll];
      Double_t yp = (sMulti->GetPositionY())[ll];
      fAmpli1=yp;
      MultiSignal[ll]=fAmpli1;
      MultiCharge[ll]=TMath::Infinity();
      
      Time1=xp*fTimeBin;
      Int_t bin    = H1->GetXaxis()->FindBin(xp);
      //	  if(bin<1000){
      // for (Int_t ii=bin-NIntSamp;ii<bin+NIntSamp;ii++) {
      if((bin-fPreSamples>0)&&(bin+fPostSamples<fNSamples)){
	MultiCharge[ll]=0;
	for (Int_t ii=bin-fPreSamples;ii<bin+fPostSamples;ii++) {
	  //	      if(H1->GetBinContent(ii)>0.005){ 
	  MultiCharge[ll] += H1->GetBinContent(ii)*1e-3/fImpedance*fTimeBin*1e-9/1E-12;//charge in pC as opposed to for single hit where it's in nC
	}
      }
    }
    
    
    //fEnergy = fCharge/pCMeV;	  
    if(fProcessing==2){
      for(Int_t ll=0;ll<nfound2;ll++){ //peak loop per channel
	fCharge = 0.;
	//Float_t xp   = xpeaks[ll];
	Double_t xp = (sRC->GetPositionX())[ll];
	//Float_t yp   = ypeaks[ll];
	Double_t yp = (sRC->GetPositionY())[ll];
	fAmpli2=yp;
	RCSignal[ll]=fAmpli2;
	RCCharge[ll]=TMath::Infinity();
	
	Time2=xp*fTimeBin;
	Int_t bin    = H2->GetXaxis()->FindBin(xp);
	//	    if(bin<1000){
	//	      for (Int_t ii=bin-NIntSamp;ii<bin+NIntSamp;ii++) {
	if((bin-30>0)&&(bin+30<fNSamples)){
	  RCCharge[ll]=0;
	  for (Int_t ii=bin-30;ii<bin+30;ii++) {
	    //	      if(H2->GetBinContent(ii)>0.005){ 
	    RCCharge[ll] += H2->GetBinContent(ii)*1e-3/fImpedance*fTimeBin*1e-9/1E-12;//charge in pC as opposed to for single hit where it's in nC
	  }
	}
	//fEnergy = fCharge/pCMeV;	  
      }
    }
  }
  //    }
    //}
      
    sigmaxbin = hSigFinal->TH1::GetMaximumBin();
  
    Int_t begin = sigmaxbin-10>0? sigmaxbin-10:0;//if(sigmaxbin-10)>0, begin=sigmaxbun-10, else begin = 0
    Int_t end = sigmaxbin+150>fNSamples? sigmaxbin+150:fNSamples;
    
    if(fProcessing==0){
      if(fTimeCut==1){ //If we're using the time cut...
	if(CalcTime(fIMax)>10.) { //use the time cut
	  hPedSpectrum[GetChID()]->Fill(fPed);
	  hAmpSpectrum[GetChID()]->Fill(TMath::MaxElement(fNSamples,Signal));
	  hAmpChargeRatio[GetChID()]->Fill(TMath::MaxElement(fNSamples,Signal)/fCharge);
	  hAmpCharge2D[GetChID()]->Fill(TMath::MaxElement(fNSamples,Signal),fCharge);
	}
      }

      else if (fTimeCut==0){ //Otherwise, if we're not using the time cut, fill the histograms anyway
	hPedSpectrum[GetChID()]->Fill(fPed);
	hAmpSpectrum[GetChID()]->Fill(TMath::MaxElement(fNSamples,Signal));
	hAmpChargeRatio[GetChID()]->Fill(TMath::MaxElement(fNSamples,Signal)/fCharge);
	hAmpCharge2D[GetChID()]->Fill(TMath::MaxElement(fNSamples,Signal),fCharge);
      }
    }

     else if(fProcessing!=0){
       hPedSpectrum[GetChID()]->Fill(fPed);
       //      std::cout<<"nfound1 = "<<nfound1<<std::endl;
       for(int i = 0;i<nfound1;i++){
    	hAmpSpectrum[GetChID()]->Fill(MultiSignal[i]);
    	if(MultiCharge[i]<TMath::Infinity()){
     	  hAmpChargeRatio[GetChID()]->Fill(MultiSignal[i]/MultiCharge[i]);
	  hAmpCharge2D[GetChID()]->Fill(MultiSignal[i],MultiCharge[i]);
     	}
       }

       if(fProcessing==2){
     	for(int i = 0;i<nfound2;i++){
     	  hAmpSpectrumRC[GetChID()]->Fill(RCSignal[i]);
     	  if(RCCharge[i]<TMath::Infinity()){
     	    hAmpChargeRatioRC[GetChID()]->Fill(RCSignal[i]/RCCharge[i]);
	    hAmpCharge2DRC[GetChID()]->Fill(RCSignal[i],RCCharge[i]);
     	  }
     	}
       }
     }

    if(GetChID()<waveformhitmax){
      for(int i=0;i<fNSamples;i++){
	hSignal[GetChID()]->SetBinContent(i+1,Signal[i]);
	if(ProcessedAmp[i]<TMath::Infinity())	hRCVoltage[GetChID()]->SetBinContent(i+1, ProcessedAmp[i]);
	
      }
    }
    
    if(waveformhit==0) std::cout<<"parameters are: fProcessing = "<<fProcessing<<" fNewPed = "<<fNewPed<<" fTimeCut = "<<fTimeCut<<std::endl;
    waveformhit++;//after every hit increase the value of the hit counting variable
    
}

