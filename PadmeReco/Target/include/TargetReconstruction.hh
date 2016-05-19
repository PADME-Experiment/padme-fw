// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#ifndef TargetReconstruction_H
#define TargetReconstruction_H

#include "PadmeVReconstruction.hh"

class TargetReconstruction : public PadmeVReconstruction
{

public:
  
  TargetReconstruction(TFile*, TString);
  ~TargetReconstruction();

  void ParseConfFile(TString);
  virtual void Init(PadmeVReconstruction*);
  virtual void ProcessEvent(TMCVEvent*,TMCEvent*);
  virtual void EndProcessing();

};
#endif
