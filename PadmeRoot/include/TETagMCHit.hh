// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2022-06-08
//
// --------------------------------------------------------------
#ifndef TETagMCHit_H
#define TETagMCHit_H

#include "TMCVHit.hh"

class TETagMCHit : public TMCVHit
{

public:

  TETagMCHit();
  ~TETagMCHit();

  ClassDef(TETagMCHit,1);
};
#endif
