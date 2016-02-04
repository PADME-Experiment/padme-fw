// TPixGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-04
// --------------------------------------------------------------

#ifndef TPixGeometry_H
#define TPixGeometry_H 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalVolume;

class TPixGeometry
{

public:

  ~TPixGeometry();
  static TPixGeometry* GetInstance();

private:

  static TPixGeometry* fInstance;

protected:

  TPixGeometry();

public:

  // Position of center of TPix box
  G4double GetTPixPosX() { return fTPixCenterPosX; }
  G4double GetTPixPosY() { return fTPixCenterPosY; }
  G4double GetTPixPosZ() { return fTPixCenterPosZ; }

  // Rotation of TPix box
  G4double GetTPixRotX() { return fTPixRotX; }
  G4double GetTPixRotY() { return fTPixRotY; }
  G4double GetTPixRotZ() { return fTPixRotZ; }

  // Size of TPix box
  G4double GetTPixSizeX() { return fChipNominalSizeX+fChipGap; }
  G4double GetTPixSizeY() { return fTPixNRows*(fChipNominalSizeY+fChipGap); }
  G4double GetTPixSizeZ() { return fTPixNCols*(fChipNominalSizeZ+fChipGap); }

  // Get/Set number of chip rows/cols in TPix
  G4int GetTPixNRows()        { return fTPixNRows; }
  //void  SetTPixNRows(G4int r) { fTPixNRows = r; }
  G4int GetTPixNCols()        { return fTPixNCols; }
  void  SetTPixNCols(G4int c) { fTPixNCols = c; }

  // Position of center of chip at given row/col
  G4double GetChipPosX(G4int,G4int);
  G4double GetChipPosY(G4int,G4int);
  G4double GetChipPosZ(G4int,G4int);

  // Size of TPix silicon chips
  G4double GetChipSizeX() { return fChipNominalSizeX; }
  G4double GetChipSizeY() { return fChipNominalSizeY; }
  G4double GetChipSizeZ() { return fChipNominalSizeZ; }

  // Set position of TPix center
  void SetTPixPosX(G4double x) { fTPixCenterPosX = x; }
  //void SetTPixPosY(G4double y) { fTPixCenterPosY = y; } // Always centered at 0
  void SetTPixPosZ(G4double z) { fTPixCenterPosZ = z; }

  // Set rotation angles of TPix
  //void SetTPixRotX(G4double a) { fTPixRotX = a; }
  void SetTPixRotY(G4double a) { fTPixRotY = a; } // Can rotate only around Y axis
  //void SetTPixRotZ(G4double a) { fTPixRotZ = a; }

  // Get name of TPix sensitive detector
  G4String GetTPixSensitiveDetectorName() { return fTPixSensitiveDetectorName; }

private:

  G4int    fTPixNRows;
  G4int    fTPixNCols;

  G4double fChipNominalSizeX;
  G4double fChipNominalSizeY;
  G4double fChipNominalSizeZ;

  G4double fChipGap; // Gap size between adjacent fingers

  G4double fTPixCenterPosX; // Position along X axis of TPix center
  G4double fTPixCenterPosY; // Position along Y axis of TPix center
  G4double fTPixCenterPosZ; // Position along Z axis of TPix center

  G4double fTPixRotX; // Rotation of TPix around X axis
  G4double fTPixRotY; // Rotation of TPix around Y axis
  G4double fTPixRotZ; // Rotation of TPix around Z axis

  G4String fTPixSensitiveDetectorName;

};

#endif
