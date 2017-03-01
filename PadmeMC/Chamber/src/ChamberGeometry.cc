// ChamberGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-06-10
// --------------------------------------------------------------

#include "ChamberGeometry.hh"

using namespace CLHEP;

ChamberGeometry* ChamberGeometry::fInstance = 0;

ChamberGeometry* ChamberGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new ChamberGeometry(); }
  return fInstance;
}

ChamberGeometry::ChamberGeometry()
{

  fVCInMagThick = 1.*cm;

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

  //fVCBackFacePosZ = 224.5*cm; // Z position of back face of VC flange towards thin window
  fVCBackFacePosZ = 224.9*cm; // Z position of back face of VC flange towards thin window

  fVCMostExternalX = 156.6*cm; // Absolute value of largest X coordinate of the chamber

  //fVCEWGap = 5.9*mm; // Gap for o-ring between main VC body and thin window flange

  fVCCFThick = 20.*mm; // Thickness of VC flange toward thin window flange

  fVCCThick = 10.*mm; // Thickness of big cylinder
  fVCCRIn = 335.*mm; // Inner radius of big cylinder
  //fVCCLength = 140.*cm; // Length of big cylinder
  fVCCFrontFacePosZ = 89.1*cm; // Z position of front face of big cylinder

  // Thin window in front of ECal (EW)

  fEWBackFacePosZ = 227.5*cm; // Z position of window: 2.5cm before front face of ECal

  fEWRadius = 310.*mm; // Radius of window
  fEWConvexity = 30.*mm; // Convexity of window at its center

  fEWFrontMylarThick = 0.1*mm; // Thickness of mylar layer inside VC
  fEWKevlarThick = 0.6*mm;     // Thickness of kevlar layer between mylar layers
  fEWBackMylarThick = 0.1*mm;  // Thickness of mylar layer outside VC

  // Steel flange around thin window (three steel rings)

  fEWF1Thick = 15.*mm;
  fEWF2Thick = 15.*mm;
  fEWF3Thick = 9.*mm;

  fEWF1ROut = 360.*mm;
  fEWF2ROut = 390.*mm;
  fEWF3RIn = 325.5*mm;

  // Vertex coordinates of all sections of the chamber

  fVCExtVtx[0][0] = G4ThreeVector( 234.00*mm,-112.5*mm,-555.*mm);
  fVCExtVtx[0][1] = G4ThreeVector(-234.00*mm,-112.5*mm,-555.*mm);
  fVCExtVtx[0][2] = G4ThreeVector(-234.00*mm, 112.5*mm,-555.*mm);
  fVCExtVtx[0][3] = G4ThreeVector( 234.00*mm, 112.5*mm,-555.*mm);

  fVCExtVtx[1][0] = G4ThreeVector( 234.00*mm,-112.5*mm, -52.38*mm);
  fVCExtVtx[1][1] = G4ThreeVector(-234.00*mm,-112.5*mm, -52.38*mm);
  fVCExtVtx[1][2] = G4ThreeVector(-234.00*mm, 112.5*mm, -52.38*mm);
  fVCExtVtx[1][3] = G4ThreeVector( 234.00*mm, 112.5*mm, -52.38*mm);

  fVCExtVtx[2][0] = G4ThreeVector( 234.00*mm,-112.5*mm,  52.38*mm);
  fVCExtVtx[2][1] = G4ThreeVector(-234.00*mm,-112.5*mm,  52.38*mm);
  fVCExtVtx[2][2] = G4ThreeVector(-234.00*mm, 112.5*mm,  52.38*mm);
  fVCExtVtx[2][3] = G4ThreeVector( 234.00*mm, 112.5*mm,  52.38*mm);

  fVCExtVtx[3][0] = G4ThreeVector( 234.00*mm,-112.5*mm, 369.26*mm);
  fVCExtVtx[3][1] = G4ThreeVector(-540.00*mm,-112.5*mm, 369.26*mm);
  fVCExtVtx[3][2] = G4ThreeVector(-540.00*mm, 112.5*mm, 369.26*mm);
  fVCExtVtx[3][3] = G4ThreeVector( 234.00*mm, 112.5*mm, 369.26*mm);

  fVCExtVtx[4][0] = G4ThreeVector( 234.00*mm,-112.5*mm, 509.*mm);
  fVCExtVtx[4][1] = G4ThreeVector(-540.00*mm,-112.5*mm, 509.*mm);
  fVCExtVtx[4][2] = G4ThreeVector(-540.00*mm, 112.5*mm, 509.*mm);
  fVCExtVtx[4][3] = G4ThreeVector( 234.00*mm, 112.5*mm, 509.*mm);

  fVCExtVtx[5][0] = G4ThreeVector( 234.00*mm,-112.5*mm, 509.*mm);
  fVCExtVtx[5][1] = G4ThreeVector(-663.83*mm,-112.5*mm, 509.*mm);
  fVCExtVtx[5][2] = G4ThreeVector(-663.83*mm, 112.5*mm, 509.*mm);
  fVCExtVtx[5][3] = G4ThreeVector( 234.00*mm, 112.5*mm, 509.*mm);

  fVCExtVtx[6][0] = G4ThreeVector( 234.00*mm,-137.5*mm, 552.*mm);
  fVCExtVtx[6][1] = G4ThreeVector(-706.09*mm,-137.5*mm, 552.*mm);
  fVCExtVtx[6][2] = G4ThreeVector(-706.09*mm, 137.5*mm, 552.*mm);
  fVCExtVtx[6][3] = G4ThreeVector( 234.00*mm, 137.5*mm, 552.*mm);

  fVCExtVtx[7][0] = G4ThreeVector( 234.00*mm,-137.5*mm, 799.*mm);
  fVCExtVtx[7][1] = G4ThreeVector(-948.83*mm,-137.5*mm, 799.*mm);
  fVCExtVtx[7][2] = G4ThreeVector(-948.83*mm, 137.5*mm, 799.*mm);
  fVCExtVtx[7][3] = G4ThreeVector( 234.00*mm, 137.5*mm, 799.*mm);

  fVCExtVtx[8][0] = G4ThreeVector(  234.00*mm,-175.*mm, 864.*mm);
  fVCExtVtx[8][1] = G4ThreeVector(-1012.71*mm,-175.*mm, 864.*mm);
  fVCExtVtx[8][2] = G4ThreeVector(-1012.71*mm, 175.*mm, 864.*mm);
  fVCExtVtx[8][3] = G4ThreeVector(  234.00*mm, 175.*mm, 864.*mm);

  fVCExtVtx[9][0] = G4ThreeVector(  234.00*mm,-175.*mm,1160.4*mm);
  fVCExtVtx[9][1] = G4ThreeVector(-1304.00*mm,-175.*mm,1160.4*mm);
  fVCExtVtx[9][2] = G4ThreeVector(-1304.00*mm, 175.*mm,1160.4*mm);
  fVCExtVtx[9][3] = G4ThreeVector(  234.00*mm, 175.*mm,1160.4*mm);

  fVCExtVtx[10][0] = G4ThreeVector(  234.00*mm,-175.*mm,1947.53*mm);
  fVCExtVtx[10][1] = G4ThreeVector(-1304.00*mm,-175.*mm,1947.53*mm);
  fVCExtVtx[10][2] = G4ThreeVector(-1304.00*mm, 175.*mm,1947.53*mm);
  fVCExtVtx[10][3] = G4ThreeVector(  234.00*mm, 175.*mm,1947.53*mm);

  fVCExtVtx[11][0] = G4ThreeVector( 234.00*mm,-175.*mm,2249.*mm);
  fVCExtVtx[11][1] = G4ThreeVector(-422.77*mm,-175.*mm,2249.*mm);
  fVCExtVtx[11][2] = G4ThreeVector(-422.77*mm, 175.*mm,2249.*mm);
  fVCExtVtx[11][3] = G4ThreeVector( 234.00*mm, 175.*mm,2249.*mm);

  //--------------------------------//

  fVCIntVtx[0][0] = G4ThreeVector( 224.00*mm,-102.5*mm,-545.*mm); // Z ok
  fVCIntVtx[0][1] = G4ThreeVector(-224.00*mm,-102.5*mm,-545.*mm); // X ok
  fVCIntVtx[0][2] = G4ThreeVector(-224.00*mm, 102.5*mm,-545.*mm); // Y ok
  fVCIntVtx[0][3] = G4ThreeVector( 224.00*mm, 102.5*mm,-545.*mm);

  fVCIntVtx[1][0] = G4ThreeVector( 224.00*mm,-102.5*mm, -56.42*mm); // X ok
  fVCIntVtx[1][1] = G4ThreeVector(-224.00*mm,-102.5*mm, -56.42*mm); // Y ok
  fVCIntVtx[1][2] = G4ThreeVector(-224.00*mm, 102.5*mm, -56.42*mm); // Z ok
  fVCIntVtx[1][3] = G4ThreeVector( 224.00*mm, 102.5*mm, -56.42*mm);

  fVCIntVtx[2][0] = G4ThreeVector( 224.00*mm,-102.5*mm,  56.42*mm); // X ok
  fVCIntVtx[2][1] = G4ThreeVector(-224.00*mm,-102.5*mm,  56.42*mm); // Y ok
  fVCIntVtx[2][2] = G4ThreeVector(-224.00*mm, 102.5*mm,  56.42*mm); // Z ok
  fVCIntVtx[2][3] = G4ThreeVector( 224.00*mm, 102.5*mm,  56.42*mm);

  fVCIntVtx[3][0] = G4ThreeVector( 224.00*mm,-102.5*mm, 373.30*mm); // Y ok
  fVCIntVtx[3][1] = G4ThreeVector(-530.00*mm,-102.5*mm, 373.30*mm); // X ok
  fVCIntVtx[3][2] = G4ThreeVector(-530.00*mm, 102.5*mm, 373.30*mm); // Z ok
  fVCIntVtx[3][3] = G4ThreeVector( 224.00*mm, 102.5*mm, 373.30*mm);

  fVCIntVtx[4][0] = G4ThreeVector( 224.00*mm,-102.5*mm, 519.*mm); // Z ok
  fVCIntVtx[4][1] = G4ThreeVector(-530.00*mm,-102.5*mm, 519.*mm); // Y ok
  fVCIntVtx[4][2] = G4ThreeVector(-530.00*mm, 102.5*mm, 519.*mm); // X ok
  fVCIntVtx[4][3] = G4ThreeVector( 224.00*mm, 102.5*mm, 519.*mm);

  fVCIntVtx[5][0] = G4ThreeVector( 224.00*mm,-102.5*mm, 519.*mm); // Z ok
  fVCIntVtx[5][1] = G4ThreeVector(-659.79*mm,-102.5*mm, 519.*mm); // Y ok
  fVCIntVtx[5][2] = G4ThreeVector(-659.79*mm, 102.5*mm, 519.*mm); // X ok
  fVCIntVtx[5][3] = G4ThreeVector( 224.00*mm, 102.5*mm, 519.*mm);

  fVCIntVtx[6][0] = G4ThreeVector( 224.00*mm,-127.5*mm, 552.*mm); // Z ok ?
  fVCIntVtx[6][1] = G4ThreeVector(-692.22*mm,-127.5*mm, 552.*mm); // Y ok
  fVCIntVtx[6][2] = G4ThreeVector(-692.22*mm, 127.5*mm, 552.*mm); // X ok ?
  fVCIntVtx[6][3] = G4ThreeVector( 224.00*mm, 127.5*mm, 552.*mm);

  fVCIntVtx[7][0] = G4ThreeVector( 224.00*mm,-127.5*mm, 799.*mm);
  fVCIntVtx[7][1] = G4ThreeVector(-934.92*mm,-127.5*mm, 799.*mm); // Z ok ?
  fVCIntVtx[7][2] = G4ThreeVector(-934.92*mm, 127.5*mm, 799.*mm); // Y ok
  fVCIntVtx[7][3] = G4ThreeVector( 224.00*mm, 127.5*mm, 799.*mm); // X ok ?

  fVCIntVtx[8][0] = G4ThreeVector( 224.00*mm,-165.*mm, 864.*mm); // Z ok ?
  fVCIntVtx[8][1] = G4ThreeVector(-998.79*mm,-165.*mm, 864.*mm); // Y ok
  fVCIntVtx[8][2] = G4ThreeVector(-998.79*mm, 165.*mm, 864.*mm); // X ok ?
  fVCIntVtx[8][3] = G4ThreeVector( 224.00*mm, 165.*mm, 864.*mm);

  fVCIntVtx[9][0] = G4ThreeVector(  224.00*mm,-165.*mm,1164.44*mm); // Y ok
  fVCIntVtx[9][1] = G4ThreeVector(-1294.00*mm,-165.*mm,1164.44*mm); // X ok
  fVCIntVtx[9][2] = G4ThreeVector(-1294.00*mm, 165.*mm,1164.44*mm); // Z ok
  fVCIntVtx[9][3] = G4ThreeVector(  224.00*mm, 165.*mm,1164.44*mm);

  fVCIntVtx[10][0] = G4ThreeVector(  224.00*mm,-165.*mm,1940.38*mm); // X ok
  fVCIntVtx[10][1] = G4ThreeVector(-1294.00*mm,-165.*mm,1940.38*mm); // Y ok
  fVCIntVtx[10][2] = G4ThreeVector(-1294.00*mm, 165.*mm,1940.38*mm); // Z ok
  fVCIntVtx[10][3] = G4ThreeVector(  224.00*mm, 165.*mm,1940.38*mm);

  fVCIntVtx[11][0] = G4ThreeVector( 224.00*mm,-165.*mm,2239.*mm); // Z ok
  fVCIntVtx[11][1] = G4ThreeVector(-421.10*mm,-165.*mm,2239.*mm); // Y ok
  fVCIntVtx[11][2] = G4ThreeVector(-421.10*mm, 165.*mm,2239.*mm); // X ok
  fVCIntVtx[11][3] = G4ThreeVector( 224.00*mm, 165.*mm,2239.*mm);

  //printf("In geo %f %f %f\n",fVCExtVtx[1][0].x(),fVCExtVtx[1][0].y(),fVCExtVtx[1][0].z());

  // Properties of the beam entrance hole
  fVCInHoleRadius = 5.*cm;
  fVCInHoleThick = fVCIntVtx[0][0].z()-fVCExtVtx[0][0].z();
  fVCInHolePosZ = 0.5*(fVCExtVtx[0][0].z()+fVCIntVtx[0][0].z());

}

ChamberGeometry::~ChamberGeometry()
{}
