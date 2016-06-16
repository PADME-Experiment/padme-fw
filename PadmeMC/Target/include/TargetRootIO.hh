// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-05-18
//
// --------------------------------------------------------------
#ifndef TargetRootIO_h
#define TargetRootIO_h 1

#include "MCVRootIO.hh"

#include "TTree.h"
#include "TBranch.h"

#include "globals.hh"

class TTargetMCEvent;
class TargetGeometry;
class TDetectorInfo;

class TargetRootIO : public MCVRootIO
{
public:

  TargetRootIO();
  virtual ~TargetRootIO();

  static TargetRootIO* GetInstance();

  void NewRun(G4int,TFile*,TDetectorInfo*);
  //void NewRun(G4int,TFile*);
  void EndRun();
  void SaveEvent(const G4Event*);
  void Close();

private:

  TTree* fEventTree;
  TBranch* fTargetBranch;
  TTargetMCEvent* fEvent;

  TargetGeometry* fGeoPars;

};
#endif // TargetRootIO_h
