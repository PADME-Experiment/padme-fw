// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#ifndef EVetoReconstruction_H
#define EVetoReconstruction_H

#include "PadmeVReconstruction.hh"
#include "TRandom2.h"

class EVetoReconstruction : public PadmeVReconstruction
{

public:
  
  EVetoReconstruction(TFile*, TString);
  ~EVetoReconstruction();

  // void ParseConfFile(TString);
  // virtual void Init(PadmeVReconstruction*);
  // virtual void ProcessEvent(TMCVEvent*,TMCEvent*);
  // virtual void EndProcessing();
  virtual void HistoInit();
  virtual void AnalyzeEvent(TRawEvent* evt);
  void ConvertMCDigitsToRecoHits(TMCVEvent* tEvent,TMCEvent* tMCEvent);
  TRandom2 *random;
  
  Double_t fSigmaNoiseForMC;
  Double_t fEVetoDigiTimeWindow;

};
#endif
