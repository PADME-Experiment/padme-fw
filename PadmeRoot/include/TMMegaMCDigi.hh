// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (davide.quaranta@roma1.infn.it) 2024-06-04
//
// --------------------------------------------------------------
#ifndef TMMegaMCDigi_H
#define TMMegaMCDigi_H

#include "TMCVDigi.hh"

class TMMegaMCDigi : public TMCVDigi
{

public:

  TMMegaMCDigi();
  ~TMMegaMCDigi();
  void SetNHitxCh(Int_t value) { fNHitxCh = value; };
  Int_t GetNHitxCh() const { return fNHitxCh; };

private:
  Int_t fNHitxCh=0;
  ClassDef(TMMegaMCDigi,1);
};
#endif