// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------

#include "THEPVetoMCEvent.hh"

#include "THEPVetoMCHit.hh"

ClassImp(THEPVetoMCEvent)

THEPVetoMCEvent::THEPVetoMCEvent() : TMCVEvent(THEPVetoMCHit::Class())
{;}

THEPVetoMCEvent::~THEPVetoMCEvent()
{;}
