// Written By M. Raggi 6/12/2019

#include "DigitizerChannelECal.hh"
#include "TMath.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TTree.h"

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>

void DigitizerChannelECal::PrintConfig(){
  std::cout << "Hi I'm the ECal: " << std::endl;
  std::cout << "Signal width: "   << fSignalWidth << " samples" << std::endl;
  std::cout << "fUseAbsSignals: " << fUseAbsSignals << std::endl;  
}


void DigitizerChannelECal::Init(PadmeVRecoConfig *cfg){

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


  fMultihit       = cfg->GetParOrDefault("RECO","Multihit",0);
  fUseAbsSignals  = cfg->GetParOrDefault("RECO","UseAbsSignals",0);
  
  
  std::cout << cfg->GetName() << "*******************************" <<  std::endl;
  PrintConfig();
}


void DigitizerChannelECal::SetAbsSignals(){
  for(UShort_t i = 0;i<fNSamples;i++){
    if (fSamples[i] < 2048) {
      fSamples[i] = 4096 - fSamples[i];
    }
  }
}
 
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


Double_t DigitizerChannelECal::CalcPedestal() {
  fPed = 0.;
  fNPedSamples = 0;
  //Call this only after the maximum is set
  if(fIMax - fPedOffset < 0) {
    //What to do if not enough samples for pedestal calculation... 
    //Use the first few samples
    fPed = fSamples[0];
    fNPedSamples = 1;
    return fPed;
  } 

  for(Short_t i = fIMax - fPedOffset; 
      i >= 0 &&  fNPedSamples <  fPedMaxNSamples; 
      --i,++fNPedSamples) {
    //    std::cout << i << "  " << fNPedSamples << std::endl;
    fPed+= fSamples[i];
  }
  

  fPed /= fNPedSamples;

  return fPed;
}


Double_t DigitizerChannelECal::ZSupHit(Float_t Thr, UShort_t NAvg) {
  Double_t rms1000  = TMath::RMS(NAvg,&fSamples[0]);
  Double_t ZSupHit=-1;
  //  if(rms1000>Thr){
  if(rms1000>4.){
    ZSupHit=0;
  }else{
    ZSupHit=1;
  }
  //std::cout<<"compute zero supp "<<rms1000<<" Zsup "<<ZSupHit<<std::endl;
  return ZSupHit;
}

Double_t DigitizerChannelECal::CalcCharge(UShort_t iMax) {
  
  Short_t begin = iMax-fPreSamples > 0? iMax-fPreSamples:0;
  Short_t end = iMax+fPostSamples < fNSamples? iMax+fPostSamples:fNSamples;
 
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
  Double_t IsZeroSup = ZSupHit(5.,1000.);
  CalcCharge(fIMax);
  if (fCharge < .001) return;
  CalcTime(fIMax);

  TRecoVHit *Hit = new TRecoVHit();
  Hit->SetTime(fTime);
  // M. Raggi going to charge
  Double_t fEnergy= fCharge*1000./15; //going from nC to MeV using 15pC/MeV
  //  Hit->SetEnergy(fCharge);
  Hit->SetEnergy(fEnergy);
  hitArray.push_back(Hit);

  // std::cout << "Hit charge:  " << fCharge << "  Time: " << fTime << std::endl;
  
}

void DigitizerChannelECal::ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray){
  
  
}


void DigitizerChannelECal::Reconstruct(std::vector<TRecoVHit *> &hitArray){
  if(fUseAbsSignals) {
    SetAbsSignals();
  }

  CalcMaximum();
  CalcPedestal();
  if(fPed - fMax < fMinAmplitude ) return; //altro taglio da capire fatto in che unita'? conteggi?

  if(fMultihit) {
    ReconstructMultiHit(hitArray);
  } else {
    ReconstructSingleHit(hitArray);
  }
  // std::cout << "Maximum: " << fMax << "  at position: "<< fIMax << std::endl;
  // std::cout << "Pedestal: " << fPed << "  from: " << fNPedSamples << " samples"<< std::endl;
}



