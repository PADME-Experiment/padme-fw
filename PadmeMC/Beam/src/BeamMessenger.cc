// BeamMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2016-02-11 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "BeamMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "BeamGenerator.hh"
#include "BeamParameters.hh"

BeamMessenger::BeamMessenger(BeamGenerator* bgen)
:fBeamGenerator(bgen)
{

  fBeamParameters = BeamParameters::GetInstance();

  fBeamGeneratorDir = new G4UIdirectory("/Detector/Beam/");
  fBeamGeneratorDir->SetGuidance("UI commands to control Beam generation");

  /*
  fSetTargetRadiusCmd = new G4UIcommand("/Detector/Beam/TargetRadius",this);
  fSetTargetRadiusCmd->SetGuidance("Set radius of Beam target in cm.");
  G4UIparameter* trRadiusParameter = new G4UIparameter("Radius",'d',false);
  trRadiusParameter->SetParameterRange("Radius > 0. && Radius <= 5.");
  fSetTargetRadiusCmd->SetParameter(trRadiusParameter);
  fSetTargetRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetLengthCmd = new G4UIcommand("/Detector/Beam/TargetLength",this);
  fSetTargetLengthCmd->SetGuidance("Set length of Beam target in cm.");
  G4UIparameter* tlLengthParameter = new G4UIparameter("Length",'d',false);
  tlLengthParameter->SetParameterRange("Length > 0. && Length <= 10.");
  fSetTargetLengthCmd->SetParameter(tlLengthParameter);
  fSetTargetLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetFrontFaceZCmd = new G4UIcommand("/Detector/Beam/TargetFrontFaceZ",this);
  fSetTargetFrontFaceZCmd->SetGuidance("Set position along Z of Target front face wrt Beam front face in cm.");
  G4UIparameter* tffPosZParameter = new G4UIparameter("PosZ",'d',false);
  tffPosZParameter->SetParameterRange("PosZ >= 0. && PosZ <= 70.");
  fSetTargetFrontFaceZCmd->SetParameter(tffPosZParameter);
  fSetTargetFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetBeamFrontFaceZCmd = new G4UIcommand("/Detector/Beam/FrontFaceZ",this);
  fSetBeamFrontFaceZCmd->SetGuidance("Set position along Z of Beam front face in cm.");
  G4UIparameter* dffPosZParameter = new G4UIparameter("PosZ",'d',false);
  dffPosZParameter->SetParameterRange("PosZ > -1000. && PosZ <= -180.");
  fSetBeamFrontFaceZCmd->SetParameter(dffPosZParameter);
  fSetBeamFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  */
}

BeamMessenger::~BeamMessenger()
{

  delete fBeamGeneratorDir;

  /*
  delete fSetTargetRadiusCmd;
  delete fSetTargetLengthCmd;

  delete fSetTargetFrontFaceZCmd;

  delete fSetBeamFrontFaceZCmd;
  */

}

void BeamMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{
  /*
  if ( cmd == fSetTargetRadiusCmd ) {
    G4double r; std::istringstream is(par); is >> r;
    fBeamGeometry->SetTargetRadius(r*cm);
  }

  if ( cmd == fSetTargetLengthCmd ) {
    G4double l; std::istringstream is(par); is >> l;
    fBeamGeometry->SetTargetLength(l*cm);
  }

  if ( cmd == fSetTargetFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fBeamGeometry->SetTargetFrontFacePosZ(z*cm);
  }

  if ( cmd == fSetBeamFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fBeamGeometry->SetBeamFrontFacePosZ(z*cm);
  }
  */
}
