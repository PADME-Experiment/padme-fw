#include "DigitizerChannelReco.hh"
#include <iostream>

void DigitizerChannelReco::PrintConfig(){
  std::cout << "Signal width: " << fSignalWidth << " samples" << std::endl;
  
  
}


void DigitizerChannelReco::Init(PadmeVRecoConfig *cfg){

  fTimeBin        = cfg->GetParOrDefault("ADC","TimeBin",1.);
  fVoltageBin     = cfg->GetParOrDefault("ADC","VoltageBin",0.001);
  fImpedance      = cfg->GetParOrDefault("ADC","InputImpedance",50.);

  fSignalWidth    = cfg->GetParOrDefault("RECO","SignalWindow",1024);
  //std::cout << "Using signal window of " << fSignalWidth << " samples" << std::endl;
  fPedOffset      = cfg->GetParOrDefault("RECO","PedestalOffset",100);
  
  fPreSamples     = cfg->GetParOrDefault("RECO","SignalPreSamples",1024);
  fPostSamples    = cfg->GetParOrDefault("RECO","SignalPostSamples",1024);
  fPedMaxNSamples = cfg->GetParOrDefault("RECO","NumberOfSamplesPedestal",100);  

  fMinAmplitude    = cfg->GetParOrDefault("RECO","MinAmplitude",10);
  fAmpThresholdLow = cfg->GetParOrDefault("RECO","AmplThrLow",10.);
  fAmpThresholdHigh= cfg->GetParOrDefault("RECO","AmplThrHigh",20.);


  fMultihit       = cfg->GetParOrDefault("RECO","",0);


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


Double_t DigitizerChannelReco::CalcPedestal() {
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



Double_t DigitizerChannelReco::CalcCharge(UShort_t iMax) {
  
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

  fCharge = ((1.*end-1.*begin) * fPed) - fCharge;
  fCharge *= (fVoltageBin*fTimeBin/fImpedance);
  return fCharge;
}

Double_t DigitizerChannelReco::CalcTime(UShort_t iMax) {
  fTime = 0.;
  //currently looking only at the signal rising edge
  
  float t1;
  float t2;

  float t3;
  float t4;

  
  float val1;
  float val2;

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
      t3 = i + (fAmpThresholdLow - val1)/(val2 - val1);
      t3_ok = 1;
    }
    if( t3_ok = 1 && t4_ok == 0 && val1 <= fAmpThresholdHigh && val2 > fAmpThresholdHigh) {
      t4 = i + (fAmpThresholdHigh - val1)/(val2 - val1);
      t4_ok = 1;
    }



  }
  
  //  fTime = (t1 + t2)/2.;
  fTime = t3 - (t4-t3)*fAmpThresholdLow/(fAmpThresholdHigh - fAmpThresholdLow);

  

  return fTime;
}


void DigitizerChannelReco::ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray){
  CalcCharge(fIMax);
  CalcTime(fIMax);

  TRecoVHit *Hit = new TRecoVHit();
  Hit->SetTime(fTime);
  Hit->SetEnergy(fCharge);
  hitArray.push_back(Hit);

  
  // std::cout << "Hit charge:  " << fCharge << "  Time: " << fTime << std::endl;
  
}

void DigitizerChannelReco::ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray){
  
  
}


void DigitizerChannelReco::Reconstruct(std::vector<TRecoVHit *> &hitArray){
  
  CalcMaximum();
  CalcPedestal();
  if(fPed - fMax < fMinAmplitude ) return;

  if(fMultihit) {
    ReconstructMultiHit(hitArray);
  } else {
    ReconstructSingleHit(hitArray);
  }
  // std::cout << "Maximum: " << fMax << "  at position: "<< fIMax << std::endl;
  // std::cout << "Pedestal: " << fPed << "  from: " << fNPedSamples << " samples"<< std::endl;
}



