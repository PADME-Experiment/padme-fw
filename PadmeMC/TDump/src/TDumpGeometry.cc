// TDumpGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-01-28
// --------------------------------------------------------------

#include "TDumpGeometry.hh"

TDumpGeometry* TDumpGeometry::fInstance = 0;

TDumpGeometry* TDumpGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new TDumpGeometry(); }
  return fInstance;
}

TDumpGeometry::TDumpGeometry()
{

  // Inizialize default parameters

  fTDumpGap = 0.1*mm;

  fTargetRadius = 3.5*cm;
  fTargetLength = 6.0*cm;

  fTargetFrontFacePosZ = 10.*cm;

  fBrickNominalSizeX = 10.*cm;
  fBrickNominalSizeY =  5.*cm;
  fBrickNominalSizeZ = 20.*cm;

  fNBricks = 40;

  fTDumpFrontFacePosZ = -180.*cm;

  fTDumpSensitiveDetectorName = "TDumpSD";

  // Derived quantitites
  fTDumpSizeX = 3.*fBrickNominalSizeX;
  fTDumpSizeY = 2.*fBrickNominalSizeY+fBrickNominalSizeX;
  fTDumpSizeZ = 4.*fBrickNominalSizeZ;

}

TDumpGeometry::~TDumpGeometry()
{}

G4double TDumpGeometry::GetBrickPosX(G4int nB)
{

  if (nB<0 || nB>fNBricks) {
    printf("TDumpGeometry::GetBrickPosY - ERROR - Requested brick number %d\n",nB);
    return 0.;
  }

  G4int iB = nB%10;

  if (iB==0 || iB==3) {
    return  fBrickNominalSizeX; // Left top and bottom
  } else if (iB==1 || iB==4) {
    return  0.*cm;       // Center top and bottom
  } else if (iB==2 || iB==5) {
    return -fBrickNominalSizeX; // Right top and bottom
  } else if (iB==6) {
    return  fBrickNominalSizeX+0.5*fBrickNominalSizeY;
  } else if (iB==7) {
    return  fBrickNominalSizeX-0.5*fBrickNominalSizeY;
  } else if (iB==8) {
    return -fBrickNominalSizeX+0.5*fBrickNominalSizeY;
  } else {
    return -fBrickNominalSizeX-0.5*fBrickNominalSizeY;
  }

}

G4double TDumpGeometry::GetBrickPosY(G4int nB)
{

  if (nB<0 || nB>fNBricks) {
    printf("TDumpGeometry::GetBrickPosX - ERROR - Requested brick number %d\n",nB);
    return 0.;
  }

  G4int iB = nB%10;

  if (iB<3) {
    return -(0.5*fBrickNominalSizeX+0.5*fBrickNominalSizeY); // Bricks 0,1,2 at bottom
  } else if (iB<6) {
    return  (0.5*fBrickNominalSizeX+0.5*fBrickNominalSizeY); // Bricks 3,4,5 at top
  } else {
    return 0.*cm; // Bricks 6,7,8,9 at center
  }

}


G4double TDumpGeometry::GetBrickPosZ(G4int nB)
{

  if (nB<0 || nB>fNBricks) {
    printf("TDumpGeometry::GetBrickPosY - ERROR - Requested brick number %d\n",nB);
    return 0.;
  }

  G4int zB = nB/10;
  return -0.5*fTDumpSizeZ+(zB*1.+0.5)*fBrickNominalSizeZ;

}

G4double TDumpGeometry::GetBrickRotZ(G4int nB)
{

  if (nB<0 || nB>fNBricks) {
    printf("TDumpGeometry::GetBrickPosY - ERROR - Requested brick number %d\n",nB);
    return 0.;
  }

  G4int iB = nB%10;

  if (iB>=6) return 90.*deg;
  return 0.*deg;

}
