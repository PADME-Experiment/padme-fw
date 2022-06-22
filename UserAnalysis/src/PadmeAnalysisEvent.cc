#include "PadmeAnalysisEvent.hh"

PadmeAnalysisEvent::PadmeAnalysisEvent(){
  
  RecoEvent            =0;
  TargetRecoEvent      =0;
  EVetoRecoEvent       =0;
  PVetoRecoEvent       =0;
  HEPVetoRecoEvent     =0;
  ECalRecoEvent        =0;
  SACRecoEvent         =0;
  ETagRecoEvent        =0; //MR 06/22
  TargetRecoBeam       =0;
  SACRecoCl            =0;
  ECalRecoCl           =0;
  PVetoRecoCl          =0;
  EVetoRecoCl          =0;
  ETagRecoCl           =0; //MR 06/22
  HEPVetoRecoCl        =0;
  MCTruthEvent         =0;

}
