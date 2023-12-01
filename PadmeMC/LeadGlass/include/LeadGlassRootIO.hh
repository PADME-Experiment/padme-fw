// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2023-09-21
//
// --------------------------------------------------------------
#ifndef LeadGlassRootIO_h
#define LeadGlassRootIO_h 1

#include "MCVRootIO.hh"

#include "TTree.h"
#include "TBranch.h"

#include "globals.hh"

class TLeadGlassMCEvent;
class LeadGlassGeometry;
class TDetectorInfo;

class LeadGlassRootIO : public MCVRootIO
{
public:

  LeadGlassRootIO();
  virtual ~LeadGlassRootIO();

  static LeadGlassRootIO* GetInstance();

  void NewRun(G4int,TFile*,TDetectorInfo*);
  void EndRun();
  void SaveEvent(const G4Event*);
  void Close();

private:

  TTree* fEventTree;
  TBranch* fLeadGlassBranch;
  TLeadGlassMCEvent* fEvent;

  LeadGlassGeometry* fGeoPars;

};
#endif // LeadGlassRootIO_h
