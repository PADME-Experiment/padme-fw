//Class to define Hits in a Cluster
#include "VetoClusterHits.hh"

#include <iostream>
#include "cstdlib"
#include "math.h"

VetoClusterHits::VetoClusterHits()
{

  // Initialize to invalid channel
  fChannelId = -1;
  fEnergy    = 0.;
  fTime      = 1.E9;
}

void VetoClusterHits::Clear(){
  // Initialize to invalid channel
  fChannelId = -1;
  fEnergy    = 0.;
  fTime      = 1.E9;
  fPosition  = TVector3(-1.E9,-1.E9,-1.E9);
}

VetoClusterHits::VetoClusterHits(const VetoClusterHits& h){
  // Initialize to invalid channel
  fChannelId = h.fChannelId;
  fPosition  = h.fPosition;
  fEnergy    = h.fEnergy;
  fTime      = h.fTime;   
  fHitIndex  = h.fHitIndex;
}

VetoClusterHits::~VetoClusterHits()
{;}

void VetoClusterHits::Print()
{
  std::cout << "Hit - ChannelId = " << fChannelId
            << " HitPosition = (" << fPosition.X() << "," << fPosition.Y() << "," << fPosition.Z() << ")"
            << " Energy = " << fEnergy
            << " Time = " << fTime << std::endl;
}
