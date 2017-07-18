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

  // Z position where beam has t=0
  //void SetBeamStartZ(Double_t z) { fEHistoBeamStartZ = z; }

  // Time length of e+ bunch (40. ns for the time being)
  //void SetBeamBunchLengthT(Double_t t) { if (t > 0.) fEHistoBeamBunchLengthT = t; }

private:

  TTree*       fEventTree;
  TBranch*     fSACBranch;
  TSACMCEvent* fEvent;

  // Parameters used for energy distribution histogram

  //Double_t fEHistoBeamStartZ;
  //Double_t fEHistoBeamBunchLengthT;
  //
  //Int_t    fEHistoNBins;
  //Double_t fEHistoTStart;
  //Double_t fEHistoTStep;
  //Double_t fEHistoTEnd;

  SACGeometry* fGeoPars;

};
#endif // SACRootIO_h
