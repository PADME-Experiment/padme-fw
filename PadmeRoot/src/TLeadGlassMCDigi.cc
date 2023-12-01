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
  fWaveform = new TH1D("lgwf","LeadGlass waveform",1024,-0.5,1023.5);
  fCoGX = 0.;
  fCoGY = 0.;
}

TLeadGlassMCDigi::~TLeadGlassMCDigi()
{
  delete fWaveform;
}
