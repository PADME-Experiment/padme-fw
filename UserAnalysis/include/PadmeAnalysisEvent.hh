#ifndef PadmeAnalysisEvent_h
#define PadmeAnalysisEvent_h 1

#include "TRecoEvent.hh"
#include "TTargetRecoEvent.hh"
#include "TTargetRecoBeam.hh"
#include "TECalRecoEvent.hh"
#include "TPVetoRecoEvent.hh"
#include "TETagRecoEvent.hh"  //MR 06/22
#include "TEVetoRecoEvent.hh"
#include "TSACRecoEvent.hh"
#include "THEPVetoRecoEvent.hh"
#include "TRecoVHit.hh"
#include "TMCTruthEvent.hh"

class PadmeAnalysisEvent{
public:

  PadmeAnalysisEvent();
  ~PadmeAnalysisEvent(){;};

  TRecoEvent*           RecoEvent       ;
  TTargetRecoEvent*     TargetRecoEvent ;
  TEVetoRecoEvent*      EVetoRecoEvent  ;
  TPVetoRecoEvent*      PVetoRecoEvent  ;
  THEPVetoRecoEvent*    HEPVetoRecoEvent;
  TECalRecoEvent*       ECalRecoEvent   ;
  TSACRecoEvent*        SACRecoEvent    ;
  TETagRecoEvent*       ETagRecoEvent   ;//MR 06/22
  TTargetRecoBeam*      TargetRecoBeam  ;

  TRecoVClusCollection* SACRecoCl       ;
  TRecoVClusCollection* ECalRecoCl      ;
  TRecoVClusCollection* PVetoRecoCl     ;
  TRecoVClusCollection* ETagRecoCl      ;//MR 06/22
  TRecoVClusCollection* EVetoRecoCl     ;
  TRecoVClusCollection* HEPVetoRecoCl   ;
  TMCTruthEvent*        MCTruthEvent    ;

};
#endif
