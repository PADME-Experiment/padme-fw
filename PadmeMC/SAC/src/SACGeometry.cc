// SACGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "SACGeometry.hh"

SACGeometry* SACGeometry::fInstance = 0;

SACGeometry* SACGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new SACGeometry(); }
  return fInstance;
}

SACGeometry::SACGeometry()
{

  // Inizialize default parameters

  fCrystalNominalSizeX =  2.*cm;
  fCrystalNominalSizeY =  2.*cm;
  fCrystalNominalSizeZ = 20.*cm;

  fSACNRows = 7;
  fSACNCols = 7;

  fCrystalGap = 0.1*mm;

  fSACFrontFacePosZ = 300.*cm; // from center of yoke, i.e. 370cm from target, 70cm from front of ECal

  fSACSensitiveDetectorName = "SACSD";

}

SACGeometry::~SACGeometry()
{}

G4int SACGeometry::ExistsCrystalAt(G4int row, G4int col)
{

  // Verify we are within SAC box
  if ( row<0 || row>=fSACNRows || col<0 || col>=fSACNCols ) return 0;

// Only crystals position 2 & 4 are used in the first/last row/column
//  if (row==0 || row==fSACNRows-1) {
//    if (col==2 || col==4) return 1;
//    return 0;
//  }
//  if (col==0 || col==fSACNCols-1) {
//    if (row==2 || row==4) return 1;
//    return 0;
//  }

  // Remove central crystal (assumes an odd number of crystals per row/column)
  //if (row==fSACNRows/2 && col==fSACNCols/2) return 0;

  // All other crystals are there
  return 1;

}

G4double SACGeometry::GetCrystalPosX(G4int row, G4int col)
{

  // Verify we are within SAC box
  if (!ExistsCrystalAt(row,col)) {
    printf("SACGeometry::GetCrystalPosX - ERROR - Requested crystal at row %d col %d\n",row,col);
    return 0.;
  }

  // Return X position of center of crystal, correcting for crystals in first and last row
  G4double posX = fCrystalNominalSizeX*(-fSACNCols*0.5+col+0.5);
  //  if (row==0 || row==fSACNRows-1) {
  //if (col==2) posX += 0.5*fCrystalNominalSizeX;
  //if (col==4) posX -= 0.5*fCrystalNominalSizeX;
  //  }
  return posX;

}

G4double SACGeometry::GetCrystalPosY(G4int row, G4int col)
{

  // Verify we are within SAC box
  if (!ExistsCrystalAt(row,col)) {
    printf("SACGeometry::GetCrystalPosY - ERROR - Requested crystal at row %d col %d\n",row,col);
    return 0.;
  }

  // Return Y position of center of crystal, correcting for crystals in first and last column
  G4double posY = fCrystalNominalSizeY*(-fSACNRows*0.5+row+0.5);
//  if (col==0 || col==fSACNCols-1) {
//    if (row==2) posY += 0.5*fCrystalNominalSizeY;
//    if (row==4) posY -= 0.5*fCrystalNominalSizeY;
//  }
  return posY;

}

G4double SACGeometry::GetCrystalPosZ(G4int row, G4int col)
{

  // Verify we are within SAC box
  if (!ExistsCrystalAt(row,col)) {
    printf("SACGeometry::GetCrystalPosZ - ERROR - Requested crystal at row %d col %d\n",row,col);
    return 0.;
  }

  // Return Z position of center of crystal in local coordinate system
  return 0.*cm;

}
