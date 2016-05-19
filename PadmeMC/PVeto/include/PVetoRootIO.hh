// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-05-18
//
// --------------------------------------------------------------
#ifndef PVetoRootIO_h
#define PVetoRootIO_h 1

#include "MCVRootIO.hh"

#include "TTree.h"
#include "TBranch.h"

#include "globals.hh"

class TPVetoMCEvent;
class PVetoGeometry;
class TDetectorInfo;

class PVetoRootIO : public MCVRootIO
{
public:

  PVetoRootIO();
  virtual ~PVetoRootIO();

  static PVetoRootIO* GetInstance();

  void NewRun(G4int,TFile*,TDetectorInfo*);
  void EndRun();
  void SaveEvent(const G4Event*);
  void Close();

private:

  TTree* fEventTree;
  TBranch* fPVetoBranch;
  TPVetoMCEvent* fEvent;

  PVetoGeometry* fGeoPars;

};
#endif // PVetoRootIO_h
