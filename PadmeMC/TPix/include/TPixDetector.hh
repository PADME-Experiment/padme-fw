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
  G4LogicalVolume* GetTPixBoxLogicalVolume() { return fTPixBoxVolume; }
  G4LogicalVolume* GetTPixChipLogicalVolume() { return fTPixChipVolume; }

  //void SetTPixChamberWallAngle(G4double);
  //void SetTPixChamberWallCorner(G4ThreeVector);

private:

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fTPixVolume;
  G4LogicalVolume* fTPixBoxVolume;
  G4LogicalVolume* fTPixChipVolume;
  TPixMessenger* fTPixMessenger;

};

#endif
