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

  //   Support is T-shaped with a squared hole
  // Y  +------+
  // ^  |      |   L2
  // |  |      +-------+
  // |  |         +-+  |
  // |  |L4     HL| |  |L1
  // |  |         +-+  |
  // |  |      +-------+
  // |  |  L3  |     HD
  // |  +------+   |---|
  // +-------------------> X
  fTSupportL1 = 4.0*cm;
  fTSupportL2 = 5.0*cm;
  fTSupportL3 = 4.5*cm;
  fTSupportL4 = 8.0*cm;
  fTSupportHoleL = 1.5*cm; // Length of squared hole side
  fTSupportHoleD = 2.0*cm; // Distance from L1 to center of squared hole
  fTSupportThick = 1.0*mm; // Thickness of T-shaped support (to be verified)

  //fTargetFrontFacePosZ = -70.*cm; // Relative to center of magnet
  //  fTargetFrontFacePosZ = -100.*cm; // Relative to center of magnet
  fTargetFrontFacePosZ = -103.*cm; // Relative to center of magnet after construction M. Raggi 10/10/2018

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
