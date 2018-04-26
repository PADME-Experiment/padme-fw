#ifndef TRecoVObject_H
#define TRecoVObject_H

#include "TObject.h"
#include "TVector3.h"

class TRecoVObject : public TObject
{

public:

  TRecoVObject();
  virtual ~TRecoVObject(){};

  void Print(Option_t* option="") const;

public:
 

  ClassDef(TRecoVObject,1);
};
#endif
