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
  fTime      = 0.;
}

void TMCVHit::Print(Option_t *) const {
  cout << "ChannelId = " << fChannelId << endl
       << "HitPosition = (" << fPosition.X() << "," << fPosition.Y() << "," << fPosition.Z() << ")" << endl
       << "Energy = " << fEnergy << endl
       << "Time = " << fTime << endl << endl;
}
