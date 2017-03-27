// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2017-03-15
//
// --------------------------------------------------------------
#ifndef TPixRootIO_h
#define TPixRootIO_h 1

#include "MCVRootIO.hh"

#include "TTree.h"
#include "TBranch.h"

#include "globals.hh"

class TTPixMCEvent;
class TPixGeometry;
class TDetectorInfo;

class TPixRootIO : public MCVRootIO
{
public:

  TPixRootIO();
  virtual ~TPixRootIO();

  static TPixRootIO* GetInstance();

  void NewRun(G4int,TFile*,TDetectorInfo*);
  //void NewRun(G4int,TFile*);
  void EndRun();
  void SaveEvent(const G4Event*);
  void Close();

private:

  TTree*        fEventTree;
  TBranch*      fTPixBranch;
  TTPixMCEvent* fEvent;

  TPixGeometry* fGeoPars;

};
#endif // TPixRootIO_h
