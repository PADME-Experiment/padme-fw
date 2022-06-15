// ETagGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef ETagGeometry_H
#define ETagGeometry_H 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalVolume;

class ETagGeometry
{

public:

  ~ETagGeometry();
  static ETagGeometry* GetInstance();
  std::vector<G4String> GetHashTable();

private:

  static ETagGeometry* fInstance;

protected:

  ETagGeometry();

public:

  void SetDetectorSetup(G4int s) { fDetectorSetup = s; }
  G4int GetDetectorSetup() { return fDetectorSetup; }

  void SetVerboseLevel(G4int v) { fVerbose = v; }
  G4int GetVerboseLevel() { return fVerbose; }

  // Size of ETag
  G4double GetETagSizeX() { return fETagSizeX; }
  G4double GetETagSizeY() { return fETagSizeY; }
  G4double GetETagSizeZ() { return fETagSizeZ; }

  G4double GetETagBarSizeX() { return fETagBarSizeX; }
  G4double GetETagBarSizeY() { return fETagBarSizeY; }
  G4double GetETagBarSizeZ() { return fETagBarSizeZ; }

  G4double GetETagHoleSizeX() { return fETagHoleSizeX; }
  G4double GetETagHoleSizeY() { return fETagHoleSizeY; }
  G4double GetETagHoleSizeZ() { return fETagHoleSizeZ; }

  G4double GetETagShortBarSizeX() { return fETagShortBarSizeX; }
  G4double GetETagShortBarSizeY() { return fETagShortBarSizeY; }
  G4double GetETagShortBarSizeZ() { return fETagShortBarSizeZ; }

  // Position of ETag Volume
  G4double GetETagPosX() { return 0.; }
  G4double GetETagPosY() { return 0.; }
  G4double GetETagPosZ() { return fETagFrontFacePosZ+fETagSizeZ*0.5; }

  G4int GetETagNBars() { return fETagNBars; }

  // Set position along Z of ETag front face
  void SetETagFrontFacePosZ(G4double z) { fETagFrontFacePosZ = z; }
  
  // Get name of ETag sensitive detector
  G4String GetETagSensitiveDetectorName() { return fETagSensitiveDetectorName; }

private:

  G4int fVerbose; // Verbose level

  G4int fDetectorSetup;

  G4double fETagSizeX;
  G4double fETagSizeY;
  G4double fETagSizeZ;

  G4double fETagBarSizeX;
  G4double fETagBarSizeY;
  G4double fETagBarSizeZ;

  G4double fETagShortBarSizeX;
  G4double fETagShortBarSizeY;
  G4double fETagShortBarSizeZ;

  G4double fETagHoleSizeX;
  G4double fETagHoleSizeY;
  G4double fETagHoleSizeZ;
  
  G4int fETagNBars;

  G4double fETagFrontFacePosZ; // Position along Z axis of ETag front face
  G4String fETagSensitiveDetectorName;

};

#endif
