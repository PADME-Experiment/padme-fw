// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2023-09-21
//
// --------------------------------------------------------------
#ifndef TLeadGlassMCDigi_H
#define TLeadGlassMCDigi_H

#include "TH1D.h"

#include "TMCVDigi.hh"

class TLeadGlassMCDigi : public TMCVDigi
{

public:

  TLeadGlassMCDigi();
  ~TLeadGlassMCDigi();

  TH1D* GetWaveform() { return fWaveform; }

  void SetCoGX(Double_t v) { fCoGX = v; }
  void SetCoGY(Double_t v) { fCoGY = v; }

  Double_t GetCoGX() { return fCoGX; }
  Double_t GetCoGY() { return fCoGY; }

private:

  TH1D* fWaveform;

  Double_t fCoGX;
  Double_t fCoGY;

  ClassDef(TLeadGlassMCDigi,1);
};
#endif
