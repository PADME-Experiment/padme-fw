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

  fTDumpRadius = 3.5*cm;
  fTDumpLength = 6.0*cm;

  fBrickSizeX = 10.*cm;
  fBrickSizeY =  5.*cm;
  fBrickSizeZ = 20.*cm;

  fNBricks = 40;

  fTDumpFrontFacePosZ = -56.*cm;

  fTDumpSensitiveDetectorName = "TDumpSD";

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
    return  fBrickSizeX; // Left top and bottom
  } else if (iB==1 || iB==4) {
    return  0.*cm;       // Center top and bottom
  } else if (iB==2 || iB==5) {
    return -fBrickSizeX; // Right top and bottom
  } else if (iB==6) {
    return  fBrickSizeX+0.5*fBrickSizeY;
  } else if (iB==7) {
    return  fBrickSizeX-0.5*fBrickSizeY;
  } else if (iB==8) {
    return -fBrickSizeX+0.5*fBrickSizeY;
  } else {
    return -fBrickSizeX-0.5*fBrickSizeY;
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
    return -(0.5*fBrickSizeX+0.5*fBrickSizeY); // Bricks 0,1,2 at bottom
  } else if (iB<6) {
    return  (0.5*fBrickSizeX+0.5*fBrickSizeY); // Bricks 3,4,5 at top
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
  printf("zB is %d\n",zB);
  return fTDumpFrontFacePosZ+(zB*1.+0.5)*fBrickSizeZ;

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
