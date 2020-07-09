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

  void CalibrationRunEnable()  { fCalibrationRun = true; }
  void CalibrationRunDisable() { fCalibrationRun = false; }
  G4bool CalibrationRun() { return fCalibrationRun; }

  G4double GetCalibRunEnergy() { return fCalibRunEnergy; }
  void SetCalibRunEnergy(G4double e) { fCalibRunEnergy  = e; }

  G4double GetCalibRunCenterX() { return fCalibRunCenterX; }
  void SetCalibRunCenterX(G4double x) { fCalibRunCenterX = x; }

  G4double GetCalibRunCenterY() { return fCalibRunCenterY; }
  void SetCalibRunCenterY(G4double y) { fCalibRunCenterY = y; }

  G4double GetCalibRunRadius()  { return fCalibRunRadius; }
  void SetCalibRunRadius(G4double r)  { fCalibRunRadius  = r; }

  void IlluminationRunEnable() {fIlluminationRun = true;}
  void IlluminationRunDisable() {fIlluminationRun = false;}
  G4bool  IlluminationRun(){return fIlluminationRun;}

  void IlluminationRandomEnergyEnable(){fIllumRandomEnergy = true;}
  void IlluminationRandomEnergyDisable(){fIllumRandomEnergy = false;}
  G4bool IlluminationRandomEnergy() {return fIllumRandomEnergy;}

  G4double GetIllumRunEnergy(){return fIllumRunEnergy;}
  void SetIllumRunEnergy(G4double x){fIllumRunEnergy = x;}

  G4double GetIllumRunMinEnergy(){return fIllumRunMinEnergy;}
  void SetIllumRunMinEnergy(G4double x){fIllumRunMinEnergy = x;}

  G4double GetIllumRunMaxEnergy(){return fIllumRunMaxEnergy;}
  void SetIllumRunMaxEnergy(G4double x){fIllumRunMaxEnergy = x;}

  G4double GetIllumRunMinRadius(){return fIllumRunMinRadius;}
  void SetIllumRunMinRadius(G4double x){fIllumRunMinRadius = x;}

  G4double GetIllumRunMaxRadius(){return fIllumRunMaxRadius;}
  void SetIllumRunMaxRadius(G4double x){fIllumRunMaxRadius = x;}

  G4int GetNIllumPartPerBunch()        { return fNIllumPartPerBunch; }
  void SetNIllumPartPerBunch(G4int n) { fNIllumPartPerBunch  = n;    }

  
  G4int GetIllumRunEnergyModel(){return fIllumRunEnergyModel;}
  void SetIllumRunEnergyModel(G4int x){fIllumRunEnergyModel = x;}

  G4int GetIllumRunRadiusModel(){return fIllumRunRadiusModel;}
  void SetIllumRunRadiusModel(G4int x){fIllumRunRadiusModel = x;}

  

private:

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

  // Insert Uboson decays
  G4int fNUbosonDecaysPerBunch;
  G4double fUbosonMass;

  // Insert three gamma decays
  G4int fNThreePhotonDecaysPerBunch;
  G4String fThreePhotonDecaysFilename;
  
  // Insert two gamma decays
  G4int fNTwoPhotonDecaysPerBunch;
  G4String fTwoPhotonDecaysFilename;

  // In calibration mode we shoot gamma of fixed energy from
  // the center of the Target to a circle on the ECal surface (flat distribution)
  G4bool   fCalibrationRun;
  G4double fCalibRunEnergy;  // Energy of the photon
  G4double fCalibRunCenterX; // X of center of cirlce 
  G4double fCalibRunCenterY; // Y of center of circle
  G4double fCalibRunRadius;  // Radius of circle

  //Illumination run
  //Fixed number of particles with random/fixed energies at random position in the ECal
  G4bool fIlluminationRun;
  G4bool fIllumRandomEnergy; //Random or fixed energy
  //If fixed
  G4double fIllumRunEnergy;
  //If random
  G4double fIllumRunMinEnergy;
  G4double fIllumRunMaxEnergy;

  G4double fIllumRunMinRadius; //Min Radius at ECal
  G4double fIllumRunMaxRadius; //Max Radius at ECal
  G4int  fNIllumPartPerBunch; //Number of particles per bunch
  G4int fIllumRunEnergyModel; //Distribution model for the energy
  G4int fIllumRunRadiusModel; //Distribution model for the radius

};
#endif
