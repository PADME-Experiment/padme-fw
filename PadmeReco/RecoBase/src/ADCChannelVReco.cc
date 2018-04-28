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

  ;
}

void ADCChannelVReco::Reconstruct(){
  CalcPedestal();
  SetT0(0);
  CalcTimeCharge();
  //  std::cout << "Pedestal is " << GetPedestal() << std::endl;
  //  std::cout << "Time is " << GetTime2080LeadZeroCross() << std::endl;
}
