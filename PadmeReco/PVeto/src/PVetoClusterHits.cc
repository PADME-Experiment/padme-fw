//Class to define Hits in a Cluster
#include "PVetoClusterHits.hh"

#include <iostream>
#include "cstdlib"
#include "math.h"

PVetoClusterHits::PVetoClusterHits()
{

  // Initialize to invalid channel
  fChannelId = -1;
  fEnergy    = 0.;
  fTime      = 1.E9;
}

void PVetoClusterHits::Clear(){
  // Initialize to invalid channel
  fChannelId = -1;
  fEnergy    = 0.;
  fTime      = 1.E9;
  fPosition  = TVector3(-1.E9,-1.E9,-1.E9);
}

PVetoClusterHits::PVetoClusterHits(const PVetoClusterHits& h){
  // Initialize to invalid channel
  fChannelId = h.fChannelId;
  fPosition  = h.fPosition;
  fEnergy    = h.fEnergy;
  fTime      = h.fTime;                                                                             }                                       
PVetoClusterHits::~PVetoClusterHits()
{;}

void PVetoClusterHits::Print()
{
  std::cout << "Hit - ChannelId = " << fChannelId
            << " HitPosition = (" << fPosition.X() << "," << fPosition.Y() << "," << fPosition.Z() << ")"
            << " Energy = " << fEnergy
            << " Time = " << fTime << std::endl;
}
