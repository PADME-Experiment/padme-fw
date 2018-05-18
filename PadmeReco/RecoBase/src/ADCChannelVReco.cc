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
      fGain=8;//11.6; //E5
      fFireThreshold=12;//7;
      fSignalRangeBegin  =100;
      fSignalRangeEnd    =800;
      fNoiseRangeBegin=20;
      fNoiseRangeEnd=80;
      fPedestalRangeBegin=20;
      fPedestalRangeEnd  =80;
      fUseMultipeak = false;
  ;
}

void ADCChannelVReco::ReconstructSinglePeak(){
  CalcPedestal();
  SetT0(0);
  CalcTimeCharge();
  
  

  TRecoVHit *Hit = new TRecoVHit();
  Hit->SetTime(GetTime2080LeadZeroCross());
  //  Hit->SetPosition();
  Hit->SetEnergy(GetCharge());
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

  



}
