// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------

#include "TPVetoMCEvent.hh"

#include "TPVetoMCHit.hh"

ClassImp(TPVetoMCEvent)

TPVetoMCEvent::TPVetoMCEvent() : TMCVEvent(TPVetoMCHit::Class())
{;}

TPVetoMCEvent::~TPVetoMCEvent()
{;}
