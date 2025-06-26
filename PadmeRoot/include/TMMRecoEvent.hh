#ifndef TMMRecoEvent_H
#define TMMRecoEvent_H

#include "TRecoVObject.hh"

class TMMRecoEvent : public TRecoVObject {

public:

  TMMRecoEvent(); 
  ~TMMRecoEvent();

private:
  
  // add here possible global reco quantities

  ClassDef(TMMRecoEvent,1);
};
#endif
