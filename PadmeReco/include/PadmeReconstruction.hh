#ifndef PadmeReconstruction_H
#define PadmeReconstruction_H

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
class TTPixMCEvent;

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

private:

  TChain* BuildChain(TString);
  TString CheckProtocols(TString);

  void InitDetectorsInfo();
  void InitLibraries();

  void ShowSubDetectorInfo(TDetectorInfo*,TString);

  TObjArray* fInputFileNameList;
  //TString fConfigFileName;
  TFile* fHistoFile;
  TChain* fMCChain;

  TMCEvent* fMCEvent;

  TTargetMCEvent*  fTargetMCEvent;
  TEVetoMCEvent*   fEVetoMCEvent;
  TPVetoMCEvent*   fPVetoMCEvent;
  THEPVetoMCEvent* fHEPVetoMCEvent;
  TECalMCEvent*    fECalMCEvent;
  TSACMCEvent*     fSACMCEvent;
  TTPixMCEvent*    fTPixMCEvent;

  typedef std::vector<PadmeVReconstruction*> RecoVector;
  RecoVector fRecoLibrary;
 
  Int_t fNEvt;
  Int_t fNProcessedEventsInTotal;

};
#endif
