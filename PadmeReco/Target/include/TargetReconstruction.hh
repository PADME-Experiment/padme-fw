// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#ifndef TargetReconstruction_H
#define TargetReconstruction_H

#include "PadmeVReconstruction.hh"
#include "TRecoVHit.hh"
#include <vector>

class TTargetRecoBeam;
class TTargetSignalFitParams;

class TargetReconstruction : public PadmeVReconstruction
{

public:
  
  TargetReconstruction(TFile*, TString);
  ~TargetReconstruction();

  // void ParseConfFile(TString);
  // virtual void Init(PadmeVReconstruction*);
  virtual void ProcessEvent(TMCVEvent*,TMCEvent*);
  virtual void ProcessEvent(TRawEvent*);
  virtual void ProcessEvent(TRecoVObject* recoObj, TRecoEvent* tRecoEvent);
  virtual void EndProcessing();
  virtual void HistoInit();
  virtual void AnalyzeEvent(TRawEvent* evt);
  virtual void ReconstructBeam();
  TTargetRecoBeam* getRecoBeam(){return fTargetRecoBeam;}
  vector<TTargetSignalFitParams *> &getSignalFitParams(){return fSignalFitParams;}
  void RetrieveSignalFitParams();
  Bool_t writeFitParams(){return fWriteFitParams;}
  Bool_t writeTargetBeam(){return fWriteTargetBeam;}

private:

  static Double_t fitProfile(Double_t *x, Double_t *par);
  TTargetRecoBeam* fTargetRecoBeam;
  vector<TTargetSignalFitParams *> fSignalFitParams;
  TH1F * hprofile;

  Bool_t fWriteFitParams;
  Bool_t fWriteTargetBeam;
};
#endif

