
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

  fTargetFrontFacePosZ = -100.*cm; // Relative to center of magnet

  fTargetPitch = 1*mm;
  fTargetStripSize =850*um;
  fTargetGapSize =150*um;

  fTargetDigiNChannels =32;
  fTargetFastDigitization = true; // Use fast digitization
  fTargetSaveWaveformToDigi = false; // Do not save waveforms to digi
  fTargetReduceWaveform = false; // Do not reduce waveforms to integration window in digi

  // noise derived for 0.1 ns resolution
  fTargetDigiNoiseRMS = 0.87;   // uampere noise from 1800 e-
  //fTargetDigiNoiseRMS  = 2.42; // uampere noise from 5000 e-

  fTargetDigiThreshold = 4e-10; // uC low thres
  //fTargetDigiThreshold = 2e-9; // uC 1sigma thres with 1800 e- noise

  fTargetDigiBaseline = 0.; // uA
  //fTargetDigiBaseline = 10.; // uA 

  // 1.024 us window res 1ns 
  fTargetDigiNBins = 1024; //    

  fTargetDigiTrigOffset = 0.; // ns trigger offset    

  fTargetDigiWindow = 100.; // ns signal integration window
  //fTargetDigiWindow = 40.; // ns signal integration window

  // parameters for dedicated studies

  fTargetDigiNTrackDiv =100;
  fTargetDigiCCD=0.1;  // fix-remove

  // noise derived for 0.1 ns resolution
  fTargetDigiNoiseChargeRMS=2.33e-9; // uC charge noise from 1800 e-
  //fTargetDigiNoiseChargeRMS=6.47e-9; // uC charge noise from 5000 e-

  //  fTargetDigiMeV2Q=1.901e-9;  // RC Filter
  //fTargetDigiMeV2Q=5.01e-10;  // CSA Filter gain 1
  fTargetDigiMeV2Q=2.004e-9;  // CSA Filter gain 4

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

  buffer << "fTargetReduceWaveform " << fTargetReduceWaveform;
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

  buffer << "fTargetDigiNBins " << fTargetDigiNBins;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetDigiTrigOffset " << fTargetDigiTrigOffset;
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
