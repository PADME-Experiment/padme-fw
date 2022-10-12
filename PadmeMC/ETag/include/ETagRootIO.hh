// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-05-18
//
// --------------------------------------------------------------
#ifndef ETagRootIO_h
#define ETagRootIO_h 1

#include "MCVRootIO.hh"

#include "TTree.h"
#include "TBranch.h"

#include "globals.hh"

class TETagMCEvent;
class ETagGeometry;
class TDetectorInfo;

class ETagRootIO : public MCVRootIO
{
public:

  ETagRootIO();
  virtual ~ETagRootIO();

  static ETagRootIO* GetInstance();

  void NewRun(G4int,TFile*,TDetectorInfo*);
  void EndRun();
  void SaveEvent(const G4Event*);
  void Close();

private:

  TTree* fEventTree;
  TBranch* fETagBranch;
  TETagMCEvent* fEvent;

  ETagGeometry* fGeoPars;

};
#endif // ETagRootIO_h
