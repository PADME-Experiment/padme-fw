// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2017-03-15
//
// --------------------------------------------------------------
#ifndef TTPixMCHit_H
#define TTPixMCHit_H

#include "TMCVHit.hh"

class TTPixMCHit : public TMCVHit
{

public:

  TTPixMCHit();
  ~TTPixMCHit();

  ClassDef(TTPixMCHit,1);
};
#endif
