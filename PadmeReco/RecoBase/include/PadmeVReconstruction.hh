// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-21
//
// --------------------------------------------------------------
#ifndef PadmeVReconstruction_H
#define PadmeVReconstruction_H

#include "TFile.h"

#include "TMCEvent.hh"
#include "TMCVEvent.hh"

#include "PadmeVNamedModule.hh"

class PadmeVReconstruction : public PadmeVNamedModule
{
public:

  PadmeVReconstruction(TFile*, TString, TString);
  virtual ~PadmeVReconstruction();
  //virtual TRecoVEvent* ProcessEvent(TDetectorVEvent* = 0, Event* = 0) = 0;
  virtual void ProcessEvent(TMCVEvent* = 0,TMCEvent* = 0);
  virtual void Init(PadmeVReconstruction*) = 0;
  virtual void EndProcessing() = 0; ///< Call from derived classes
  virtual void ParseConfFile(TString);

  static void Exception(TString);

public:

  //TRecoVEvent* GetRecoEvent() { return fRecoEvent; };
  //void         SetRecoEvent(TRecoVEvent* value) { fRecoEvent = value; };

  PadmeVReconstruction* GetMainReco() { return fMainReco; };
  void                  SetMainReco(PadmeVReconstruction* value) { fMainReco = value; };

  TFile* GetHistoFile() { return fHistoFile; };

  TString GetConfigFileName() { return fConfigFileName; };
  void    SetConfigFileName(TString val) { fConfigFileName = val; };

  // Use to get an existing directory or create if not already made
  //TDirectory* GetOrMakeDir(TDirectory *inDir,TString dirName);	  

protected:

  TFile* fHistoFile;
  PadmeVReconstruction* fMainReco;

  //TRecoVEvent * fRecoEvent;

  TString fConfigFileName;

};
#endif
