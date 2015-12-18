// EVetoGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef EVetoGeometry_H
#define EVetoGeometry_H 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalVolume;

class EVetoGeometry
{

public:

  ~EVetoGeometry();
  static EVetoGeometry* GetInstance();

private:

  static EVetoGeometry* fInstance;

protected:

  EVetoGeometry();

public:

  // Position of center of EVeto box
  G4double GetEVetoPosX();
  G4double GetEVetoPosY();
  G4double GetEVetoPosZ();

  // Size of EVeto box
  G4double GetEVetoSizeX();
  G4double GetEVetoSizeY();
  G4double GetEVetoSizeZ();

  // Number of fingers in EVeto
  G4int GetEVetoNFingers() { return fEVetoNFingers; }
  void  SetEVetoNFingers(G4int f) { fEVetoNFingers = f; }

  // Position of center of finger at given index
  G4double GetFingerPosX(G4int);
  G4double GetFingerPosY(G4int);
  G4double GetFingerPosZ(G4int);

  // Size of EVeto scintillator fingers
  G4double GetFingerSizeX();
  G4double GetFingerSizeY();
  G4double GetFingerSizeZ();

  // Set nominal size of finger
  void SetFingerNominalSizeX(G4double s) { fFingerNominalSizeX = s; }
  void SetFingerNominalSizeY(G4double s) { fFingerNominalSizeY = s; }
  void SetFingerNominalSizeZ(G4double s) { fFingerNominalSizeZ = s; }

  // Set position along Z of EVeto front face
  //void SetEVetoFrontFacePosZ(G4double z) { fEVetoFrontFacePosZ = z; }

  // Set position along Y of EVeto internal face
  void SetEVetoInnerFacePosY(G4double y) { fEVetoInnerFacePosY = y; }

  // Get name of EVeto sensitive detector
  G4String GetEVetoSensitiveDetectorName() { return fEVetoSensitiveDetectorName; }

private:

  G4double fFingerNominalSizeX;
  G4double fFingerNominalSizeY;
  G4double fFingerNominalSizeZ;

  G4int fEVetoNFingers;

  G4double fFingerGap; // Gap size between adjacent fingers

  //G4double fEVetoFrontFacePosZ; // Position along Z axis of EVeto front face
  G4double fEVetoInnerFacePosY; // Position along Y axis of EVeto inner face

  G4String fEVetoSensitiveDetectorName;

};

#endif
