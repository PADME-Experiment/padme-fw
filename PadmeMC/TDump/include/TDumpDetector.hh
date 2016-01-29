// TDumpDetector.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-01-28
// --------------------------------------------------------------

#ifndef TDumpDetector_H
#define TDumpDetector_H 1

#include "globals.hh"
#include "G4LogicalVolume.hh"

#include "TDumpMessenger.hh"

class TDumpDetector
{

public:

  ~TDumpDetector();
  TDumpDetector(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

  G4LogicalVolume* GetTDumpLogicalVolume()  { return fTDumpVolume; }
  G4LogicalVolume* GetTargetLogicalVolume() { return fTargetVolume; }

private:

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fTDumpVolume;
  G4LogicalVolume* fTargetVolume;

  TDumpMessenger* fTDumpMessenger;

};

#endif
