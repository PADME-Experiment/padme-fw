// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-21
//
// --------------------------------------------------------------

#include "TECalMCDigi.hh"

#include "Riostream.h"

ClassImp(TECalMCDigi)

TECalMCDigi::TECalMCDigi() : TMCVDigi()
{
  fSignal = 0.;
}

TECalMCDigi::~TECalMCDigi()
{;}

void TECalMCDigi::Print(Option_t *) const {

  std::cout << "ECal Digi - ChannelId = " << fChannelId
	    << " Energy = " << fEnergy
	    << " Time = " << fTime
	    << " Signal = " << fSignal
	    << std::endl;

}
