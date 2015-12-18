// ECalGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef ECalGeometry_H
#define ECalGeometry_H 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalVolume;

class ECalGeometry
{

public:

  ~ECalGeometry();
  static ECalGeometry* GetInstance();

private:

  static ECalGeometry* fInstance;

protected:

  ECalGeometry();

public:

  // Position of center of ECal box
  G4double GetECalPosX();
  G4double GetECalPosY();
  G4double GetECalPosZ();

  // Size of ECal box
  G4double GetECalSizeX();
  G4double GetECalSizeY();
  G4double GetECalSizeZ();

  // Number of rows and columns of crystals in ECAL
  G4int GetECalNRows() { return fECalNRows; }
  G4int GetECalNCols() { return fECalNCols; }
  void  SetECalNRows(G4int r) { fECalNRows = r; }
  void  SetECalNCols(G4int c) { fECalNCols = c; }

  // Check if crystal exists at given row/column
  G4int ExistsCrystalAt(G4int,G4int);

  // Position of center of crystal at given row/column
  G4double GetCrystalPosX(G4int,G4int);
  G4double GetCrystalPosY(G4int,G4int);
  G4double GetCrystalPosZ(G4int,G4int);

  // Size of BGO crystal
  G4double GetCrystalSizeX();
  G4double GetCrystalSizeY();
  G4double GetCrystalSizeZ();

  // Set nominal size of crystal
  void SetCrystalNominalSizeX(G4double s) { fCrystalNominalSizeX = s; }
  void SetCrystalNominalSizeY(G4double s) { fCrystalNominalSizeY = s; }
  void SetCrystalNominalSizeZ(G4double s) { fCrystalNominalSizeZ = s; }

  // Set position along Z of ECal front face
  void SetECalFrontFacePosZ(G4double z) { fECalFrontFacePosZ = z; }

  // Set inner and outer radii of ECal
  void SetECalInnerRadius(G4double r) { fECalInnerRadius = r; }
  void SetECalOuterRadius(G4double r) { fECalOuterRadius = r; }

  // Get name of ECal sensitive detector
  G4String GetECalSensitiveDetectorName() { return fECalSensitiveDetectorName; }

private:

  G4double fCrystalNominalSizeX;
  G4double fCrystalNominalSizeY;
  G4double fCrystalNominalSizeZ;

  G4int fECalNRows;
  G4int fECalNCols;

  G4double fCrystalGap; // Gap size between adjacent crystals

  G4double fECalFrontFacePosZ; // Position along Z axis of ECal front face

  G4double fECalInnerRadius; // Radius of hole at ECal center
  G4double fECalOuterRadius; // External radius of ECal

  G4String fECalSensitiveDetectorName;

};

#endif
