// LAVDetector.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#ifndef LAVDetector_H
#define LAVDetector_H 1

#include "globals.hh"
#include "G4LogicalVolume.hh"

#include "LAVMessenger.hh"

class LAVDetector
{

public:

  ~LAVDetector();
  LAVDetector(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

  G4LogicalVolume* GetLAVLogicalVolume() { return fLAVVolume; }

private:

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fLAVVolume;
  LAVMessenger* fLAVMessenger;

};

#endif
