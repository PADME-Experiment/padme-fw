// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#ifndef PVetoReconstruction_H
#define PVetoReconstruction_H

#include "PadmeVReconstruction.hh"
#include "ADCChannelVReco.hh"

class PVetoReconstruction : public PadmeVReconstruction
{

public:
  
  PVetoReconstruction(TFile*, TString);
  ~PVetoReconstruction();

  //  void ParseConfFile(TString);
  //  virtual void Init(PadmeVReconstruction*);
  virtual void ProcessEvent(TMCVEvent*,TMCEvent*);
  //  virtual void ProcessEvent(TRawEvent*);
  //  virtual void EndProcessing();
  virtual void HistoInit();
  virtual void AnalyzeEvent(TRawEvent* evt);

private:
  //ADCChannelVReco *fChannelReco;
  

};
#endif
