//Class to define Hits in a Cluster
#include "EVetoClusterHits.hh"

#include <iostream>
#include "cstdlib"
#include "math.h"

EVetoClusterHits::EVetoClusterHits()
{

  // Initialize to invalid channel
  fChannelId = -1;
  fEnergy    = 0.;
  fTime      = 1.E9;
}

void EVetoClusterHits::Clear(){
  // Initialize to invalid channel
  fChannelId = -1;
  fEnergy    = 0.;
  fTime      = 1.E9;
  fPosition  = TVector3(-1.E9,-1.E9,-1.E9);
}

EVetoClusterHits::EVetoClusterHits(const EVetoClusterHits& h){
  // Initialize to invalid channel
  fChannelId = h.fChannelId;
  fPosition  = h.fPosition;
  fEnergy    = h.fEnergy;
  fTime      = h.fTime;                                                                             }                                       
EVetoClusterHits::~EVetoClusterHits()
{;}

void EVetoClusterHits::Print()
{
  std::cout << "Hit - ChannelId = " << fChannelId
            << " HitPosition = (" << fPosition.X() << "," << fPosition.Y() << "," << fPosition.Z() << ")"
            << " Energy = " << fEnergy
            << " Time = " << fTime << std::endl;
}
