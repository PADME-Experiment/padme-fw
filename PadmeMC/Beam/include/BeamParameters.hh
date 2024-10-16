// BeamParameters.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-10
// --------------------------------------------------------------

#ifndef BeamParameters_H
#define BeamParameters_H 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

class BeamParameters
{

public:

  ~BeamParameters();
  static BeamParameters* GetInstance();

private:

  static BeamParameters* fInstance;

protected:

  BeamParameters();

public:

  void SetDetectorSetup(G4int);
  G4int GetDetectorSetup() { return fDetectorSetup; }

  void EnableBeamLine() { fBeamLineEnabled = true; }
  void DisableBeamLine() { fBeamLineEnabled = false; }
  G4bool BeamLineEnabled() { return fBeamLineEnabled; }

  G4int GetNPositronsPerBunch()        { return fNPositronsPerBunch; }
  void  SetNPositronsPerBunch(G4int n) { fNPositronsPerBunch = n;    }

  void NPositronsPerBunchEnableSpread()  { fNPositronsPerBunchApplySpread = true;  }
  void NPositronsPerBunchDisableSpread() { fNPositronsPerBunchApplySpread = false; }
  G4bool NPositronsPerBunchApplySpread() { return fNPositronsPerBunchApplySpread;  }

  void BeamEnableBunchStructure()  { fBeamApplyBunchStructure = true;  }
  void BeamDisableBunchStructure() { fBeamApplyBunchStructure = false; }
  G4bool BeamApplyBunchStructure() { return fBeamApplyBunchStructure;  }

  G4double GetBunchTimeLength() { return fBunchTimeLength; }
  void SetBunchTimeLength(G4double t) { fBunchTimeLength = t; }

  G4double GetMicroBunchTimeLength() { return fMicroBunchTimeLength; }
  void SetMicroBunchTimeLength(G4double t) { fMicroBunchTimeLength = t; }

  G4double GetMicroBunchTimeDelay() { return fMicroBunchTimeDelay; }
  void SetMicroBunchTimeDelay(G4double t) { fMicroBunchTimeDelay = t; }

  G4double GetBeamCenterPosX() { return fBeamCenterPosX; }
  void SetBeamCenterPosX(G4double x) { fBeamCenterPosX = x; }

  G4double GetBeamCenterPosY() { return fBeamCenterPosY; }
  void SetBeamCenterPosY(G4double y) { fBeamCenterPosY = y; }
  
  // M. Raggi 15/03/2019
  G4double GetBeamCenterPosZ() { return fBeamCenterPosZ; }
  void SetBeamCenterPosZ(G4double z) { fBeamCenterPosZ = z; }

  void BeamCenterPosEnableSpread()  { fBeamCenterPosApplySpread = true;  }
  void BeamCenterPosDisableSpread() { fBeamCenterPosApplySpread = false;  }
  G4bool BeamCenterPosApplySpread() { return fBeamCenterPosApplySpread;  }

  G4double GetBeamCenterPosXSpread() { return fBeamCenterPosXSpread; }
  void SetBeamCenterPosXSpread(G4double x) { fBeamCenterPosXSpread = x; }

  G4double GetBeamCenterPosYSpread() { return fBeamCenterPosYSpread; }
  void SetBeamCenterPosYSpread(G4double y) { fBeamCenterPosYSpread = y; }

  G4double GetBeamMomentum() { return fBeamMomentum; }
  void SetBeamMomentum(G4double p) { fBeamMomentum = p; }

  void BeamMomentumEnableSpread()  { fBeamMomentumApplySpread = true;  }
  void BeamMomentumDisableSpread() { fBeamMomentumApplySpread = false;  }
  G4bool BeamMomentumApplySpread() { return fBeamMomentumApplySpread;  }

  G4double GetBeamMomentumSpread() { return fBeamMomentumSpread; }
  void SetBeamMomentumSpread(G4double s) { fBeamMomentumSpread = s; }

  G4ThreeVector GetBeamDirection() { return fBeamDirection; }
  void SetBeamDirection(G4ThreeVector d) { fBeamDirection = d; }

  void BeamEnableEmittance()  { fBeamApplyEmittance = true;  }
  void BeamDisableEmittance() { fBeamApplyEmittance = false;  }
  G4bool BeamApplyEmittance() { return fBeamApplyEmittance; }

  G4double GetBeamEmittanceX() { return fBeamEmittanceX; }
  void SetBeamEmittanceX(G4double e) { fBeamEmittanceX = e; }

  G4double GetBeamEmittanceY() { return fBeamEmittanceY; }
  void SetBeamEmittanceY(G4double e) { fBeamEmittanceY = e; }

  void BeamEnableSpot()  { fBeamApplySpot = true;  }
  void BeamDisableSpot() { fBeamApplySpot = false;  }
  G4bool BeamApplySpot() { return fBeamApplySpot; }
  void SetBeamSpotX(G4double e) { fBeamSpotX = e; }
  G4double GetBeamSpotX() { return fBeamSpotX; }
  void SetBeamSpotY(G4double e) { fBeamSpotY = e; }
  G4double GetBeamSpotY() { return fBeamSpotY; }
  void SetBeamSpotZ(G4double e) { fBeamSpotZ = e; }
  G4double GetBeamSpotZ() { return fBeamSpotZ; }
  void SetBeamSpotSpreadX(G4double e) { fBeamSpotSpreadX = e; }
  G4double GetBeamSpotSpreadX() { return fBeamSpotSpreadX; }
  void SetBeamSpotSpreadY(G4double e) { fBeamSpotSpreadY = e; }
  G4double GetBeamSpotSpreadY() { return fBeamSpotSpreadY; }




  G4int GetNUbosonDecaysPerBunch()        { return fNUbosonDecaysPerBunch; }
  void  SetNUbosonDecaysPerBunch(G4int n) { fNUbosonDecaysPerBunch = n;    }

  G4double GetUbosonMass() { return fUbosonMass; }
  void SetUbosonMass(G4double m) { fUbosonMass = m; }

  G4int GetNThreePhotonDecaysPerBunch()        { return fNThreePhotonDecaysPerBunch; }
  void SetNThreePhotonDecaysPerBunch(G4int n) { fNThreePhotonDecaysPerBunch = n;    }

  G4String GetThreePhotonDecaysFilename()        { return fTwoPhotonDecaysFilename; }
  void SetThreePhotonDecaysFilename(G4String f) { fTwoPhotonDecaysFilename = f; }
  
  G4int GetNTwoPhotonDecaysPerBunch()        { return fNTwoPhotonDecaysPerBunch; }
  void SetNTwoPhotonDecaysPerBunch(G4int n) { fNTwoPhotonDecaysPerBunch = n;    }

  G4String GetTwoPhotonDecaysFilename()        { return fTwoPhotonDecaysFilename; }
  void SetTwoPhotonDecaysFilename(G4String f) { fTwoPhotonDecaysFilename = f; }

  // BhaBha data cards M. Raggi 20/05/2021
  G4int GetNBhaBhaPerBunch()        { return fNBhaBhaPerBunch; }
  void SetNBhaBhaPerBunch(G4int n) { fNBhaBhaPerBunch = n;    }

  G4String GetBhaBhaFilename()        { return fBhaBhaFilename; }
  void SetBhaBhaFilename(G4String f) { fBhaBhaFilename = f; }

  G4int GetBhaBhaLinesToSkip()       { return fBhaBhaLinesToSkip; }
  void SetBhaBhaLinesToSkip(G4int n) { fBhaBhaLinesToSkip = n;    }

  G4int GetNBabayagaPerBunch()        { return fNBabayagaPerBunch; }
  void SetNBabayagaPerBunch(G4int n) { fNBabayagaPerBunch = n;    }

  G4String GetBabayagaFilename()        { return fBabayagaFilename; }
  void SetBabayagaFilename(G4String f) { fBabayagaFilename = f; }

  G4int GetBabayagaLinesToSkip()       { return fBabayagaLinesToSkip; }
  void SetBabayagaLinesToSkip(G4int n) { fBabayagaLinesToSkip = n;    }

  G4double GetDecayLength() { return fDecayLength; }
  void SetDecayLength(G4double d) { fDecayLength = d; }

  // Calibration run parameters

  void CalibrationRunEnable()  { fCalibrationRun = true; }
  void CalibrationRunDisable() { fCalibrationRun = false; }
  G4bool CalibrationRun() { return fCalibrationRun; }

  G4String GetCalibRunDetector() { return fCalibRunDetector; }
  void SetCalibRunDetector(G4String d) { fCalibRunDetector = d; }

  G4String GetCalibRunParticle() { return fCalibRunParticle; }
  void SetCalibRunParticle(G4String p) { fCalibRunParticle = p; }

  G4double GetCalibRunEnergy() { return fCalibRunEnergy; }
  void SetCalibRunEnergy(G4double e) { fCalibRunEnergy  = e; }

  G4double GetCalibRunCenterX() { return fCalibRunCenterX; }
  void SetCalibRunCenterX(G4double x) { fCalibRunCenterX = x; }

  G4double GetCalibRunCenterY() { return fCalibRunCenterY; }
  void SetCalibRunCenterY(G4double y) { fCalibRunCenterY = y; }

  G4double GetCalibRunRadius()  { return fCalibRunRadius; }
  void SetCalibRunRadius(G4double r)  { fCalibRunRadius  = r; }

  G4double GetBeamTargetPosZ() { return fBeamTargetPosZ; }
  void SetBeamTargetPosZ(G4double v) { fBeamTargetPosZ = v; }

  G4double GetBeamTargetPosXSpread() { return fBeamTargetPosXSpread; }
  void SetBeamTargetPosXSpread(G4double v) { fBeamTargetPosXSpread = v; }

  G4double GetBeamTargetPosYSpread() { return fBeamTargetPosYSpread; }
  void SetBeamTargetPosYSpread(G4double v) { fBeamTargetPosYSpread = v; }

  G4double GetBeamTargetEmittanceX() { return fBeamTargetEmittanceX; }
  void SetBeamTargetEmittanceX(G4double v) { fBeamTargetEmittanceX = v; }

  G4double GetBeamTargetEmittanceY() { return fBeamTargetEmittanceY; }
  void SetBeamTargetEmittanceY(G4double v) { fBeamTargetEmittanceY = v; }

  G4double GetBeamTargetPathLength() { return fBeamTargetPathLength; }
  void SetBeamTargetPathLength(G4double v) { fBeamTargetPathLength = v; }

private:

  G4int fDetectorSetup;

  G4bool fBeamLineEnabled;

  // Average number of positrons in each bunch
  G4int  fNPositronsPerBunch;
  G4bool fNPositronsPerBunchApplySpread; // Poisson distribution

  // Bunch time structure
  G4bool   fBeamApplyBunchStructure;
  G4double fBunchTimeLength;
  G4double fMicroBunchTimeLength;
  G4double fMicroBunchTimeDelay;

  // Position and spread of beam at Target front face
  G4double fBeamCenterPosX;
  G4double fBeamCenterPosY;
  G4double fBeamCenterPosZ;  //M Raggi 15/03/2019

  G4bool   fBeamCenterPosApplySpread;
  G4double fBeamCenterPosXSpread;
  G4double fBeamCenterPosYSpread;

  // Beam energy
  G4double fBeamMomentum;
  G4bool   fBeamMomentumApplySpread;
  G4double fBeamMomentumSpread;

  // Beam direction
  G4ThreeVector fBeamDirection;

  // Beam emittance (spread of X,Y beam direction components)
  G4bool   fBeamApplyEmittance;
  G4double fBeamEmittanceX;
  G4double fBeamEmittanceY;

  // Beam spot (spread of X at a given ZForX and Y at a given ZForY)
  G4bool   fBeamApplySpot;
  G4double fBeamSpotX;
  G4double fBeamSpotY;
  G4double fBeamSpotZ;
  G4double fBeamSpotSpreadX;
  G4double fBeamSpotSpreadY;

  // Insert Uboson decays
  G4int fNUbosonDecaysPerBunch;
  G4double fUbosonMass;

  // Insert three gamma decays
  G4int fNThreePhotonDecaysPerBunch;
  G4String fThreePhotonDecaysFilename;
  
  // Insert two gamma decays
  G4int fNTwoPhotonDecaysPerBunch;
  G4String fTwoPhotonDecaysFilename;

  // Insert BhaBha M. Raggi 20/05/2021
  G4int fNBhaBhaPerBunch;
  G4String fBhaBhaFilename;
  G4int fBhaBhaLinesToSkip;

  G4int fNBabayagaPerBunch;
  G4String fBabayagaFilename;
  G4int fBabayagaLinesToSkip;

  // Displaced vertex decay length (only used in TwoGamma and ThreeGamma generation)
  G4double fDecayLength;

  // In calibration mode we shoot gamma of fixed energy from
  // the center of the Target to a circle on the ECal surface (flat distribution)
  G4bool   fCalibrationRun;
  G4String fCalibRunDetector; // Detector to be calibrated
  G4String fCalibRunParticle; // Particle to use for calibration
  G4double fCalibRunEnergy;  // Energy of the photon
  G4double fCalibRunCenterX; // X of center of cirlce at detector position
  G4double fCalibRunCenterY; // Y of center of circle at detector position
  G4double fCalibRunRadius;  // Radius of circle

  // Beam distribution at target
  G4double fBeamTargetPosZ; // Z position (1um in front of target)
  G4double fBeamTargetPosXSpread; // Sigma of X coordinate
  G4double fBeamTargetPosYSpread; // Sigma of Y coordinate
  G4double fBeamTargetEmittanceX; // Emittance X component
  G4double fBeamTargetEmittanceY; // Emittance Y component
  G4double fBeamTargetPathLength; // Length of path from beam origin to target

};
#endif
