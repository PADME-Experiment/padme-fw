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

  ClassDef(TTargetMCHit,1);
};
#endif
