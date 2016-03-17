// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-17
//
// --------------------------------------------------------------

#include "TSACMCEvent.hh"

#include "TSACMCHit.hh"

ClassImp(TSACMCEvent)

TSACMCEvent::TSACMCEvent() : TMCVEvent(TSACMCHit::Class())
{;}

TSACMCEvent::~TSACMCEvent()
{;}
