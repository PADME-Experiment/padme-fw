// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------
#ifndef ECalRootIO_h
#define ECalRootIO_h 1

#include "MCVRootIO.hh"

#include "TTree.h"
#include "TBranch.h"

#include "globals.hh"

class TECalMCEvent;
class ECalGeometry;
class TDetectorInfo;

class ECalRootIO : public MCVRootIO
{
public:

  ECalRootIO();
  virtual ~ECalRootIO();

  static ECalRootIO* GetInstance();

  void NewRun(G4int,TFile*,TDetectorInfo*);
  //void NewRun(G4int,TFile*);
  void EndRun();
  void SaveEvent(const G4Event*);
  void Close();

private:

  TTree*        fEventTree;
  TBranch*      fECalBranch;
  TECalMCEvent* fEvent;

  ECalGeometry* fGeoPars;

};
#endif // ECalRootIO_h
