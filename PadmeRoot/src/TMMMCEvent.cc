// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------

#include "TMMMCEvent.hh"

#include "TMMMCHit.hh"
#include "TMMMCDigi.hh"

ClassImp(TMMMCEvent)

TMMMCEvent::TMMMCEvent() : TMCVEvent(TMMMCHit::Class(),TMMMCDigi::Class())
{;}

TMMMCEvent::~TMMMCEvent()
{;}
