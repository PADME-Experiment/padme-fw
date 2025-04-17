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

  // Size of long bars
  G4double GetETagBarSizeX() { return fETagBarSizeX; }
  G4double GetETagBarSizeY() { return fETagBarSizeY; }
  G4double GetETagBarSizeZ() { return fETagBarSizeZ; }

  // Size of short bars
  G4double GetETagShortBarSizeX() { return fETagShortBarSizeX; }
  G4double GetETagShortBarSizeY() { return fETagShortBarSizeY; }
  G4double GetETagShortBarSizeZ() { return fETagShortBarSizeZ; }

  // Thickness of paint around bars
  G4double GetETagBarPaintThick() { return fETagBarPaintThick; }

  // Vertical gap between bars
  G4double GetETagBarGapY() { return fETagBarGapY; }

  // Distance between back of bars and back of ETag structure
  G4double GetETagBarBackDisp() { return fETagBarBackDisp; }

  // Size of metal frame
  G4double GetETagFrameSizeX() { return fETagFrameSizeX; }
  G4double GetETagFrameSizeY() { return fETagFrameSizeY; }
  G4double GetETagFrameSizeZ() { return fETagFrameSizeZ; }
  G4double GetETagFrameThick() { return fETagFrameThick; }

  // Size of tedlar foil
  G4double GetETagTedlarSizeX() { return fETagTedlarSizeX; }
  G4double GetETagTedlarSizeY() { return fETagTedlarSizeY; }
  G4double GetETagTedlarSizeZ() { return fETagTedlarSizeZ; }

  // Position of ETag Volume
  G4double GetETagPosX() { return fETagPosX; }
  G4double GetETagPosY() { return fETagPosY; }
  G4double GetETagPosZ() { return fETagPosZ; }

  G4int GetETagNBars() { return fETagNBars; }

  // Set position along Z of ETag front face
  void SetETagFrontFacePosZ(G4double z) { fETagFrontFacePosZ = z; }
  
  // Get name of ETag digitizer
  G4String GetETagDigitizerName() { return fETagDigitizerName; }

  // Get name of ETag sensitive detector
  G4String GetETagSensitiveDetectorName() { return fETagSensitiveDetectorName; }

private:

  G4int fVerbose; // Verbose level

  G4int fDetectorSetup;

  G4double fETagSizeX;
  G4double fETagSizeY;
  G4double fETagSizeZ;

  G4double fETagPosX;
  G4double fETagPosY;
  G4double fETagPosZ;

  G4double fETagBarSizeX;
  G4double fETagBarSizeY;
  G4double fETagBarSizeZ;

  G4double fETagShortBarSizeX;
  G4double fETagShortBarSizeY;
  G4double fETagShortBarSizeZ;

  G4double fETagBarPaintThick;
  G4double fETagBarGapY;

  G4double fETagBarBackDisp;

  G4double fETagFrameSizeX;
  G4double fETagFrameSizeY;
  G4double fETagFrameSizeZ;
  G4double fETagFrameThick;

  G4double fETagTedlarSizeX;
  G4double fETagTedlarSizeY;
  G4double fETagTedlarSizeZ;

  G4int fETagNBars;

  G4double fETagFrontFacePosZ; // Position along Z axis of ETag front face

  G4String fETagSensitiveDetectorName;
  G4String fETagDigitizerName;

};

#endif
