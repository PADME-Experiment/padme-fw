// HEPVetoGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef HEPVetoGeometry_H
#define HEPVetoGeometry_H 1

#include <vector>

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

class G4LogicalVolume;

class HEPVetoGeometry
{

public:

  ~HEPVetoGeometry();
  static HEPVetoGeometry* GetInstance();
  std::vector<G4String> GetHashTable();

private:

  static HEPVetoGeometry* fInstance;

protected:

  HEPVetoGeometry();

public:

  // Position of center of HEPVeto box
  G4double GetHEPVetoPosX() { return fHEPVetoPosX; }
  G4double GetHEPVetoPosY() { return fHEPVetoPosY; }
  G4double GetHEPVetoPosZ() { return fHEPVetoPosZ; }

  // Size of HEPVeto box
  G4double GetHEPVetoSizeX() { return fHEPVetoSizeX; }
  G4double GetHEPVetoSizeY() { return fHEPVetoSizeY; }
  G4double GetHEPVetoSizeZ() { return fHEPVetoSizeZ; }

  // Position of center of finger at given index
  G4double GetFingerPosX(G4int);
  G4double GetFingerPosY(G4int);
  G4double GetFingerPosZ(G4int);

  // Size of HEPVeto scintillator fingers
  G4double GetFingerSizeX() { return fFingerSizeX; }
  G4double GetFingerSizeY() { return fFingerSizeY; }
  G4double GetFingerSizeZ() { return fFingerSizeZ; }
  void SetFingerSizeX(G4double s) { fFingerSizeX = s; UpdateDerivedMeasures(); }
  void SetFingerSizeY(G4double s) { fFingerSizeY = s; UpdateDerivedMeasures(); }
  void SetFingerSizeZ(G4double s) { fFingerSizeZ = s; UpdateDerivedMeasures(); }

  // Rotation angle of HEPVeto around Y axis
  G4double GetHEPVetoRotY() { return fHEPVetoRotY; }

  // Angle of vacuum chamber wall behind HEPVeto wrt X axis
  void SetHEPVetoChamberWallAngle(G4double a) { fHEPVetoChamberWallAngle = a; UpdateDerivedMeasures(); }

  // Number of fingers in HEPVeto
  G4int GetHEPVetoNFingers()        { return fHEPVetoNFingers; }
  void  SetHEPVetoNFingers(G4int f) { fHEPVetoNFingers = f; UpdateDerivedMeasures(); }

  // Thickness of the support structure between HEPVeto and diagonal wall of the vacuum chamber
  G4double GetHEPVetoSupportThickness() { return fHEPVetoSupportThickness; }
  void SetHEPVetoSupportThickness(G4double t) { fHEPVetoSupportThickness = t; UpdateDerivedMeasures(); }

  // Thickness of the wall of the vacuum chamber behind HEPVeto
  G4double GetHEPVetoChamberWallThickness() { return fHEPVetoChamberWallThickness; }
  void SetHEPVetoChamberWallThickness(G4double t) { fHEPVetoChamberWallThickness = t; UpdateDerivedMeasures(); }

   // Distance from the corner on the back face of the vacuum chamber
  G4double GetHEPVetoDistanceToCorner() { return fHEPVetoDistanceToCorner; }
  void SetHEPVetoDistanceToCorner(G4double d) { fHEPVetoDistanceToCorner = d; UpdateDerivedMeasures(); }

  // Coordinates of the corner on the back face of the vacuum chamber
  void SetHEPVetoChamberWallCorner(G4ThreeVector c) { fHEPVetoChamberWallCorner = c; UpdateDerivedMeasures(); }

  // Get name of HEPVeto sensitive detector
  G4String GetHEPVetoSensitiveDetectorName() { return fHEPVetoSensitiveDetectorName; }

private:

  void UpdateDerivedMeasures();

  // Number of fingers in HEPVeto
  G4int fHEPVetoNFingers;

  // Size of scintillator finger
  G4double fFingerSizeX;
  G4double fFingerSizeY;
  G4double fFingerSizeZ;

  G4double fFingerStep; // Step between fingers

  G4double fHEPVetoPosX; // Position along X axis of HEPVeto center
  G4double fHEPVetoPosY; // Position along Y axis of HEPVeto center
  G4double fHEPVetoPosZ; // Position along Z axis of HEPVeto center

  G4double fHEPVetoSizeX; // Size of HEPVeto box
  G4double fHEPVetoSizeY; // Size of HEPVeto box
  G4double fHEPVetoSizeZ; // Size of HEPVeto box

  G4double fHEPVetoRotY; // Rotation of HEPVeto around Y axis

  // Angle of vacuum chamber wall behind HEPVeto wrt X axis
  G4double fHEPVetoChamberWallAngle;

  // Distance from the corner on the back face of the vacuum chamber
  G4double fHEPVetoDistanceToCorner;

  // Coordinates of the corner on the back face of the vacuum chamber
  G4ThreeVector fHEPVetoChamberWallCorner;

  G4double fHEPVetoSupportThickness; // Thickness of HEPVeto support structure
  G4double fHEPVetoChamberWallThickness; // Thickness of vacuum chamber wall

  G4String fHEPVetoSensitiveDetectorName;

};

#endif
