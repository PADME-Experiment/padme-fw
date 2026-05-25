// MMGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
// --------------------------------------------------------------

#include "MMGeometry.hh"

MMGeometry* MMGeometry::fInstance = 0;

MMGeometry* MMGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new MMGeometry(); }
  return fInstance;
}

MMGeometry::MMGeometry()
{

  fVerbose = 0; // Do not show debug output

  fDetectorSetup = 50; // Default to 2025 setup


  //readout plane composition
  fMMCopperSizeZ = 0.018*mm; //strip
  fMMFR4SizeZ = 0.900*mm; //FR4 layers updated 11/02/26 ? /da primo strato raggio 10 cm 
  fMMNomexSizeZ = 10*mm; //ok 11/02/26 //da rimuovere nel primo pannello raggio 10 cm 
  fMMKaptonSizeZ = 0.050*mm; //ok 11/02/26
  fMMFR4ReadoutSizeZ = 0.050*mm; //ok 11/02/26
  fMMCarbonSizeZ = 0.004*mm; //thickness of carbon resistive layer
  fMMLayerGap = 0*um; //otherwise Geant4 overlaps the layers in a weird way

 
  // size of Faraday FR4 panels
  fMMFaradayPanelSizeX = 880*mm; //set to 670*mm when shifted;////11/02/26
  fMMFaradayPanelSizeY = 880*mm;
  fMMFaradayPanelSizeZ = 0.700*mm;
  fMMFaradayFR4SizeZ = 0.700*mm; //da rimuovere nel primo pannello raggio 10 cm 
  fMMFaradayCopperSizeZ = 0.100*mm; //da rimuovere nel primo pannello raggio 10 cm 
  fMMLateralFR4 = 10*mm;
  fMMLateralBrass = 0.5*mm;
  //size of strips
  fMMStripPitch = 1.2*mm; //to be updated //1mm
  fMMStripWidth = 1.2*mm; //same
  

  // size of active area --> strip related (rectangular)
  fMMPanelSizeV1 =(fMMStripPitch*(512+fV1StripHole))*mm; //active area updated 11/02/26
  fMMPanelSizeV2 =(fMMStripPitch*(512+fV2StripHole))*mm;
  fV1StripStartPos = -fMMPanelSizeV1/2.; //to be changed when i have readout maps
  fV2StripStartPos = -fMMPanelSizeV2/2.;
  //setting non active channels (pcb holes) false
  for(int i=0; i<512+fV1StripHole; i++){
    isChV1Active[i]=true;
  }
  isChV1Active[256]=false;
  for(int i=0; i<512+fV2StripHole; i++){
    isChV2Active[i]=true;
  }
  for(int i=256; i<256+fV2StripHole; i++){
  isChV2Active[i]=false;
  }
  
  
  // size of frame area --> squared
  fMMPanelSizeX= fMMPanelSizeV2;
  fMMPanelSizeY= fMMPanelSizeV2;
  fMMPanelSizeZ = 3*fMMCopperSizeZ + 2*fMMFR4SizeZ + fMMKaptonSizeZ + fMMFR4ReadoutSizeZ+fMMNomexSizeZ + fMMCarbonSizeZ+2*fMMFaradayCopperSizeZ+2*fMMFaradayPanelSizeZ; 
  //size of drift region
  fMMDriftSizeX = fMMPanelSizeX;
  fMMDriftSizeY = fMMPanelSizeY;
  fMMDriftSizeZ = 100*mm; //right value 
  //size of amplification gap
  fMMAmpGapSizeX = fMMPanelSizeX;
  fMMAmpGapSizeY = fMMPanelSizeY;
  fMMAmpGapSizeZ = 0.150*mm; //active area updated 11/02/26

  // size of the detector mother volume
  // fMMSizeX = fMMPanelSizeX + 5*mm;  // add 5 mm as in other detectors
  // fMMSizeY = fMMPanelSizeY + 5*mm;
  // fMMSizeZ = fMMDriftSizeZ + 2*fMMAmpGapSizeZ + 2*fMMPanelSizeZ + 5*mm; 
  fMMSizeX = fMMFaradayPanelSizeX + 5*mm;  // add 5 mm as in other detectors
  fMMSizeY = fMMFaradayPanelSizeY + 5*mm;
  fMMSizeZ = fMMDriftSizeZ + 2*fMMAmpGapSizeZ + 2*fMMPanelSizeZ + 5*mm; 
  
  //size of pads
  fMMXPadArea = 0.49*mm2;
  fMMYPadArea = 0.25*mm2;
  fMMPadDistance = 1.1*mm;
  fXPadStartPos = -fMMPanelSizeX/2.; //to be changed when i have readout maps
  fYPadStartPos = -fMMPanelSizeY/2.;
  
  //size of meshes
  fMMAmpMeshSizeZ = (0.018/2)*mm;
  fMMCathodeMeshSizeZ = (0.018/2)*mm;//(0.030/4)*mm; //updated 16/02/26
  fMMPadDistance = 1.1*mm;

  //fMMFrontFacePosZ = 2612.41*mm - fMMSizeZ - 25*mm; // Relative to center of magnet (using runIII ECal position)
  fMMFrontFacePosZ = 2353.32*mm;//2526.5*mm - fMMSizeZ - 45*mm; //  SURVEY  INFO 2353.32!!!!! vs 2349.084 mm calculated
  fMMDisplacementX=10*mm; //50*mm+fMMFaradayPanelSizeX/2 for slipped chamber configuration;//10*mm; //
  fMMDisplacementY=20*mm; //0*mm;//;
  fMMRotationAngle=0*deg; // z-x 0.1 deg (testa in avanti)
  fMMDigitizerName = "MMDigitizer";
  fMMSensitiveDetectorName = "MMSD";
}



std::vector<G4String> MMGeometry::GetHashTable()
{
  std::vector<G4String> hash;
  std::ostringstream buffer;
  return hash;
}





MMGeometry::~MMGeometry()
{}
