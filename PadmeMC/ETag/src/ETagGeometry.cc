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
  // ETag number of rows of bars
  fETagNBars=15;
  
  //  Bars Dimension
  fETagBarSizeX = 660*mm;
  fETagBarSizeY = 44*mm;
  fETagBarSizeZ = 5*mm;

  fETagShortBarSizeX = 265*mm;
  fETagShortBarSizeY = fETagBarSizeY;
  fETagShortBarSizeZ = fETagBarSizeZ;

  //inner hole size
  fETagHoleSizeX = 3*fETagBarSizeY;
  fETagHoleSizeY = 3*fETagBarSizeY;
  fETagHoleSizeZ = fETagBarSizeZ+0.2*mm;

  // size of the detector mother volume
  fETagSizeX = fETagBarSizeX            +5*mm;
  fETagSizeY = fETagBarSizeY*fETagNBars +5*mm;
  fETagSizeZ = fETagBarSizeZ            +5*mm;

  //  fETagFrontFacePosZ = 2380.*mm; // Relative to center of magnet
  fETagFrontFacePosZ = 2557.*mm; // M.R. 04/23 after Ecal new postion for Run III
  
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
