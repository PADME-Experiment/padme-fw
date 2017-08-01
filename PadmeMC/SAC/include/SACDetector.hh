// SACDetector.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#ifndef SACDetector_H
#define SACDetector_H 1

#include "globals.hh"

#include "SACGeometry.hh"

class G4LogicalVolume;
class SACMessenger;

class SACDetector
{

public:

  ~SACDetector();
  SACDetector(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

  G4LogicalVolume* GetSACLogicalVolume() { return fSACVolume; }
  G4LogicalVolume* GetCrystalLogicalVolume() { return fCrystalVolume; }
  G4LogicalVolume* GetCellLogicalVolume() { return fCellVolume; }

  G4double GetSACFrontFaceZ() { return SACGeometry::GetInstance()->GetSACFrontFacePosZ(); }

private:

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fSACVolume;
  G4LogicalVolume* fCrystalVolume;
  G4LogicalVolume* fCellVolume;

  SACMessenger* fSACMessenger;

};

#endif
