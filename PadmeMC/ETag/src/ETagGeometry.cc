// ETagGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// Modified to become ETag by M. Raggi 06/06/2022
// --------------------------------------------------------------

#include "ETagGeometry.hh"

ETagGeometry* ETagGeometry::fInstance = 0;

ETagGeometry* ETagGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new ETagGeometry(); }
  return fInstance;
}

ETagGeometry::ETagGeometry()
{

  fVerbose = 0; // Do not show debug output

  fDetectorSetup = 10; // Default to 2019 setup

  // ETag number of rows of bars
  fETagNBars=15;
  
  // Long bars dimensions
  fETagBarSizeX = 660.*mm;
  fETagBarSizeY = 44.*mm;
  fETagBarSizeZ = 5.*mm;

  // Short bars dimensions
  fETagShortBarSizeX = 265.*mm;
  fETagShortBarSizeY = fETagBarSizeY;
  fETagShortBarSizeZ = fETagBarSizeZ;

  // Dimensions and thickness of metal frame
  fETagFrameSizeX = 774.*mm;
  fETagFrameSizeY = 728.*mm;
  fETagFrameSizeZ = 240.*mm;
  fETagFrameThick = 3.*mm;

  // Dimensions of tedlar foil
  fETagTedlarSizeX = fETagFrameSizeX-2.*fETagFrameThick;
  fETagTedlarSizeY = fETagFrameSizeY-2.*fETagFrameThick;
  fETagTedlarSizeZ = 50.*um;

  // Thickness of paint around bars (same thickness as BGO crystals in ECal)
  fETagBarPaintThick = 125.*um;

  // Gap between ETag bars
  fETagBarGapY = 50.*um;

  // Distance between back of bars and back of ETag structure
  fETagBarBackDisp = 13.*mm;

  // ETag main box dimensions
  fETagSizeX = fETagFrameSizeX;
  fETagSizeY = fETagFrameSizeY;
  fETagSizeZ = fETagFrameSizeZ;

  //  fETagFrontFacePosZ = 2380.*mm; // Relative to center of magnet
  //fETagFrontFacePosZ = 2557.*mm; // M.R. 04/23 after Ecal new postion for Run III
  fETagFrontFacePosZ = 2612.41*mm-50.*um-fETagSizeZ; // Assumes ETag frame almost (50um) directly in contact with ECal front face

  // Etag main box position
  fETagPosX = 0.;
  fETagPosY = 0.;
  fETagPosZ = fETagFrontFacePosZ+fETagSizeZ*0.5;

  fETagDigitizerName = "ETagDigitizer";
  fETagSensitiveDetectorName = "ETagSD";

}

std::vector<G4String> ETagGeometry::GetHashTable()
{
  std::vector<G4String> hash;
  std::ostringstream buffer;
  return hash;
}

ETagGeometry::~ETagGeometry()
{}
