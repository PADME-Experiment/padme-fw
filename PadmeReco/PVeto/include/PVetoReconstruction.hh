// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#ifndef PVetoReconstruction_H
#define PVetoReconstruction_H

#include "PadmeVReconstruction.hh"

class PVetoReconstruction : public PadmeVReconstruction
{

public:
  
  PVetoReconstruction(TFile*, TString);
  ~PVetoReconstruction();

  void ParseConfFile(TString);
  virtual void Init(PadmeVReconstruction*);
  virtual void ProcessEvent(TMCVEvent*,TMCEvent*);
  virtual void EndProcessing();

};
#endif
