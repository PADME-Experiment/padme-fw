// MMegaGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
// --------------------------------------------------------------

#include "MMegaGeometry.hh"

MMegaGeometry* MMegaGeometry::fInstance = 0;

MMegaGeometry* MMegaGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new MMegaGeometry(); }
  return fInstance;
}

MMegaGeometry::MMegaGeometry()
{

  fVerbose = 0; // Do not show debug output

  fDetectorSetup = 50; // Default to 2025 setup


  //readout plane composition
  fMMegaCopperSizeZ = 0.018*mm; //strip
  fMMegaFR4SizeZ = 0.900*mm; //FR4 layers updated 11/02/26 ?
  fMMegaNomexSizeZ = 10*mm; //ok 11/02/26
  fMMegaKaptonSizeZ = 0.050*mm; //ok 11/02/26
  fMMegaFR4ReadoutSizeZ = 0.050*mm; //ok 11/02/26
  fMMegaCarbonSizeZ = 0.004*mm; //thickness of carbon resistive layer
  fMMegaLayerGap = 0*um; //otherwise Geant4 overlaps the layers in a weird way

 
  // size of Faraday FR4 panels
  fMMegaFaradayPanelSizeX = 880*mm; //11/02/26
  fMMegaFaradayPanelSizeY = 880*mm;
  fMMegaFaradayPanelSizeZ = 0.700*mm;
  fMMegaFaradayFR4SizeZ = 0.700*mm;
  fMMegaFaradayCopperSizeZ = 0.100*mm;

  //size of strips
  fMMegaStripPitch = 1.2*mm; //to be updated //1mm
  fMMegaStripWidth = 1.2*mm; //same
  

  // size of active area --> strip related (rectangular)
  fMMegaPanelSizeV1 =(fMMegaStripPitch*(512+1))*mm; //active area updated 11/02/26
  fMMegaPanelSizeV2 =(fMMegaStripPitch*(512+6))*mm;
  fV1StripStartPos = -fMMegaPanelSizeV1/2.; //to be changed when i have readout maps
  fV2StripStartPos = -fMMegaPanelSizeV2/2.;
  //setting non active channels (pcb holes) false
  for(int i=0; i<512+1; i++){
    isChV1Active[i]=true;
  }
  isChV1Active[256]=false;
  for(int i=0; i<512+6; i++){
    isChV2Active[i]=true;
  }
  for(int i=256; i<256+6; i++){
  isChV2Active[i]=false;
  }
  
  
  // size of frame area --> squared
  fMMegaPanelSizeX= fMMegaPanelSizeV2;
  fMMegaPanelSizeY= fMMegaPanelSizeV2;
  fMMegaPanelSizeZ = 3*fMMegaCopperSizeZ + 2*fMMegaFR4SizeZ + fMMegaKaptonSizeZ + fMMegaFR4ReadoutSizeZ+fMMegaNomexSizeZ + fMMegaCarbonSizeZ+2*fMMegaFaradayCopperSizeZ+2*fMMegaFaradayPanelSizeZ; 
  //size of drift region
  fMMegaDriftSizeX = fMMegaPanelSizeX;
  fMMegaDriftSizeY = fMMegaPanelSizeY;
  fMMegaDriftSizeZ = 100*mm; //right value 
  //size of amplification gap
  fMMegaAmpGapSizeX = fMMegaPanelSizeX;
  fMMegaAmpGapSizeY = fMMegaPanelSizeY;
  fMMegaAmpGapSizeZ = 0.150*mm; //active area updated 11/02/26

  // size of the detector mother volume
  // fMMegaSizeX = fMMegaPanelSizeX + 5*mm;  // add 5 mm as in other detectors
  // fMMegaSizeY = fMMegaPanelSizeY + 5*mm;
  // fMMegaSizeZ = fMMegaDriftSizeZ + 2*fMMegaAmpGapSizeZ + 2*fMMegaPanelSizeZ + 5*mm; 
  fMMegaSizeX = fMMegaFaradayPanelSizeX + 5*mm;  // add 5 mm as in other detectors
  fMMegaSizeY = fMMegaFaradayPanelSizeY + 5*mm;
  fMMegaSizeZ = fMMegaDriftSizeZ + 2*fMMegaAmpGapSizeZ + 2*fMMegaPanelSizeZ + 5*mm; 
  
  //size of pads
  fMMegaXPadArea = 0.49*mm2;
  fMMegaYPadArea = 0.25*mm2;
  fMMegaPadDistance = 1.1*mm;
  fXPadStartPos = -fMMegaPanelSizeX/2.; //to be changed when i have readout maps
  fYPadStartPos = -fMMegaPanelSizeX/2.;
  
  //size of meshes
  fMMegaAmpMeshSizeZ = (0.018/2)*mm;
  fMMegaCathodeMeshSizeZ = (0.018/2)*mm;//(0.030/4)*mm; //updated 16/02/26
  fMMegaPadDistance = 1.1*mm;

  //fMMegaFrontFacePosZ = 2612.41*mm - fMMegaSizeZ - 25*mm; // Relative to center of magnet (using runIII ECal position)
  fMMegaFrontFacePosZ = 2526.5*mm - fMMegaSizeZ - 45*mm; // USE THE SURVEY !!!!!
  fMMegaDisplacementX=0*mm; //10*mm;
  fMMegaDisplacementY=0*mm;//20*mm;
  fMMegaRotationAngle=0*deg;
  fMMegaDigitizerName = "MMegaDigitizer";
  fMMegaSensitiveDetectorName = "MMegaSD";
}



std::vector<G4String> MMegaGeometry::GetHashTable()
{
  std::vector<G4String> hash;
  std::ostringstream buffer;
  return hash;
}





MMegaGeometry::~MMegaGeometry()
{}
