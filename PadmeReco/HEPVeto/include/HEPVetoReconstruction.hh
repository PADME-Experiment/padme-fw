// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#ifndef HEPVetoReconstruction_H
#define HEPVetoReconstruction_H

#include "PadmeVReconstruction.hh"
#include "TRandom2.h"

class HEPVetoReconstruction : public PadmeVReconstruction
{

public:
  
  HEPVetoReconstruction(TFile*, TString);
  ~HEPVetoReconstruction();

  // void ParseConfFile(TString);
  // virtual void Init(PadmeVReconstruction*);
  // virtual void ProcessEvent(TMCVEvent*,TMCEvent*);
  virtual void AnalyzeEvent(TRawEvent* rawEv);
  virtual void HistoInit();
  // virtual void EndProcessing();
  void ConvertMCDigitsToRecoHits(TMCVEvent* tEvent,TMCEvent* tMCEvent);
  TRandom2 *random;

};
#endif
