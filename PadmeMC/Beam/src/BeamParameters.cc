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

  // Default is 2019 (setup 10) and no beamline
  fDetectorSetup = 10;
  fBeamLineEnabled = false;

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
  fBeamCenterPosZ = -1028.001*mm; // 1um before actual Target front face

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

  // Beam spot (spread of X,Y beam at a given Z)
  fBeamApplySpot = false;
  fBeamSpotX = 0.1*mm;
  fBeamSpotY = 0.1*mm;
  fBeamSpotZ = 2683*mm;

  // Insert Uboson decays
  fNUbosonDecaysPerBunch = 0;
  fUbosonMass = 22.*MeV;

  // Insert three gamma decays
  fNThreePhotonDecaysPerBunch = 0;
  fThreePhotonDecaysFilename = "ThreePhotonDecays.list";
  
  // Insert two gamma decays
  fNTwoPhotonDecaysPerBunch = 0;
  fTwoPhotonDecaysFilename = "TwoPhotonDecays.list";

  fNBhaBhaPerBunch = 0;
  fBhaBhaFilename = "BhaBha.list";
  fBhaBhaLinesToSkip = 0;

  fNBabayagaPerBunch = 0;
  fBabayagaFilename = "Babayaga.list"; 
  fBabayagaLinesToSkip = 0;

  // Decay length for displaced vertex (currently used only for TwoGamma events)
  fDecayLength = 0.*mm;

  // Initialize calibration run to some sensible values
  fCalibrationRun  = false;
  fCalibRunDetector = "ECal";
  fCalibRunParticle = "gamma";
  fCalibRunEnergy  = 50.*MeV;
  fCalibRunCenterX = 14.*cm;
  fCalibRunCenterY = 14.*cm;
  fCalibRunRadius  =  1.*cm;

  // Beam parameters at Target
  fBeamTargetPosZ = -1028.001*mm; // 1um before actual Target front face
  fBeamTargetPosXSpread = 0.8*mm; // Need distribution
  fBeamTargetPosYSpread = 1.3*mm; // Need distribution
  fBeamTargetEmittanceX = 1.*mrad; // Need distribution
  fBeamTargetEmittanceY = 1.*mrad; // Need distribution
  fBeamTargetPathLength = 0.*cm; // MUST be modified for BeamLine settings

}

BeamParameters::~BeamParameters()
{}

void BeamParameters::SetDetectorSetup(G4int setup)
{
  fDetectorSetup = setup;

  if (fDetectorSetup == 10) {

    // Year 2019
    printf("BeamParameters::SetDetectorSetup - Setting beam to default parameters for year 2019\n");

    if (fBeamLineEnabled) printf("BeamParameters::SetDetectorSetup - WARNING - No BeamLine defined for year 2019 (setup 10).");

    // Average number of positrons in each bunch
    fNPositronsPerBunch = 25000;

    // Bunch time structure
    fBunchTimeLength = 250.*ns;

    // Position and spread of beam at Target front face (use default Target position: can be changed)
    fBeamCenterPosX = 0.*cm;
    fBeamCenterPosY = 0.*cm;
    fBeamCenterPosZ = -1028.001*mm; // 1um before actual Target front face

    fBeamCenterPosXSpread = 1.*mm;
    fBeamCenterPosYSpread = 1.*mm;

    // Beam momentum
    fBeamMomentum = 545.*MeV;
    fBeamMomentumSpread = 0.01*fBeamMomentum; // 1% of beam momentum

    // Beam direction
    fBeamDirection = G4ThreeVector(0.,0.,1.);

    // Beam emittance (spread of X,Y beam direction components)
    fBeamEmittanceX = 1.*mrad;
    fBeamEmittanceY = 1.*mrad;

  } else if (fDetectorSetup == 20 || fDetectorSetup == 30) {

    // Year 2020/2021
    printf("BeamParameters::SetDetectorSetup - Setting beam to default parameters for year 2020/2021\n");

    fBeamMomentum = 490.*MeV;
    fBunchTimeLength = 250.*ns;
    if (fBeamLineEnabled) {
      printf("BeamParameters::SetDetectorSetup - BeamLine is ENABLED\n");
      fNPositronsPerBunch = 25000;
      fBeamCenterPosX = 0.*cm;
      fBeamCenterPosY = 0.*cm;
      fBeamCenterPosZ = -1028.001*mm; // 1um before actual Target front face
      fBeamCenterPosXSpread = 1.*mm;
      fBeamCenterPosYSpread = 1.*mm;
      fBeamMomentumSpread = 0.01*fBeamMomentum; // 1% of beam momentum
      fBeamDirection = G4ThreeVector(0.,0.,1.);
      fBeamEmittanceX = 1.*mrad;
      fBeamEmittanceY = 1.*mrad;
    } else {
      printf("BeamParameters::SetDetectorSetup - BeamLine is DISABLED\n");
      fNPositronsPerBunch = 25000;
      fBeamCenterPosX = 0.*cm;
      fBeamCenterPosY = 0.*cm;
      fBeamCenterPosZ = -1028.001*mm; // 1um before actual Target front face
      fBeamCenterPosXSpread = 1.*mm;
      fBeamCenterPosYSpread = 1.*mm;
      fBeamMomentumSpread = 0.01*fBeamMomentum; // 1% of beam momentum
      fBeamDirection = G4ThreeVector(0.,0.,1.);
      fBeamEmittanceX = 1.*mrad;
      fBeamEmittanceY = 1.*mrad;
    }

  } else if (fDetectorSetup == 40) {

    // Year 2022
    printf("BeamParameters::SetDetectorSetup - Setting beam to default parameters for year 2022\n");

    fBeamMomentum = 282.8*MeV;
    fBunchTimeLength = 250.*ns;
    if (fBeamLineEnabled) {
      printf("BeamParameters::SetDetectorSetup - BeamLine is ENABLED\n");
      fNPositronsPerBunch = 30000;
      fBeamCenterPosX =  7233.*mm; //
      fBeamCenterPosY =     0.*mm; // Starting in front of Mylar window
      fBeamCenterPosZ = -9581.*mm; //
      fBeamCenterPosXSpread = 1.*mm;
      fBeamCenterPosYSpread = 1.*mm;
      fBeamMomentumSpread = 0.7*MeV;
      fBeamDirection = G4ThreeVector(-1.,0.,1.);
      fBeamEmittanceX = 1.*mrad;
      fBeamEmittanceY = 1.*mrad;
    } else {
      printf("BeamParameters::SetDetectorSetup - BeamLine is DISABLED\n");
      fNPositronsPerBunch = 4000;
      fBeamCenterPosX =        0.*mm; //
      fBeamCenterPosY =        0.*mm; // Starting 1um before actual Target front face 
      fBeamCenterPosZ = -1028.001*mm; //
      fBeamCenterPosXSpread = 1.*mm;
      fBeamCenterPosYSpread = 1.*mm;
      fBeamMomentumSpread = 0.004*fBeamMomentum; // 0.4% of beam momentum
      fBeamDirection = G4ThreeVector(0.,0.,1.);
      fBeamEmittanceX = 1.*mrad;
      fBeamEmittanceY = 1.*mrad;
    }

  } else {
    printf("BeamParameters::SetDetectorSetup - ERROR - Setup %d is not available.\n",fDetectorSetup);
  }
}
