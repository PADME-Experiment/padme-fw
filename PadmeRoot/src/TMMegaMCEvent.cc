// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (davide.quaranta@roma1.infn.it) 2024-06-04
//
// --------------------------------------------------------------

#include "TMMegaMCEvent.hh"
#include "TMMegaMCHit.hh"
#include "TMMegaMCDigi.hh"

ClassImp(TMMegaMCEvent)

TMMegaMCEvent::TMMegaMCEvent() : TMCVEvent(TMMegaMCHit::Class(),TMMegaMCDigi::Class())
{;}

TMMegaMCEvent::~TMMegaMCEvent()
{;}