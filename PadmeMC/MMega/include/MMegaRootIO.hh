// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
//
// --------------------------------------------------------------
#ifndef MMegaRootIO_h
#define MMegaRootIO_h 1

#include "MCVRootIO.hh"

#include "TTree.h"
#include "TBranch.h"

#include "globals.hh"

class TMMegaMCEvent;  
class MMegaGeometry;
class TDetectorInfo;

class MMegaRootIO : public MCVRootIO
{
public:

  MMegaRootIO();
  virtual ~MMegaRootIO();

  static MMegaRootIO* GetInstance();

  void NewRun(G4int,TFile*,TDetectorInfo*);
  void EndRun();
  void SaveEvent(const G4Event*);
  void Close();

private:

  TTree* fEventTree;
  TBranch* fMMegaBranch;
  TMMegaMCEvent* fEvent;

  MMegaGeometry* fGeoPars;

};
#endif // MMegaRootIO_h
