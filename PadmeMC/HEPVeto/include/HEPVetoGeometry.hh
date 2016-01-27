// HEPVetoGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef HEPVetoGeometry_H
#define HEPVetoGeometry_H 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalVolume;

class HEPVetoGeometry
{

public:

  ~HEPVetoGeometry();
  static HEPVetoGeometry* GetInstance();

private:

  static HEPVetoGeometry* fInstance;

protected:

  HEPVetoGeometry();

public:

  // Position of center of HEPVeto box
  G4double GetHEPVetoPosX() { return fHEPVetoCenterPosX; }
  G4double GetHEPVetoPosY() { return fHEPVetoCenterPosY; }
  G4double GetHEPVetoPosZ() { return fHEPVetoCenterPosZ; }

  // Rotation of HEPVeto box
  G4double GetHEPVetoRotX() { return fHEPVetoRotX; }
  G4double GetHEPVetoRotY() { return fHEPVetoRotY; }
  G4double GetHEPVetoRotZ() { return fHEPVetoRotZ; }

  // Size of HEPVeto box
  G4double GetHEPVetoSizeX() { return fFingerNominalSizeX; }
  G4double GetHEPVetoSizeY() { return fFingerNominalSizeY; }
  G4double GetHEPVetoSizeZ() { return fFingerNominalSizeZ*fHEPVetoNFingers; }

  // Number of fingers in HEPVeto
  G4int GetHEPVetoNFingers()        { return fHEPVetoNFingers; }
  void  SetHEPVetoNFingers(G4int f) { fHEPVetoNFingers = f; }

  // Position of center of finger at given index
  G4double GetFingerPosX(G4int);
  G4double GetFingerPosY(G4int);
  G4double GetFingerPosZ(G4int);

  // Size of HEPVeto scintillator fingers
  G4double GetFingerSizeX() { return fFingerNominalSizeX-fFingerGap; }
  G4double GetFingerSizeY() { return fFingerNominalSizeY-fFingerGap; }
  G4double GetFingerSizeZ() { return fFingerNominalSizeZ-fFingerGap; }

  // Set nominal size of finger
  void SetFingerNominalSizeX(G4double s) { fFingerNominalSizeX = s; }
  void SetFingerNominalSizeY(G4double s) { fFingerNominalSizeY = s; }
  void SetFingerNominalSizeZ(G4double s) { fFingerNominalSizeZ = s; }

  // Set position of HEPVeto center
  void SetHEPVetoPosX(G4double x) { fHEPVetoCenterPosX = x; }
  //void SetHEPVetoPosY(G4double y) { fHEPVetoCenterPosY = y; } // Always centered at 0
  void SetHEPVetoPosZ(G4double z) { fHEPVetoCenterPosZ = z; }

  // Set rotation angles of HEPVeto
  //void SetHEPVetoRotX(G4double a) { fHEPVetoRotX = a; }
  void SetHEPVetoRotY(G4double a) { fHEPVetoRotY = a; } // Can rotate only around Y axis
  //void SetHEPVetoRotZ(G4double a) { fHEPVetoRotZ = a; }

  // Get name of HEPVeto sensitive detector
  G4String GetHEPVetoSensitiveDetectorName() { return fHEPVetoSensitiveDetectorName; }

private:

  G4int    fHEPVetoNFingers;
  G4double fFingerNominalSizeX;
  G4double fFingerNominalSizeY;
  G4double fFingerNominalSizeZ;
  G4double fFingerGap; // Gap size between adjacent fingers

  G4double fHEPVetoCenterPosX; // Position along X axis of HEPVeto center
  G4double fHEPVetoCenterPosY; // Position along Y axis of HEPVeto center
  G4double fHEPVetoCenterPosZ; // Position along Z axis of HEPVeto center

  G4double fHEPVetoRotX; // Rotation of HEPVeto arount X axis
  G4double fHEPVetoRotY; // Rotation of HEPVeto arount Y axis
  G4double fHEPVetoRotZ; // Rotation of HEPVeto arount Z axis

  G4String fHEPVetoSensitiveDetectorName;

};

#endif
