#ifndef PadmeAnaInMCHitsEvent_H
#define PadmeAnaInMCHitsEvent_H

#include "PadmeVAnaInEvent.hh"

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TObjArray.h"

class TMCEvent;
class TTargetMCEvent;
class TEVetoMCEvent;
class TPVetoMCEvent;
class THEPVetoMCEvent;
class TECalMCEvent;
class TSACMCEvent;
class TDetectorInfo;


class PadmeAnaInMCHitsEvent:public PadmeVAnaInEvent {

public: 
  PadmeAnaInMCHitsEvent(){;};
  ~PadmeAnaInMCHitsEvent(){;};
  
  void Init();
  
private:

  TChain* BuildChain(TString);
  TString CheckProtocols(TString);

  TObjArray* fInputFileNameList;
  TFile* fHistoFile;
  TChain* fMCChain;
  TMCEvent* fMCEvent;
  TTargetMCEvent* fTargetMCEvent;
  TEVetoMCEvent* fEVetoMCEvent;
  TPVetoMCEvent* fPVetoMCEvent;
  THEPVetoMCEvent* fHEPVetoMCEvent;
  TECalMCEvent* fECalMCEvent;
  TSACMCEvent* fSACMCEvent;

};


#endif
