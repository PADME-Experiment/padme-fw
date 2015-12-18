// LAVGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef LAVGeometry_H
#define LAVGeometry_H 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalVolume;

class LAVGeometry
{

public:

  ~LAVGeometry();
  static LAVGeometry* GetInstance();

private:

  static LAVGeometry* fInstance;

protected:

  LAVGeometry();

public:

  // Position of center of LAV tubs
  G4double GetLAVPosX() { return 0.; }
  G4double GetLAVPosY() { return 0.; }
  G4double GetLAVPosZ() { return fLAVFrontFacePosZ+fLAVZLength*0.5; }

  // Size of LAV tubs
  G4double GetLAVInnerRadius() { return fLAVInnerRadius; }
  G4double GetLAVOuterRadius() { return fLAVOuterRadius; }
  G4double GetLAVZLength()     { return fLAVZLength; }
  void SetLAVInnerRadius(G4double r) { fLAVInnerRadius = r; }
  void SetLAVOuterRadius(G4double r) { fLAVOuterRadius = r; }
  void SetLAVZLength(G4double z)     { fLAVZLength = z; }

  // Set position along Z of LAV front face
  void SetLAVFrontFacePosZ(G4double z) { fLAVFrontFacePosZ = z; }

  // Get name of LAV sensitive detector
  G4String GetLAVSensitiveDetectorName() { return fLAVSensitiveDetectorName; }

private:

  G4double fLAVInnerRadius;
  G4double fLAVOuterRadius;
  G4double fLAVZLength;

  G4double fLAVFrontFacePosZ; // Position along Z axis of LAV front face

  G4String fLAVSensitiveDetectorName;

};

#endif
