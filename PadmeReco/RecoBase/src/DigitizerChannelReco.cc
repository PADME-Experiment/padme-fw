#include "DigitizerChannelReco.hh"
#include "TObject.h"
#include "TVector3.h"
#include "TSpectrum.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TTree.h"
#include "TLegend.h"

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>

int waveformhitmax=10;//2*96;
int printing=0;
char name[256];
bool AnalogFlag=0;//print analog signals?
int NZSupEvents[96]={0};
TString detectorname;
int ndebugfail=0;
void DigitizerChannelReco::PrintConfig(){
  std::cout << "Signal width: " << fSignalWidth << " samples" << std::endl;
  std::cout << "fUseAbsSignals: " << fUseAbsSignals << std::endl;  
}


DigitizerChannelReco::~DigitizerChannelReco(){
  if(fGlobalMode->IsPedestalMode() || fGlobalMode->GetGlobalDebugMode()){
    /*    for(int ii=0;ii<hSignal[GetChID()]->GetNbinsX();ii++){
      std::cout<<"sig height = "<<hSignal[GetChID()]->GetBinContent(ii)<<std::endl;
      }*/
    SaveDebugHistos();
  }
;}


void DigitizerChannelReco::Init(GlobalRecoConfigOptions *gMode, PadmeVRecoConfig *cfg){
  detectorname=cfg->GetName();

  fGlobalMode = gMode;
  std::string name;

  HTSpec = new TH1D("tspec","tspec",1000,0.,1000.);;
 
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


  fMultihit       = cfg->GetParOrDefault("RECO","Multihit",0); //if MultiHit=1, Peaks finding with TSpectrum
  fUseAbsSignals  = cfg->GetParOrDefault("RECO","UseAbsSignals",1);


  fProcessing          = cfg->GetParOrDefault("RECO","Processing",3);
  fPeakSearchWidth     = cfg->GetParOrDefault("RECO","PeakSearchWidth",6);
  fZeroSuppression     = cfg->GetParOrDefault("RECO","ZeroSuppression",5.);
  fChargeCut           = cfg->GetParOrDefault("RECO","ChargeCut",0.01);
  fDPParameterR1       = cfg->GetParOrDefault("RECO","fDPParameterR1",650.);
  fDPParameterR2       = cfg->GetParOrDefault("RECO","fDPParameterR2",100.);
  fDPParameterC        = cfg->GetParOrDefault("RECO","fDPParameterC",0.030e-9);
  fmVtoMeV             = cfg->GetParOrDefault("RECO","ConversionFactor",29.52);

  //  PrepareTmpHistos();  //Temp histo servono anche in non debug mode
  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode()){
    std::cout<<"preparing debug histos"<<std::endl;
    PrepareDebugHistos(); //debugging mode histos
  }
  std::cout << cfg->GetName() << "*******************************" <<  std::endl;
  PrintConfig();

  if (fMultihit==0)
    {
      if (fabs(fAmpThresholdLow-fAmpThresholdHigh)<0.001)
	{
	  std::cout<<cfg->GetName()<<" Digitizer config. ERROR: inconsistent confuguration .... fAmpThresholdLow and fAmpThresholdHigh cannot be ~equal if singleHit reco is selected"<<std::endl;
	}
    }
}

void DigitizerChannelReco::PrepareDebugHistos(){ //Beth 20/10/21 copied from 190917padme-fw, in turn copied from Mauro's DigitizerChannelECal structure to create a set of debug histograms that are produced only in debug mode
  
  static char* fileoutname;
  char name[256];
  char newname[256];

  if(fProcessing==0&&fNewPed==0&&fTimeCut==0) fileoutname="RecoAnSingleHitOldPedNoTimeCut.root";
  if(fProcessing==0&&fNewPed==1&&fTimeCut==0) fileoutname="RecoAnSingleHitNewPedNoTimeCut.root";

  if(fProcessing==0&&fNewPed==0&&fTimeCut==1) fileoutname="RecoAnSingleHitOldPedTimeCut.root";
  if(fProcessing==0&&fNewPed==1&&fTimeCut==1) fileoutname="RecoAnSingleHitNewPedTimeCut.root";

  if(fProcessing==1) fileoutname="RecoAnMultiHit.root";
  if(fProcessing==2) fileoutname="RecoAnRCProcessing.root";
  if(fProcessing==3)
    if(detectorname=="EVeto") fileoutname="EVetoRecoAnDeriv.root";
    else if(detectorname=="PVeto") fileoutname="PVetoRecoAnDeriv.root";

  std::cout<<"Created "<<fileoutname<<std::endl;

  fileOut    = new TFile(fileoutname, "RECREATE");
  
  hListWaveform       = new TList(); //Waveform histograms

  //these RC lists are used to store the RC plots. If fProcessing==2 then lists without RC will be multihit.
  if(fProcessing==2){
    hListAmpRC            = new TList(); //Amplitude spectra
    hListAmpChargeRatioRC = new TList(); //Spectra of ratio of amplitude to charge
    hListAmpCharge2DRC    = new TList(); //2D amplitude/charge plots
  }

  if(fProcessing==3)
    hListWaveformDeriv = new TList();
  
  Reco = new TTree("RECO","RECO");//Tree variables:
  //  Reco->Branch("ElCh",&ElCh);
  //  hListTmp->Add(hSat    = new TH1F("hSat","hSat",1000,0.,1000.));

  hNoHitsRaw = new TH1F("RawHits","RawHits",20,0,20);//number of hits reconstructed by TSpectrum on raw signal
  hNoHitsDeriv = new TH1F("DerivHits","DerivHits",20,0,20);//number of hits reconstructed by TSpectrum on derivatives
  hDeltaNoHits = new TH1F("DeltaNoHits","DeltaNoHits",20,-10,10);
  hDeltaTime = new TH1F("DeltaTime","tRawPeak-tDerivPeak",100,0,10);
  hDeltaTimeOneHit = new TH1F("DeltaTimeOneHit","tRawPeak-tDerivPeakOneHit",100,0,10);
  hDeltaTimeTwoHit = new TH1F("DeltaTimeTwoHit","tRawPeak-tDerivPeakTwoHit",100,0,10);
  hZeroCrossTimeDiff = new TH1F("ZeroCrossTimeDiff","tDerivZeroCross-tDerivPeak",100,0,10);
  hZeroCrossTimeDiffOneHit = new TH1F("ZeroCrossTimeDiffOneHit","(tDerivZeroCross-tDerivPeak)One Hit",100,0,10);
  hRawV = new TH1F("RawV","RawV",100,0,400);
  hRawVCorrect = new TH1F("RawVCorrect","RawVCorrect",100,0,400);
  hRawVOneHit = new TH1F("RawVOneHit","RawVOneHit",100,0,400);
  hRawVMultiHit = new TH1F("RawVMultiHit","RawVMultiHit",100,0,400);
  hRawVMultiHitCorrect = new TH1F("RawVMultiHitCorrect","RawVMultiHitCorrect",100,0,400);
  hDerivV = new TH1F("DerivV","DerivV",100,0,200);
  hOccupancyRaw = new TH1F("OccupancyRaw","OccupancyRaw",96,0,96);
  hOccupancyDeriv = new TH1F("OccupancyDeriv","OccupancyDeriv",96,0,96);
  hMinTimeDiffRaw = new TH1F("MinTimeDiffRaw","MinTimeDiffRaw",100,0,100);
  hMinTimeDiffDeriv = new TH1F("MinTimeDiffDeriv","MinTimeDiffDeriv",100,0,100);
  hVRatio = new TH1F("VRatio","VRatio",50,0,5);  
  hNZSupEvents = new TH1F("hNZSupEvents","hNZSupEvents",96,0,96);
  hNoiseRMSAvg = new TH1F("hNoiseRMSAvg","hNoiseRMSAvg",96,0,96);
  hRawVvsDerivZeroCrossTimeDiff = new TH2F("RawVvsZeroCrossTimeDiff","RawVvs(tDerivZeroCrossing-tDerivPeak)",100,0,10,800,0,100);
  hDerivVvsDeltaTime = new TH2F("DerivVvstRawPeak-tDerivPeak","DerivVvs(tRawPeak-tDerivPeak)",200,-100,100,800,0,200);

  for(int ii=0;ii<96;ii++){
    sprintf(name, "DeltaNoHitsChannel%d",ii);
    //    std::cout<<"GetChID() "<<GetChID()<<" name "<<name<<std::endl;
    hDeltaNoHitsPerChannel[ii] = new TH1F(name,name,20,-10,10);

    sprintf(name, "DeltaTimeChannel%d",ii);
    //    std::cout<<"GetChID() "<<GetChID()<<" name "<<name<<std::endl;
    hDeltaTimePerChannel[ii] = new TH1F(name,name,200,-100,100);

    sprintf(name, "RawVChannel%d",ii);
    hRawVPerChannel[ii] = new TH1F(name,name,100,0,400);

    sprintf(name, "RawVOneHitChannel%d",ii);
    hRawVOneHitPerChannel[ii] = new TH1F(name,name,100,0,400);

    sprintf(name, "RawVCorrectChannel%d",ii);
    hRawVCorrectPerChannel[ii] = new TH1F(name,name,100,0,400);

    sprintf(name, "DerivVChannel%d",ii);
    hDerivVPerChannel[ii] = new TH1F(name,name,100,0,200);

    sprintf(name, "NoHitsRawChannelPerEvent%d",ii);
    hNoHitsRawPerChannel[ii] = new TH1F(name,name,20,0,20);

    sprintf(name, "NoHitsDerivChannelPerEvent%d",ii);
    hNoHitsDerivPerChannel[ii] = new TH1F(name,name,20,0,20);

    sprintf(name, "MinTimeDiffRaw%d",ii);
    hMinTimeDiffRawPerChannel[ii] = new TH1F(name,name,100,0,100);

    sprintf(name, "MinTimeDiffDeriv%d",ii);
    hMinTimeDiffDerivPerChannel[ii] = new TH1F(name,name,100,0,100);

    sprintf(name,"hNoiseRMSPerChannel%d",ii);
    hNoiseRMSPerChannel[ii] = new TH1F(name,name,20,0,10);

    sprintf(name, "RawVvsDeltaTimeChannel%d",ii);
    hRawVvsDeltaTimePerChannel[ii] = new TH2F(name,name,200,-100,100,800,0,200);

    sprintf(name, "DerivVvsDeltaTimeChannel%d",ii);
    hDerivVvsDeltaTimePerChannel[ii] = new TH2F(name,name,200,-100,100,800,0,200);

  }  
}

void DigitizerChannelReco::PrepareTmpHistos(){ //Copied from DigitizerChannelEcal.cc Beth 7/6/2019
  /*  hListTmp    = new TList();
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
  } */
}

void DigitizerChannelReco::SaveDebugHistos(){ //Beth 7/6/19
  std::cout<<"Save Debug"<<std::endl;
  std::cout<<"Event Counter is "<<EventCounter<<std::endl;
  std::cout<<"AnalogFlag is "<<AnalogFlag<<std::endl;
  hOccupancyRawPerEvent = (TH1F*) hOccupancyRaw->Clone();
  hOccupancyRawPerEvent->Scale(1./(EventCounter+1));//EventCounter starts from 0 so will be 1 less than the total number of events used
  hOccupancyRawPerEvent->SetNameTitle("hOccupancyRawPerEvent","hOccupancyRawPerEvent");

  hOccupancyDerivPerEvent = (TH1F*)hOccupancyDeriv->Clone();
  hOccupancyDerivPerEvent->Scale(1./(EventCounter+1));
  hOccupancyDerivPerEvent->SetNameTitle("hOccupancyDerivPerEvent","hOccupancyDerivPerEvent");

  hOccupancyRatio = (TH1F*) hOccupancyDeriv->Clone();
  hOccupancyRatio->Divide(hOccupancyRaw);
  hOccupancyRatio->SetNameTitle("hOccupancyRatio","hOccupancyRatio");
  fileOut->cd();
  Reco->Write();
  
  fileOut->mkdir("Waveforms");
  fileOut->cd("Waveforms");
  //  hListWaveform->Write();

  for(int ii = 0;ii<hSignal.size();ii++) //if(nRawHitVec[ii]!=nDerivHitVec[ii]) 
    //if(ii<waveformhitmax)
      if(AnalogFlag==1)
	if(RawPrintFlag[ii]==1)
	  hSignal[ii]->Write();  

  fileOut->mkdir("WaveformTSpec");
  fileOut->cd("WaveformTSpec");
  for(int ii = 0;ii<hRaw.size();ii++) //if(nRawHitVec[ii]!=nDerivHitVec[ii])
    //if(ii<waveformhitmax)
      if(AnalogFlag==1)//    if(RawPrintFlag[ii]==1)
      //    if(ii<96)
	if(RawPrintFlag[ii]==1)
	  hRaw[ii]->Write();  

  if(fProcessing==3){
    fileOut->mkdir("DerivativeTSpec");
    fileOut->cd("DerivativeTSpec");
    for(int ii = 0;ii<hDeriv.size();ii++) //if(nRawHitVec[ii]!=nDerivHitVec[ii]) 
      //if(ii<waveformhitmax)
      if(AnalogFlag==1)//    if(DerivPrintFlag[ii]==1)
	//    if(ii<96)
	if(DerivPrintFlag[ii]==1)
	  hDeriv[ii]->Write();  

    fileOut->mkdir("NoZeroDerivativeTSpec");
    fileOut->cd("NoZeroDerivativeTSpec");
    for(int ii = 0;ii<hDerivNoZero.size();ii++) //if(nRawHitVec[ii]!=nDerivHitVec[ii]) 
      //if(ii<waveformhitmax)
      if(AnalogFlag==1)//    if(DerivPrintFlag[ii]==1)
	//    if(ii<96)
	if(DerivNoZeroPrintFlag[ii]==1)
	  hDerivNoZero[ii]->Write();  
  } 

  //  std::cout<<hNoHitsRaw<<std::endl;

  /*  fileOut->mkdir("DeltaHitsChannel");  
  fileOut->cd("DeltaHitsChannel");  
  for(int ii=0;ii<96;ii++) hDeltaNoHitsPerChannel[ii]->Write();

  fileOut->mkdir("DeltaTimeChannel");  
  fileOut->cd("DeltaTimeChannel");  
  for(int ii=0;ii<96;ii++) hDeltaTimePerChannel[ii]->Write();

  fileOut->mkdir("RawVChannel");  
  fileOut->cd("RawVChannel");  
  for(int ii=0;ii<96;ii++) hRawVPerChannel[ii]->Write();

  fileOut->mkdir("RawVOneHitChannel");  
  fileOut->cd("RawVOneHitChannel");  
  for(int ii=0;ii<96;ii++) hRawVOneHitPerChannel[ii]->Write();

  fileOut->mkdir("RawVCorrectChannel");  
  fileOut->cd("RawVCorrectChannel");  
  for(int ii=0;ii<96;ii++) hRawVCorrectPerChannel[ii]->Write();

  fileOut->mkdir("DerivVChannel");  
  fileOut->cd("DerivVChannel");  
  for(int ii=0;ii<96;ii++) hDerivVPerChannel[ii]->Write();

  fileOut->mkdir("NoHitsRawChannel");  
  fileOut->cd("NoHitsRawChannel");  
  for(int ii=0;ii<96;ii++) hNoHitsRawPerChannel[ii]->Write();

  fileOut->mkdir("NoHitsDerivChannel");  
  fileOut->cd("NoHitsDerivChannel");  
  for(int ii=0;ii<96;ii++)  hNoHitsDerivPerChannel[ii]->Write();

  fileOut->mkdir("RawMinTimeDiffChannel");  
  fileOut->cd("RawMinTimeDiffChannel");  
  for(int ii=0;ii<96;ii++)  hMinTimeDiffRawPerChannel[ii]->Write();

  fileOut->mkdir("DerivMinTimeDiffChannel");  
  fileOut->cd("DerivMinTimeDiffChannel");  
  for(int ii=0;ii<96;ii++)  hMinTimeDiffDerivPerChannel[ii]->Write();

  fileOut->mkdir("NoiseRMS");  
  fileOut->cd("NoiseRMS");  
  for(int ii=0;ii<96;ii++)  hNoiseRMSPerChannel[ii]->Write();*/

  fileOut->cd("../");
  hNoHitsRaw->Write();
  hNoHitsDeriv->Write();
  hDeltaNoHits->Write();
  hDeltaTime->Write();
  hDeltaTimeOneHit->Write();
  hDeltaTimeTwoHit->Write();
  hZeroCrossTimeDiff->Write();
  hZeroCrossTimeDiffOneHit->Write();
  hRawV->Write();
  hRawVCorrect->Write();
  hRawVOneHit->Write();
  hRawVMultiHit->Write();
  hRawVMultiHitCorrect->Write();
  hDerivV->Write();
  hOccupancyRaw->Write();
  hOccupancyDeriv->Write();
  hOccupancyRawPerEvent->Write();
  hOccupancyDerivPerEvent->Write();
  hOccupancyRatio->Write();
  hMinTimeDiffRaw->Write();
  hMinTimeDiffDeriv->Write();
  hRawVvsDerivZeroCrossTimeDiff->Write();
  hVRatio->Write();
  hNZSupEvents->Write();
  hNoiseRMSAvg->Divide(hNZSupEvents);
  hNoiseRMSAvg->Write();
  fileOut->Close();

  std::cout<<"ndebug fail "<<ndebugfail<<std::endl;
}

void DigitizerChannelReco::SetAbsSignals(){
  Double_t ScaleFactor=1;
  if(detectorname=="PVeto")  ScaleFactor=SetPVetoChaGain();
  else  if(detectorname=="EVeto")  ScaleFactor=SetEVetoChaGain();
  //  std::cout<<GetChID()<<" "<<ScaleFactor<<std::endl;
  for(UShort_t i = 0;i<fNSamples;i++){
    //    std::cout<<"i "<<i<<" "<<fSamples[i]<<std::endl;
    //    if (fSamples[i] < 2048) {
      fSamples[i] = ScaleFactor*(4096 - fSamples[i]);
      //}
  }
}
 
Double_t DigitizerChannelReco::ZeroCrossingFinder(UShort_t x1, UShort_t x2, double h1, double h2){//takes two points (x1,h1) and (x2,h2) and using linear interpolation find the point where the straight line passing through them crosses 0. x1 and x2 in samples

  double x0=-1;
  x0 = x2 - h2*((x2-x1)/(h2-h1));
  
  return x0;
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

Double_t DigitizerChannelReco::CalcPedestal() {  //the pedestal is calculated using the first 150 samples
  fPed = 0.;
  fNPedSamples = 0;
  fPedMaxNSamples=150;//200;

  // average of first fPedMaxNSamples
  for(Short_t i = 0 ; i  <   fPedMaxNSamples; i++) {
       fNPedSamples ++;
       fPed+= fSamples[i];
  }
  //std::cout << " fNPedSamples " << fNPedSamples <<" fPed Veto"<< std::endl;
  fPed /= fNPedSamples;
  //std::cout <<  fPed << " fNPedSamples " << fNPedSamples << std::endl;
  return fPed;
}

Double_t DigitizerChannelReco::CalcCharge(UShort_t iMax) {
  
  Short_t begin = iMax-fPreSamples > 0? iMax-fPreSamples:0;
  Short_t end = iMax+fPostSamples < fNSamples? iMax+fPostSamples:fNSamples;
 
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


void DigitizerChannelReco::DigitalProcessingRRC(Double_t *uin, Double_t *uout,int NPOINTS, Double_t timebin) {

  // Double_t R1=1300;//ohms
  // Double_t R2=100.; //ohms
  // Double_t C=0.015e-9; //nF

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
    integr+=ic[i-1]*bin_width;
    ic[i]= uin[i]/R2 - ((R1+R2)/(C*R1*R2))* integr;
    uout[i] = uin[i] - integr/(C);
  }  
}

Double_t DigitizerChannelReco::SetPVetoChaGain(){
  Double_t ScaleFactor = 1;
  //  std::cout<<"Setting cha gain"<<std::endl;
  if(GetChID()==22)	 ScaleFactor =  0.89383;
  if(GetChID()==24)	 ScaleFactor =  0.84220;
  if(GetChID()==25)	 ScaleFactor =  0.85237;
  if(GetChID()==26)	 ScaleFactor =  1.06271;
  if(GetChID()==27)	 ScaleFactor =  0.86539;
  if(GetChID()==28)	 ScaleFactor =  0.95964;
  if(GetChID()==29)	 ScaleFactor =  1.134  ;
  if(GetChID()==30)	 ScaleFactor =  0.91646;
  if(GetChID()==31)	 ScaleFactor =  0.79576;
  if(GetChID()==32)	 ScaleFactor =  1.15361;
  if(GetChID()==33)	 ScaleFactor =  0.82434;
  if(GetChID()==34)	 ScaleFactor =  0.90825;
  if(GetChID()==35)	 ScaleFactor =  0.90099;
  if(GetChID()==36)	 ScaleFactor =  0.88136;
  if(GetChID()==37)	 ScaleFactor =  0.98483;
  if(GetChID()==38)	 ScaleFactor =  1.04746;
  if(GetChID()==39)	 ScaleFactor =  1.03663;
  if(GetChID()==40)	 ScaleFactor =  1.05651;
  if(GetChID()==41)	 ScaleFactor =  1.02239;
  if(GetChID()==42)	 ScaleFactor =  0.91358;
  if(GetChID()==43)	 ScaleFactor =  0.87653;
  if(GetChID()==44)	 ScaleFactor =  1.04864;
  if(GetChID()==45)	 ScaleFactor =  1.00114;
  if(GetChID()==46)	 ScaleFactor =  1.16712;
  if(GetChID()==47)	 ScaleFactor =  1.12873;
  if(GetChID()==48)	 ScaleFactor =  1.04787;
  if(GetChID()==49)	 ScaleFactor =  1.03366;
  if(GetChID()==50)	 ScaleFactor =  1.12382;
  if(GetChID()==51)	 ScaleFactor =  0.78289;
  if(GetChID()==52)	 ScaleFactor =  0.92276;
  if(GetChID()==53)	 ScaleFactor =  1.10371;
  if(GetChID()==54)	 ScaleFactor =  1.03439;
  if(GetChID()==55)	 ScaleFactor =  0.84243;
  if(GetChID()==56)	 ScaleFactor =  0.91272;
  if(GetChID()==57)	 ScaleFactor =  0.94740;
  if(GetChID()==58)	 ScaleFactor =  1.72974;
  if(GetChID()==59)	 ScaleFactor =  0.98525;
  if(GetChID()==60)	 ScaleFactor =  1.48557;
  if(GetChID()==61)	 ScaleFactor =  0.84290;
  if(GetChID()==62)	 ScaleFactor =  1.07302;
  if(GetChID()==63)	 ScaleFactor =  0.95644;
  if(GetChID()==64)	 ScaleFactor =  0.98771;
  if(GetChID()==65)	 ScaleFactor =  0.95593;
  if(GetChID()==66)	 ScaleFactor =  1.06352;
  if(GetChID()==67)	 ScaleFactor =  0.94356;
  if(GetChID()==68)	 ScaleFactor =  1.03614;
  if(GetChID()==69)	 ScaleFactor =  1.02791;
  if(GetChID()==70)	 ScaleFactor =  1.15406;
  if(GetChID()==71)	 ScaleFactor =  1.16004;
  if(GetChID()==72)	 ScaleFactor =  1.03714;
  if(GetChID()==73)	 ScaleFactor =  0.92192;
  if(GetChID()==74)	 ScaleFactor =  1.07615;
  if(GetChID()==75)	 ScaleFactor =  0.92302;
  if(GetChID()==76)	 ScaleFactor =  0.99030;
  if(GetChID()==77)	 ScaleFactor =  0.84142;
  if(GetChID()==78)	 ScaleFactor =  0.95946;
  if(GetChID()==79)	 ScaleFactor =  1.60565;
  if(GetChID()==80)	 ScaleFactor =  1.18413;
  if(GetChID()==81)	 ScaleFactor =  1.63128;
  if(GetChID()==82)	 ScaleFactor =  1.11557;
  if(GetChID()==83)	 ScaleFactor =  0.63291;
  if(GetChID()==84)	 ScaleFactor =  1.07061;
  if(GetChID()==85)	 ScaleFactor =  2.63158;
  return ScaleFactor;
}

Double_t DigitizerChannelReco::SetEVetoChaGain(){
  Double_t ScaleFactor=1;
//  if (GetChID()==0)	  ScaleFactor = 2.27837 ;
//  if (GetChID()==1)	  ScaleFactor = 2.44945 ;
//  if (GetChID()==2)	  ScaleFactor = -0.00024;
//  if (GetChID()==3)	  ScaleFactor = 2.29281 ;
//  if (GetChID()==4)	  ScaleFactor = 2.5324  ;
//  if (GetChID()==5)	  ScaleFactor = 1.43228 ;
//  if (GetChID()==6)	  ScaleFactor = 2.18172 ;
//  if (GetChID()==7)	  ScaleFactor = 1.61582 ;
//  if (GetChID()==8)	  ScaleFactor = 3.80508 ;
//  if (GetChID()==9)	  ScaleFactor = 1.59203 ;
//  if (GetChID()==10)	  ScaleFactor = 2.69722 ;
//  if (GetChID()==11)	  ScaleFactor = 2.41559 ;
//  if (GetChID()==12)	  ScaleFactor = -0.00077;
//  if (GetChID()==13)	  ScaleFactor = 2.51071 ;
//  if (GetChID()==14)	  ScaleFactor = 2.07603 ;
//  if (GetChID()==15)	  ScaleFactor = 2.74678 ;
//  if (GetChID()==16)	  ScaleFactor = 2.14131 ;
//  if (GetChID()==17)	  ScaleFactor = 1.59652 ;
//  if (GetChID()==18)	  ScaleFactor = 1.09756 ;
//  if (GetChID()==19)	  ScaleFactor = 1.18081 ;
//  if (GetChID()==20)	  ScaleFactor = 2.27725 ;
//  if (GetChID()==21)	  ScaleFactor = 1.55198 ;
//  if (GetChID()==22)	  ScaleFactor = 1.57896 ;
//  if (GetChID()==23)	  ScaleFactor = 1.20597 ;
//  if (GetChID()==24)	  ScaleFactor = 0.851691;
//  if (GetChID()==25)	  ScaleFactor = 1.22119 ;
  if (GetChID()==26)	  ScaleFactor = 0.856615;
  if (GetChID()==27)	  ScaleFactor = 0.89882 ;
  if (GetChID()==28)	  ScaleFactor = 0.865298;
  if (GetChID()==29)	  ScaleFactor = 0.869438;
  if (GetChID()==30)	  ScaleFactor = 0.85474 ;
  if (GetChID()==31)	  ScaleFactor = 0.828042;
  if (GetChID()==32)	  ScaleFactor = 0.846438;
  if (GetChID()==33)	  ScaleFactor = 0.780138;
  if (GetChID()==34)	  ScaleFactor = 0.857581;
  if (GetChID()==35)	  ScaleFactor = 1.06836 ;
  if (GetChID()==36)	  ScaleFactor = 0.994798;
  if (GetChID()==37)	  ScaleFactor = 0.990249;
  if (GetChID()==38)	  ScaleFactor = 0.912875;
  if (GetChID()==39)	  ScaleFactor = 0.822158;
  if (GetChID()==40)	  ScaleFactor = 0.982159;
  if (GetChID()==41)	  ScaleFactor = 1.05156 ;
  if (GetChID()==42)	  ScaleFactor = 0.85166 ;
  if (GetChID()==43)	  ScaleFactor = 1.02658 ;
  if (GetChID()==44)	  ScaleFactor = 1.06106 ;
  if (GetChID()==45)	  ScaleFactor = 1.05025 ;
  if (GetChID()==46)	  ScaleFactor = 1.12109 ;
  if (GetChID()==47)	  ScaleFactor = 0.933666;
  if (GetChID()==48)	  ScaleFactor = 0.894675;
  if (GetChID()==49)	  ScaleFactor = 0.86492 ;
  if (GetChID()==50)	  ScaleFactor = 0.856738;
  if (GetChID()==51)	  ScaleFactor = 1.13245 ;
  if (GetChID()==56)	  ScaleFactor = 0.916978;
  if (GetChID()==57)	  ScaleFactor = 0.943916;
  if (GetChID()==58)	  ScaleFactor = 1.18774 ;
  if (GetChID()==59)	  ScaleFactor = 0.912141;
  if (GetChID()==60)	  ScaleFactor = 1.22622 ;
  if (GetChID()==61)	  ScaleFactor = 1.09939 ;
  if (GetChID()==62)	  ScaleFactor = 0.957775;
  if (GetChID()==63)	  ScaleFactor = 0.717468;
  if (GetChID()==64)	  ScaleFactor = 1.36546 ;
  if (GetChID()==65)	  ScaleFactor = 0.912748;
  if (GetChID()==66)	  ScaleFactor = 0.752954;
  if (GetChID()==67)	  ScaleFactor = 0.994185;
  if (GetChID()==68)	  ScaleFactor = 1.02538 ;
  if (GetChID()==69)	  ScaleFactor = 0.800011;
  if (GetChID()==70)	  ScaleFactor = 1.02774 ;
  if (GetChID()==71)	  ScaleFactor = 0.791105;
  if (GetChID()==72)	  ScaleFactor = 0.869039;
  if (GetChID()==73)	  ScaleFactor = 0.835778;
  if (GetChID()==74)	  ScaleFactor = 0.972259;
  if (GetChID()==75)	  ScaleFactor = 0.850822;
  if (GetChID()==76)	  ScaleFactor = 1.04118 ;
  if (GetChID()==77)	  ScaleFactor = 0.799205;
  if (GetChID()==78)	  ScaleFactor = 0.725349;
  if (GetChID()==79)	  ScaleFactor = 0.303602;
  if (GetChID()==80)	  ScaleFactor = 0.878745;
  if (GetChID()==81)	  ScaleFactor = 0.969697;
  if (GetChID()==82)	  ScaleFactor = 0.903124;
  if (GetChID()==83)	  ScaleFactor = 0.954053;
  if (GetChID()==84)	  ScaleFactor = 0.827301;
  if (GetChID()==85)	  ScaleFactor = 0.867393;
//  if (GetChID()==86)	  ScaleFactor = 0.815928;
//  if (GetChID()==87)	  ScaleFactor = 0.90814 ;
//  if (GetChID()==88)	  ScaleFactor = 0.387597;
  if (GetChID()==89) 	  ScaleFactor = 0.276622;

  return ScaleFactor;
}

Double_t DigitizerChannelReco::CalcChaTime(std::vector<TRecoVHit *> &hitArray,UShort_t iMax, UShort_t ped, Int_t fProcessing, Int_t& nfound) {
  nfound=0;
  char name[256];
  Int_t npeaks = 10;
  static TSpectrum SpectrumProcessor(20);// = new TSpectrum(20);
  double Time   = 0.;
  fCharge = 0.;

  Int_t derivpoints = 15;//Number of bins before over which to take the derivative

  //currently looking for peaks with TSpectrum to obtain multi hit times
  //Int_t npeaks =25;
  static Double_t AbsSamRec[1024];
  static Double_t AbsSamRecDP[1024];
  static Double_t AbsSamRecDeriv[1024];
  static Double_t SamRecDeriv[1024];//not set to 0

  for(UShort_t s=0;s<iMax;s++){
    AbsSamRec[s] = (Double_t) (fSamples[s]-ped)/4096.*1000.; //in mV positivo
    if(s>=derivpoints)    SamRecDeriv[s]=(AbsSamRec[s]-AbsSamRec[s-derivpoints]);
    else if(s<derivpoints) SamRecDeriv[s]=0;
    if(fProcessing==3){
      if(SamRecDeriv[s]<=0) AbsSamRecDeriv[s] = 0;//to prevent TSpectrum being confused by negative values, set negative values of the derivative to 0
      else if (SamRecDeriv[s]>0)
	AbsSamRecDeriv[s] = SamRecDeriv[s];
    }
  }

  HTSpec->Reset();
  HNoZeroDeriv->Reset();

  if(fProcessing==1){//&&hSignal.size()<waveformhitmax){
    sprintf(name, "hSignalEvent%iChannel%d", EventCounter,GetChID());
    hSignal.push_back(new TH1F(name,name,1024,0.,1024.)); //Signal waveform in digitizer counts
    hSignal[hSignal.size()-1]->SetContent(AbsSamRec);
  }

  if (fProcessing==2) {
    fPeakSearchWidth=13;
    DigitalProcessingRRC(AbsSamRec,AbsSamRecDP,iMax-1 ,fTimeBin*1e-9);
    HTSpec->SetContent(AbsSamRecDP);
  }
  else if(fProcessing==3){
    fPeakSearchWidth=5;//
    HTSpec->SetContent(AbsSamRecDeriv);
  } 
  else {//not RRC or deriv
    fPeakSearchWidth=6;
    HTSpec->SetContent(AbsSamRec);
  }

  Double_t VMax = HTSpec->GetMaximum();
  Double_t VMin = HTSpec->GetMinimum();

  TSpectrum *spec;
  //  fMinAmplitude
  if(GetChID()==58||GetChID()==60){
    fAmpThresholdHigh=12;
    fAmpThresholdLow=12;
  }

  if(VMax>fAmpThresholdHigh){
    spec = &SpectrumProcessor;// new TSpectrum(2*npeaks);  //npeaks max number of peaks
    Double_t peak_thr;
    //fProcessing ==1 => raw, fProcessing ==2 =>RRC, fProcessing ==3 => deriv
    if(fProcessing!=3) peak_thr  = fAmpThresholdLow/VMax; //minimum peak height allowed. 
    double derivthr = fAmpThresholdLow/2.11;
    if(fProcessing==3) peak_thr = derivthr/VMax;   
    if(peak_thr<0||peak_thr>1){
      if(fProcessing==1) std::cout<<"EventCounter "<<EventCounter<<" Ch "<<GetChID()<<" fampthr "<<fAmpThresholdLow<<" VMax "<<VMax<<" raw "<<peak_thr<<std::endl;
      if(fProcessing==3) std::cout<<"EventCounter "<<EventCounter<<" Ch "<<GetChID()<<" fampthr "<<fAmpThresholdLow<<" VMax "<<VMax<<" deriv "<<peak_thr<<std::endl;
    }

    nfound = spec->Search(HTSpec,fPeakSearchWidth,"",peak_thr);     
    for(Int_t ll=0;ll<nfound;ll++){ //peak loop per channel
      fCharge = 0.;
      Double_t xp = (spec->GetPositionX())[ll];
      Double_t yp = (spec->GetPositionY())[ll];
      fAmpli=yp;
      Time=xp*fTimeBin;
      Int_t bin    = HTSpec->GetXaxis()->FindBin(xp);

      if(fProcessing==1){
	tRawHitVec.push_back(Time);//ns 
	vRawHitVec.push_back(yp);//mV
	hRawV->Fill(yp);
	hRawVPerChannel[GetChID()]->Fill(yp);
	if(nfound==1){
	  hRawVOneHit->Fill(yp);
	  hRawVOneHitPerChannel[GetChID()]->Fill(yp);
	}
	else 	  hRawVMultiHit->Fill(yp);
	hOccupancyRaw->Fill(GetChID());
      }
      if(fProcessing==3){
	int derivcrossed=0;
	double samplecross=0;
	tDerivHitVec.push_back(Time);//ns 
	vDerivHitVec.push_back(yp);//mV 
	//	if(nfound==1){
	  hDerivV->Fill(yp);
	  hDerivVPerChannel[GetChID()]->Fill(yp);
	  //}
	hOccupancyDeriv->Fill(GetChID());
	
	for(int ii = xp;ii<xp+25;ii++){
	  if(derivcrossed==0&&ii>0&&SamRecDeriv[ii-1]>0&&SamRecDeriv[ii]<0){
	    samplecross = ZeroCrossingFinder(ii,ii-1,SamRecDeriv[ii],SamRecDeriv[ii-1]);
	    derivcrossed =1;
	  }
	  else if(derivcrossed==0&&ii>0&&SamRecDeriv[ii-1]==0&&SamRecDeriv[ii]<0){//if the previous amp is 0, take the point two samples before
	    samplecross = ZeroCrossingFinder(ii,ii-2,SamRecDeriv[ii],SamRecDeriv[ii-2]);
	    derivcrossed =1;
	  }//failed in 10 hits in the first event of run_0030650_20201125_015755/run_0030650_20201125_015755_lvl1_00_166.root 
	}
	tDerivZeroCrossVec.push_back(samplecross*fTimeBin);
	
	if(samplecross==0){
	  ndebugfail++;
	}
      }//end if processing = 3
      
      if(fProcessing==3){
	TRecoVHit *Hit = new TRecoVHit();  
	Hit->SetTime(Time);
	fEnergy=fAmpli/fmVtoMeV;
	Hit->SetEnergy(fEnergy);
	//      Hit->SetEnergy(fAmpli);
	hitArray.push_back(Hit);
      }
    }//end of nfound loop
    //    delete s;
  }//closes if(VMax>thr)
  
  if((fProcessing==1&&tRawHitVec.size()!=nfound)||(fProcessing==3&&tDerivHitVec.size()!=nfound)||(tDerivHitVec.size()!=tDerivZeroCrossVec.size())){
    std::cout<<"THINGS DON'T ADD UP!"<<std::endl;
    return -100;
  }

  std::vector<int> index(nfound, 0);
  
  for (int i = 0 ; i != index.size() ; i++) {
    index[i] = i;
  }
  
  sort(index.begin(), index.end(),
       [&](const int& a, const int& b) {
	 if(fProcessing==1) return (tRawHitVec[a] < tRawHitVec[b]);
	 else if(fProcessing==3) return (tDerivHitVec[a] < tDerivHitVec[b]);
       }
       );


  if((index.size()!=nfound)){
    std::cout<<"CONFUSION!"<<std::endl;
    return -100;
  }
  //  std::cout<<"**--**--**--"<<std::endl;
  double tailcorrection=0;
  double DeltaTSortNs=0;
  double DeltaTSortSamples=0;
  for (int ii = 0 ; ii != index.size() ; ii++) {
    if(fProcessing==1){
      tRawSortHitVec.push_back(tRawHitVec[index[ii]]);
      vRawSortHitVec.push_back(vRawHitVec[index[ii]]);
      if(ii>0){
	DeltaTSortNs=tRawSortHitVec[ii]-tRawSortHitVec[ii-1];
	DeltaTSortSamples=tRawSortHitVec[ii]/fTimeBin-tRawSortHitVec[ii-1]/fTimeBin;
	tRawDiffHitVec.push_back(DeltaTSortNs);
	tailcorrection = TailHeight(DeltaTSortSamples);
	vRawSortTailCorrectHitVec.push_back(vRawSortHitVec[ii]-vRawSortTailCorrectHitVec[ii-1]*tailcorrection);
	//	std::cout<<"Channel "<<GetChID()<<" nhits "<<nfound<<" ii "<<ii<<" DeltaTSort "<<DeltaTSortSamples<<" tailcorrection "<<tailcorrection<<" rawbefore "<<vRawSortHitVec[ii]<<" rawafter "<<vRawSortTailCorrectHitVec[ii]<<" correct previous "<<vRawSortTailCorrectHitVec[ii-1]<<std::endl;
      }
      else vRawSortTailCorrectHitVec.push_back(vRawSortHitVec[ii]);
      hRawVCorrect->Fill(vRawSortTailCorrectHitVec[ii]);
      if(nfound>1) hRawVMultiHitCorrect->Fill(vRawSortTailCorrectHitVec[ii]);
      hRawVCorrectPerChannel[GetChID()]->Fill(vRawSortTailCorrectHitVec[ii]);
    }//end if processing ==1
    else if(fProcessing==3){
      tDerivSortHitVec.push_back(tDerivHitVec[index[ii]]);
      vDerivSortHitVec.push_back(vDerivHitVec[index[ii]]);
      if(ii>0) tDerivDiffHitVec.push_back(tDerivSortHitVec[ii]-tDerivSortHitVec[ii-1]);
    }
  }//end loop over hits
  if(AnalogFlag==1){
    if(fProcessing==1){//&&hRaw.size()<waveformhitmax){
      hRaw.push_back((TH1F*)HTSpec->Clone());
      sprintf(name, "hRawEvent%iChannel%d", EventCounter,GetChID());
      hRaw[hRaw.size()-1]->SetNameTitle(name,name);
      RawPrintFlag.push_back(0);//by default don't print the signal to the root file
    }

    if(fProcessing==3){//&&hDeriv.size()<waveformhitmax){
      hDeriv.push_back((TH1F*)HTSpec->Clone());
      sprintf(name, "hDerivEvent%iChannel%d", EventCounter,GetChID());
      hDeriv[hDeriv.size()-1]->SetNameTitle(name,name);
      DerivPrintFlag.push_back(0);//by default don't print the signal to the root file
      DerivNoZeroPrintFlag.push_back(0);//
      HNoZeroDeriv->SetContent(SamRecDeriv);
      sprintf(name, "hDerivNoZeroEvent%iChannel%d", EventCounter,GetChID());
      hDerivNoZero.push_back((TH1F*)HNoZeroDeriv->Clone());
      hDerivNoZero[hDerivNoZero.size()-1]->SetNameTitle(name,name);
    }
  }
  //  Time = Time*fTimeBin; // ********BUG?????? Time is already multiplied by fTimeBin above...********
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
  Double_t IsZeroSup = ZSupHit(fZeroSuppression,1000.);   //io 8/06 to try
  //  Double_t IsZeroSup = ZSupHit(5,1000.);   //io 8/06 to try
  if(IsZeroSup==1 && !fGlobalMode->IsPedestalMode()) return; //perform zero suppression unless you are doing pedestals
  Double_t ped=CalcPedestal();

  fProcessing=1;
  CalcChaTime(hitArray,1024,ped,fProcessing,nfoundraw);
  fProcessing=3;
  CalcChaTime(hitArray,1024,ped,fProcessing,nfoundderiv);

  hNoHitsRaw->Fill(nfoundraw);
  hNoHitsDeriv->Fill(nfoundderiv);
  hDeltaNoHits->Fill(nfoundderiv-nfoundraw);
  hDeltaNoHitsPerChannel[GetChID()]->Fill(nfoundderiv-nfoundraw);
  hNoHitsRawPerChannel[GetChID()]->Fill(nfoundraw);
  hNoHitsDerivPerChannel[GetChID()]->Fill(nfoundderiv);

  if(nfoundraw>1){
    Double_t MinHitTimeDiffRaw=*min_element(tRawDiffHitVec.begin(), tRawDiffHitVec.end());
    //std::cout<<nfoundraw<<" "<<tRawDiffHitVec.size()<<std::endl;
    //	std::cout<<MinHitTimeDiffRaw<<std::endl;
    hMinTimeDiffRaw->Fill(MinHitTimeDiffRaw);
    hMinTimeDiffRawPerChannel[GetChID()]->Fill(MinHitTimeDiffRaw);

  }
  
  if(nfoundderiv>1){
    Double_t MinHitTimeDiffDeriv=*min_element(tDerivDiffHitVec.begin(), tDerivDiffHitVec.end());
    hMinTimeDiffDeriv->Fill(MinHitTimeDiffDeriv);
    hMinTimeDiffDerivPerChannel[GetChID()]->Fill(MinHitTimeDiffDeriv);
  }
  for(int ii=0;ii<tDerivHitVec.size();ii++){
    if((GetChID()==77||GetChID()==79||GetChID()==80)&&printing<waveformhitmax&&AnalogFlag==1){
      DerivPrintFlag[DerivPrintFlag.size()-1]=1;//set these analog signals to print
      RawPrintFlag[RawPrintFlag.size()-1]=1;
      DerivNoZeroPrintFlag[DerivNoZeroPrintFlag.size()-1]=1;
      printing++;
      break;
    }
  }

  if(nfoundderiv==nfoundraw){
    for(int ii=0;ii<tDerivHitVec.size();ii++){      
      /*      if(nfoundderiv<4&&TMath::Abs(tDerivHitVec[ii]-tRawHitVec[ii])>10&&RawPrintFlag[RawPrintFlag.size()-1]==0){
	std::cout<<"event "<< EventCounter <<" channel "<<GetChID() <<" ii "<<ii<<" tRaw "<<tRawHitVec[ii]<<" ns, tDeriv "<<tDerivHitVec[ii]<<" ns, tRaw "<<tRawHitVec[ii]/fTimeBin<<" bins, tDeriv "<<tDerivHitVec[ii]/fTimeBin<<" bins "<<std::endl;
	//	std::cout<<" derivprint "<<DerivPrintFlag[DerivPrintFlag.size()-1]<<" rawprint "<<RawPrintFlag[RawPrintFlag.size()-1]<<std::endl;

	}*/

      hDeltaTime->Fill(tRawSortHitVec[ii]-tDerivSortHitVec[ii]);
      hZeroCrossTimeDiff->Fill(tDerivZeroCrossVec[ii]-tDerivHitVec[ii]);
	
      if(nfoundderiv==2) hDeltaTimeTwoHit->Fill(tRawSortHitVec[ii]-tDerivSortHitVec[ii]);    
      if(nfoundderiv==1){
	hDeltaTimeOneHit->Fill(tRawHitVec[ii]-tDerivHitVec[ii]);
	hVRatio->Fill(vRawHitVec[ii]/vDerivHitVec[ii]);
	hZeroCrossTimeDiffOneHit->Fill(tDerivZeroCrossVec[ii]-tDerivHitVec[ii]);
      }
      
      hDeltaTimePerChannel[GetChID()]->Fill(tRawSortHitVec[ii]-tDerivSortHitVec[ii]);

      hRawVvsDerivZeroCrossTimeDiff->Fill(tDerivZeroCrossVec[ii]-tDerivSortHitVec[ii],vRawSortHitVec[ii]);
      hRawVvsDeltaTimePerChannel[GetChID()]->Fill(tRawSortHitVec[ii]-tDerivSortHitVec[ii],vRawSortHitVec[ii]);

      hDerivVvsDeltaTime->Fill(tRawSortHitVec[ii]-tDerivSortHitVec[ii],vDerivSortHitVec[ii]);
      hDerivVvsDeltaTimePerChannel[GetChID()]->Fill(tRawSortHitVec[ii]-tDerivSortHitVec[ii],vDerivSortHitVec[ii]);
    }
    //    std::cout<<"nfoundderiv "<<nfoundderiv<<std::endl;
  }
  tRawHitVec.clear();
  tDerivHitVec.clear();
  tDerivZeroCrossVec.clear();
  tRawSortHitVec.clear();
  vDerivSortHitVec.clear();
  vRawSortHitVec.clear();
  vRawSortTailCorrectHitVec.clear();
  tDerivSortHitVec.clear();
  tRawDiffHitVec.clear();
  tDerivDiffHitVec.clear();
  vRawHitVec.clear();
  vDerivHitVec.clear();
  nRawHitVec.push_back(nfoundraw);
  nDerivHitVec.push_back(nfoundderiv);
}


void DigitizerChannelReco::Reconstruct(std::vector<TRecoVHit *> &hitArray){
  /*  if(fZeroSuppression > 1 ) {
    if (ZSupHit(fZeroSuppression,1000.) < 0) {
    std::cout<<"zerosupp problem, returning from reco"<<std::endl;
    return; 
    }
    }*/
  
  if(AnalogFlag==1)  std::cout<<"BE CAREFUL: analogflag is on"<<std::endl;

  if(fUseAbsSignals)
    SetAbsSignals();

  if(fMultihit) {
    ReconstructMultiHit(hitArray);//does CalcPedestal, then CalcChaTime
  } 
  else {
    CalcMaximum();
    CalcPedestal();
    ReconstructSingleHit(hitArray);
  }
}

Double_t DigitizerChannelReco::ZSupHit(Float_t Thr, UShort_t NAvg) {
  fRMS1000  = TMath::RMS(NAvg,&fSamples[0]);
  Double_t ZSupHit=-1;

  //  if(GetChID()==58||GetChID()==60) Thr = 7.;

  if(fRMS1000>Thr){
    ZSupHit=0;
  }else{
    ZSupHit=1;
    //    std::cout<<"compute zero supp "<<fRMS1000<<" Zsup "<<ZSupHit<<std::endl;
    hNoiseRMSAvg->AddBinContent(GetChID()+1,fRMS1000);
    hNoiseRMSPerChannel[GetChID()]->Fill(fRMS1000);
    hNZSupEvents->AddBinContent(GetChID()+1,1);

  }
  //  std::cout<<"compute zero supp "<<fRMS1000<<" Thr "<<Thr<<" Zsup "<<ZSupHit<<std::endl;
  
  return ZSupHit;
}

Double_t DigitizerChannelReco::TailHeight(Int_t DeltaT){//DeltaT in samples
  Double_t HeightFrac=0;
  Double_t Frac[152];
  Frac[0]= -0.980571   ; 
  Frac[1]= -0.981553   ;
  Frac[2]= -0.980736   ;
  Frac[3]= -0.97778    ;
  Frac[4]= -0.971722   ;
  Frac[5]= -0.964676   ;
  Frac[6]= -0.956346   ;
  Frac[7]= -0.947524   ;
  Frac[8]= -0.936815   ;
  Frac[9]= -0.925396   ;
  Frac[10]=-0.913626   ;
  Frac[11]=-0.902825   ;
  Frac[12]=-0.889087   ;
  Frac[13]=-0.875895   ;
  Frac[14]=-0.862712   ;
  Frac[15]=-0.8499     ;
  Frac[16]=-0.83571    ;
  Frac[17]=-0.820816   ;
  Frac[18]=-0.807027   ;
  Frac[19]=-0.793632   ;
  Frac[20]=-0.778125   ;
  Frac[21]=-0.763295   ;
  Frac[22]=-0.749179   ;
  Frac[23]=-0.736244   ;
  Frac[24]=-0.72276    ;
  Frac[25]=-0.70841    ;
  Frac[26]=-0.695512   ;
  Frac[27]=-0.683497   ;
  Frac[28]=-0.671098   ;
  Frac[29]=-0.658029   ;
  Frac[30]=-0.645816   ;
  Frac[31]=-0.635959   ;
  Frac[32]=-0.625102   ;
  Frac[33]=-0.614064   ;
  Frac[34]=-0.603985   ;
  Frac[35]=-0.594267   ;
  Frac[36]=-0.584422   ;
  Frac[37]=-0.573764   ;
  Frac[38]=-0.564061   ;
  Frac[39]=-0.55551    ;
  Frac[40]=-0.546319   ;
  Frac[41]=-0.535383   ;
  Frac[42]=-0.526042   ;
  Frac[43]=-0.517341   ;
  Frac[44]=-0.507091   ;
  Frac[45]=-0.49763    ;
  Frac[46]=-0.488771   ;
  Frac[47]=-0.480027   ;
  Frac[48]=-0.470536   ;
  Frac[49]=-0.460673   ;
  Frac[50]=-0.452545   ;
  Frac[51]=-0.443358   ;
  Frac[52]= -0.434426  ;
  Frac[53]= -0.425019  ;
  Frac[54]= -0.416526  ;
  Frac[55]= -0.40837   ;
  Frac[56]= -0.398906  ;
  Frac[57]= -0.388772  ;
  Frac[58]= -0.381358  ;
  Frac[59]= -0.372678  ;
  Frac[60]= -0.363733  ;
  Frac[61]= -0.354299  ;
  Frac[62]= -0.345341  ;
  Frac[63]= -0.337202  ;
  Frac[64]= -0.32762   ;
  Frac[65]= -0.317911  ;
  Frac[66]= -0.309973  ;
  Frac[67]= -0.302964  ;
  Frac[68]= -0.294315  ;
  Frac[69]= -0.285437  ;
  Frac[70]= -0.278386  ;
  Frac[71]= -0.271311  ;
  Frac[72]= -0.264793  ;
  Frac[73]= -0.256882  ;
  Frac[74]= -0.250253  ;
  Frac[75]= -0.244707  ;
  Frac[76]= -0.237993  ;
  Frac[77]= -0.231874  ;
  Frac[78]= -0.226499  ;
  Frac[79]= -0.221193  ;
  Frac[80]= -0.215178  ;
  Frac[81]= -0.209712  ;
  Frac[82]= -0.204143  ;
  Frac[83]= -0.198955  ;
  Frac[84]= -0.194389  ;
  Frac[85]= -0.188292  ;
  Frac[86]= -0.183074  ;
  Frac[87]= -0.178442  ;
  Frac[88]= -0.173292  ;
  Frac[89]= -0.166721  ;
  Frac[90]= -0.162393  ;
  Frac[91]= -0.15742   ;
  Frac[92]= -0.152343  ;
  Frac[93]= -0.147001  ;
  Frac[94]= -0.142241  ;
  Frac[95]= -0.137595  ;
  Frac[96]= -0.133313  ;
  Frac[97]= -0.128435  ;
  Frac[98]= -0.124631  ;
  Frac[99]= -0.121106  ;
  Frac[100]= -0.116854 ;
  Frac[101]= -0.112579 ;
  Frac[102]= -0.109927 ;
  Frac[103]= -0.107044 ;
  Frac[104]= -0.103422 ;
  Frac[105]= -0.0997127;
  Frac[106]= -0.0967979;
  Frac[107]= -0.094159 ;
  Frac[108]= -0.0911629;
  Frac[109]= -0.0879251;
  Frac[110]= -0.0850502 ;
  Frac[111]= -0.0825226;
  Frac[112]= -0.079926 ;
  Frac[113]= -0.0767524;
  Frac[114]= -0.0745703;
  Frac[115]= -0.0722342;
  Frac[116]= -0.0694902;
  Frac[117]= -0.0661824;
  Frac[118]= -0.0644465;
  Frac[119]= -0.0624207;
  Frac[120]= -0.0603031;
  Frac[121]= -0.0573222;
  Frac[122]= -0.0552809;
  Frac[123]= -0.0533513;
  Frac[124]= -0.0513799;
  Frac[125]= -0.0488961;
  Frac[126]= -0.0473815;
  Frac[127]= -0.0456104;
  Frac[128]= -0.0440546;
  Frac[129]= -0.0414905;
  Frac[130]= -0.0400789;
  Frac[131]= -0.0383941;
  Frac[132]= -0.0363219;
  Frac[133]= -0.0342883;
  Frac[134]= -0.0328866;
  Frac[135]= -0.0312922;
  Frac[136]= -0.029854 ;
  Frac[137]= -0.0274867;
  Frac[138]= -0.0255809;
  Frac[139]= -0.0241595;
  Frac[140]= -0.0217186;
  Frac[141]= -0.0202501;
  Frac[142]= -0.0183742;
  Frac[143]= -0.0168845;
  Frac[144]= -0.0142908;
  Frac[145]= -0.0126114;
  Frac[146]= -0.0110446;
  Frac[147]= -0.0093668;
  Frac[148]= -0.0068487;
  Frac[149]= -0.0049751;
  Frac[150]= -0.0037547;
  Frac[151]= -0.0024154;
  Frac[152]= -0.0003822;

  if(DeltaT<153)  HeightFrac = -1*Frac[DeltaT];//The fractions of heights in this method are for a negative signal, in the rest of the reco positive signals are used, so signal height fraction needs to be multiplied by -1.
  else HeightFrac=0;
  return HeightFrac;

}
