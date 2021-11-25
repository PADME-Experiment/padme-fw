#ifndef PadmeAnalysisEvent_h
#define PadmeAnalysisEvent_h 1

#include "TRecoEvent.hh"
#include "TTargetRecoEvent.hh"
#include "TTargetRecoBeam.hh"
#include "TECalRecoEvent.hh"
#include "TPVetoRecoEvent.hh"
#include "TEVetoRecoEvent.hh"
#include "TSACRecoEvent.hh"
#include "THEPVetoRecoEvent.hh"
#include "TRecoVHit.hh"
#include "TMCTruthEvent.hh"

class PadmeAnalysisEvent{
public:

  PadmeAnalysisEvent();
  ~PadmeAnalysisEvent(){;};


  TRecoEvent*                     RecoEvent            ;
  TTargetRecoEvent*               TargetRecoEvent      ;
  TEVetoRecoEvent*                EVetoRecoEvent       ;
  TPVetoRecoEvent*                PVetoRecoEvent       ;
  THEPVetoRecoEvent*              HEPVetoRecoEvent     ;
  TECalRecoEvent*                 ECalRecoEvent        ;
  TSACRecoEvent*                  SACRecoEvent         ;
  TTargetRecoBeam*                TargetRecoBeam       ;
  TRecoVClusCollection*           SACRecoCl            ;
  TRecoVClusCollection*           ECalRecoCl           ;
  TRecoVClusCollection*           PVetoRecoCl          ;
  TRecoVClusCollection*           EVetoRecoCl          ;
  TRecoVClusCollection*           HEPVetoRecoCl        ;
  TMCTruthEvent*                  MCTruthEvent         ;


};
#endif
