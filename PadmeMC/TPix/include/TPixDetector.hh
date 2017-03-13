// TPixDetector.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-04
// --------------------------------------------------------------

#ifndef TPixDetector_H
#define TPixDetector_H 1

#include "globals.hh"
#include "G4LogicalVolume.hh"

#include "TPixMessenger.hh"

class TPixDetector
{

public:

  ~TPixDetector();
  TPixDetector(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

  G4LogicalVolume* GetTPixLogicalVolume() { return fTPixVolume; }
  G4LogicalVolume* GetChipLogicalVolume() { return fChipVolume; }

  //void SetTPixChamberWallAngle(G4double);
  //void SetTPixChamberWallCorner(G4ThreeVector);

private:

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fTPixVolume;
  G4LogicalVolume* fChipVolume;
  TPixMessenger* fTPixMessenger;

};

#endif
