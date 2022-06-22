// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2022-06-16
//
// --------------------------------------------------------------
#ifndef ETagReconstruction_H
#define ETagReconstruction_H

#include "PadmeVReconstruction.hh"
#include "TRandom2.h"
class ETagSimpleClusterization;

class ETagReconstruction : public PadmeVReconstruction
{

public:
  
  ETagReconstruction(TFile*, TString);
  ~ETagReconstruction();
  // virtual void ProcessEvent(TMCVEvent*,TMCEvent*);
  virtual void HistoInit();
  virtual void AnalyzeEvent(TRawEvent*);
  void ConvertMCDigitsToRecoHits(TMCVEvent*,TMCEvent*);
  void BuildClusters();

private:

  TRandom2* random;
  Double_t fSigmaNoiseForMC;
  Double_t fETagDigiTimeWindow;
  ETagSimpleClusterization *fClusterization;

};
#endif
