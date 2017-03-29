// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------
#ifndef TTargetMCHit_H
#define TTargetMCHit_H

#include "TMCVHit.hh"

class TTargetMCHit : public TMCVHit
{

public:

  TTargetMCHit();
  ~TTargetMCHit();

public:

  void SetHitVal(Int_t v) { fHitVal = v; }
  Int_t GetHitVal() { return fHitVal; }

protected:
  Int_t fHitVal;

  ClassDef(TTargetMCHit,1);
};
#endif
