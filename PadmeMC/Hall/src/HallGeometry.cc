// HallGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-01-11
// --------------------------------------------------------------

#include "HallGeometry.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"

HallGeometry* HallGeometry::fInstance = 0;

HallGeometry* HallGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new HallGeometry(); }
  return fInstance;
}

HallGeometry::HallGeometry()
{

  // Inizialize default parameters

  fFloorPosY             = -130.*cm;
  fBackWallFrontFacePosZ =  480.*cm;
  fSideWallFrontFacePosX = -200.*cm;

  fBlockGap = 0.1*mm;

  fBlockNominalSide = 1.*m;

  fBlockPosition[ 0] = G4ThreeVector(-50.*cm,50.*cm,-350.*cm);
  fBlockPosition[ 1] = G4ThreeVector(-50.*cm,50.*cm,-250.*cm);
  fBlockPosition[ 2] = G4ThreeVector(-50.*cm,50.*cm,-150.*cm);
  fBlockPosition[ 3] = G4ThreeVector(-50.*cm,50.*cm, -50.*cm);
  fBlockPosition[ 4] = G4ThreeVector(-50.*cm,50.*cm,  50.*cm);
  fBlockPosition[ 5] = G4ThreeVector( 50.*cm,50.*cm,  50.*cm);
  fBlockPosition[ 6] = G4ThreeVector(150.*cm,50.*cm,  50.*cm);
  fBlockPosition[ 7] = G4ThreeVector(250.*cm,50.*cm,  50.*cm);
  fBlockPosition[ 8] = G4ThreeVector(350.*cm,50.*cm,  50.*cm);
  fBlockPosition[ 9] = G4ThreeVector(450.*cm,50.*cm,  50.*cm);
  fBlockPosition[10] = G4ThreeVector(-50.*cm,50.*cm, 150.*cm);
  fBlockPosition[11] = G4ThreeVector( 50.*cm,50.*cm, 150.*cm);
  fBlockPosition[12] = G4ThreeVector(150.*cm,50.*cm, 150.*cm);
  fBlockPosition[13] = G4ThreeVector(250.*cm,50.*cm, 150.*cm);
  fBlockPosition[14] = G4ThreeVector(350.*cm,50.*cm, 150.*cm);
  fBlockPosition[15] = G4ThreeVector(450.*cm,50.*cm, 150.*cm);

  fBlockPosition[16] = G4ThreeVector(-50.*cm,150.*cm,-350.*cm);
  fBlockPosition[17] = G4ThreeVector(-50.*cm,150.*cm,-250.*cm);
  fBlockPosition[18] = G4ThreeVector(-50.*cm,150.*cm,-150.*cm);
  fBlockPosition[19] = G4ThreeVector(-50.*cm,150.*cm, -50.*cm);
  fBlockPosition[20] = G4ThreeVector(-50.*cm,150.*cm,  50.*cm);
  fBlockPosition[21] = G4ThreeVector( 50.*cm,150.*cm,  50.*cm);
  fBlockPosition[22] = G4ThreeVector(150.*cm,150.*cm,  50.*cm);
  fBlockPosition[23] = G4ThreeVector(250.*cm,150.*cm,  50.*cm);
  fBlockPosition[24] = G4ThreeVector(350.*cm,150.*cm,  50.*cm);
  fBlockPosition[25] = G4ThreeVector(450.*cm,150.*cm,  50.*cm);
  fBlockPosition[26] = G4ThreeVector(-50.*cm,150.*cm, 150.*cm);
  fBlockPosition[27] = G4ThreeVector( 50.*cm,150.*cm, 150.*cm);
  fBlockPosition[28] = G4ThreeVector(150.*cm,150.*cm, 150.*cm);
  fBlockPosition[29] = G4ThreeVector(250.*cm,150.*cm, 150.*cm);
  fBlockPosition[30] = G4ThreeVector(350.*cm,150.*cm, 150.*cm);
  fBlockPosition[31] = G4ThreeVector(450.*cm,150.*cm, 150.*cm);

}

HallGeometry::~HallGeometry()
{}

G4ThreeVector HallGeometry::GetBlockPosition(G4int i)
{
  return G4ThreeVector(GetBlockPosX(i),GetBlockPosY(i),GetBlockPosZ(i));
}

G4double HallGeometry::GetBlockPosX(G4int i)
{
  return fSideWallFrontFacePosX+fBlockPosition[i].x();
}

G4double HallGeometry::GetBlockPosY(G4int i)
{
  return fFloorPosY+fBlockPosition[i].y();
}

G4double HallGeometry::GetBlockPosZ(G4int i)
{
  return fBackWallFrontFacePosZ+fBlockPosition[i].z();
}
