// BeamParameters.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-10
// --------------------------------------------------------------

#include "BeamParameters.hh"

BeamParameters* BeamParameters::fInstance = 0;

BeamParameters* BeamParameters::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new BeamParameters(); }
  return fInstance;
}

BeamParameters::BeamParameters()
{

  // Average number of positrons in each bunch
  fNPositronsPerBunch = 25000;
  fNPositronsPerBunchApplySpread = true;

  // Bunch time structure
  fBeamApplyBunchStructure = true;
  fBunchTimeLength = 250.*ns;
  fMicroBunchTimeLength = 0.150*ns;
  fMicroBunchTimeDelay = 0.350*ns;

  // Position and spread of beam at Target front face (use default Target position: can be changed)
  fBeamCenterPosX = 0.*cm;
  fBeamCenterPosY = 0.*cm;
  fBeamCenterPosZ = -103.*cm;

  fBeamCenterPosApplySpread = true;
  fBeamCenterPosXSpread = 1.*mm;
  fBeamCenterPosYSpread = 1.*mm;

  // Beam momentum
  fBeamMomentum = 545.*MeV;
  fBeamMomentumApplySpread = true;
  fBeamMomentumSpread = 0.01*fBeamMomentum; // 1% of beam momentum

  // Beam direction
  fBeamDirection = G4ThreeVector(0.,0.,1.);

  // Beam emittance (spread of X,Y beam direction components)
  fBeamApplyEmittance = true;
  fBeamEmittanceX = 1.*mrad;
  fBeamEmittanceY = 1.*mrad;

  // Insert Uboson decays
  fNUbosonDecaysPerBunch = 0;
  fUbosonMass = 22.*MeV;

  // Insert three gamma decays
  fNThreePhotonDecaysPerBunch = 0;
  fThreePhotonDecaysFilename = "ThreePhotonDecays.list";
  
  // Insert two gamma decays
  fNTwoPhotonDecaysPerBunch = 0;
  fTwoPhotonDecaysFilename = "TwoPhotonDecays.list";

  // Initialize calibration run to some sensible values
  fCalibrationRun  = false;
  fCalibRunEnergy  = 50.*MeV;
  fCalibRunCenterX = 14.*cm;
  fCalibRunCenterY = 14.*cm;
  fCalibRunRadius  =  1.*cm;

  //Initialize illumination run parameters
  fIlluminationRun = false;
  fIllumRandomEnergy = false;
  fIllumRunEnergy = 50.*MeV;
  fIllumRunEnergyModel = 1;
  
  fIllumRunMinEnergy = 50.*MeV;
  fIllumRunMaxEnergy = 300.*MeV;
  fIllumRunMinRadius = 14.*cm;
  fIllumRunMinRadius = 25.*cm;
  fIllumRunRadiusModel = 1;
  fNIllumPartPerBunch = 1;

}

BeamParameters::~BeamParameters()
{}
