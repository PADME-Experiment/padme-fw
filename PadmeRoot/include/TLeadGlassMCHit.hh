// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2023-09-21
//
// --------------------------------------------------------------
#ifndef TLeadGlassMCHit_H
#define TLeadGlassMCHit_H

#include "TMCVHit.hh"

class TLeadGlassMCHit : public TMCVHit
{

public:

  TLeadGlassMCHit();
  ~TLeadGlassMCHit();

  ClassDef(TLeadGlassMCHit,1);
};
#endif
