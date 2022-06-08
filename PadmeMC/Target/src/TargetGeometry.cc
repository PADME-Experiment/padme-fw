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

  fVerbose = 0; // Do not show debug output

  fDetectorSetup = 1; // Default to 2019 setup

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
  fTSupportThick = 1.6*mm; // Thickness of T-shaped support

  // Assume target at exact center. Cross position from Jan 2020 survey: (X:-1.3mm, Y:0.41mm), 1mm approximation
  // Can be modified with datacards
  fTargetDisplacementX = 0.*mm;
  fTargetDisplacementY = 0.*mm;

  //fTargetFrontFacePosZ = -70.*cm; // Relative to center of magnet
  //fTargetFrontFacePosZ = -100.*cm; // Relative to center of magnet
  //fTargetFrontFacePosZ = -103.*cm; // Relative to center of magnet after construction M. Raggi 10/10/2018
  fTargetFrontFacePosZ = -1028.*mm; // Relative to center of magnet. Jan 2020 survey (-1027.68mm, 1mm approximation)

  fTargetFastDigitization = true; // Use fast digitization

  fTargetSaveWaveformToDigi = false; // Do not save waveforms to digi

  fTargetDigitizerName = "TargetDigitizer";
  fTargetSensitiveDetectorName = "TargetSD";
  
  fTargetPitch = 1*mm;
  fTargetStripSize =850*um;
  fTargetGapSize =150*um;

  fTargetDigiNChannels =32;
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

  buffer << "fTargetDisplacementX " << fTargetDisplacementX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTargetDisplacementY " << fTargetDisplacementY;
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
