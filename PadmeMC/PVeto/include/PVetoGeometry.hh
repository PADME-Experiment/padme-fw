// PVetoGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef PVetoGeometry_H
#define PVetoGeometry_H 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalVolume;

class PVetoGeometry
{

public:

  ~PVetoGeometry();
  static PVetoGeometry* GetInstance();

private:

  static PVetoGeometry* fInstance;

protected:

  PVetoGeometry();

public:

  // Position of center of PVeto box
  G4double GetPVetoPosX();
  G4double GetPVetoPosY();
  G4double GetPVetoPosZ();

  // Size of PVeto box
  G4double GetPVetoSizeX();
  G4double GetPVetoSizeY();
  G4double GetPVetoSizeZ();

  // Number of fingers in PVeto
  G4int GetPVetoNFingers() { return fPVetoNFingers; }
  void  SetPVetoNFingers(G4int f) { fPVetoNFingers = f; }

  // Position of center of finger at given index
  G4double GetFingerPosX(G4int);
  G4double GetFingerPosY(G4int);
  G4double GetFingerPosZ(G4int);

  // Size of PVeto scintillator fingers
  G4double GetFingerSizeX();
  G4double GetFingerSizeY();
  G4double GetFingerSizeZ();

  // Set nominal size of finger
  void SetFingerNominalSizeX(G4double s) { fFingerNominalSizeX = s; }
  void SetFingerNominalSizeY(G4double s) { fFingerNominalSizeY = s; }
  void SetFingerNominalSizeZ(G4double s) { fFingerNominalSizeZ = s; }

  // Set position along X of PVeto internal face
  void SetPVetoInnerFacePosX(G4double x) { fPVetoInnerFacePosX = x; }

  // Set position along Z of PVeto front face
  void SetPVetoFrontFacePosZ(G4double z) { fPVetoFrontFacePosZ = z; }

  // Get name of PVeto sensitive detector
  G4String GetPVetoSensitiveDetectorName() { return fPVetoSensitiveDetectorName; }

private:

  G4double fFingerNominalSizeX;
  G4double fFingerNominalSizeY;
  G4double fFingerNominalSizeZ;

  G4int fPVetoNFingers;

  G4double fFingerGap; // Gap size between adjacent fingers

  G4double fPVetoInnerFacePosX; // Position along X axis of PVeto inner face
  G4double fPVetoFrontFacePosZ; // Position along Z axis of PVeto front face

  G4String fPVetoSensitiveDetectorName;

};

#endif
