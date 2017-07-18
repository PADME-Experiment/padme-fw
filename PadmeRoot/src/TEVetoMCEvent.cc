// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------

#include "TEVetoMCEvent.hh"

#include "TEVetoMCHit.hh"
#include "TEVetoMCDigi.hh"

ClassImp(TEVetoMCEvent)

TEVetoMCEvent::TEVetoMCEvent() : TMCVEvent(TEVetoMCHit::Class(),TEVetoMCDigi::Class())
{;}

TEVetoMCEvent::~TEVetoMCEvent()
{;}
