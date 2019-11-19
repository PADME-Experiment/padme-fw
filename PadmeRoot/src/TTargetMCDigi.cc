// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-21
//
// --------------------------------------------------------------

#include "TTargetMCDigi.hh"
#include "Riostream.h"

ClassImp(TTargetMCDigi)

TTargetMCDigi::TTargetMCDigi() : TMCVDigi()
{  
  fDNumber=0.;
  fCharge=0.;
}

TTargetMCDigi::~TTargetMCDigi()
{;}

void TTargetMCDigi::Print(Option_t *) const {

  std::cout << "Target Digi - ChannelId = " << fChannelId
	    << " Energy = " << fEnergy
	    << " Time = " << fTime
	    << " Charge = " << fCharge
	    << " Multi = " << fDNumber
	    << std::endl;

}
