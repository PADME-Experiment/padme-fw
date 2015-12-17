// HEPVetoDetector.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#ifndef HEPVetoDetector_H
#define HEPVetoDetector_H 1

#include "globals.hh"
#include "G4LogicalVolume.hh"

#include "HEPVetoMessenger.hh"

class HEPVetoDetector
{

public:

  ~HEPVetoDetector();
  HEPVetoDetector(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

  G4LogicalVolume* GetHEPVetoLogicalVolume() { return fHEPVetoVolume; }
  G4LogicalVolume* GetFingerLogicalVolume() { return fFingerVolume; }

private:

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fHEPVetoVolume;
  G4LogicalVolume* fFingerVolume;
  HEPVetoMessenger* fHEPVetoMessenger;

};

#endif
