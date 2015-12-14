// ECalDetector.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#ifndef ECalDetector_H
#define ECalDetector_H 1

#include "globals.hh"
#include "G4LogicalVolume.hh"

//class ECalDetectorMessenger;

class ECalDetector
{

public:

  ~ECalDetector();
  ECalDetector(G4LogicalVolume*);

  void CreateGeometry();

private:

  G4LogicalVolume* fMotherVolume;

  //ECalDetectorMessenger* fECalMessenger;

};

#endif
