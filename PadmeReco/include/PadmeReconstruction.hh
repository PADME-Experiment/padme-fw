#ifndef PadmeReconstruction_H
#define PadmeReconstruction_H

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TObjArray.h"

class TMCEvent;
class TSACMCEvent;

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

  TObjArray* fInputFileNameList;
  //TString fConfigFileName;
  TFile* fHistoFile;
  TChain* fMCChain;
  TMCEvent* fMCEvent;
  TSACMCEvent* fSACMCEvent;

  typedef std::vector<PadmeVReconstruction*> RecoVector;
  RecoVector fRecoLibrary;
 
  Int_t fNEvt;
  Int_t fNProcessedEventsInTotal;

};
#endif
