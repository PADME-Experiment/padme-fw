// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-05-18
//
// --------------------------------------------------------------
#ifndef HEPVetoRootIO_h
#define HEPVetoRootIO_h 1

#include "MCVRootIO.hh"

#include "TTree.h"
#include "TBranch.h"

#include "globals.hh"

class THEPVetoMCEvent;
class HEPVetoGeometry;
class TDetectorInfo;

class HEPVetoRootIO : public MCVRootIO
{
public:

  HEPVetoRootIO();
  virtual ~HEPVetoRootIO();

  static HEPVetoRootIO* GetInstance();

  void NewRun(G4int,TFile*,TDetectorInfo*);
  void EndRun();
  void SaveEvent(const G4Event*);
  void Close();

private:

  TTree* fEventTree;
  TBranch* fHEPVetoBranch;
  THEPVetoMCEvent* fEvent;

  HEPVetoGeometry* fGeoPars;

};
#endif // HEPVetoRootIO_h
