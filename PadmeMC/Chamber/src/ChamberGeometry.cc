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

  fDetectorSetup = 10; // Default to 2019 setup

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
  fVCOuterFacePosZ = -500.001*mm; // Position along Z of the external face of the rectangular section inside the magnet

  // Parameters for the aluminum|carbon thin window

  fEWAlThick  = 4.*mm; // Thickness of aluminum window
  fEWAlConvexity = 29.*mm; // Convexity of aluminum window at its center

  fEWCarbonThick  = 2.5*mm; // Thickness of carbon window M. Raggi June 2022 according to drawings 06/03/2017
  //  fEWCarbonThick  = 0.01*mm; // Thickness of carbon window AHHHHHHHHHH RAGGIIII
  fEWCarbonConvexity = 29.*mm; // Convexity of carbon window at its center

  fEWAlFlangeRIn = 326.*mm; // Inner radius of flange (also radius of window)
  fEWAlFlangeROut = 375.*mm; // Outer radius of flange
  fEWAlFlangeThick = 23.*mm; // Thickness of flange
  fEWAlFrontFacePosZ = fVCBackFacePosZ; // Front face of flange holding aluminum thin window

  // Properties of crossed pipes at target position

  fCPZRIn = 5.*cm;
  fCPZThick = 1.*cm;
  //fCPZLength = 29.*cm;
  fCPZLength = 270.*mm; // Adjusted to Pfeiffer Cross Piece DN 100 CF - El 2019-04-12
  //fCPZPosZ = -1.*m;
  //fCPZPosZ = -103.*cm; // Adjusted to final position - EL 2019-03-13
  fCPZPosZ = -1028.*mm; // Jan 2020 survey (-1027.68mm, 1mm approximation)
  fCPZFlangeR = 76.*mm;
  fCPZFlangeThick = 20.*mm;

  fCPXRIn = 5.*cm;
  fCPXThick = 1.*cm;
  //fCPXLength = 25.*cm;
  fCPXLength = 270.*mm; // Adjusted to Pfeiffer Cross Piece DN 100 CF - EL 2019-04-12
  //fCPXPosZ = -1.*m;
  //fCPXPosZ = -103.*cm; // Adjusted to final position - EL 2019-03-13
  fCPXPosZ = -1028.*mm; // Jan 2020 survey (-1027.68mm, 1mm approximation)
  fCPXFlangeR = 76.*mm;
  fCPXFlangeThick = 20.*mm;

  // Properties of junction pipe between cross and chamber

  fJunRIn = 5.*cm;
  fJunThick = 1.*cm;
  fJunFrontFacePosZ = fCPZPosZ+0.5*fCPZLength;
  fJunBackFacePosZ = -56.2*cm; // From vacuum chamber technical drawing
  fJunLength = fJunBackFacePosZ-fJunFrontFacePosZ;
  fJunPosZ = 0.5*(fJunBackFacePosZ+fJunFrontFacePosZ);
  fJunFlangeR = 76.*mm;
  fJunFlangeThick = 20.*mm;

  // Properties of junction pipe between cross and BTF pipes

  fBTFJunRIn = 31.5*mm;
  fBTFJunROut = 35.*mm;
  //fBTFJunLength = 95.*mm;
  fBTFJunLength = 113.*mm; // From Foggetta's drawings - EL 2019-04-15

  fBTFJunPosZ = fCPZPosZ-0.5*fCPZLength-0.5*fBTFJunLength;
  // Flange towards cross
  fBTFJunFFlgRIn = fBTFJunROut;
  fBTFJunFFlgROut = fJunFlangeR;
  fBTFJunFFlgThick = fJunFlangeThick;
  fBTFJunFFlgPosZ = fBTFJunPosZ+0.5*fBTFJunLength-0.5*fBTFJunFFlgThick;
  // Flange towards bellow
  fBTFJunBFlgRIn = fBTFJunROut;
  fBTFJunBFlgROut = 56.75*mm;
  fBTFJunBFlgThick = 17.5*mm;
  fBTFJunBFlgPosZ = fBTFJunPosZ-0.5*fBTFJunLength+0.5*fBTFJunBFlgThick;

  // Properties of bellow-replacement pipe between BTF junction and dipole magnet

  fBellowRIn = 31.5*mm;
  fBellowROut = 35.*mm;
  //fBellowLength = 184.*mm;
  fBellowLength = 150.*mm; // From Foggetta's drawings - EL 2019-04-15

  fBellowPosZ = fBTFJunPosZ-0.5*fBTFJunLength-0.5*fBellowLength;
  // Flange towards cross
  fBellowFFlgRIn = fBellowROut;
  fBellowFFlgROut = fBTFJunBFlgROut;
  fBellowFFlgThick = fBTFJunBFlgThick;
  fBellowFFlgPosZ = fBellowPosZ+0.5*fBellowLength-0.5*fBellowFFlgThick;
  // Flange towards dipole magnet
  fBellowBFlgRIn = fBellowROut;
  fBellowBFlgROut = fBellowFFlgROut;
  fBellowBFlgThick = fBellowFFlgThick;
  fBellowBFlgPosZ = fBellowPosZ-0.5*fBellowLength+0.5*fBellowBFlgThick;

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

  // Thickness, width and height of steel rectangular porthole cap
  //  fTPPHCapThick = 25.*mm;
  fTPPHCapThick = 35.*mm;  //MR EL 07/03/2019 from FANTINI drawing
  fTPPHCapWidth = 996.*mm;
  fTPPHCapHeight = 600.*mm;

  // Radius and distance of beam exit hole
  //  fTPPHHoleRadius = 106.5*mm;
  fTPPHHoleRadius = 130.6*mm; //MR EL 07/03/2019 from FANTINI drawing
  // From Cesidio's drawings for 1GeV/.8Tesla configuration
  //fTPPHHoleDist = (624.38*mm-fVCBackFaceCornerX)/cos(fVCBackFaceAngle);
  fTPPHHoleDist = 400.*mm; //MR EL 07/03/2019 from FANTINI drawing

  // Radius and distance of service hole
  fTPPHHoleSRadius = 76.6*mm; //MR EL 07/03/2019 from FANTINI drawing
  fTPPHHoleSDist = 796.*mm; //MR EL 07/03/2019 from FANTINI drawing

  //fTPPHCircRadius = 142.5*mm;
  //fTPPHCircRadius = 165.7*mm; // Measured from Fantini drawings: need Cesidio's feedback
  //fTPPHCircThick = 12.0*mm;

  // Thickness, width and height of aluminum beam exit flange (Cesidio's drawings of 08/03/2019)
  fTPPHFlangeWidth = 337.0*mm;
  fTPPHFlangeHeight = 337.0*mm;
  fTPPHFlangeThick = 16.0*mm;

  // Radius, width and thickness of thin window on the aluminum beam exit flange (Cesidio's drawings of 08/03/2019)
  fTPPHWindRadius = 25.0*mm;
  fTPPHWindWidth = 200.0*mm;
  //fTPPHWindThick = 100.0*um; // This was Mylar foil
  fTPPHWindThick = 2.0*mm; // This is Al (EL 07/03/2019)

  // Radius and thickness of steel flange in front of service hole
  // Will add cabling holes later
  fTTPHSFlangeRadius = 112.5*mm;
  fTTPHSFlangeThick = 16.0*mm;

  //fTPPHStopRadius = 55.0*mm;
  //fTPPHStopWidth = fTPPHWindWidth;
  //fTPPHStopThick = 12.0*mm;

}

ChamberGeometry::~ChamberGeometry()
{}
