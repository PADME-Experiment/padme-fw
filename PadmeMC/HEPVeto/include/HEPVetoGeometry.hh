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

  void SetVerboseLevel(G4int v) { fVerbose = v; }
  G4int GetVerboseLevel() { return fVerbose; }

  void SetDetectorSetup(G4int s) { fDetectorSetup = s; }
  G4int GetDetectorSetup() { return fDetectorSetup; }

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

  // Rotation of fingers around Y axis M. Raggi 23/06/2018
  G4double GetFingerRotY() { return fFingerRotY; }

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
  void SetHEPVetoChamberWallAngle(G4double);

  // Number of fingers in HEPVeto
  G4int GetHEPVetoNFingers()        { return fHEPVetoNFingers; }
  //void  SetHEPVetoNFingers(G4int f) { fHEPVetoNFingers = f; UpdateDerivedMeasures(); }
  void  SetHEPVetoNFingers(G4int f) { fHEPVetoNFingers = f; }

  // Thickness of the support structure between HEPVeto and diagonal wall of the vacuum chamber
  //G4double GetHEPVetoSupportThickness() { return fHEPVetoSupportThickness; }
  //void SetHEPVetoSupportThickness(G4double t) { fHEPVetoSupportThickness = t; UpdateDerivedMeasures(); }

  // Thickness of the wall of the vacuum chamber behind HEPVeto
  G4double GetHEPVetoChamberWallThickness() { return fHEPVetoChamberWallThickness; }
  void SetHEPVetoChamberWallThickness(G4double);

   // Distance from the corner on the back face of the vacuum chamber
  G4double GetHEPVetoDistanceToCorner() { return fHEPVetoDistanceToCorner; }
  void SetHEPVetoDistanceToCorner(G4double);

  // Coordinates of the corner on the back face of the vacuum chamber
  void SetHEPVetoChamberWallCorner(G4ThreeVector);

  // Get name of HEPVeto digitizer
  G4String GetHEPVetoDigitizerName() { return fHEPVetoDigitizerName; }

  // Get name of HEPVeto sensitive detector
  G4String GetHEPVetoSensitiveDetectorName() { return fHEPVetoSensitiveDetectorName; }

  // Size of groove on finger back side (add some tolerances for solid subtraction) M. Raggi
  G4double GetGrooveSizeX() { return fGrooveSizeX+10.*um; }
  G4double GetGrooveSizeY() { return fFingerSizeY+10.*um; }
  G4double GetGrooveSizeZ() { return fGrooveSizeZ; }
  G4double GetGroovePosX() { return 0.5*fFingerSizeX-0.5*fGrooveSizeX+5.*um; }
  G4double GetGroovePosY() { return 0.; }
  G4double GetGroovePosZ() { return 0.; }

private:

  void UpdateDerivedMeasures();

  G4int fVerbose; // Verbose level

  G4int fDetectorSetup;

  // Number of fingers in HEPVeto
  G4int fHEPVetoNFingers;

  // Size of scintillator finger
  G4double fFingerSizeX;
  G4double fFingerSizeY;
  G4double fFingerSizeZ;
  
  // M. Raggi
  G4double fGrooveSizeX;
  G4double fGrooveSizeZ;

  G4double fFingerRotY; // Finger rotation around Y axis

  G4double fFingerStep; // Step between fingers

  G4double fHEPVetoPosX; // Position along X axis of HEPVeto center
  G4double fHEPVetoPosY; // Position along Y axis of HEPVeto center
  G4double fHEPVetoPosZ; // Position along Z axis of HEPVeto center

  G4double fHEPVetoSizeX; // Size of HEPVeto box
  G4double fHEPVetoSizeY; // Size of HEPVeto box
  G4double fHEPVetoSizeZ; // Size of HEPVeto box

  G4double fFirstFingerDistX; // Position along X of center of first finger wrt border of T-shaped support

  G4double fFingersDistZ; // Position along Z of center of fingers wrt border of vertical support

  G4double fHEPVetoRotY; // Rotation of HEPVeto around Y axis

  // Distance between back face of the vertical support and the internal wall of the vacuum chamber
  G4double fHEPVetoDistanceToChamberWall;

  // Angle of vacuum chamber wall behind HEPVeto wrt X axis
  G4double fHEPVetoChamberWallAngle;

  // Distance from the corner on the back face of the vacuum chamber
  G4double fHEPVetoDistanceToCorner;

  // Coordinates of the corner on the back face of the vacuum chamber
  G4ThreeVector fHEPVetoChamberWallCorner;

  //G4double fHEPVetoSupportThickness; // Thickness of HEPVeto support structure
  G4double fHEPVetoChamberWallThickness; // Thickness of vacuum chamber wall

  G4String fHEPVetoDigitizerName;
  G4String fHEPVetoSensitiveDetectorName;

};

#endif
