#include "ADCChannelVReco.hh"
#include <iostream>


ADCChannelVReco::ADCChannelVReco(){
  ;
}


ADCChannelVReco::ADCChannelVReco(PadmeVRecoConfig *cfg){
  Init(cfg);
}

void ADCChannelVReco::Init(PadmeVRecoConfig *cfg){
  fType="SiPM";
  fGain=1;//11.6; //E5
  fFireThreshold=12;//7;
  fSignalRangeBegin  =200;
  fSignalRangeEnd    =800;
  fNoiseRangeBegin=20;
  fNoiseRangeEnd=80;
  fPedestalRangeBegin=20;
  fPedestalRangeEnd  =80;
  fUseMultipeak = false;
  fThreshold = 0.;
  fConfig = cfg;
}

Bool_t ADCChannelVReco::CheckSingleHit(){
  if(GetTime2080LeadZeroCross() < -100. || GetTime2080LeadZeroCross() > 1000.) 
    return false;
  
  if(GetValMax() > -0.3) 
    return false;
  return true;
}

void ADCChannelVReco::ReconstructSinglePeak(){
  CalcPedestal();
  SetT0(0);
  CalcTimeCharge();

  if (!CheckSingleHit()) return; //Hit is not actually a hit, but more like noise


  TRecoVHit *Hit = new TRecoVHit();
  Hit->SetTime(GetTime2080LeadZeroCross());
  //  Hit->SetPosition();
  // std::cout << "Pedestal: " << fPedestal<< "  Hit time:  " << GetTime2080LeadZeroCross() 
  // 	    << " Energy: "  << GetCharge() << std::endl;
  
  Hit->SetEnergy(GetValMax());
  
  fHits.push_back(Hit);

  
}

void ADCChannelVReco::Reconstruct(){
  fHits.clear();
  if(fUseMultipeak) {
    ReconstructMultiPeak();
  } else {
    ReconstructSinglePeak();
  }
  //  std::cout << "Pedestal is " << GetPedestal() << std::endl;
  //  std::cout << "Time is " << GetTime2080LeadZeroCross() << std::endl;
}

void ADCChannelVReco::Reconstruct(std::vector<TRecoVHit *> &hitArray){
  fHits.clear();
  
  Reconstruct();
  //  std::cout << "Pedestal is " << GetPedestal() << std::endl;
  //  std::cout << "Time is " << GetTime2080LeadZeroCross() << std::endl;

  //  copy the hits (if any) into the hit-array.
  //the actual free of memory is done at the place where the hits are stored...
  // not nice and not very imune to memory leaks, so be careful, a better implementation necessary
  
  for(unsigned iHit = 0;iHit < fHits.size();iHit++){
    hitArray.push_back(fHits[iHit]);
  }
  fHits.clear();

}
