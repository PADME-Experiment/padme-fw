// --------------------------------------------------------------
// History:
//
// Created by Elisa Di Meco (elisa.dimeco@lnf.infn.it) 2026-05-27
//
// --------------------------------------------------------------

#include "TTMMMCEvent.hh"

#include "TTMMMCHit.hh"
#include "TTMMMCDigi.hh"

ClassImp(TTMMMCEvent)

TTMMMCEvent::TTMMMCEvent() : TMCVEvent(TTMMMCHit::Class(),TTMMMCDigi::Class())
{;}

TTMMMCEvent::~TTMMMCEvent()
{;}
