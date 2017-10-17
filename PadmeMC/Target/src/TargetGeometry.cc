
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

  //fTargetFrontFacePosZ = -70.*cm; // Relative to center of magnet
  fTargetFrontFacePosZ = -100.*cm; // Relative to center of magnet

  fTargetPitch = 1*mm;
  fTargetStripSize =850*um;
  fTargetGapSize =150*um;

  fTargetDigiNChannels =32;
  fTargetFastDigitization = true; // Use fast digitization
  fTargetSaveWaveformToDigi = false; // Do not save waveforms to digi

  fTargetDigiNoiseRMS = 0.87;   // uampere noise from 1800 e-
  //fTargetDigiNoiseRMS  = 2.42; // uampere noise from 5000 e-
  fTargetDigiThreshold = 4e-10; // uC low thres
  //fTargetDigiThreshold = 2e-9; // uC 1sigma thres with 1800 e- noise
  fTargetDigiBaseline = 0.; // uA
  //fTargetDigiBaseline = 10.; // uA 
  fTargetDigiWindow = 100.*ns; // signal integration window
  //fTargetDigiWindow = 40.*ns; // signal integration window

  // parameters for dedicated studies
  fTargetDigiNTrackDiv =100;
  fTargetDigiCCD=0.1;  // take out
  fTargetDigiNoiseChargeRMS=2.33e-9; // uampere charge noise from 1800 e-
  //fTargetDigiNoiseChargeRMS=6.47e-9; // uampere charge noise from 5000 e-
  fTargetDigiMeV2Q=1.901e-9;  // 
  // fTargetDigiMeV2Q=1.88e-8*50;  // WAS 

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

  buffer << "fTargetPitch " << fTargetPitch;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetStripSize " << fTargetStripSize;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetGapSize " << fTargetGapSize;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetDigiNChannels " << fTargetDigiNChannels;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetDigiNTrackDiv " << fTargetDigiNTrackDiv;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetDigiNoiseRMS " << fTargetDigiNoiseRMS;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetDigiThreshold " << fTargetDigiThreshold;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetDigiBaseline " << fTargetDigiBaseline;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetDigiWindow " << fTargetDigiWindow;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetDigiNoiseChargeRMS " << fTargetDigiNoiseChargeRMS;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetDigiCCD " << fTargetDigiCCD;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetDigiMeV2Q " << fTargetDigiMeV2Q;
  hash.push_back(buffer.str());
  buffer.str("");

  return hash;
}
