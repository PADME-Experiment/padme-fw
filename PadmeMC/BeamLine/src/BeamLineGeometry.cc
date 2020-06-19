// BeamLineGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-06-10
// --------------------------------------------------------------

#include "BeamLineGeometry.hh"

using namespace CLHEP;

BeamLineGeometry* BeamLineGeometry::fInstance = 0;

BeamLineGeometry* BeamLineGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new BeamLineGeometry(); }
  return fInstance;
}

BeamLineGeometry::BeamLineGeometry()
{

  // By default the Be window is positioned
  fBeWindowEnable = true;
  fBeamFlagEnable = true;
  // Set Be window sensitive detector positioned
  // Define name of Be window sensitive detector

  fBeWSensitiveDetectorName = "BeWSD";
  fBeamFlag1SensitiveDetectorName = "BDSFL";
  fBeamFlag2SensitiveDetectorName = "BUSFL";

  // Default magnetic field
  fDHSTB002MagneticFieldY = -1.055*tesla;

  //  fQuadMagneticFieldGrad  =  10*tesla/m;
  fQuadrupolesEnable = false;
  fQ1MagneticFieldGrad =  5.*tesla/m;
  fQ2MagneticFieldGrad =  5.*tesla/m;
  //printf("***************************** tesla %f m %f \n ",tesla,m);

  // Radius of BeamPipe center
  fDHSTB002CenterRadius = 1723.*mm;

  // Z coordinate of magnet exit side (aligned with PADME Z axis)
  //  fDHSTB002ExitPosZ = -2001.*mm+400.*mm*cos(45.*deg); // Verify from drawings
  fDHSTB002ExitPosZ = -1635.00*mm; // From CAD drawings 12/04

  // Dimensions of DHSTB002 magnet

  // Angular span of the magnet
  fDHSTB002AngularSpan = 45.*deg;

  // Transversal size of the yoke
  fDHSTB002SizeX = 728.*mm;
  fDHSTB002SizeY = 670.*mm;
  fDHSTB002MinRadius = 1329.*mm;

  // Yoke has a symmetric H-shaped hole centered on beam radius
  // Y
  // ^ |-----------------------|
  // | +------+   L1    +------+
  // | |      |         |      |
  // | |      |         |      |
  // | |      |         |      |
  // | |   -  +---------+      |
  // | |   |                   |
  // | |   |L4     o         L3|
  // | |   |                   |
  // | |   -  +---------+      |
  // | |      |   L2    |      |
  // | |      |         |      |
  // | |      |         |      |
  // | +------+         +------+
  // +--------------------------->R                   
  fDHSTB002L1 = 424.*mm;
  fDHSTB002L2 = 170.*mm;
  fDHSTB002L3 = 128.*mm;
  fDHSTB002L4 =  42.*mm;

  // Dimensions of the magnetic field volume inside the magnet
  // Field is now <>0 only in the central section of the H-shaped hole (can be refined)
  fMagVolSizeX = fDHSTB002L2;
  fMagVolSizeY = fDHSTB002L4-10.*um; // Add a small tolerance to avoid overlaps
  fMagVolMinRadius = fDHSTB002CenterRadius-0.5*fMagVolSizeX;
  fMagVolMaxRadius = fDHSTB002CenterRadius+0.5*fMagVolSizeX;

  // Dimensions of the beam pipe inside the magnet

  // Transverse section
  fMagPipeSizeX = 59.*mm;
  fMagPipeSizeY = 40.*mm;
  fMagPipeMinRadius = fDHSTB002CenterRadius-0.5*fMagPipeSizeX;
  fMagPipeMaxRadius = fDHSTB002CenterRadius+0.5*fMagPipeSizeX;
  fMagPipeHoleSizeX = 53.*mm;
  fMagPipeHoleSizeY = 32.*mm;
  fMagPipeHoleMinRadius = fDHSTB002CenterRadius-0.5*fMagPipeHoleSizeX;
  fMagPipeHoleMaxRadius = fDHSTB002CenterRadius+0.5*fMagPipeHoleSizeX;

  // Length of the straight section inside the magnet
  fMagPipeLineLength = 1452.1*mm;

  // Straight section with flange (outside magnet yoke)
  fMagPipeStraightLength = 207.*mm;
  fMagPipeFlangeThick = 17.5*mm; // Thickness from DN63 datasheet
  fMagPipeFlangeRadius = 0.5*113.5*mm; // Diameter from DN63 datasheet
  fMagPipeFlangePosZ = 0.5*fMagPipeStraightLength-0.5*fMagPipeFlangeThick;

  // Positions and rotations of straight sections

  fMagPipeStraightFrontPosX = 0.;
  fMagPipeStraightFrontPosZ = fDHSTB002ExitPosZ+0.5*fMagPipeStraightLength;
  fMagPipeStraightFrontRotY = 0.*deg;

  fMagPipeStraightBackPosX = fDHSTB002CenterRadius*(1.-cos(fDHSTB002AngularSpan))+0.5*fMagPipeStraightLength*sin(fDHSTB002AngularSpan);
  fMagPipeStraightBackPosZ = fDHSTB002ExitPosZ-fDHSTB002CenterRadius*sin(fDHSTB002AngularSpan)-0.5*fMagPipeStraightLength*cos(fDHSTB002AngularSpan);
  fMagPipeStraightBackRotY = -(180.*deg-fDHSTB002AngularSpan);

  fMagPipeStraightSidePosX = fDHSTB002CenterRadius*(1.-cos(fDHSTB002AngularSpan))-(fMagPipeLineLength+0.5*fMagPipeStraightLength)*sin(fDHSTB002AngularSpan);
  fMagPipeStraightSidePosZ = fDHSTB002ExitPosZ-fDHSTB002CenterRadius*sin(fDHSTB002AngularSpan)+(fMagPipeLineLength+0.5*fMagPipeStraightLength)*cos(fDHSTB002AngularSpan);
  fMagPipeStraightSideRotY = fDHSTB002AngularSpan;

  // Entrance (upstream) and exit (downstream) coordinates of center of magnet pipe

  fMagPipeEnterPosX = fDHSTB002CenterRadius*(1.-cos(fDHSTB002AngularSpan))+fMagPipeStraightLength*sin(fDHSTB002AngularSpan);
  fMagPipeEnterPosY = 0.;
  fMagPipeEnterPosZ = fDHSTB002ExitPosZ-fDHSTB002CenterRadius*sin(fDHSTB002AngularSpan)-fMagPipeStraightLength*cos(fDHSTB002AngularSpan);

  fMagPipeExitPosX = 0.;
  fMagPipeExitPosY = 0.;
  fMagPipeExitPosZ = fDHSTB002ExitPosZ+fMagPipeStraightLength;

  // Properties of junction between magnet pipe and long pipe to Be window

  fBeJunctionLength = 288.*mm;
  fBeJunctionRIn = 26.5*mm;
  fBeJunctionROut = 30.0*mm;

  fBeJunctionFlangeFrontRadius = 0.5*113.5*mm; // Diameter from DN63 datasheet;
  fBeJunctionFlangeFrontThick = 17.5*mm; // Thickness from DN63 datasheet;

  fBeJunctionFlangeBackRadius = 0.5*113.5*mm; // Diameter from DN63 datasheet;
  fBeJunctionFlangeBackThick = 17.5*mm; // Thickness from DN63 datasheet;

  // Properties of long pipe to Be window

  fBePipeLength = 1664.*mm;
  fBePipeRIn = 21.0*mm;
  fBePipeROut = 24.0*mm;

  fBePipeFlangeFrontRadius = 0.5*113.5*mm; // Diameter from DN63 datasheet;
  fBePipeFlangeFrontThick = 17.5*mm; // Thickness from DN63 datasheet;

  fBePipeFlangeBackRadius = 0.5*113.5*mm; // Diameter from DN63 datasheet;
  fBePipeFlangeBackThick = 17.5*mm; // Thickness from DN63 datasheet;

  // Properties of Be thin window and its support flange

  fBeWindowRadius = 30.5*mm;
  fBeWindowThick = 250.*um;
  fBeWindowFlangeRadius = 0.5*113.5*mm;
  fBeWindowFlangeThick = 36.*mm; // was 17.5*mm (?)

  // Geometry of the quadrupoles 

  fQuadBoxSizeX = 348.*mm; // M. Raggi From L. Foggetta Drawings 11/04/2019
  fQuadBoxSizeY = 348.*mm; // guess L. Foggetta Drawings are 2 dimentional 11/04/2019
  fQuadBoxSizeZ = 186.*mm; // M. Raggi From L. Foggetta Drawings 11/04/2019
  // There is a inner square hole of unknown size with the coils insideit 
	       
  fQuadMagSizeX = 50.*mm; // M. Raggi From P. Valente data sheet  08/04/2019
  fQuadMagSizeY = 50.*mm; // M. Raggi From P. Valente data sheet  08/04/2019
  fQuadMagSizeZ = 200.*mm; // M. Raggi From P. Valente data sheet  08/04/2019

  //Front face of the box to front face of the flange M. Raggi From L. Foggetta Drawings 11/04/2019
  fQ2DistFromDHSTB002 = 379.*mm; 
  fQ1Q2Dist = 900.*mm; //center to center

}

BeamLineGeometry::~BeamLineGeometry()
{}
