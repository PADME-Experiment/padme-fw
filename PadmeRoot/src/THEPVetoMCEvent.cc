// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------

#include "THEPVetoMCEvent.hh"

#include "THEPVetoMCHit.hh"
#include "THEPVetoMCDigi.hh"

ClassImp(THEPVetoMCEvent)

THEPVetoMCEvent::THEPVetoMCEvent() : TMCVEvent(THEPVetoMCHit::Class(),THEPVetoMCDigi::Class())
{;}

THEPVetoMCEvent::~THEPVetoMCEvent()
{;}
