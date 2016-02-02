// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------

#include "TECalMCEvent.hh"

#include "TECalMCHit.hh"

ClassImp(TECalMCEvent)

TECalMCEvent::TECalMCEvent() : TMCVEvent(TECalMCHit::Class())
{;}

TECalMCEvent::~TECalMCEvent()
{;}
