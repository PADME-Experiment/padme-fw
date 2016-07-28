// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#ifndef EVetoReconstruction_H
#define EVetoReconstruction_H

#include "PadmeVReconstruction.hh"

class EVetoReconstruction : public PadmeVReconstruction
{

public:
  
  EVetoReconstruction(TFile*, TString);
  ~EVetoReconstruction();

  void ParseConfFile(TString);
  virtual void Init(PadmeVReconstruction*);
  virtual void ProcessEvent(TMCVEvent*,TMCEvent*);
  virtual void EndProcessing();

};
#endif
