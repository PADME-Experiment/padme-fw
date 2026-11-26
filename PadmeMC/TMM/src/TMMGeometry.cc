// TMMGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Elisa Di Meco (elisa.dimeco@lnf.infn.it) 2026-05-27
// --------------------------------------------------------------

#include "TMMGeometry.hh"

TMMGeometry* TMMGeometry::fInstance = 0;

TMMGeometry* TMMGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new TMMGeometry(); }
  return fInstance;
}

TMMGeometry::TMMGeometry()
{

  fVerbose = 0; // Do not show debug output

  fDetectorSetup = 50; // Default to 2025 setup


  //readout plane composition ????
  fTMMCopperSizeZ = 0.018*mm; //strip
  fTMMFR4SizeZ = 0.900*mm; //FR4 layers updated 11/02/26 
  fTMMNomexSizeZ = 10*mm; //ok 11/02/26 
  fTMMKaptonSizeZ = 0.050*mm; //ok 11/02/26

  fTMMFR4ReadoutSizeZ = 0.050*mm; //ok 11/02/26
  fTMMCarbonSizeZ = 0.004*mm; //thickness of carbon resistive layer
  fTMMLayerGap = 0*um; //otherwise Geant4 overlaps the layers in a weird way
  //front panel
  fTMMMylarClosingZ = 100*um;
 
  // size of Faraday FR4 panel
  fTMMFaradayPanelSizeX = 200*mm; //set to 670*mm when shifted;////11/02/26
  fTMMFaradayPanelSizeY = 200*mm;
  fTMMFaradayPanelSizeZ = 1.700*mm;
  fTMMFaradayFR4SizeZ = 0.700*mm; //da rimuovere nel primo pannello raggio 10 cm 
  fTMMFaradayCopperSizeZ = 0.050*mm; //da rimuovere nel primo pannello raggio 10 cm 
  fTMMLateralFR4 = 10*mm;
  fTMMLateralBrass = 0.5*mm;
  //size of strips
  fTMMStripPitch = 0.250*mm; //updated 11/02/26
  fTMMStripWidth = 0.250*mm; //same
  
  

  fTMMAluFrameSizeZ = 2.2*cm; //updated 28/05/26
  fTMMAluFrameSizeX = 187.3*mm; //updated 28/05/26
  fTMMAluFrameSizeY = 187.3*mm; //updated 28/05/26
  fTMMAluFrameInternalSizeX = fTMMAluFrameSizeX - 2*1.58*cm;
  fTMMAluFrameInternalSizeY = fTMMAluFrameSizeY - 2*1.58*cm;
  fTMMExtFrameZ = 1.7*mm; //updated 28/05/26
  fTMMExtFrameY = fTMMAluFrameSizeY-4*cm; //updated 28/05/26
  fTMMExtFrameX = fTMMAluFrameSizeY-4*cm; ; //updated 28/05/26
  
  // size of active area --> squared
  fTMMPanelSizeX= 100*mm;
  fTMMPanelSizeY= 100*mm;
  //  active area starte --> squared
  fVXStripStartPos= -fTMMPanelSizeX/2.;
  fVYStripStartPos= -fTMMPanelSizeY/2.;

  fTMMPanelSizeZ = 2*fTMMCopperSizeZ  + 1*fTMMKaptonSizeZ + 1*fTMMFR4ReadoutSizeZ + 1*fTMMCarbonSizeZ+5*fTMMFaradayCopperSizeZ+1*fTMMFaradayPanelSizeZ+fTMMExtFrameZ; 
  //size of drift region
  fTMMDriftSizeX = fTMMPanelSizeX;
  fTMMDriftSizeY = fTMMPanelSizeY;
  fTMMDriftSizeZ = 5*mm; //updated 27/05/26
  //size of amplification gap
  fTMMAmpGapSizeX = fTMMPanelSizeX;
  fTMMAmpGapSizeY = fTMMPanelSizeY;
  fTMMAmpGapSizeZ = 0.128*mm; //updated 27/05/26

  // size of the detector mother volume
  // fTMMSizeX = fTMMPanelSizeX + 5*mm;  // add 5 mm as in other detectors
  // fTMMSizeY = fTMMPanelSizeY + 5*mm;
  // fTMMSizeZ = fTMMDriftSizeZ + 2*fTMMAmpGapSizeZ + 2*fTMMPanelSizeZ + 5*mm; 
  fTMMSizeX = fTMMFaradayPanelSizeX + 5*mm;  // add 5 mm as in other detectors
  fTMMSizeY = fTMMFaradayPanelSizeY + 5*mm;

  fTMMSizeZ = fTMMAluFrameSizeZ +  1*fTMMFR4ReadoutSizeZ +fTMMFaradayCopperSizeZ+ fTMMExtFrameZ + 5*mm; 
  //size of pads
  fTMMXPadArea = 0.49*mm2;
  fTMMYPadArea = 0.25*mm2;
  fTMMPadDistance = 1.1*mm;
  fXPadStartPos = -fTMMPanelSizeX/2.; //to be changed when i have readout maps
  fYPadStartPos = -fTMMPanelSizeY/2.;
  
  //size of meshes
  fTMMAmpMeshSizeZ = (0.030/2)*mm; //mesh 70x30
  fTMMCathodeMeshSizeZ = (0.030/2)*mm;//mesh 70x30
  fTMMPadDistance = 1.1*mm;

  
  fTMMFrontFacePosZ = 3384.683*mm;// allignaments (2.781*mm, -1.232*mm, 3382.683*mm)
  fTMMDisplacementX=2.781*mm; 
  fTMMDisplacementY=-1.232*mm; 
  
  //**************** RUN IV ******************************

  //   Survey points (X, Y, Z) [mm]:
  //   1: [-129.95  129.45 3384.59]
  //   2: [-128.15   84.84 3384.53]
  //   3: [ 123.04  -63.21 3381.  ]

  // Face normal (world): [ 0.013582 -0.000797  0.999907]
  //   tilt vs Z axis        : 0.7796 deg
  //   slope XZ (about Y)    : +0.7782 deg
  //   slope YZ (about X)    : +0.0457 deg

  // Reference points  CAD(local)  ->  measured(world, lifted on plane):
  //   R1: [101.074 129.949]  ->  [ 101.69      130.26     3381.444183]
  //   R2: [127.104 -98.791]  ->  [ 131.48      -97.78     3380.857807]
  //   baseline CAD 230.216 mm, measured 229.978 mm
  //   pure 2D XY angle (your script): +0.9505 deg
  //   in-plane roll gamma           : +0.9518 deg
  //   in-plane residuals [mm]: [[ 0.0154 -0.118 ]
  //  [-0.0154  0.118 ]]  RMS 0.1190
  //   NOTE: position Z is the surveyed plane surface at the local origin; add the offset surface -> volume centre if needed.

  // Method: plane normal + CAD reference points (in-plane 2D fit)
  // Rotation matrix R (active, local -> world):
  // [[ 0.99977  -0.016599  0.013582]
  //  [ 0.016611  0.999862 -0.000797]
  //  [-0.013567  0.001022  0.999907]]
  // Geant4 angles (rotateX, rotateY, rotateZ in this order):
  //   about X: +0.0586 deg = +1.0224 mrad
  //   about Y: +0.7774 deg = +13.5675 mrad
  //   about Z: +0.9519 deg = +16.6135 mrad

  fTMMRotationAngleX=0.0586*deg; 
  fTMMRotationAngleY=0.7774*deg; 
  fTMMRotationAngleZ=0.9519*deg; 

  fTMMDigitizerName = "TMMDigitizer";
  fTMMSensitiveDetectorName = "TMMSD";
}



std::vector<G4String> TMMGeometry::GetHashTable()
{
  std::vector<G4String> hash;
  std::ostringstream buffer;
  return hash;
}





TMMGeometry::~TMMGeometry()
{}
