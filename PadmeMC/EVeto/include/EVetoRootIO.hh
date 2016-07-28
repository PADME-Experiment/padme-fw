// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-05-18
//
// --------------------------------------------------------------
#ifndef EVetoRootIO_h
#define EVetoRootIO_h 1

#include "MCVRootIO.hh"

#include "TTree.h"
#include "TBranch.h"

#include "globals.hh"

class TEVetoMCEvent;
class EVetoGeometry;
class TDetectorInfo;

class EVetoRootIO : public MCVRootIO
{
public:

  EVetoRootIO();
  virtual ~EVetoRootIO();

  static EVetoRootIO* GetInstance();

  void NewRun(G4int,TFile*,TDetectorInfo*);
  void EndRun();
  void SaveEvent(const G4Event*);
  void Close();

private:

  TTree* fEventTree;
  TBranch* fEVetoBranch;
  TEVetoMCEvent* fEvent;

  EVetoGeometry* fGeoPars;

};
#endif // EVetoRootIO_h
