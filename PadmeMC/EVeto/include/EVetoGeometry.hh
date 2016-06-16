// EVetoGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef EVetoGeometry_H
#define EVetoGeometry_H 1

#include <vector>

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalVolume;

class EVetoGeometry
{

public:

  ~EVetoGeometry();
  static EVetoGeometry* GetInstance();
  std::vector<G4String> GetHashTable();

private:

  static EVetoGeometry* fInstance;

protected:

  EVetoGeometry();

public:

  // Position of center of EVeto box
  G4double GetEVetoPosX() { return fEVetoInnerFacePosX+0.5*GetEVetoSizeX(); }
  G4double GetEVetoPosY() { return 0.*cm; }
  G4double GetEVetoPosZ() { return fEVetoFrontFacePosZ+0.5*GetEVetoSizeZ(); }

  // Size of EVeto box
  G4double GetEVetoSizeX() { return fSupportNominalSizeX; }
  G4double GetEVetoSizeY() { return fFingerNominalSizeY+fSupportGap+2.*fSupportNominalSizeY; }
  G4double GetEVetoSizeZ() { return fSupportNominalSizeZ; }

  // Number of fingers in EVeto
  G4int GetEVetoNFingers() { return fEVetoNFingers; }
  void  SetEVetoNFingers(G4int f) { fEVetoNFingers = f; }

  // Position within EVeto of center of finger at given index
  G4double GetFingerPosX(G4int);
  G4double GetFingerPosY(G4int);
  G4double GetFingerPosZ(G4int);

  // Rotation of fingers around Y axis
  //G4double GetFingerRotX();
  G4double GetFingerRotY() { return fFingerRotY; }
  //G4double GetFingerRotZ();

  // Size of EVeto scintillator fingers
  G4double GetFingerSizeX() { return fFingerNominalSizeX; }
  G4double GetFingerSizeY() { return fFingerNominalSizeY; }
  G4double GetFingerSizeZ() { return fFingerNominalSizeZ; }

  // Supports holding the fingers

  G4double GetSupportUSizeX() { return fSupportNominalSizeX-fSupportGap; }
  G4double GetSupportUSizeY() { return fSupportNominalSizeY-fSupportGap; }
  G4double GetSupportUSizeZ() { return fSupportNominalSizeZ-fSupportGap; }
  G4double GetSupportUPosX() { return 0.*cm; }
  G4double GetSupportUPosY() { return 0.5*(GetEVetoSizeY()-fSupportNominalSizeY); }
  G4double GetSupportUPosZ() { return 0.*cm; }

  G4double GetSupportDSizeX() { return fSupportNominalSizeX-fSupportGap; }
  G4double GetSupportDSizeY() { return fSupportNominalSizeY-fSupportGap; }
  G4double GetSupportDSizeZ() { return fSupportNominalSizeZ-fSupportGap; }
  G4double GetSupportDPosX() { return 0.*cm; }
  G4double GetSupportDPosY() { return -0.5*(GetEVetoSizeY()-fSupportNominalSizeY); }
  G4double GetSupportDPosZ() { return 0.*cm; }

  // Set nominal size of finger
  void SetFingerNominalSizeX(G4double s) { fFingerNominalSizeX = s; }
  void SetFingerNominalSizeY(G4double s) { fFingerNominalSizeY = s; }
  void SetFingerNominalSizeZ(G4double s) { fFingerNominalSizeZ = s; }

  // Set position along X of EVeto internal face
  void SetEVetoInnerFacePosX(G4double x) { fEVetoInnerFacePosX = x; }

  // Set position along Z of EVeto front face
  void SetEVetoFrontFacePosZ(G4double z) { fEVetoFrontFacePosZ = z; }

  // Get name of EVeto sensitive detector
  G4String GetEVetoSensitiveDetectorName() { return fEVetoSensitiveDetectorName; }

private:

  G4double fFingerGap; // Gap size between adjacent fingers

  G4int fEVetoNFingers;

  G4double fFingerNominalSizeX;
  G4double fFingerNominalSizeY;
  G4double fFingerNominalSizeZ;

  //G4double fFingerRotX;
  G4double fFingerRotY; // Finger rotation around Y axis
  //G4double fFingerRotZ;

  G4double fSupportGap; // Gap size between fingers and support

  G4double fSupportNominalSizeX;
  G4double fSupportNominalSizeY;
  G4double fSupportNominalSizeZ;

  G4double fEVetoInnerFacePosX; // Position along X axis of EVeto inner face (i.e. inner face of support)
  G4double fEVetoFrontFacePosZ; // Position along Z axis of EVeto front face (i.e. front face of support)

  G4String fEVetoSensitiveDetectorName;

};

#endif
