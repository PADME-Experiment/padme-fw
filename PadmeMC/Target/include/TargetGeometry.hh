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

  G4double fTargetFrontFacePosZ; // Position along Z axis of Target front face

  G4bool fTargetFastDigitization; // Enable/disable use of fast digitization

  G4bool fTargetSaveWaveformToDigi; // Enable/disable saving of digitized waveforms to persistent digis

  G4String fTargetSensitiveDetectorName;

};

#endif
