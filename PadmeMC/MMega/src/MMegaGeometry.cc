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

  fDetectorSetup = 10; // Default to 2019 setup


  // size of front and back panels
  fMMegaPanelSizeX = 600*mm;
  fMMegaPanelSizeY = 600*mm;
  fMMegaPanelSizeZ = 3*fMMegaCopperSizeZ + 2*fMMegaFreonSizeZ + 2*fMMegaKaptonSizeZ + fMMegaNomexSizeZ + fMMegaCarbonSizeZ; 
  
  // size of the detector mother volume
  fMMegaSizeX = fMMegaPanelSizeX + 5*mm;  // add 5 mm as in other detectors
  fMMegaSizeY = fMMegaPanelSizeY + 5*mm;
  fMMegaSizeZ = fMMegaDriftSizeZ + 2*fMMegaAmpGapSizeZ + 2*fMMegaPanelSizeZ + 5*mm; 

  //size of drift region
  fMMegaDriftSizeX = fMMegaPanelSizeX;
  fMMegaDriftSizeY = fMMegaPanelSizeY;
  fMMegaDriftSizeZ = 100*mm;
  
  //size of amplification gap
  fMMegaAmpGapSizeX = fMMegaPanelSizeX;
  fMMegaAmpGapSizeY = fMMegaPanelSizeY;
  fMMegaAmpGapSizeZ = 0.150*mm;

  //readout plane composition
  fMMegaCopperSizeZ = 0.018*mm;
  fMMegaFreonSizeZ = 0.500*mm;
  fMMegaNomexSizeZ = 10*mm;
  fMMegaKaptonSizeZ = 0.050*mm;
  fMMegaCarbonSizeZ = 0.004*mm; //thickness of carbon resistive layer
  fMMegaLayerGap = 0.1*um; //otherwise Geant4 overlaps the layers in a weird way

  //size of strips
  fStripPitch = 0.400*mm; //to be updated
  fStripWidth = 0.300*mm; //same
  fXStripStartPos = -fMMegaPanelSizeX/2.; //to be changed when i have readout maps
  fYStripStartPos = -fMMegaPanelSizeX/2.;

  //size of pads
  fMMegaXPadArea = 0.6*mm2;
  fMMegaYPadArea = 0.7*mm2;
  fMMegaPadDistance = 1.1*mm;
  fXPadStartPos = -fMMegaPanelSizeX/2.; //to be changed when i have readout maps
  fYPadStartPos = -fMMegaPanelSizeX/2.;
  
  //size of meshes
  fMMegaAmpMeshSizeZ = (0.018/2)*mm;
  fMMegaCathodeMeshSizeZ = (0.030/4)*mm;
  fMMegaPadDistance = 1.1*mm;

  fMMegaFrontFacePosZ = 2612.41*mm - fMMegaSizeZ - 25*mm; // Relative to center of magnet (using runIII ECal position)
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
