
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
  fMylarWindowEnable = true;

  // Define name of Be and Mylar window sensitive detectors
  fBeWSensitiveDetectorName = "BeWSD";
  fMylarWSensitiveDetectorName = "MylarWSD";
  fBeamFlagEnable = true;
  // Set Be window sensitive detector positioned
  // Define name of Be window sensitive detector
  fBeamFlag1SensitiveDetectorName = "BDSFL";
  fBeamFlag2SensitiveDetectorName = "BUSFL";
  // Default magnetic field
  fDHSTB002MagneticFieldY = -1.055*tesla;
  fDHSTB001MagneticFieldY = -1.055*tesla;

  //  fQuadMagneticFieldGrad  =  10*tesla/m;
  fQuadrupolesEnable = false;

  fBeamLineSetup=0;  //M. Raggi 06/2020
  //printf("***************************** tesla %f m %f \n ",tesla,m);
  printf("******Beam Line Geometry *********************** %f \n",fBeamLineSetup);

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

  // Dimensions of the beam pipe inside the magnets DHSTB001 and DHSTB002
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
  
  // Properties of Mylar thin window and its support flange Raggi 06/2020
  //verifica con foggetta tutte le misure
  fMylarWindowRadius = 30.5*mm;
  fMylarWindowThick =  0.125*mm;  //Checked wiht BTF report M. Raggi 03.21
  fWindowThickness  =  0.125*mm;  //Checked wiht BTF report M. Raggi 03.21
  fMylarWindowFlangeRadius = 0.5*113.5*mm;
  fMylarWindowFlangeThick = 36.*mm; // was 17.5*mm (?)

  fQuadMagSizeZ = 263.*mm; // checked on drawing M. Raggi and E. Leonardi 02/03/2021

  ///********M.R. Default values for RUN II ***********
  //         PADME-- DHSTB002 -- Q4 --- Q3 --|  |-- Q2----Q1-----MyW
  ///        When BTF current I is read the gradient is obtained using
  //         Quadrupoles Gradient = 8.372E-2 * I + 2.3767E-2

  fQ1MagneticFieldGrad =  2.810*tesla/m; // M.R. from L FOggetta e-mail 31/03/2021
  fQ2MagneticFieldGrad =  2.510*tesla/m;
  fQ3MagneticFieldGrad =  3.666*tesla/m; // was 3.833*tesla/m; in July Run
  fQ4MagneticFieldGrad =  3.925*tesla/m;

  ////////////////////////////////////////////////////////////////////////
  // Collimators after mylar sltb4(LR) <-- sltb3(UPDW)  <-- Mylar 
  ///////////////////////////////////////////////////////////////////////
  // November 2020 tune L. Foggetta
  // the aperture is calculated using the sum of TBXX-TBXX_M which is the maximum 
  //TB4R   26.16 TB4L   22.30          TB3 Up 26.06      TB3 Down 23.32	
  //TB4R_M 28.44 TB4L_M 24.01 	       TB3U_M 26.024	 TB3D_M   26.39    	
  //TB4Ape  2.28         1.71 = 4mm    TB4Ape  0                   3.07 = 3.07 mm

  fSLTB4Aperture= 4.0*mm;
  fSLTB3Aperture= 3.1*mm;
  fSLTB2Aperture= 1.7*mm;
  fSLTB5Aperture= 2.*mm;

//  fQ1MagneticFieldGrad =  3.010*tesla/m; // M.R. tuning better Y 1mm
//  fQ2MagneticFieldGrad =  2.710*tesla/m;
//  fQ1MagneticFieldGrad =  3.210*tesla/m; // M.R. tuning better Y 1mm
//  fQ2MagneticFieldGrad =  2.910*tesla/m;
  
  //Front face of the box to front face of the flange M. Raggi From L. Foggetta Drawings 11/04/2019
  fQ4DistFromDHSTB002 = 380.*mm; // Checked on drawing M. Raggi and E. Leonardi 02/03/2021
  fQ1Q2Dist = 1100.*mm; //center to center quads in LINAC Checked on drawing M. Raggi and E. Leonardi 02/03/2021
  fQ3Q4Dist =  900.*mm; //center to center quads in BTF checked on drawing M. Raggi and E. Leonardi 02/03/2021

  // BEAM line 2020 geometry parameters M. Raggi 02.2021   
  fWallThickness        =  141.5*cm;   //check
  fWallPipeLen          =  251.4*cm;   //check
  fWallHoleRadius       =   30.0*cm;   //check
  fWallMylarWinDistance =  297.0*cm;   //was wrong? 
  //  fWallMylarWinDistance =  267.0*cm;   //Check 
  fDHSTB002WallDistance =  380.5*cm;   //was wrong ?
  //fDHSTB002WallDistance =  411.5*cm;   //check seem to be 4115 08/2022

  // BEAM line 2022 geometry parameters M. Raggi 08.2022 
  // Dimensions of the beam pipe inside the pulsed Magnet
  // 

  fDHSTB002ToPulsedMagnet = 6642.*mm;   //flange to flange Raggi
  fPulsedPipeSizeX =   63.*mm; //full size
  fPulsedPipeSizeY =   24.*mm; //full size
  fPulsedPipeLength= 1044.*mm;

  fPulsedPipeHoleSizeX = 57.*mm;
  fPulsedPipeHoleSizeY = 18.*mm;

  fPulsedFlangeR     =  0.5*113.5*mm;
  fPulsedFlangeThick =  17.5*mm;        
               
  //  DN 63 pipes specs  diametro interno 66 esterno 70
  f2020PipeOuterRadius  =    3.5*cm; // Using DN specs
  f2020PipeInnerRadius  =    3.3*cm; // Using DN specs

  fBTFTargetEnable = false;
  fBTFTargetThickness = 24.4*mm;
  fBTFTargetDistance  = 2370.*mm; 
}

BeamLineGeometry::~BeamLineGeometry()
{}
