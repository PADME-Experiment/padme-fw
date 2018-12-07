#ifndef TTargetRecoEvent_H
#define TTargetRecoEvent_H

#include "TRecoVObject.hh"
#include "TTargetRecoBeam.hh"

class TTargetRecoEvent : public TRecoVObject {

public:

  TTargetRecoEvent(); 
  ~TTargetRecoEvent();
  // TTargetRecoBeam* getTargetRecoBeam(){return fTargetRecoBeam;}
  // void setTargetRecoBeam(TTargetRecoBeam* iBeam){fTargetRecoBeam = iBeam;};


private:
  
  //  TTargetRecoBeam* fTargetRecoBeam;

  ClassDef(TTargetRecoEvent,1);
};
#endif
