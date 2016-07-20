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

  G4String GetThreePhotonDecaysFilename()        { return fThreePhotonDecaysFilename; }
  void SetThreePhotonDecaysFilename(G4String f) { fThreePhotonDecaysFilename = f; }

  G4double GetBeamOriginPosZ()        { return fBeamOriginPosZ; }
  void     SetBeamOriginPosZ(G4double z) { fBeamOriginPosZ = z; }

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

  // Origin of beam along Z (i.e. Z of beam when t=0)
  G4double fBeamOriginPosZ;

};
#endif
