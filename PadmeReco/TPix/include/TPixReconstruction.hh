// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2017-03-27
//
// --------------------------------------------------------------
#ifndef TPixReconstruction_H
#define TPixReconstruction_H

#include "PadmeVReconstruction.hh"

class TPixReconstruction : public PadmeVReconstruction
{

public:
  
  TPixReconstruction(TFile*, TString);
  ~TPixReconstruction();

  void ParseConfFile(TString);
  virtual void Init(PadmeVReconstruction*);
  virtual void ProcessEvent(TMCVEvent*,TMCEvent*);
  virtual void EndProcessing();

};
#endif
