// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2021-03-12
//
// --------------------------------------------------------------
#ifndef MCTruthRootIO_h
#define MCTruthRootIO_h 1

#include "MCVRootIO.hh"

#include "TTree.h"
#include "TBranch.h"

#include "globals.hh"

class TMCTruthEvent;
class TDetectorInfo;
class MCTruthManager;

class MCTruthRootIO : public MCVRootIO
{
public:

  MCTruthRootIO();
  virtual ~MCTruthRootIO();

  static MCTruthRootIO* GetInstance();

  void NewRun(G4int,TFile*,TDetectorInfo*);
  void EndRun();
  void SaveEvent(const G4Event*);
  void Close();

private:

  TTree*         fEventTree;
  TBranch*       fMCTruthBranch;
  TMCTruthEvent* fEvent;

  MCTruthManager* fManager;

};
#endif // MCTruthRootIO_h
