// ChamberGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-06-10
// --------------------------------------------------------------

#ifndef ChamberGeometry_H
#define ChamberGeometry_H 1

#define CHAMBERGEOMETRY_VCINMAGWALL_NVERTICES 14
#define CHAMBERGEOMETRY_VCINMAGPLATE_NVERTICES 6
#define CHAMBERGEOMETRY_VCOUTMAGWALL_NVERTICES 6

#include "globals.hh"
#include "G4TwoVector.hh"
#include "G4SystemOfUnits.hh"

class ChamberGeometry
{

public:

  ~ChamberGeometry();
  static ChamberGeometry* GetInstance();

private:

  static ChamberGeometry* fInstance;

protected:

  ChamberGeometry();

public:

  G4double GetVCInMagThick() { return fVCInMagThick; }

  G4double GetVCInMagWallSizeY() { return fVCInMagWallSizeY; }
  G4int GetVCInMagWallNVertices() { return CHAMBERGEOMETRY_VCINMAGWALL_NVERTICES; }
  G4TwoVector GetVCInMagWallVertex(G4int p) { return fVCInMagWallVertex[p]; }

  G4double GetVCInMagWallHoleRadius() { return fVCInMagWallHoleRadius; }
  G4double GetVCInMagWallHolePosZ() { return fVCInMagWallHolePosZ; }

  G4int GetVCInMagPlateNVertices() { return CHAMBERGEOMETRY_VCINMAGPLATE_NVERTICES; }
  G4TwoVector GetVCInMagPlateVertex(G4int p) { return fVCInMagPlateVertex[p]; }

  G4double GetVCOutMagWallSizeY() { return fVCInMagWallSizeY; }
  G4int GetVCOutMagWallNVertices() { return CHAMBERGEOMETRY_VCOUTMAGWALL_NVERTICES; }
  G4TwoVector GetVCOutMagWallVertex(G4int p) { return fVCOutMagWallVertex[p]; }

private:

  G4double fVCInMagThick;

  G4double fVCInMagWallSizeY;
  G4TwoVector fVCInMagWallVertex[CHAMBERGEOMETRY_VCINMAGWALL_NVERTICES];
  G4double fVCInMagWallHoleRadius;
  G4double fVCInMagWallHolePosZ;

  G4TwoVector fVCInMagPlateVertex[CHAMBERGEOMETRY_VCINMAGPLATE_NVERTICES];

  G4TwoVector fVCOutMagWallVertex[CHAMBERGEOMETRY_VCOUTMAGWALL_NVERTICES];

};

#endif
