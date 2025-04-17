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

  void SetCoGX(Double_t v) { fCoGX = v; }
  void SetCoGY(Double_t v) { fCoGY = v; }

  Double_t GetCoGX() { return fCoGX; }
  Double_t GetCoGY() { return fCoGY; }

  void SetTimeSpread(Double_t t) { fTimeSpread = t; }
  Double_t GetTimeSpread() { return fTimeSpread; }

private:

  Double_t fCoGX;
  Double_t fCoGY;

  Double_t fTimeSpread;

  ClassDef(TLeadGlassMCDigi,1);
};
#endif
