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
  G4double GetPVetoPosX() { return fPVetoInnerFacePosX+0.5*GetPVetoSizeX(); }
  G4double GetPVetoPosY() { return 0.*cm; }
  G4double GetPVetoPosZ() { return fPVetoFrontFacePosZ+0.5*GetPVetoSizeZ(); }

  // Size of PVeto box
  G4double GetPVetoSizeX() { return fSupportSizeX; }
  G4double GetPVetoSizeY() { return fFingerSizeY+2.*fSupportSizeY; }
  G4double GetPVetoSizeZ() { return fSupportSizeZ; }

  // Number of fingers in PVeto
  G4int GetPVetoNFingers() { return fPVetoNFingers; }
  void  SetPVetoNFingers(G4int f) { fPVetoNFingers = f; }

  // Position within PVeto of center of finger at given index
  G4double GetFingerPosX(G4int);
  G4double GetFingerPosY(G4int);
  G4double GetFingerPosZ(G4int);

  // Rotation of fingers around Y axis
  G4double GetFingerRotY() { return fFingerRotY; }

  // Size of PVeto scintillator fingers
  G4double GetFingerSizeX() { return fFingerSizeX; }
  G4double GetFingerSizeY() { return fFingerSizeY; }
  G4double GetFingerSizeZ() { return fFingerSizeZ; }
  void SetFingerSizeX(G4double s) { fFingerSizeX = s; }
  void SetFingerSizeY(G4double s) { fFingerSizeY = s; }
  void SetFingerSizeZ(G4double s) { fFingerSizeZ = s; }

  // Size of groove on finger back side (add some tolerances for solid subtraction)
  G4double GetGrooveSizeX() { return fGrooveSizeX+10.*um; }
  G4double GetGrooveSizeY() { return fFingerSizeY+10.*um; }
  G4double GetGrooveSizeZ() { return fGrooveSizeZ; }
  G4double GetGroovePosX() { return 0.5*fFingerSizeX-0.5*fGrooveSizeX+5.*um; }
  G4double GetGroovePosY() { return 0.; }
  G4double GetGroovePosZ() { return 0.; }

  // Supports holding the fingers

  G4double GetSupportUSizeX() { return fSupportSizeX; }
  G4double GetSupportUSizeY() { return fSupportSizeY; }
  G4double GetSupportUSizeZ() { return fSupportSizeZ; }
  G4double GetSupportUPosX() { return 0.*cm; }
  G4double GetSupportUPosY() { return 0.5*GetPVetoSizeY()-0.5*fSupportSizeY; }
  G4double GetSupportUPosZ() { return 0.*cm; }

  G4double GetSupportDSizeX() { return fSupportSizeX; }
  G4double GetSupportDSizeY() { return fSupportSizeY; }
  G4double GetSupportDSizeZ() { return fSupportSizeZ; }
  G4double GetSupportDPosX() { return 0.*cm; }
  G4double GetSupportDPosY() { return -0.5*GetPVetoSizeY()+0.5*fSupportSizeY; }
  G4double GetSupportDPosZ() { return 0.*cm; }

  // Set position along X of PVeto internal face
  void SetPVetoInnerFacePosX(G4double x) { fPVetoInnerFacePosX = x; }

  // Set position along Z of PVeto front face
  void SetPVetoFrontFacePosZ(G4double z) { fPVetoFrontFacePosZ = z; }

  // Get name of PVeto sensitive detector
  G4String GetPVetoSensitiveDetectorName() { return fPVetoSensitiveDetectorName; }

private:

  G4double fFingerDist0; // Distance of center of first finger from front face (in Z) of PVeto box
  G4double fFingerPitch; // Distance along Z between centers of two adjacent fingers

  G4int fPVetoNFingers;

  G4double fFingerSizeX;
  G4double fFingerSizeY;
  G4double fFingerSizeZ;

  G4double fFingerRotY; // Finger rotation around Y axis

  G4double fGrooveSizeX;
  G4double fGrooveSizeZ;

  G4double fSupportSizeX;
  G4double fSupportSizeY;
  G4double fSupportSizeZ;

  G4double fPVetoInnerFacePosX; // Position along X axis of PVeto inner face (i.e. inner face of support)
  G4double fPVetoFrontFacePosZ; // Position along Z axis of PVeto front face (i.e. front face of support)

  G4String fPVetoSensitiveDetectorName;

};

#endif
