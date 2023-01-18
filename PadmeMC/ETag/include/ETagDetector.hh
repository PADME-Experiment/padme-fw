// ETagDetector.hh
// --------------------------------------------------------------
// History:
//
// Created by Mauro Raggi (mauro.raggi@roma1.infn.it) 2022-06-01
// --------------------------------------------------------------

#ifndef ETagDetector_H
#define ETagDetector_H 1

#include "globals.hh"
#include "G4LogicalVolume.hh"

#include "ETagMessenger.hh"

class ETagDetector
{

public:

  ~ETagDetector();
  ETagDetector(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

  G4LogicalVolume* GetETagLogicalVolume() { return fETagVolume; }

private:

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fETagVolume;
  G4LogicalVolume* fDeltaAbsVolume;

  G4LogicalVolume* fETagShortBarVolume;
  G4LogicalVolume* fETagBarVolume;
  ETagMessenger* fETagMessenger;

};

#endif
