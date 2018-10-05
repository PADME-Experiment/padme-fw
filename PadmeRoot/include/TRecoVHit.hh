// --------------------------------------------------------------
#ifndef TRecoVHit_H
#define TRecoVHit_H

#include "TObject.h"
#include "TVector3.h"
#include "TMCVHit.hh"

class TRecoVHit : public TMCVHit
{

public:

  TRecoVHit();
  virtual ~TRecoVHit(){};


public:
 
  ClassDef(TRecoVHit,1);
};
#endif
