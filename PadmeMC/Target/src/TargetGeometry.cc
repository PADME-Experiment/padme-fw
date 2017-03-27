// TargetGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "TargetGeometry.hh"

TargetGeometry* TargetGeometry::fInstance = 0;

TargetGeometry* TargetGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new TargetGeometry(); }
  return fInstance;
}

TargetGeometry::TargetGeometry()
{

  // Inizialize default parameters

  fTargetSizeX =   2.*cm;
  fTargetSizeY =   2.*cm;
  fTargetSizeZ = 100.*um;

  fTargetFrontFacePosZ = -70.*cm; // Relative to center of magnet

  fTargetFastDigitization = true; // Use fast digitization

  fTargetSaveWaveformToDigi = false; // Do not save waveforms to digi

  fTargetSensitiveDetectorName = "TargetSD";

}

TargetGeometry::~TargetGeometry()
{}

std::vector<G4String> TargetGeometry::GetHashTable()
{

  std::vector<G4String> hash;
  std::ostringstream buffer;

  buffer << "fTargetSizeX " << fTargetSizeX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetSizeY " << fTargetSizeY;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetSizeZ " << fTargetSizeZ;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetFrontFacePosZ " << fTargetFrontFacePosZ;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetFastDigitization " << fTargetFastDigitization;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetSaveWaveformToDigi " << fTargetSaveWaveformToDigi;
  hash.push_back(buffer.str());
  buffer.str("");

  return hash;
}
