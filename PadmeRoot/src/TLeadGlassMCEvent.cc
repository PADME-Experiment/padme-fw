// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2023-09-21
//
// --------------------------------------------------------------

#include "TLeadGlassMCEvent.hh"
#include "TLeadGlassMCHit.hh"
#include "TLeadGlassMCDigi.hh"

ClassImp(TLeadGlassMCEvent)

TLeadGlassMCEvent::TLeadGlassMCEvent() : TMCVEvent(TLeadGlassMCHit::Class(),TLeadGlassMCDigi::Class())
{;}

TLeadGlassMCEvent::~TLeadGlassMCEvent()
{;}
