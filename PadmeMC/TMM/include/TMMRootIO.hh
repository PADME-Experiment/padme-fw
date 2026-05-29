// --------------------------------------------------------------
// History:
//
// Created by Elisa Di Meco (elisa.dimeco@lnf.infn.it) 2026-05-27
//
// --------------------------------------------------------------
#ifndef TMMRootIO_h
#define TMMRootIO_h 1

#include "MCVRootIO.hh"

#include "TTree.h"
#include "TBranch.h"

#include "globals.hh"

class TTMMMCEvent;  
class TMMGeometry;
class TDetectorInfo;

class TMMRootIO : public MCVRootIO
{
public:

  TMMRootIO();
  virtual ~TMMRootIO();

  static TMMRootIO* GetInstance();

  void NewRun(G4int,TFile*,TDetectorInfo*);
  void EndRun();
  void SaveEvent(const G4Event*);
  void Close();

private:

  TTree* fEventTree;
  TBranch* fTMMBranch;
  TTMMMCEvent* fEvent;

  TMMGeometry* fGeoPars;

};
#endif // TMMRootIO_h
