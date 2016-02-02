// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------
#ifndef TECalMCHit_H
#define TECalMCHit_H

#include "TMCVHit.hh"

class TECalMCHit : public TMCVHit
{

public:

  TECalMCHit();
  ~TECalMCHit();

  ClassDef(TECalMCHit,1);
};
#endif
