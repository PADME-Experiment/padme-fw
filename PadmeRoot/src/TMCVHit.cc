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
  cout << "ChannelId = " << fChannelId << endl;
  cout << "HitPosition = (" << fPosition.X() << "," << fPosition.Y() << "," << fPosition.Z() << ")" << endl;
  cout << "Energy = " << fEnergy << endl;
  cout << "Time = " << fTime << endl << endl;
}
