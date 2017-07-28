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

  // Some basic chamber quantities from Cesidio's CAD drawings

  fVCBackFacePosZ = 2300.0*mm; // Z position of back face of VC flange towards thin window
  //fVCBackFaceAngle = atan2( (2269.8-1981.85) , (1290.0-403.78) ); // Angle of TPix/HEPVeto face of chamber
  fVCBackFaceAngle = 18.*deg; // Angle of TPix/HEPVeto face of chamber
  printf("Chamber angle %f\n",fVCBackFaceAngle);
  fVCBackFaceThick = fVCBackFacePosZ-2269.8*mm; // Thickness of back faces
  fVCBackFaceCornerX = 403.78*mm;
  fVCBackFaceCornerZ = fVCBackFacePosZ;

  fVCInnerSizeX = 435.0*mm; // Internal size along X of the rectangular section inside the magnet
  fVCInnerSizeY = 205.0*mm; // Internal size along Y of the rectangular section inside the magnet
  fVCInnerFacePosZ = -490.0*mm; // Position along Z of the internal face of the rectangular section inside the magnet

  // Parameters for the aluminum thin window

  fEWAlThick  = 4.*mm; // Thickness of aluminum window
  fEWAlConvexity = 29.*mm; // Convexity of window at its center
  fEWAlFlangeRIn = 326.*mm; // Inner radius of flange (also radius of window)
  fEWAlFlangeROut = 375.*mm; // Outer radius of flange
  fEWAlFlangeThick = 23.*mm; // Thickness of flange
  fEWAlFrontFacePosZ = fVCBackFacePosZ; // Front face of flange holding aluminum thin window

  // Properties of crossed pipes at target position

  fCPZRIn = 5.*cm;
  fCPZThick = 1.*cm;
  fCPZLength = 29.*cm;
  fCPZPosZ = -1.*m;

  fCPXRIn = 5.*cm;
  fCPXThick = 1.*cm;
  fCPXLength = 25.*cm;
  fCPXPosZ = -1.*m;

}

ChamberGeometry::~ChamberGeometry()
{}
