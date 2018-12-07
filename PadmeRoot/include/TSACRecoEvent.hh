#ifndef TSACRecoEvent_H
#define TSACRecoEvent_H

#include "TRecoVObject.hh"
#include "TRecoVClusCollection.hh"

class TSACRecoEvent : public TRecoVObject {

public:

  TSACRecoEvent(); 
  ~TSACRecoEvent();

private:
  

  ClassDef(TSACRecoEvent,1);
};
#endif
