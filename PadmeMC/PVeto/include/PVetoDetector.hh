// PVetoDetector.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#ifndef PVetoDetector_H
#define PVetoDetector_H 1

#include "globals.hh"
#include "G4LogicalVolume.hh"

#include "PVetoMessenger.hh"

class PVetoDetector
{

public:

  ~PVetoDetector();
  PVetoDetector(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

  G4LogicalVolume* GetPVetoLogicalVolume() { return fPVetoVolume; }
  G4LogicalVolume* GetFingerLogicalVolume() { return fFingerVolume; }

  // Define displacement of PVeto along Z due to position of magnetic volume
  void SetPVetoDisplacePosZ(G4double z) { fPVetoDisplacePosZ = z; }

private:

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fPVetoVolume;
  G4LogicalVolume* fFingerVolume;
  PVetoMessenger* fPVetoMessenger;

  G4double fPVetoDisplacePosZ; // Displacement of PVeto along Z due to positioning inside magnetic volume

};

#endif
