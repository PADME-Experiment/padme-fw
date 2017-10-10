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
  std::cout << "Hit - ChannelId = " << fChannelId
	    << " HitPosition = (" << fPosition.X() << "," << fPosition.Y() << "," << fPosition.Z() << ")"
	    << " Energy = " << fEnergy
	    << " Time = " << fTime << std::endl;
}
