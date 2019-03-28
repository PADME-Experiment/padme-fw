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

  // Handle (de-)activation of fast digitization
  void EnableFastDigitization() { fTargetFastDigitization = true; }
  void DisableFastDigitization() { fTargetFastDigitization = false; }
  G4bool FastDigitizationIsEnabled() { return fTargetFastDigitization; }

  // Handle saving of digitized waveforms to persistent digis
  void EnableSaveWaveformToDigi() { fTargetSaveWaveformToDigi = true; }
  void DisableSaveWaveformToDigi() { fTargetSaveWaveformToDigi = false; }
  G4bool SaveWaveformToDigiIsEnabled() { return fTargetSaveWaveformToDigi; }

  // Get name of Target sensitive detector
  G4String GetTargetSensitiveDetectorName() { return fTargetSensitiveDetectorName; }

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

  G4String fTargetSensitiveDetectorName;

};

#endif
