// TPixGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-04
// --------------------------------------------------------------

#ifndef TPixGeometry_H
#define TPixGeometry_H 1

#include <vector>

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

class G4LogicalVolume;

class TPixGeometry
{

public:

  ~TPixGeometry();
  static TPixGeometry* GetInstance();
  std::vector<G4String> GetHashTable();

private:

  static TPixGeometry* fInstance;

protected:

  TPixGeometry();

public:

  void SetVerboseLevel(G4int v) { fVerbose = v; }
  G4int GetVerboseLevel() { return fVerbose; }

  void SetDetectorSetup(G4int);
  G4int GetDetectorSetup() { return fDetectorSetup; }

  // Position of center of TPix main box
  G4double GetBoxPosX() { return fBoxPosX; }
  G4double GetBoxPosY() { return fBoxPosY; }
  G4double GetBoxPosZ() { return fBoxPosZ; }

  // Dimensions of TPix main box
  G4double GetBoxSizeX() { return fBoxSizeX; }
  G4double GetBoxSizeY() { return fBoxSizeY; }
  G4double GetBoxSizeZ() { return fBoxSizeZ; }

  // Displacement of center of TimePix within TPix box
  G4double GetTPixDispX() { return fTPixDispX; }
  G4double GetTPixDispY() { return fTPixDispY; }
  G4double GetTPixDispZ() { return fTPixDispZ; }

  // Absolute position of center of TPix
  G4double GetTPixPosX() { return fTPixPosX; }
  G4double GetTPixPosY() { return fTPixPosY; }
  G4double GetTPixPosZ() { return fTPixPosZ; }

  // Rotation of TPix box
  G4double GetTPixRotY() { return fTPixRotY; }

  // Size of TPix volume
  G4double GetTPixSizeX() { return fTPixSizeX; }
  G4double GetTPixSizeY() { return fTPixSizeY; }
  G4double GetTPixSizeZ() { return fTPixSizeZ; }

  // Get/Set number of chip rows/cols in TPix
  G4int GetTPixNRows() { return fTPixNRows; }
  G4int GetTPixNCols() { return fTPixNCols; }
  void  SetTPixNRows(G4int);
  void  SetTPixNCols(G4int);

  // Position of center of chip at given row/col
  G4double GetChipPosX(G4int,G4int);
  G4double GetChipPosY(G4int,G4int);
  G4double GetChipPosZ(G4int,G4int);

  // Size of TPix silicon chips
  G4double GetChipSizeX() { return fChipSizeX; }
  G4double GetChipSizeY() { return fChipSizeY; }
  G4double GetChipSizeZ() { return fChipSizeZ; }

  G4double GetCuSlabThick() { return fCuSlabThick; }

  G4double GetCuBarThick() { return fCuBarThick; }
  G4double GetCuBarHeight() { return fCuBarHeight; }

  G4double GetCuSuppThick() { return fCuSuppThick; }
  G4double GetCuSuppHeight() { return fCuSuppHeight; }
  G4double GetCuSuppWidth() { return fCuSuppWidth; }
  G4double GetCuHoleWidth() { return fCuHoleWidth; }

  G4double GetPCBBarThick() { return fPCBBarThick; }
  G4double GetPCBBarHeight() { return fPCBBarHeight; }
  G4double GetPCBBarDispZ() { return fPCBBarDispZ; }

  // Angle of vacuum chamber wall behind HEPVeto wrt X axis
  void SetTPixChamberWallAngle(G4double);

  // Thickness of the support structure between TPix and diagonal wall of the vacuum chamber
  G4double GetTPixSupportThickness() { return fTPixSupportThickness; }
  void SetTPixSupportThickness(G4double);

   // Distance from the corner on the back face of the vacuum chamber
  G4double GetTPixDistanceToCorner() { return fTPixDistanceToCorner; }
  void SetTPixDistanceToCorner(G4double);

  // Coordinates of the corner on the back face of the vacuum chamber
  void SetTPixChamberWallCorner(G4ThreeVector);

  // Get name of TPix digitizer
  G4String GetTPixDigitizerName() { return fTPixDigitizerName; }

  // Get name of TPix sensitive detector
  G4String GetTPixSensitiveDetectorName() { return fTPixSensitiveDetectorName; }

private:

  void UpdateDerivedMeasures();

  G4int fVerbose; // Verbose level

  G4int fDetectorSetup;

  G4int    fTPixNRows;
  G4int    fTPixNCols;

  G4double fChipSizeX;
  G4double fChipSizeY;
  G4double fChipSizeZ;

  G4double fChipStep; // Gap size between adjacent fingers

  G4double fTPixSizeX;
  G4double fTPixSizeY;
  G4double fTPixSizeZ;

  G4double fTPixPosX; // Absolute position along X axis of TPix center
  G4double fTPixPosY; // Absolute position along Y axis of TPix center
  G4double fTPixPosZ; // Absolute position along Z axis of TPix center

  G4double fTPixRotY; // Rotation of TPix around Y axis

  // TimePix main box dimensions
  G4double fBoxSizeX;
  G4double fBoxSizeY;
  G4double fBoxSizeZ;

  // TimePix main box position
  G4double fBoxPosX;
  G4double fBoxPosY;
  G4double fBoxPosZ;

  // TimePix displacement within main box
  G4double fTPixDispX;
  G4double fTPixDispY;
  G4double fTPixDispZ;

  // Copper support structure

  // Thin copper slab behind TimePix itself (X,Y dimensions = TimePix)
  G4double fCuSlabThick;

  // Top and bottom Cu bars behind the readout boards (X dimension = TimePix
  G4double fCuBarThick;
  G4double fCuBarHeight;

  // Cu support frame
  G4double fCuSuppThick;
  G4double fCuSuppWidth;
  G4double fCuSuppHeight;
  G4double fCuHoleWidth;

  // PCB bars
  G4double fPCBBarThick;
  G4double fPCBBarHeight;
  G4double fPCBBarDispZ; // Distance between back face of PCB and front face of TimePix

  // Angle of vacuum chamber wall behind HEPVeto wrt X axis
  G4double fTPixChamberWallAngle;

  // Distance from the corner on the back face of the vacuum chamber
  G4double fTPixDistanceToCorner;

  // Coordinates of the corner on the back face of the vacuum chamber
  G4ThreeVector fTPixChamberWallCorner;

  G4double fTPixSupportThickness; // Thickness of HEPVeto support structure

  G4String fTPixDigitizerName;
  G4String fTPixSensitiveDetectorName;

};

#endif
