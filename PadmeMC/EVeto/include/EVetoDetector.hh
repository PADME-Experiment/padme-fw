// EVetoDetector.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#ifndef EVetoDetector_H
#define EVetoDetector_H 1

#include "globals.hh"
#include "G4LogicalVolume.hh"

#include "EVetoMessenger.hh"

class EVetoDetector
{

public:

  ~EVetoDetector();
  EVetoDetector(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

  G4LogicalVolume* GetEVetoLogicalVolume() { return fEVetoVolume; }
  G4LogicalVolume* GetFingerLogicalVolume() { return fFingerVolume; }

  // Define displacement of EVeto along Z due to position of magnetic volume
  void SetEVetoDisplacePosZ(G4double z) { fEVetoDisplacePosZ = z; }

private:

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fEVetoVolume;
  G4LogicalVolume* fFingerVolume;
  EVetoMessenger* fEVetoMessenger;

  G4double fEVetoDisplacePosZ; // Displacement of EVeto along Z due to positioning inside magnetic volume

};

#endif
