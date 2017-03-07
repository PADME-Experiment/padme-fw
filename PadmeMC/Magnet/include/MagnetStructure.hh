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

#include "MagnetMessenger.hh" 

class MagnetStructure
{

public:

  ~MagnetStructure();
  MagnetStructure(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

  //G4LogicalVolume* GetMagneticVolume() { return fMagneticVolume; }

  //void EnableMagneticField()  { fMagneticField = 1; }
  //void DisableMagneticField() { fMagneticField = 0; }

  //void SetMagneticVolumeVisible()   { fMagneticVolumeIsVisible = 1; }
  //void SetMagneticVolumeInvisible() { fMagneticVolumeIsVisible = 0; }

private:

  G4LogicalVolume* fMotherVolume;
  //G4LogicalVolume* fMagneticVolume;

  MagnetMessenger* fMagnetMessenger;

  //G4int fMagneticField; // =1 Field ON, =0 Field OFF

  //G4int fMagneticVolumeIsVisible; // =1 Visible, =0 Invisible

};

#endif
