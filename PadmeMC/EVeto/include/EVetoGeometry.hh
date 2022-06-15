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

  void SetVerboseLevel(G4int v) { fVerbose = v; }
  G4int GetVerboseLevel() { return fVerbose; }

  void SetDetectorSetup(G4int s) { fDetectorSetup = s; }
  G4int GetDetectorSetup() { return fDetectorSetup; }

  // Position of center of EVeto box
  G4double GetEVetoPosX() { return fEVetoInnerFacePosX-0.5*GetEVetoSizeX(); }
  G4double GetEVetoPosY() { return 0.*cm; }
  G4double GetEVetoPosZ() { return fEVetoFrontFacePosZ+0.5*GetEVetoSizeZ(); }

  // Size of EVeto box
  G4double GetEVetoSizeX() { return fSupportSizeX; }
  G4double GetEVetoSizeY() { return fFingerSizeY+2.*fSupportSizeY; }
  G4double GetEVetoSizeZ() { return fSupportSizeZ; }

  // Number of fingers in EVeto
  G4int GetEVetoNFingers() { return fEVetoNFingers; }
  void  SetEVetoNFingers(G4int f) { fEVetoNFingers = f; }

  // Position within EVeto of center of finger at given index
  G4double GetFingerPosX(G4int);
  G4double GetFingerPosY(G4int);
  G4double GetFingerPosZ(G4int);

  // Rotation of fingers around Y axis
  G4double GetFingerRotY() { return fFingerRotY; }

  // Size of EVeto scintillator fingers
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
  G4double GetGroovePosX() { return -0.5*fFingerSizeX+0.5*fGrooveSizeX-5.*um; }
  G4double GetGroovePosY() { return 0.; }
  G4double GetGroovePosZ() { return 0.; }

  // Supports holding the fingers

  G4double GetSupportUSizeX() { return fSupportSizeX; }
  G4double GetSupportUSizeY() { return fSupportSizeY; }
  G4double GetSupportUSizeZ() { return fSupportSizeZ; }
  G4double GetSupportUPosX() { return 0.*cm; }
  G4double GetSupportUPosY() { return 0.5*GetEVetoSizeY()-0.5*fSupportSizeY; }
  G4double GetSupportUPosZ() { return 0.*cm; }

  G4double GetSupportDSizeX() { return fSupportSizeX; }
  G4double GetSupportDSizeY() { return fSupportSizeY; }
  G4double GetSupportDSizeZ() { return fSupportSizeZ; }
  G4double GetSupportDPosX() { return 0.*cm; }
  G4double GetSupportDPosY() { return -0.5*GetEVetoSizeY()+0.5*fSupportSizeY; }
  G4double GetSupportDPosZ() { return 0.*cm; }

  // Set position along X of EVeto internal face
  void SetEVetoInnerFacePosX(G4double x) { fEVetoInnerFacePosX = x; }

  // Set position along Z of EVeto front face
  void SetEVetoFrontFacePosZ(G4double z) { fEVetoFrontFacePosZ = z; }

  // Get name of EVeto digitizer
  G4String GetEVetoDigitizerName() { return fEVetoDigitizerName; }

  // Get name of EVeto sensitive detector
  G4String GetEVetoSensitiveDetectorName() { return fEVetoSensitiveDetectorName; }

private:

  G4int fVerbose; // Verbose level

  G4int fDetectorSetup;

  G4double fFingerDist0; // Distance of center of first finger from front face (in Z) of PVeto box
  G4double fFingerPitch; // Distance along Z between centers of two adjacent fingers

  G4int fEVetoNFingers;

  G4double fFingerSizeX;
  G4double fFingerSizeY;
  G4double fFingerSizeZ;

  G4double fFingerRotY; // Finger rotation around Y axis

  G4double fGrooveSizeX;
  G4double fGrooveSizeZ;

  G4double fSupportSizeX;
  G4double fSupportSizeY;
  G4double fSupportSizeZ;

  G4double fFingerCenterPosX;

  G4double fEVetoInnerFacePosX; // Position along X axis of EVeto inner face (i.e. inner face of support)
  G4double fEVetoFrontFacePosZ; // Position along Z axis of EVeto front face (i.e. front face of support)

  G4String fEVetoDigitizerName;
  G4String fEVetoSensitiveDetectorName;

};

#endif
