// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#ifndef SACReconstruction_H
#define SACReconstruction_H

#include "PadmeVReconstruction.hh"

class SACReconstruction : public PadmeVReconstruction
{

public:
  
  SACReconstruction(TFile*, TString);
  ~SACReconstruction();

  void ParseConfFile(TString);
  virtual void Init(PadmeVReconstruction*);
  virtual void ProcessEvent(TMCVEvent*,TMCEvent*);
  virtual void EndProcessing();

};
#endif
