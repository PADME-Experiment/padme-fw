// TargetGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef TargetGeometry_H
#define TargetGeometry_H 1

#include <vector>

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalVolume;

class TargetGeometry
{

public:

  ~TargetGeometry();
  static TargetGeometry* GetInstance();
  std::vector<G4String> GetHashTable();

private:

  static TargetGeometry* fInstance;

protected:

  TargetGeometry();

public:

  // Position of center of Target box
  G4double GetTargetPosX() { return 0.; }
  G4double GetTargetPosY() { return 0.; }
  G4double GetTargetPosZ() { return fTargetFrontFacePosZ+fTargetSizeZ*0.5; }

  // Size of Target box
  G4double GetTargetSizeX() { return fTargetSizeX; }
  G4double GetTargetSizeY() { return fTargetSizeY; }
  G4double GetTargetSizeZ() { return fTargetSizeZ; }
  void     SetTargetSizeX(G4double s) { fTargetSizeX = s; }
  void     SetTargetSizeY(G4double s) { fTargetSizeY = s; }
  void     SetTargetSizeZ(G4double s) { fTargetSizeZ = s; }

  // Size of Target support
  G4double GetTSupportL1() { return fTSupportL1; }
  G4double GetTSupportL2() { return fTSupportL2; }
  G4double GetTSupportL3() { return fTSupportL3; }
  G4double GetTSupportL4() { return fTSupportL4; }
  G4double GetTSupportHoleL() { return fTSupportHoleL; }
  G4double GetTSupportHoleD() { return fTSupportHoleD; }
  G4double GetTSupportThick() { return fTSupportThick; }
  G4double GetTSupportPosX() { return -0.5*fTSupportL2+fTSupportHoleD; }
  // Support is before Target
  G4double GetTSupportPosZ() { return fTargetFrontFacePosZ-0.5*fTSupportThick; }
  // Support is after Target
  //G4double GetTSupportPosZ() { return fTargetFrontFacePosZ+fTargetSizeZ+0.5*fTSupportThick; }

  // Set position along Z of Target front face
  G4double GetTargetFrontFacePosZ() { return fTargetFrontFacePosZ; }
  void     SetTargetFrontFacePosZ(G4double z) { fTargetFrontFacePosZ = z; }
  
  // Set Target Pitch and Strip and Get Gap Size;
  G4double GetTargetPitch() { return fTargetPitch; }
  void     SetTargetPitch(G4double p) { fTargetPitch = p; }

  G4double GetTargetStripSize() { return fTargetStripSize; }
  void     SetTargetStripSize(G4double s) { fTargetStripSize = s; }

  G4double GetTargetGapSize() { return fTargetPitch-fTargetStripSize; }
  // no setter as is set by difference

  // Set number of Target Channels
  G4int    GetTargetDigiNChannels() { return fTargetDigiNChannels; }
  void     SetTargetDigiNChannels(G4int c) { fTargetDigiNChannels = c; }

  // Handle (de-)activation of fast digitization
  void EnableFastDigitization() { fTargetFastDigitization = true; }
  void DisableFastDigitization() { fTargetFastDigitization = false; }
  G4bool FastDigitizationIsEnabled() { return fTargetFastDigitization; }

  // Handle saving of digitized waveforms to persistent digis
  void EnableSaveWaveformToDigi() { fTargetSaveWaveformToDigi = true; }
  void DisableSaveWaveformToDigi() { fTargetSaveWaveformToDigi = false; }
  G4bool SaveWaveformToDigiIsEnabled() { return fTargetSaveWaveformToDigi; }
  
  // Reduce persistence of digitized waveforms to integration window
  void EnableReduceWaveform() { fTargetReduceWaveform = true; }
  void DisableReduceWaveform() { fTargetReduceWaveform = false; }
  G4bool ReduceWaveformIsEnabled() { return fTargetReduceWaveform; }

  // Get name of Target sensitive detector
  G4String GetTargetSensitiveDetectorName() { return fTargetSensitiveDetectorName; }
  
   // Set RMS Noise, Threshold, Baseline, Bins, offset and integration window
  G4double GetTargetDigiNoiseRMS() { return fTargetDigiNoiseRMS; }
  void     SetTargetDigiNoiseRMS(G4double n) { fTargetDigiNoiseRMS = n; }

  G4double GetTargetDigiThreshold() { return fTargetDigiThreshold; }
  void     SetTargetDigiThreshold(G4double t) { fTargetDigiThreshold = t; }

  G4double GetTargetDigiBaseline() { return fTargetDigiBaseline; }
  void     SetTargetDigiBaseline(G4double b) { fTargetDigiBaseline = b; }

  G4int    GetTargetDigiNBins() { return fTargetDigiNBins; }
  void     SetTargetDigiNBins(G4int nb) { fTargetDigiNBins = nb; }

  G4double GetTargetDigiTrigOffset() { return fTargetDigiTrigOffset; }
  void     SetTargetDigiTrigOffset(G4double o) { fTargetDigiTrigOffset = o; }

  G4double GetTargetDigiWindow() { return fTargetDigiWindow; }
  void     SetTargetDigiWindow(G4double w) { fTargetDigiWindow = w; }
  
    // Set parameters for digitization for dedicated studies
  G4int    GetTargetDigiNTrackDiv() { return fTargetDigiNTrackDiv; }
  void     SetTargetDigiNTrackDiv(G4int d) { fTargetDigiNTrackDiv = d; }

  G4double GetTargetDigiCCD() { return fTargetDigiCCD; }
  void     SetTargetDigiCCD(G4double c) { fTargetDigiCCD = c; }

  G4double GetTargetDigiNoiseChargeRMS() { return fTargetDigiNoiseChargeRMS; }
  void     SetTargetDigiNoiseChargeRMS(G4double nc) { fTargetDigiNoiseChargeRMS = nc; }

  G4double GetTargetDigiMeV2Q() { return fTargetDigiMeV2Q; }
  void     SetTargetDigiMeV2Q(G4double m) { fTargetDigiMeV2Q = m; }

private:

  G4double fTargetSizeX;
  G4double fTargetSizeY;
  G4double fTargetSizeZ;

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
  G4double fTSupportL1;
  G4double fTSupportL2;
  G4double fTSupportL3;
  G4double fTSupportL4;
  G4double fTSupportHoleL; // Length of hole side
  G4double fTSupportHoleD; // Distance from L1 to center of hole
  G4double fTSupportThick; // Thickness of T-shaped support

  G4double fTargetFrontFacePosZ; // Position along Z axis of Target front face

  G4bool fTargetFastDigitization; // Enable/disable use of fast digitization

  G4bool fTargetSaveWaveformToDigi; // Enable/disable saving of digitized waveforms to persistent digis
  
  G4double fTargetPitch;  // Target Channel Pitch (def 1 mm)
  G4double fTargetStripSize;  // Target Strip Size (def 850 um)
  G4double fTargetGapSize;  // Target Gap Size (Pitch-Strip) (by diff 150 um)


  // digitizer parameters

  G4int fTargetDigiNChannels;  // Number of Target Channels
  G4bool fTargetReduceWaveform; // Enable/disable reducing of digitized waveforms to integration window in persistent digis

  G4double fTargetDigiNoiseRMS;  // Noise RMS in micro Ampere from 1800-5000 e- 
  G4double fTargetDigiThreshold;  // Threshold for digitization of Target channels (uC)
  G4double fTargetDigiBaseline;  // Baseline for digitization of Target channels (uA)
  G4int fTargetDigiNBins;  // Number of bins for digitization of Target channels 
  G4double fTargetDigiTrigOffset;  // Offset for digitization (ns)
  G4double fTargetDigiWindow;  // Integration Window for digitization of Target channels (ns)

  // digitizer parameters to change for dedicated study

  G4int fTargetDigiNTrackDiv;  // number of steps for target signal derivation
  G4double fTargetDigiCCD;  // beam test based value
  G4double fTargetDigiNoiseChargeRMS; // Fluctuation of noise charge for FastDigi
  G4double fTargetDigiMeV2Q;  // MeV to charge conversion factor for calibration of FastDigi

  G4String fTargetSensitiveDetectorName;

};

#endif
