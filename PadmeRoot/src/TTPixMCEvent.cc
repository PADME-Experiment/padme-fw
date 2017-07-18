// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2017-03-15
//
// --------------------------------------------------------------

#include "TTPixMCEvent.hh"

#include "TTPixMCHit.hh"
#include "TTPixMCDigi.hh"

ClassImp(TTPixMCEvent)

TTPixMCEvent::TTPixMCEvent() : TMCVEvent(TTPixMCHit::Class(),TTPixMCDigi::Class())
{;}

TTPixMCEvent::~TTPixMCEvent()
{;}
