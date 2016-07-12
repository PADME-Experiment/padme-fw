// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------

#include "TMCVHit.hh"

#include "Riostream.h"

ClassImp(TMCVHit)

TMCVHit::TMCVHit()
{
  // Initialize to invalid channel
  fChannelId = -1;
  fEnergy    = 0.;
  fTime      = 1.E9;
}

void TMCVHit::Print(Option_t *) const {
  std::cout << "ChannelId = " << fChannelId << std::endl;
  std::cout << "HitPosition = (" << fPosition.X() << "," << fPosition.Y() << "," << fPosition.Z() << ")" << std::endl;
  std::cout << "Energy = " << fEnergy << std::endl;
  std::cout << "Time = " << fTime << std::endl << std::endl;
}
