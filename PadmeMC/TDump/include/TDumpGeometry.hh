// TDumpGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-01-28
// --------------------------------------------------------------

#ifndef TDumpGeometry_H
#define TDumpGeometry_H 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalVolume;

class TDumpGeometry
{

public:

  ~TDumpGeometry();
  static TDumpGeometry* GetInstance();

private:

  static TDumpGeometry* fInstance;

protected:

  TDumpGeometry();

public:

  // Position of center of TDump tube
  G4double GetTDumpPosX() { return 0.*cm; }
  G4double GetTDumpPosY() { return 0.*cm; }
  G4double GetTDumpPosZ() { return fTDumpFrontFacePosZ+0.5*fTDumpLength; }

  // Size of TDump cylinder
  G4double GetTDumpInnerRadius() { return 0.*cm; }
  G4double GetTDumpOuterRadius() { return fTDumpRadius; }
  G4double GetTDumpSizeZ()       { return fTDumpLength; }

  // Size of lead bricks
  G4double GetBrickSizeX() { return fBrickSizeX; }
  G4double GetBrickSizeY() { return fBrickSizeY; }
  G4double GetBrickSizeZ() { return fBrickSizeZ; }

  // Number, position, and rotation around Z axis of lead bricks
  G4int GetNBricks() { return fNBricks; }
  G4double GetBrickPosX(G4int);
  G4double GetBrickPosY(G4int);
  G4double GetBrickPosZ(G4int);
  G4double GetBrickRotZ(G4int);

  void SetTDumpRadius(G4double r) { fTDumpRadius = r; }
  void SetTDumpLength(G4double l) { fTDumpLength = l; }

  // Set position along Z of TDump front face
  void SetTDumpFrontFacePosZ(G4double z) { fTDumpFrontFacePosZ = z; }

  // Get name of TDump sensitive detector
  G4String GetTDumpSensitiveDetectorName() { return fTDumpSensitiveDetectorName; }

private:

  G4double fTDumpRadius;
  G4double fTDumpLength;

  G4double fBrickSizeX;
  G4double fBrickSizeY;
  G4double fBrickSizeZ;

  G4int fNBricks;

  G4double fTDumpFrontFacePosZ; // Position along Z axis of TDump front face

  G4String fTDumpSensitiveDetectorName;

};

#endif
