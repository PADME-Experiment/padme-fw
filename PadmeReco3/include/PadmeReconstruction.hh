#ifndef PadmeReconstruction_H
#define PadmeReconstruction_H

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TTimeStamp.h"

#include "utlConfigParser.hh"
#include "PadmeVRecoConfig.hh"

class HistoSvc;
class TrigTimeSvc;
class RunConditionSvc;
class RunConfigurationSvc;

class PadmeVReconstruction;
class GlobalRecoConfigOptions;

class RecoRootIOManager;

class ECalReconstruction;
class ETagReconstruction;
class TargetReconstruction;

class TRawEvent;
class TMCEvent;
class TRecoEvent;

//class TMCTruthEvent;
#include "TMCTruthEvent.hh"

class TTargetMCEvent;
class TEVetoMCEvent;
class TPVetoMCEvent;
class THEPVetoMCEvent;
class TECalMCEvent;
class TSACMCEvent;
class TETagMCEvent;
class TTPixMCEvent;

class TTargetRecoEvent;
class TEVetoRecoEvent;
class TPVetoRecoEvent;
class THEPVetoRecoEvent;
class TECalRecoEvent;
class TSACRecoEvent;
class TETagRecoEvent;
class TTPixRecoEvent;
class TLeadGlassRecoEvent;

class TDetectorInfo;

//class PadmeReconstruction : public PadmeVReconstruction
class PadmeReconstruction
{

public:

  //PadmeReconstruction(TObjArray*, TString, TFile*, Int_t, UInt_t);
  PadmeReconstruction(TObjArray*, TString, TString, Int_t, UInt_t);
  ~PadmeReconstruction();

  //void Init(PadmeVReconstruction*) { return; }
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

  // Give access to detector reconstructions
  ECalReconstruction* GetECalReconstruction() { return fECalReconstruction; }
  ETagReconstruction* GetETagReconstruction() { return fETagReconstruction; }
  TargetReconstruction* GetTargetReconstruction() { return fTargetReconstruction; }

  TMCTruthEvent* GetMCTruthEvent() { return fMCTruthEvent; }

  GlobalRecoConfigOptions* GetGlobalRecoConfigOptions() { return fGlobalRecoConfigOptions; }

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

  HistoSvc* fHistoSvc;
  TrigTimeSvc* fTrigTimeSvc;
  RunConditionSvc* fRunConditionSvc;
  RunConfigurationSvc* fRunConfigurationSvc;

  RecoRootIOManager* fRecoRootIOManager;
  TString fOutputFileName;
  TFile* fOutputFile;

  TChain* fMCChain;
  TChain* fRawChain;
  TChain* fRecoChain;

  TRawEvent* fRawEvent;

  TMCEvent* fMCEvent;

  TMCTruthEvent* fMCTruthEvent;

  TTargetMCEvent*  fTargetMCEvent;
  TEVetoMCEvent*   fEVetoMCEvent;
  TPVetoMCEvent*   fPVetoMCEvent;
  THEPVetoMCEvent* fHEPVetoMCEvent;
  TECalMCEvent*    fECalMCEvent;
  TSACMCEvent*     fSACMCEvent;
  TETagMCEvent*    fETagMCEvent;
  TTPixMCEvent*    fTPixMCEvent;

  TRecoEvent*        fRecoEvent;
  TTargetRecoEvent*  fTargetRecoEvent;
  TEVetoRecoEvent*   fEVetoRecoEvent;
  TPVetoRecoEvent*   fPVetoRecoEvent;
  THEPVetoRecoEvent* fHEPVetoRecoEvent;
  TECalRecoEvent*    fECalRecoEvent;
  TSACRecoEvent*     fSACRecoEvent;
  TETagRecoEvent*    fETagRecoEvent;
  TTPixRecoEvent*    fTPixRecoEvent;
  TLeadGlassRecoEvent* fLeadGlassRecoEvent;
 
  typedef std::vector<PadmeVReconstruction*> RecoVector;
  RecoVector fRecoLibrary;

  ECalReconstruction* fECalReconstruction;
  ETagReconstruction* fETagReconstruction;
  TargetReconstruction* fTargetReconstruction;
 
  Int_t fNEvt;
  Int_t fNProcessedEventsInTotal;

  GlobalRecoConfigOptions* fGlobalRecoConfigOptions;

};
#endif
