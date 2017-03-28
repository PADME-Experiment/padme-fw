// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-21
//
// --------------------------------------------------------------

#include "TMCVDigi.hh"

#include "Riostream.h"

ClassImp(TMCVDigi)

TMCVDigi::TMCVDigi()
{
  // Initialize to invalid channel
  fChannelId = -1;
  fEnergy    = 0.;
  fTime      = 1.E9;
}

void TMCVDigi::Print(Option_t *) const {
  std::cout << "Digi - ChannelId = " << fChannelId
	    << " Energy = " << fEnergy
	    << " Time = " << fTime << std::endl;
}
