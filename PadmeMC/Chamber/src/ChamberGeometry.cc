// ChamberGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-06-10
// --------------------------------------------------------------

#include "ChamberGeometry.hh"

ChamberGeometry* ChamberGeometry::fInstance = 0;

ChamberGeometry* ChamberGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new ChamberGeometry(); }
  return fInstance;
}

ChamberGeometry::ChamberGeometry()
{

  fVCInMagThick = 0.8*cm;

  fVCInMagWallSizeY = 23.*cm-2.*fVCInMagThick;
  //fVCInMagWallVertex[ 0] = G4TwoVector(-54.4*cm, 52.4*cm);
  fVCInMagWallVertex[ 0] = G4TwoVector(-54.4*cm, 51.6*cm);
  fVCInMagWallVertex[ 1] = G4TwoVector(-54.4*cm, 37.3*cm);
  fVCInMagWallVertex[ 2] = G4TwoVector(-24.2*cm,  5.2*cm);
  fVCInMagWallVertex[ 3] = G4TwoVector(-24.2*cm,-56.2*cm);
  fVCInMagWallVertex[ 4] = G4TwoVector( 24.2*cm,-56.2*cm);
  fVCInMagWallVertex[ 5] = G4TwoVector( 24.2*cm, 52.0*cm);
  fVCInMagWallVertex[ 6] = G4TwoVector( 23.4*cm, 52.0*cm);
  fVCInMagWallVertex[ 7] = G4TwoVector( 23.4*cm,-55.4*cm); 
  fVCInMagWallVertex[ 8] = G4TwoVector(-23.4*cm,-55.4*cm); 
  fVCInMagWallVertex[ 9] = G4TwoVector(-23.4*cm,  5.5*cm);
  fVCInMagWallVertex[10] = G4TwoVector(-53.6*cm, 37.6*cm);
  fVCInMagWallVertex[11] = G4TwoVector(-53.6*cm, 52.4*cm);
  fVCInMagWallVertex[12] = G4TwoVector(-56.0*cm, 52.4*cm);
  fVCInMagWallVertex[13] = G4TwoVector(-56.0*cm, 51.6*cm);

  fVCInMagWallHoleRadius = 5.*cm;
  fVCInMagWallHolePosZ = 0.5*(fVCInMagWallVertex[3].y()+fVCInMagWallVertex[8].y());

  fVCInMagPlateVertex[0] = G4TwoVector(-54.4*cm, 52.0*cm);
  fVCInMagPlateVertex[1] = G4TwoVector(-54.4*cm, 37.3*cm);
  fVCInMagPlateVertex[2] = G4TwoVector(-24.2*cm,  5.2*cm);
  fVCInMagPlateVertex[3] = G4TwoVector(-24.2*cm,-56.2*cm);
  fVCInMagPlateVertex[4] = G4TwoVector( 24.2*cm,-56.2*cm);
  fVCInMagPlateVertex[5] = G4TwoVector( 24.2*cm, 52.0*cm);
  
  fVCOutMagWallVertex[ 0] = G4TwoVector( -56.0*cm, 51.6*cm);
  fVCOutMagWallVertex[ 1] = G4TwoVector( -68.2*cm, 51.6*cm);
  fVCOutMagWallVertex[ 2] = G4TwoVector(-156.6*cm,143.1*cm);
  fVCOutMagWallVertex[ 3] = G4TwoVector(-156.0*cm,143.6*cm);
  fVCOutMagWallVertex[ 4] = G4TwoVector( -67.9*cm, 52.4*cm);
  fVCOutMagWallVertex[ 5] = G4TwoVector( -56.0*cm, 52.4*cm);

}

ChamberGeometry::~ChamberGeometry()
{}
