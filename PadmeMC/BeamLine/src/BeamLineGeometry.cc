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

  // Some basic chamber quantities from Cesidio's CAD drawings
  fBeWThick=250.*um;  //250 microns 
  fBeWDiameter=61.*mm;

  //  fBeWPosZ=-3374.2 -1000.*cos(45*deg);  //1 meter from DHSTB002
  //  fBeWPosX= 659.5  +1000.*sin(45*deg);  //1 meter from DHSTB002

  // Be window support flange.
  fBeFlThick = 17.5*mm;       //From leibold specs
  fBeFlDiameter = 113.5*mm;   //From DHSTB002 inner chamber design
  //  fBeFlInnHole =66*mm;  //not used the flage is filled 

  fDHSTB002Thick         =  670.*mm; // from drawings
  fDHSTB002Radius        = 2057.*mm; //radius
  fDHSTB002InnHole       = 1389.*mm; //radius
  fDHSTB002CenterRadius  = 1723.*mm; //radius

  //Metal box DHSTB002 inner chamber design from chamber design
  fDHSTB002PipeThick    = 40.*mm;   
  fDHSTB002PipeRadius   = fDHSTB002CenterRadius+0.5*59.*mm;  //radius
  fDHSTB002PipeInnHole  = fDHSTB002CenterRadius-0.5*59.*mm;  //radius

  //pipe hole from DHSTB002 inner chamber design
  fDHSTB002PipeGapThick    = 32.*mm;   
  fDHSTB002PipeGapRadius   = fDHSTB002CenterRadius+0.5*53*mm;  //radius
  fDHSTB002PipeGapInnHole  = fDHSTB002CenterRadius-0.5*53*mm;  //radius

  fDHSTB002PosY = 0*mm;
  fDHSTB002PosZ = -2001.*mm+400.*mm*cos(45.*deg); 
  
  double TGapXSize =170*mm;
  //First gap in the H structure
  fDHSTB002Gap1Thick   = 128.*mm ; //From Drawings   
  fDHSTB002Gap1Radius  = (fDHSTB002CenterRadius-TGapXSize/2);
  fDHSTB002Gap1InnHole = (fDHSTB002InnHole+127.*mm) ; //Inner wall thickness 

  fDHSTB002Gap1PosX = 0.*mm ;
  fDHSTB002Gap1PosY = 0.*mm ;
  fDHSTB002Gap1PosZ = 0.*mm ;

  //Second gap in the H structure
  fDHSTB002Gap2Thick   = 128.*mm ; //From Drawings      
  fDHSTB002Gap2Radius  = fDHSTB002Radius-127.*mm;
  fDHSTB002Gap2InnHole = fDHSTB002CenterRadius+0.5*TGapXSize;

  fDHSTB002Gap2PosX = 0.*mm;
  fDHSTB002Gap2PosY = 0.*mm; 
  fDHSTB002Gap2PosZ = 0.*mm;

  //Thin gap in the H structure check drawings
  fDHSTB002TGapThick   = 42.*mm; //From Drawings          
  fDHSTB002TGapRadius  = fDHSTB002CenterRadius+86.*mm;
  fDHSTB002TGapInnHole = fDHSTB002CenterRadius-86.*mm;

  fDHSTB002TGap2PosX = 0.*mm;
  fDHSTB002TGap2PosY = 0.*mm;
  fDHSTB002TGap2PosZ = 0.*mm;

  //Gamma line pipe every number is currently a guess I have no drawing M. Raggi 25/03/2019

  fDHSTB002GLineGapPosX = 0.*mm;
  fDHSTB002GLineGapPosY = 0.*mm;
  fDHSTB002GLineGapPosZ = 0.*mm;

  fDHSTB002GLineGapSizeX = 120.*mm; //random numbers to be fixed
  fDHSTB002GLineGapSizeY = 170.*mm; //random numbers to be fixed
  fDHSTB002GLineGapSizeZ = 170.*mm; //random numbers to be fixed

  fDHSTB002GLinePipePosX = 0.*mm;
  fDHSTB002GLinePipePosY = 0.*mm;
  fDHSTB002GLinePipePosZ = -5000.*mm;

  fDHSTB002GLinePipeSizeX = 59.*mm; //from drawings same as curved pipe
  fDHSTB002GLinePipeSizeY = fDHSTB002PipeThick; //from drawings same as curved pipe
  fDHSTB002GLinePipeSizeZ = 1645.1*mm; //1452.1 from drawings or 1645.1 flangia flangia
  fDHSTB002GLinePipeGapSizeX = 53.*mm;  //from drawings same as curved pipe
  fDHSTB002GLinePipeGapSizeY = fDHSTB002PipeGapThick;  //from drawings same as curved pipe
  fDHSTB002GLinePipeGapSizeZ = fDHSTB002GLinePipeSizeZ+2.*mm; //from drawings same as curved pipe

  // Default magnetic field
  fDHSTB002MagneticFieldY = -1.029*tesla;

  // Radius of BeamPipe center
  fDHSTB002CenterRadius = 1723.*mm;

  // Z coordinate of magnet exit side (alignet with PADME Z axis)
  fDHSTB002ExitPosZ = -2001.*mm+400.*mm*cos(45.*deg);

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

  // Straight section with flange (outside magnet yoke)
  fMagPipeStraightLength = 210.*mm;
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

}

BeamLineGeometry::~BeamLineGeometry()
{}
