// TargetDetector.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#ifndef TargetDetector_H
#define TargetDetector_H 1

#include "globals.hh"
#include "G4LogicalVolume.hh"

#include "TargetGeometry.hh"
#include "TargetMessenger.hh"

class TargetDetector
{

public:

  ~TargetDetector();
  TargetDetector(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

  G4LogicalVolume* GetTargetLogicalVolume() { return fTargetVolume; }

  G4double GetTargetFrontFaceZ() { return TargetGeometry::GetInstance()->GetTargetFrontFacePosZ(); }
  G4double GetTargetThickness()  { return TargetGeometry::GetInstance()->GetTargetSizeZ(); }

private:

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fTargetVolume;

  TargetMessenger* fTargetMessenger;

};

#endif
