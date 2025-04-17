// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2023-09-21
//
// --------------------------------------------------------------

#include "TLeadGlassMCDigi.hh"

ClassImp(TLeadGlassMCDigi)

TLeadGlassMCDigi::TLeadGlassMCDigi() : TMCVDigi()
{
  fCoGX = 0.;
  fCoGY = 0.;
  fTimeSpread = 0.;
}

TLeadGlassMCDigi::~TLeadGlassMCDigi()
{;}
