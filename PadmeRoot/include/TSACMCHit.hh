// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-17
//
// --------------------------------------------------------------
#ifndef TSACMCHit_H
#define TSACMCHit_H

#include "TMCVHit.hh"

//#include "TH1D.h"

#define TSACMCHIT_N_BINS 400

class TSACMCHit : public TMCVHit
{

public:

  TSACMCHit();
  ~TSACMCHit();

  ClassDef(TSACMCHit,1);
};
#endif
