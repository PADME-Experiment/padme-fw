#ifndef PadmeReconstruction_H
#define PadmeReconstruction_H

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TTimeStamp.h"

#include "utlConfigParser.hh"
#include "PadmeVRecoConfig.hh"

class TRawEvent;
class TMCEvent;
class TRecoEvent;

class TTargetMCEvent;
class TEVetoMCEvent;
class TPVetoMCEvent;
class THEPVetoMCEvent;
class TECalMCEvent;
class TSACMCEvent;
class TTPixMCEvent;

class TTargetRecoEvent;
class TEVetoRecoEvent;
class TPVetoRecoEvent;
class THEPVetoRecoEvent;
class TECalRecoEvent;
class TSACRecoEvent;
class TTPixRecoEvent;

class TDetectorInfo;

#include "PadmeVReconstruction.hh"

class PadmeReconstruction : public PadmeVReconstruction
{

public:

  PadmeReconstruction(TObjArray*, TString, TFile*, Int_t, UInt_t);
  ~PadmeReconstruction();

  void Init(PadmeVReconstruction*) { return; }
  void Init(Int_t, UInt_t);
  Bool_t NextEvent();
  void EndProcessing();

  PadmeVReconstruction* FindReco(TString);

  Int_t GetRunNumber();
  Int_t GetEventNumber();
  TTimeStamp GetEventTime();
  ULong64_t GetRunClock();
  UInt_t GetEventStatus();
  UInt_t GetTriggerMask();
  Bool_t IsSimulated();

private:

  TChain* BuildChain(TString);
  TString CheckProtocols(TString);

  void InitDetectorsInfo();
  void InitLibraries();
  void InitRunningModeFlags();

  void HistoInit();
  void ProcessEvent(TRawEvent*);

  void ShowSubDetectorInfo(TDetectorInfo*,TString);

  TObjArray* fInputFileNameList;
  utl::ConfigParser *fConfigParser;
  PadmeVRecoConfig *fConfig;
  //TString fConfigFileName;
  TFile* fHistoFile;
  TChain* fMCChain;
  TChain* fRawChain;
  TChain* fRecoChain;

  TRawEvent* fRawEvent;

  TMCEvent* fMCEvent;

  TTargetMCEvent*  fTargetMCEvent;
  TEVetoMCEvent*   fEVetoMCEvent;
  TPVetoMCEvent*   fPVetoMCEvent;
  THEPVetoMCEvent* fHEPVetoMCEvent;
  TECalMCEvent*    fECalMCEvent;
  TSACMCEvent*     fSACMCEvent;
  TTPixMCEvent*    fTPixMCEvent;


  TRecoEvent*        fRecoEvent;
  TTargetRecoEvent*  fTargetRecoEvent;
  TEVetoRecoEvent*   fEVetoRecoEvent;
  TPVetoRecoEvent*   fPVetoRecoEvent;
  THEPVetoRecoEvent* fHEPVetoRecoEvent;
  TECalRecoEvent*    fECalRecoEvent;
  TSACRecoEvent*     fSACRecoEvent;
  TTPixRecoEvent*    fTPixRecoEvent;
  
  typedef std::vector<PadmeVReconstruction*> RecoVector;
  RecoVector fRecoLibrary;
 
  Int_t fNEvt;
  Int_t fNProcessedEventsInTotal;

};
#endif
