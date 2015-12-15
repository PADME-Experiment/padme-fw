// TargetGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef TargetGeometry_H
#define TargetGeometry_H 1

#include "globals.hh"

class G4LogicalVolume;

class TargetGeometry
{

public:

  ~TargetGeometry();
  static TargetGeometry* GetInstance();

private:

  static TargetGeometry* fInstance;

protected:

  TargetGeometry();

public:

  // Position of center of Target box
  G4double GetTargetPosX();
  G4double GetTargetPosY();
  G4double GetTargetPosZ();

  // Size of Target box
  G4double GetTargetSizeX() { return fTargetSizeX; }
  G4double GetTargetSizeY() { return fTargetSizeY; }
  G4double GetTargetSizeZ() { return fTargetSizeZ; }
  void     SetTargetSizeX(G4double s) { fTargetSizeX = s; }
  void     SetTargetSizeY(G4double s) { fTargetSizeY = s; }
  void     SetTargetSizeZ(G4double s) { fTargetSizeZ = s; }

  // Set position along Z of Target front face
  void SetTargetFrontFacePosZ(G4double z) { fTargetFrontFacePosZ = z; }

  // Get name of Target sensitive detector
  G4String GetTargetSensitiveDetectorName() { return fTargetSensitiveDetectorName; }

private:

  G4double fTargetSizeX;
  G4double fTargetSizeY;
  G4double fTargetSizeZ;

  G4double fTargetFrontFacePosZ; // Position along Z axis of Target front face

  G4String fTargetSensitiveDetectorName;

};

#endif
