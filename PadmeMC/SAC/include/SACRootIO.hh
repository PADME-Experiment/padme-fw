// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-17
//
// --------------------------------------------------------------
#ifndef SACRootIO_h
#define SACRootIO_h 1

#include "MCVRootIO.hh"

#include "TTree.h"
#include "TBranch.h"

#include "globals.hh"

class TSACMCEvent;
class SACGeometry;
class TDetectorInfo;

class SACRootIO : public MCVRootIO
{
public:

  SACRootIO();
  virtual ~SACRootIO();

  static SACRootIO* GetInstance();

  void NewRun(G4int,TFile*,TDetectorInfo*);
  //void NewRun(G4int,TFile*);
  void EndRun();
  void SaveEvent(const G4Event*);
  void Close();

private:

  TTree*       fSACTree; //Tree to hold SAC Hits in one run
  TBranch*     fSACBranch;
  TSACMCEvent* fEvent;

  SACGeometry* fGeoPars;

};
#endif // SACRootIO_h
