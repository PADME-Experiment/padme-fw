// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#ifndef TargetReconstruction_H
#define TargetReconstruction_H

#include "PadmeVReconstruction.hh"

#include "TargetRecoEvent.hh"
#include "TargetRecoHit.hh"

#include "TTargetMCEvent.hh"


// ROOT TO BE MOVED TO STD FILE
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"

class TargetRecoHit;

class TargetReconstruction : public PadmeVReconstruction
{

public:
  
  TargetReconstruction(TFile*, TString);
  ~TargetReconstruction();

  void ParseConfFile(TString);
  virtual void Init(PadmeVReconstruction*);
  virtual void ProcessEvent(TMCVEvent*,TMCEvent*);
  void MakeTargetRecoHitCollection(TTargetMCEvent*);
  void MakeTargetRecoEvent();
  virtual void EndProcessing();
  //  int  PosToChannel(double pos);


private: 

  TargetRecoEvent * fRecoEvent;
  std::vector<TargetRecoHit*>* fTargetRecoHitCollection;
  //  TargetRecoHit * fRecoHit;

  TFile * baryfile;
  TH2D * barycoord;


};
#endif
