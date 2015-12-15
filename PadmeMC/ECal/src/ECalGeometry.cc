// ECalGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "ECalGeometry.hh"

ECalGeometry* ECalGeometry::fInstance = 0;

ECalGeometry* ECalGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new ECalGeometry(); }
  return fInstance;
}

ECalGeometry::ECalGeometry()
{

  // Inizialize default parameters

  fCrystalNominalSizeX = 1.*cm;
  fCrystalNominalSizeY = 1.*cm;
  fCrystalNominalSizeZ = 22.*cm;

  fECalNRows = 30;
  fECalNCols = 30;

  fCrystalGap = 0.1*mm;

  fECalFrontFacePosZ = 180.*cm;

  fECalInnerRadius = 4.*cm;
  fECalOuterRadius = 15.*cm;

  fECalSensitiveDetectorName = "ECalSD";

}

ECalGeometry::~ECalGeometry()
{}

G4double ECalGeometry::GetECalPosX()
{
  return 0.;
}

G4double ECalGeometry::GetECalPosY()
{
  return 0.;
}

G4double ECalGeometry::GetECalPosZ()
{
  return fECalFrontFacePosZ+fCrystalNominalSizeZ*0.5;
}

G4double ECalGeometry::GetECalSizeX()
{
  return fCrystalNominalSizeX*fECalNRows;
}

G4double ECalGeometry::GetECalSizeY()
{
  return fCrystalNominalSizeY*fECalNCols;
}

G4double ECalGeometry::GetECalSizeZ()
{
  return fCrystalNominalSizeZ;
}

G4double ECalGeometry::GetCrystalSizeX()
{
  return fCrystalNominalSizeX-fCrystalGap;
}

G4double ECalGeometry::GetCrystalSizeY()
{
  return fCrystalNominalSizeY-fCrystalGap;
}

G4double ECalGeometry::GetCrystalSizeZ()
{
  return fCrystalNominalSizeZ-fCrystalGap;
}

G4int ECalGeometry::ExistsCrystalAt(G4int row, G4int col)
{

  // Verify we are within ECal box
  if ( row<0 || row>=fECalNRows || col<0 || col>=fECalNCols ) return 0;

  // Compute X/Y position of center of crystal
  G4double posX = fCrystalNominalSizeX*(-fECalNRows*0.5+row+0.5);
  G4double posY = fCrystalNominalSizeY*(-fECalNCols*0.5+col+0.5);

  // See if center of crystal falls inside inner-outer radius range
  G4double r2 = posX*posX+posY*posY;
  if (r2<fECalInnerRadius*fECalInnerRadius || r2>fECalOuterRadius*fECalOuterRadius) return 0;

  return 1;

}

G4double ECalGeometry::GetCrystalPosX(G4int row, G4int col)
{

  // Verify we are within ECal box
  if ( row<0 || row>=fECalNRows || col<0 || col>=fECalNCols ) {
    printf("ECalGeometry::GetCrystalPosX - ERROR - Requested position of crystal at row %d col %d (outside ECal box)\n",row,col);
    return 0.;
  }

  // Return X position of center of crystal
  return fCrystalNominalSizeX*(-fECalNRows*0.5+row+0.5);

}

G4double ECalGeometry::GetCrystalPosY(G4int row, G4int col)
{

  // Verify we are within ECal box
  if ( row<0 || row>=fECalNRows || col<0 || col>=fECalNCols ) {
    printf("ECalGeometry::GetCrystalPosY - ERROR - Requested position of crystal at row %d col %d (outside ECal box)\n",row,col);
    return 0.;
  }

  // Return Y position of center of crystal
  return fCrystalNominalSizeY*(-fECalNCols*0.5+col+0.5);

}

G4double ECalGeometry::GetCrystalPosZ(G4int row, G4int col)
{

  // Verify we are within ECal box
  if ( row<0 || row>=fECalNRows || col<0 || col>=fECalNCols ) {
    printf("ECalGeometry::GetCrystalPosZ - ERROR - Requested position of crystal at row %d col %d (outside ECal box)\n",row,col);
    return 0.;
  }

  // Return Z position of center of crystal
  return 0.;

}
