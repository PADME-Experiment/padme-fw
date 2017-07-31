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
  //printf("Chamber angle %f\n",fVCBackFaceAngle);
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

  // Properties of porthole caps

  fPHCapRadius[0] = 112.5*mm;
  fPHCapThick[0] = 16.0*mm;
  fPHCapFlangeThick[0] = 29.0*mm;
  fPHCapPos[0] = G4ThreeVector(-0.5*fVCInnerSizeX-fPHCapFlangeThick[0]-0.5*fPHCapThick[0],0.,-350.*mm);

  fPHCapRadius[1] = 112.5*mm;
  fPHCapThick[1] = 16.0*mm;
  fPHCapFlangeThick[1] = 29.0*mm;
  fPHCapPos[1] = G4ThreeVector(-0.5*fVCInnerSizeX-fPHCapFlangeThick[1]-0.5*fPHCapThick[1],0.,350.*mm);

  fPHCapRadius[2] = 112.5*mm;
  fPHCapThick[2] = 16.0*mm;
  fPHCapFlangeThick[2] = 29.0*mm;
  fPHCapPos[2] = G4ThreeVector(0.5*fVCInnerSizeX+fPHCapFlangeThick[2]+0.5*fPHCapThick[2],0.,-350.*mm);

  fPHCapRadius[3] = 112.5*mm;
  fPHCapThick[3] = 16.0*mm;
  fPHCapFlangeThick[3] = 29.0*mm;
  fPHCapPos[3] = G4ThreeVector(0.5*fVCInnerSizeX+fPHCapFlangeThick[3]+0.5*fPHCapThick[3],0.,30.*mm);

  fPHCapRadius[4] = 112.5*mm;
  fPHCapThick[4] = 16.0*mm;
  fPHCapFlangeThick[4] = 25.1*mm;
  fPHCapPos[4] = G4ThreeVector(-375.07*mm-fPHCapFlangeThick[4]-0.5*fPHCapThick[4],0.,790.*mm);

  fPHCapRadius[5] = 112.5*mm;
  fPHCapThick[5] = 16.0*mm;
  fPHCapFlangeThick[5] = 25.1*mm;
  fPHCapPos[5] = G4ThreeVector(1020.0*mm+fPHCapFlangeThick[5]+0.5*fPHCapThick[5],0.,790.*mm);

  fPHCapRadius[6] = 142.5*mm;
  fPHCapThick[6] = 16.0*mm;
  fPHCapFlangeThick[6] = 75.17*mm;
  fPHCapPos[6] = G4ThreeVector(-324.83*mm-fPHCapFlangeThick[6]-0.5*fPHCapThick[6],0.,1400.*mm);

  fPHCapRadius[7] = 167.5*mm;
  fPHCapThick[7] = 16.0*mm;
  fPHCapFlangeThick[7] = 35.0*mm;
  fPHCapPos[7] = G4ThreeVector(1290.0*mm+fPHCapFlangeThick[7]+0.5*fPHCapThick[7],0.,1750.*mm);

  // Properties of large TPix porthole

  fTPPHCapThick = 25.*mm;
  fTPPHCapWidth = 996.*mm;
  fTPPHCapHeight = 600.*mm;

  fTPPHHoleRadius = 106.5*mm;
  fTPPHHoleDist = (624.38*mm-fVCBackFaceCornerX)/cos(fVCBackFaceAngle);

  fTPPHCircRadius = 142.5*mm;
  fTPPHCircThick = 12.0*mm;

  fTPPHWindRadius = 25.0*mm;
  fTPPHWindWidth = 100.0*mm;
  fTPPHWindThick = 100.0*um;

  fTPPHStopRadius = 55.0*mm;
  fTPPHStopWidth = fTPPHWindWidth;
  fTPPHStopThick = 12.0*mm;

}

ChamberGeometry::~ChamberGeometry()
{}
