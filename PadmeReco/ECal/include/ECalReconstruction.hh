// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#ifndef ECalReconstruction_H
#define ECalReconstruction_H

#include "PadmeVReconstruction.hh"

class ECalReconstruction : public PadmeVReconstruction
{

public:
  
  ECalReconstruction(TFile*, TString);
  ~ECalReconstruction();

  void ParseConfFile(TString);
  virtual void Init(PadmeVReconstruction*);
  virtual void ProcessEvent(TMCVEvent*,TMCEvent*);
  virtual void EndProcessing();

};
#endif
