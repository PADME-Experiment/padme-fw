// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------

#include "TETagMCEvent.hh"
#include "TETagMCHit.hh"
#include "TETagMCDigi.hh"

ClassImp(TETagMCEvent)

TETagMCEvent::TETagMCEvent() : TMCVEvent(TETagMCHit::Class(),TETagMCDigi::Class())
{;}

TETagMCEvent::~TETagMCEvent()
{;}
