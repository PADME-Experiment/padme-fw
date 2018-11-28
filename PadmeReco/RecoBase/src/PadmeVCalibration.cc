#include "PadmeVCalibration.hh"
#include <iostream>
#include <string>

PadmeVCalibration::PadmeVCalibration(){  
  // fTimeCalib.clear();
  // fEnergyCalib.clear();  
  
  fTimeCalib = 0;
  fEnergyCalib = 0;

}


void PadmeVCalibration::Init(PadmeVRecoConfig *cfg){
  fNChannels = cfg->GetParOrDefault("DETECTOR","NCHANNELS",1.);
  
  //prepare the arrays to store
  fTimeCalib = new double[fNChannels];
  fEnergyCalib = new double[fNChannels];


  for(int i = 0;i< fNChannels;++i) {
    fTimeCalib[i] = cfg->GetParOrDefault("TimeCalibration",std::to_string(i),0.);
    fEnergyCalib[i] = cfg->GetParOrDefault("EnergyCalibration",""+i,1.);
  }
}

PadmeVCalibration::~PadmeVCalibration(){
  delete [] fTimeCalib;
  delete [] fEnergyCalib;
}

void PadmeVCalibration::PerformCalibration(std::vector<TRecoVHit *> &Hits) {
  for(unsigned int iHit = 0;iHit < Hits.size();iHit++){
    int ich = Hits[iHit]->GetChannelId();
    //    std::cout << "Applying time offset for channel " << ich << "  " << GetChannelT0(ich) << std::endl;
    Hits[iHit]->SetTime( Hits[iHit]->GetTime() - GetChannelT0(ich) );
    Hits[iHit]->SetEnergy(Hits[iHit]->GetEnergy()*GetChannelEnergyResponse(ich));
  }
}
