// MagnetStructure.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-01-11
// --------------------------------------------------------------

#ifndef MagnetStructure_H
#define MagnetStructure_H 1

#include "globals.hh"
#include "G4LogicalVolume.hh"

//#include "MagnetMessenger.hh" 

class MagnetStructure
{

public:

  ~MagnetStructure();
  MagnetStructure(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

  void SetMagnetVisible()   { fMagnetIsVisible = true;  }
  void SetMagnetInvisible() { fMagnetIsVisible = false; }

private:

  G4LogicalVolume* fMotherVolume;

  //MagnetMessenger* fMagnetMessenger;

  G4bool fMagnetIsVisible;

};

#endif
