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

  // Position of center of TDump
  G4double GetTDumpPosX() { return 0.*cm; }
  G4double GetTDumpPosY() { return 0.*cm; }
  G4double GetTDumpPosZ() { return fTDumpFrontFacePosZ+0.5*fTDumpSizeZ; }

  // Size of TDump
  G4double GetTDumpSizeX() { return fTDumpSizeX; }
  G4double GetTDumpSizeY() { return fTDumpSizeY; }
  G4double GetTDumpSizeZ() { return fTDumpSizeZ; }

  // Position of center of TDump target in local coordinates
  G4double GetTargetPosX() { return 0.*cm; }
  G4double GetTargetPosY() { return 0.*cm; }
  G4double GetTargetPosZ() { return -0.5*fTDumpSizeZ+fTargetFrontFacePosZ+0.5*fTargetLength; }

  // Size of TDump target
  G4double GetTargetInnerRadius() { return 0.*cm; }
  G4double GetTargetOuterRadius() { return fTargetRadius; }
  G4double GetTargetSizeZ()       { return fTargetLength; }

  // Size of lead bricks
  G4double GetBrickSizeX() { return fBrickNominalSizeX-fTDumpGap; }
  G4double GetBrickSizeY() { return fBrickNominalSizeY-fTDumpGap; }
  G4double GetBrickSizeZ() { return fBrickNominalSizeZ-fTDumpGap; }

  // Number, position, and rotation around Z axis of lead bricks
  G4int GetNBricks() { return fNBricks; }
  G4double GetBrickPosX(G4int);
  G4double GetBrickPosY(G4int);
  G4double GetBrickPosZ(G4int);
  G4double GetBrickRotZ(G4int);

  // Set dimensions of the Tdump cylinder
  void SetTargetRadius(G4double r) { fTargetRadius = r; }
  void SetTargetLength(G4double l) { fTargetLength = l; }

  // Set position along Z of TDump cylinder front face wrt TDump front face
  void SetTargetFrontFacePosZ(G4double z) { fTargetFrontFacePosZ = z; }

  // Set position along Z of TDump front face
  void SetTDumpFrontFacePosZ(G4double z) { fTDumpFrontFacePosZ = z; }

  // Get name of TDump sensitive detector
  G4String GetTDumpSensitiveDetectorName() { return fTDumpSensitiveDetectorName; }

private:

  G4double fTDumpGap;

  G4double fTargetRadius;
  G4double fTargetLength;

  G4double fTargetFrontFacePosZ; // Position along Z axis of TDump target front face in local coordinates

  G4int fNBricks;

  G4double fBrickNominalSizeX;
  G4double fBrickNominalSizeY;
  G4double fBrickNominalSizeZ;

  G4double fTDumpSizeX;
  G4double fTDumpSizeY;
  G4double fTDumpSizeZ;

  G4double fTDumpFrontFacePosZ; // Position along Z axis of TDump front face wrt TDump front face

  G4String fTDumpSensitiveDetectorName;

};

#endif
