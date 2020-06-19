#include "PadmeVCalibration.hh"
#include "RecoVChannelID.hh"
#include "TRecoVHit.hh"
#include "TRecoVCluster.hh"

#include <iostream>
#include <string>
#include <map>
#include <vector>


PadmeVCalibration::PadmeVCalibration(){  

  fNChannels = 0;
  fTimeCalibMap = 0;
  fTimeCalibMapMC = 0;
  fEnergyCalibMap = 0;
  fCommonT0 = 0;
  fCommonT0MC = 0;
  fCommonEnergyScale = 1;
  

}
void PadmeVCalibration::InitCommonT0(PadmeVRecoConfig *cfg)
{
  fCommonT0          = (double)cfg->GetParOrDefault("TimeCalibration",      "Common_t0",          0.);
  fCommonT0MC        = (double)cfg->GetParOrDefault("TimeCalibration",      "Common_t0MC",        0.);
}
void PadmeVCalibration::InitCommonEnergyScale(PadmeVRecoConfig *cfg)
{
  fCommonEnergyScale = (double)cfg->GetParOrDefault("EnergyCalibration",    "Common_EnergyScale", 1.);
}

void PadmeVCalibration::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr ){
  fNChannels = cfg->GetParOrDefault("DETECTOR","NCHANNELS",1.);
  
  //prepare the maps (key=chid) to store constants
  fTimeCalibMap   = new std::map<int, double>;
  fEnergyCalibMap = new std::map<int, double>;
  fTimeCalibMapMC   = new std::map<int, double>;

  InitCommonT0(cfg);
  InitCommonEnergyScale(cfg);

  
  // calibration maps 
  std::vector<int> chIds = chIdMgr->GetChannelIDVector();
  if (chIds.size() != fNChannels) std::cout<<" PadmeVCalibration::Init .... WARNING - chIds.size() = "<<chIds.size()<<" != fNChannels = "<<fNChannels<<std::endl;
  if (chIds.size() == 0)          std::cout<<" PadmeVCalibration::Init .... WARNING - chIds.size() = 0;  fNChannels = "                  <<fNChannels<<std::endl;
  for (unsigned int j=0; j<chIds.size();++j)
    {
      //std::cout<<" init calib consts .... j "<<j<<" ID="<<chIds[j]<<" .... "<<std::endl;
      double t0 = (double)cfg->GetParOrDefault("TimeCalibration",      "t0_"+std::to_string(chIds[j]),0.);
      double es = (double)cfg->GetParOrDefault("EnergyCalibration",    "ES_"+std::to_string(chIds[j]),1.);
      double t0MC = (double)cfg->GetParOrDefault("TimeCalibration",      "t0MC_"+std::to_string(chIds[j]),0.);
      //std::cout<<" init calibration consts .... j "<<j<<" ID="<<chIds[j]<<" .... t0, es = "<<t0<<" "<<es<<std::endl;
      if (  fTimeCalibMap->find(chIds[j])==fTimeCalibMap->end()  )   fTimeCalibMap->insert ( std::pair<int,double>(chIds[j], t0) ); 
      if (fEnergyCalibMap->find(chIds[j])==fEnergyCalibMap->end()) fEnergyCalibMap->insert ( std::pair<int,double>(chIds[j], es) ); 
      if (  fTimeCalibMapMC->find(chIds[j])==fTimeCalibMapMC->end()  )   fTimeCalibMapMC->insert ( std::pair<int,double>(chIds[j], t0MC) ); 
      //std::cout<<" init calibration consts  .... map size = "<<fTimeCalibMap->size()<<std::endl;
    }
  std::cout<<"Energy calibration constants initialized for "<<fEnergyCalibMap->size()<<" values of channelIDs"<<std::endl;
  std::cout<<"Time   calibration constants initialized for "<<fTimeCalibMap->size()<<" values of channelIDs"<<std::endl;
  std::cout<<"Time   MC calibration constants initialized for "<<fTimeCalibMapMC->size()<<" values of channelIDs"<<std::endl;
}  
void PadmeVCalibration::PerformCalibration(std::vector<TRecoVHit *> &Hits)
{
  for(unsigned int iHit = 0; iHit < Hits.size(); ++iHit){
    int ich = Hits[iHit]->GetChannelId();
    //std::cout << "Applying DATA time offset for channel " << ich << "  " << GetChannelT0(ich) << std::endl;

    Hits[iHit]->SetTime( Hits[iHit]->GetTime() - GetChannelT0(ich) );
    Hits[iHit]->SetEnergy(Hits[iHit]->GetEnergy()*GetChannelEnergyResponse(ich));
  }
}

void PadmeVCalibration::PerformMCCalibration(std::vector<TRecoVHit *> &Hits)
{
  for(unsigned int iHit = 0; iHit < Hits.size(); ++iHit){
    int ich = Hits[iHit]->GetChannelId();
    //std::cout << "Applying MC time offset for channel " << ich << "  " << GetChannelT0MC(ich) << std::endl;
    Hits[iHit]->SetTime( Hits[iHit]->GetTime() - GetChannelT0MC(ich) );
  }
}
PadmeVCalibration::~PadmeVCalibration(){
  if (fTimeCalibMap)   delete fTimeCalibMap;
  if (fEnergyCalibMap) delete fEnergyCalibMap;
  if (fTimeCalibMapMC) delete fTimeCalibMapMC;
}

