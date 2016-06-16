// PVetoGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef PVetoGeometry_H
#define PVetoGeometry_H 1

#include <vector>

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalVolume;

class PVetoGeometry
{

public:

  ~PVetoGeometry();
  static PVetoGeometry* GetInstance();
  std::vector<G4String> GetHashTable();

private:

  static PVetoGeometry* fInstance;

protected:

  PVetoGeometry();

public:

  // Position of center of PVeto box
  G4double GetPVetoPosX() { return fPVetoInnerFacePosX-0.5*GetPVetoSizeX(); }
  G4double GetPVetoPosY() { return 0.*cm; }
  G4double GetPVetoPosZ() { return fPVetoFrontFacePosZ+0.5*GetPVetoSizeZ(); }

  // Size of PVeto box
  G4double GetPVetoSizeX() { return fSupportNominalSizeX; }
  G4double GetPVetoSizeY() { return fFingerNominalSizeY+fSupportGap+2.*fSupportNominalSizeY; }
  G4double GetPVetoSizeZ() { return fSupportNominalSizeZ; }

  // Number of fingers in PVeto
  G4int GetPVetoNFingers() { return fPVetoNFingers; }
  void  SetPVetoNFingers(G4int f) { fPVetoNFingers = f; }

  // Position within PVeto of center of finger at given index
  G4double GetFingerPosX(G4int);
  G4double GetFingerPosY(G4int);
  G4double GetFingerPosZ(G4int);

  // Rotation of fingers around Y axis
  //G4double GetFingerRotX();
  G4double GetFingerRotY() { return fFingerRotY; }
  //G4double GetFingerRotZ();

  // Size of PVeto scintillator fingers
  G4double GetFingerSizeX() { return fFingerNominalSizeX; }
  G4double GetFingerSizeY() { return fFingerNominalSizeY; }
  G4double GetFingerSizeZ() { return fFingerNominalSizeZ; }

  // Supports holding the fingers

  G4double GetSupportUSizeX() { return fSupportNominalSizeX-fSupportGap; }
  G4double GetSupportUSizeY() { return fSupportNominalSizeY-fSupportGap; }
  G4double GetSupportUSizeZ() { return fSupportNominalSizeZ-fSupportGap; }
  G4double GetSupportUPosX() { return 0.*cm; }
  G4double GetSupportUPosY() { return 0.5*(GetPVetoSizeY()-fSupportNominalSizeY); }
  G4double GetSupportUPosZ() { return 0.*cm; }

  G4double GetSupportDSizeX() { return fSupportNominalSizeX-fSupportGap; }
  G4double GetSupportDSizeY() { return fSupportNominalSizeY-fSupportGap; }
  G4double GetSupportDSizeZ() { return fSupportNominalSizeZ-fSupportGap; }
  G4double GetSupportDPosX() { return 0.*cm; }
  G4double GetSupportDPosY() { return -0.5*(GetPVetoSizeY()-fSupportNominalSizeY); }
  G4double GetSupportDPosZ() { return 0.*cm; }

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

  G4double fFingerGap; // Gap size between adjacent fingers

  G4int fPVetoNFingers;

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

  G4double fPVetoInnerFacePosX; // Position along X axis of PVeto inner face (i.e. inner face of support)
  G4double fPVetoFrontFacePosZ; // Position along Z axis of PVeto front face (i.e. front face of support)

  G4String fPVetoSensitiveDetectorName;

};

#endif
