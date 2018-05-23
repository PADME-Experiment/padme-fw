#include "DigitizerChannelReco.hh"
#include <iostream>

void DigitizerChannelReco::PrintConfig(){
  std::cout << "Signal width: " << fSignalWidth << " samples" << std::endl;
  
  
}


void DigitizerChannelReco::Init(PadmeVRecoConfig *cfg){

  fSignalWidth    = cfg->GetParOrDefault("RECO","SignalWindow",1024);
  //std::cout << "Using signal window of " << fSignalWidth << " samples" << std::endl;
  fPedOffset      = cfg->GetParOrDefault("RECO","PedestalOffset",100);
  
  fPreSamples     = cfg->GetParOrDefault("RECO","SignalPreSamples",1024);
  fPostSamples    = cfg->GetParOrDefault("RECO","SignalPostSamples",1024);
  fPedMaxNSamples = cfg->GetParOrDefault("RECO","NumberOfSamplesPedestal",100);    
}


Short_t DigitizerChannelReco::CalcMaximum() {
  
  return fMax;
}


Double_t DigitizerChannelReco::CalcPedestal() {
  
  return fPed;
}





void DigitizerChannelReco::Reconstruct(std::vector<TRecoVHit *> &hitArray){
  
  ;
}



