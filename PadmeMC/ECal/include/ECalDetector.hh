// ECalDetector.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#ifndef ECalDetector_H
#define ECalDetector_H 1

#include "globals.hh"

#include "ECalGeometry.hh"

class G4LogicalVolume;
class ECalMessenger;

class ECalDetector
{

public:

  ~ECalDetector();
  ECalDetector(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

  G4LogicalVolume* GetECalLogicalVolume() { return fECalVolume; }
  G4LogicalVolume* GetCrystalLogicalVolume() { return fCrystalVolume; }
  G4LogicalVolume* GetCellLogicalVolume() { return fCellVolume; }
  G4LogicalVolume* GetECalPanelLogicalVolume() { return fECalPanelVolume; }

  G4double GetECalFrontFaceZ() { return ECalGeometry::GetInstance()->GetECalFrontFacePosZ(); }

private:

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fECalVolume;
  G4LogicalVolume* fCrystalVolume;
  G4LogicalVolume* fCellVolume;
  G4LogicalVolume* fECalPanelVolume;

  ECalMessenger* fECalMessenger;

};

#endif
