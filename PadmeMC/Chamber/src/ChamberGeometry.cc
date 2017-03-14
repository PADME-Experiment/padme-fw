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

  //fVCEWGap = 5.9*mm; // Gap for o-ring between main VC body and thin window flange

  fVCCFThick = 20.*mm; // Thickness of VC flange toward thin window flange

  fVCCThick = 10.*mm; // Thickness of big cylinder
  fVCCRIn = 335.*mm; // Inner radius of big cylinder
  //fVCCLength = 140.*cm; // Length of big cylinder
  fVCCFrontFacePosZ = 89.1*cm; // Z position of front face of big cylinder

  fVCBackFacePosZ = 224.9*cm; // Z position of back face of VC flange towards thin window

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

  // Vertex coordinates of all sections of the chamber (external shape of the steel shell)
  // A section is a rectangular shape with fixed Z
  // Vertices must be ordered as v[0]=(+x,-y); v[1]=(-x,-y);v[2]=(-x,+y);v[3]=(+x,+y)
  // This corresponds to an anti-clockwise order when looking at the section from -z (i.e. as seen from the incoming beam)
  // N.B. section 1 was used in an old model of the chamber and is not needed anymore

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

  // Vertex coordinates of all sections of the chamber (internal shape of the steel shell)

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

  fVCIntVtx[10][0] = G4ThreeVector(  224.00*mm,-165.*mm,1940.382*mm); // X ok
  fVCIntVtx[10][1] = G4ThreeVector(-1294.00*mm,-165.*mm,1940.382*mm); // Y ok
  fVCIntVtx[10][2] = G4ThreeVector(-1294.00*mm, 165.*mm,1940.382*mm); // Z ok
  fVCIntVtx[10][3] = G4ThreeVector(  224.00*mm, 165.*mm,1940.382*mm);

  fVCIntVtx[11][0] = G4ThreeVector( 224.000*mm,-165.*mm,2239.*mm); // Z ok
  fVCIntVtx[11][1] = G4ThreeVector(-421.107*mm,-165.*mm,2239.*mm); // Y ok
  fVCIntVtx[11][2] = G4ThreeVector(-421.107*mm, 165.*mm,2239.*mm); // X ok
  fVCIntVtx[11][3] = G4ThreeVector( 224.000*mm, 165.*mm,2239.*mm);

  // Define facets used to create the chamber as a tesselated solid
  // Each facet is defined by 3 or 4 (see below) vertices encoded as an integer number in the form ssv
  // where ss is the section (0-11) and v is the section's vertex (0-3) taken from
  // one of the two lists above
  // Vertices must follow an anti-clockwise order when the facet is seen from outside the solid
  // If v[3] >= 0 then the facet is quadrangular
  // If v[3] = -1 then the facet is triangular and v[3] is not used
  // Note that each side of a facet must be shared with exactly one single side
  // of another facet: if this is not the case then the facet must be split.
  //
  // +-----------+       +-----------+
  // |           |       |\          |
  // |           |       | \         |
  // |           | ----> |  \        |
  // +---+-------+       +---+-------+
  // |   |       |       |   |       |
  // +---+-------+       +---+-------+
  //     WRONG              CORRECT

  // S 0 -Z
  fVCFacetVtx[0][0] =   0;
  fVCFacetVtx[0][1] =   1;
  fVCFacetVtx[0][2] =   2;
  fVCFacetVtx[0][3] =   3;

  // S 0-2 +X
  fVCFacetVtx[1][0] =   0;
  fVCFacetVtx[1][1] =   3;
  fVCFacetVtx[1][2] =  23;
  fVCFacetVtx[1][3] =  20;

  // S 0-2 +Y
  fVCFacetVtx[2][0] =   3;
  fVCFacetVtx[2][1] =   2;
  fVCFacetVtx[2][2] =  22;
  fVCFacetVtx[2][3] =  23;

  // S 0-2 -X
  fVCFacetVtx[3][0] =   2;
  fVCFacetVtx[3][1] =   1;
  fVCFacetVtx[3][2] =  21;
  fVCFacetVtx[3][3] =  22;

  // S 0-2 -Y
  fVCFacetVtx[4][0] =   1;
  fVCFacetVtx[4][1] =   0;
  fVCFacetVtx[4][2] =  20;
  fVCFacetVtx[4][3] =  21;

  // S 2-3 +X
  fVCFacetVtx[5][0] =  20;
  fVCFacetVtx[5][1] =  23;
  fVCFacetVtx[5][2] =  33;
  fVCFacetVtx[5][3] =  30;

  // S 2-3 +Y
  fVCFacetVtx[6][0] =  23;
  fVCFacetVtx[6][1] =  22;
  fVCFacetVtx[6][2] =  32;
  fVCFacetVtx[6][3] =  33;

  // S 2-3 -X
  fVCFacetVtx[7][0] =  22;
  fVCFacetVtx[7][1] =  21;
  fVCFacetVtx[7][2] =  31;
  fVCFacetVtx[7][3] =  32;

  // S 2-3 -Y
  fVCFacetVtx[8][0] =  21;
  fVCFacetVtx[8][1] =  20;
  fVCFacetVtx[8][2] =  30;
  fVCFacetVtx[8][3] =  31;

  // S 3-4 +X
  fVCFacetVtx[9][0] =  30;
  fVCFacetVtx[9][1] =  33;
  fVCFacetVtx[9][2] =  43;
  fVCFacetVtx[9][3] =  40;

  // S 3-4 +Y
  fVCFacetVtx[10][0] =  33;
  fVCFacetVtx[10][1] =  32;
  fVCFacetVtx[10][2] =  42;
  fVCFacetVtx[10][3] =  43;

  // S 3-4 -X
  fVCFacetVtx[11][0] =  32;
  fVCFacetVtx[11][1] =  31;
  fVCFacetVtx[11][2] =  41;
  fVCFacetVtx[11][3] =  42;

  // S 3-4 -Y
  fVCFacetVtx[12][0] =  31;
  fVCFacetVtx[12][1] =  30;
  fVCFacetVtx[12][2] =  40;
  fVCFacetVtx[12][3] =  41;

  // S 4-5, -Z
  fVCFacetVtx[13][0] =  42;
  fVCFacetVtx[13][1] =  41;
  fVCFacetVtx[13][2] =  51;
  fVCFacetVtx[13][3] =  52;

  // S 4-6 +X
  fVCFacetVtx[14][0] =  40;
  fVCFacetVtx[14][1] =  43;
  fVCFacetVtx[14][2] =  63;
  fVCFacetVtx[14][3] =  60;

  // S 4-6 +Y
  fVCFacetVtx[15][0] =  43;
  fVCFacetVtx[15][1] =  42;
  fVCFacetVtx[15][2] =  62;
  fVCFacetVtx[15][3] =  63;

  // S 5-6 -X
  fVCFacetVtx[16][0] =  52;
  fVCFacetVtx[16][1] =  51;
  fVCFacetVtx[16][2] =  61;
  fVCFacetVtx[16][3] =  62;

  // S 4-6 -Y
  fVCFacetVtx[17][0] =  41;
  fVCFacetVtx[17][1] =  40;
  fVCFacetVtx[17][2] =  60;
  fVCFacetVtx[17][3] =  61;

  // S 6-7 +X
  fVCFacetVtx[18][0] =  60;
  fVCFacetVtx[18][1] =  63;
  fVCFacetVtx[18][2] =  73;
  fVCFacetVtx[18][3] =  70;

  // S 6-7 +Y
  fVCFacetVtx[19][0] =  63;
  fVCFacetVtx[19][1] =  62;
  fVCFacetVtx[19][2] =  72;
  fVCFacetVtx[19][3] =  73;

  // S 6-7 -X
  fVCFacetVtx[20][0] =  62;
  fVCFacetVtx[20][1] =  61;
  fVCFacetVtx[20][2] =  71;
  fVCFacetVtx[20][3] =  72;

  // S 6-7 -Y
  fVCFacetVtx[21][0] =  61;
  fVCFacetVtx[21][1] =  60;
  fVCFacetVtx[21][2] =  70;
  fVCFacetVtx[21][3] =  71;

  // S 7-8 +X
  fVCFacetVtx[22][0] =  70;
  fVCFacetVtx[22][1] =  73;
  fVCFacetVtx[22][2] =  83;
  fVCFacetVtx[22][3] =  80;

  // S 7-8 +Y
  fVCFacetVtx[23][0] =  73;
  fVCFacetVtx[23][1] =  72;
  fVCFacetVtx[23][2] =  82;
  fVCFacetVtx[23][3] =  83;

  // S 7-8 -X
  fVCFacetVtx[24][0] =  72;
  fVCFacetVtx[24][1] =  71;
  fVCFacetVtx[24][2] =  81;
  fVCFacetVtx[24][3] =  82;

  // S 7-8 -Y
  fVCFacetVtx[25][0] =  71;
  fVCFacetVtx[25][1] =  70;
  fVCFacetVtx[25][2] =  80;
  fVCFacetVtx[25][3] =  81;

  // S 8-9 +X
  fVCFacetVtx[26][0] =  80;
  fVCFacetVtx[26][1] =  83;
  fVCFacetVtx[26][2] =  93;
  fVCFacetVtx[26][3] =  90;

  // S 8-9 +Y
  fVCFacetVtx[27][0] =  83;
  fVCFacetVtx[27][1] =  82;
  fVCFacetVtx[27][2] =  92;
  fVCFacetVtx[27][3] =  93;

  // S 8-9 -X
  fVCFacetVtx[28][0] =  82;
  fVCFacetVtx[28][1] =  81;
  fVCFacetVtx[28][2] =  91;
  fVCFacetVtx[28][3] =  92;

  // S 8-9 -Y
  fVCFacetVtx[29][0] =  81;
  fVCFacetVtx[29][1] =  80;
  fVCFacetVtx[29][2] =  90;
  fVCFacetVtx[29][3] =  91;

  // S 9-10 +X
  fVCFacetVtx[30][0] =  90;
  fVCFacetVtx[30][1] =  93;
  fVCFacetVtx[30][2] = 103;
  fVCFacetVtx[30][3] = 100;

  // S 9-10 +Y
  fVCFacetVtx[31][0] =  93;
  fVCFacetVtx[31][1] =  92;
  fVCFacetVtx[31][2] = 102;
  fVCFacetVtx[31][3] = 103;

  // S 9-10 -X
  fVCFacetVtx[32][0] =  92;
  fVCFacetVtx[32][1] =  91;
  fVCFacetVtx[32][2] = 101;
  fVCFacetVtx[32][3] = 102;

  // S 9-10 -Y
  fVCFacetVtx[33][0] =  91;
  fVCFacetVtx[33][1] =  90;
  fVCFacetVtx[33][2] = 100;
  fVCFacetVtx[33][3] = 101;

  // S 10-11 +X
  fVCFacetVtx[34][0] = 100;
  fVCFacetVtx[34][1] = 103;
  fVCFacetVtx[34][2] = 113;
  fVCFacetVtx[34][3] = 110;

  // S 10-11 +Y
  fVCFacetVtx[35][0] = 103;
  fVCFacetVtx[35][1] = 102;
  fVCFacetVtx[35][2] = 112;
  fVCFacetVtx[35][3] = 113;

  // S 10-11 -X
  fVCFacetVtx[36][0] = 102;
  fVCFacetVtx[36][1] = 101;
  fVCFacetVtx[36][2] = 111;
  fVCFacetVtx[36][3] = 112;

  // S 10-11 -Y
  fVCFacetVtx[37][0] = 101;
  fVCFacetVtx[37][1] = 100;
  fVCFacetVtx[37][2] = 110;
  fVCFacetVtx[37][3] = 111;

  // S 11 +Z
  fVCFacetVtx[38][0] = 113;
  fVCFacetVtx[38][1] = 112;
  fVCFacetVtx[38][2] = 111;
  fVCFacetVtx[38][3] = 110;

  // S 4-(5)-6 +Y
  fVCFacetVtx[39][0] =  42;
  fVCFacetVtx[39][1] =  52;
  fVCFacetVtx[39][2] =  62;
  fVCFacetVtx[39][3] =  -1;

  // S 4-(5)-6 -Y
  fVCFacetVtx[40][0] =  51;
  fVCFacetVtx[40][1] =  41;
  fVCFacetVtx[40][2] =  61;
  fVCFacetVtx[40][3] =  -1;

  //printf("In geo %f %f %f\n",fVCExtVtx[1][0].x(),fVCExtVtx[1][0].y(),fVCExtVtx[1][0].z());

  // Quantities derived from the vertices positions
  fVCMostExternalX = -fVCExtVtx[9][1].x(); // Absolute value of largest X coordinate of the chamber

  // Properties of the beam entrance hole
  fVCInHoleRadius = 5.*cm;
  fVCInHoleThick = fVCIntVtx[0][0].z()-fVCExtVtx[0][0].z();
  fVCInHolePosZ = 0.5*(fVCExtVtx[0][0].z()+fVCIntVtx[0][0].z());

  // Properties of the beam exit hole
  fVCOutHoleRadius = 25.*mm; // Radius of the two circles
  fVCOutHoleLength = 100.*mm; // Distance between circle centers
  fVCOutHoleThick = 10.*mm; // Should be computed from distance between chamber faces
  // Distance of center of hole from edge on the VC back face
  fVCOutHoleDistToEdge = 35.*cm; // To verify with Cesidio

  // Properties of crossed pipes at target position
  fCPZRIn = 5.*cm;
  fCPZThick = 1.*cm;
  fCPZLength = 29.*cm;
  fCPZPosZ = fVCExtVtx[0][0].z()-0.5*fCPZLength;

  fCPXRIn = 5.*cm;
  fCPXThick = 1.*cm;
  fCPXLength = 25.*cm;
  fCPXPosZ = fVCExtVtx[0][0].z()-0.5*fCPZLength;

}

ChamberGeometry::~ChamberGeometry()
{}

G4double ChamberGeometry::GetVCBackFaceAngle()
{
  G4double angle =  atan2(fVCExtVtx[11][1].z()-fVCExtVtx[10][1].z(),
			  fVCExtVtx[11][1].x()-fVCExtVtx[10][1].x());
  return angle;
}

G4ThreeVector ChamberGeometry::GetVCBackFaceCorner()
{
  return G4ThreeVector(fVCExtVtx[11][1].x(),0.,fVCExtVtx[11][1].z());
}

G4double ChamberGeometry::GetVCBackFaceThickness()
{
  G4double thick =  fVCExtVtx[11][0].z()-fVCIntVtx[11][0].z();
  return thick;
}
