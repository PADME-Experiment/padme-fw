// ECalGeometryParameters.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef ECalGeometryParameters_H
#define ECalGeometryParameters_H 1

#include "globals.hh"

class G4LogicalVolume;

//class ECalMessenger;

class ECalGeometryParameters
{

public:

  ~ECalGeometryParameters();
  static ECalGeometryParameters* GetInstance();

private:

  static ECalGeometryParameters* fInstance;

protected:

  ECalGeometryParameters();

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
  G4int GetECalNRows();
  G4int GetECalNColumns();

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

private:

  G4double fCrystalNominalSizeX;
  G4double fCrystalNominalSizeY;
  G4double fCrystalNominalSizeZ;

  G4int fECalNRows;
  G4int fECalNColumns;

  G4double fCrystalGap; // Gap size between adjacent crystals

  G4double fECalFrontFacePosZ;

  G4double fECalInnerRadius; // Radius of hole at ECal center
  G4double fECalOuterRadius; // External radius of ECal

  //ECalMessenger* fECalMessenger;

};

#endif
