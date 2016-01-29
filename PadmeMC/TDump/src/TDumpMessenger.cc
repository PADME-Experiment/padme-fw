// TDumpMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2016-01-29 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "TDumpMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "TDumpDetector.hh"
#include "TDumpGeometry.hh"

TDumpMessenger::TDumpMessenger(TDumpDetector* det)
:fTDumpDetector(det)
{

  fTDumpGeometry = TDumpGeometry::GetInstance();

  fTDumpDetectorDir = new G4UIdirectory("/Detector/TDump/");
  fTDumpDetectorDir->SetGuidance("UI commands to control TDump detector geometry");

  fSetTargetRadiusCmd = new G4UIcommand("/Detector/TDump/TargetRadius",this);
  fSetTargetRadiusCmd->SetGuidance("Set radius of TDump target in cm.");
  G4UIparameter* trRadiusParameter = new G4UIparameter("Radius",'d',false);
  trRadiusParameter->SetParameterRange("Radius > 0. && Radius <= 5.");
  fSetTargetRadiusCmd->SetParameter(trRadiusParameter);
  fSetTargetRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetLengthCmd = new G4UIcommand("/Detector/TDump/TargetLength",this);
  fSetTargetLengthCmd->SetGuidance("Set length of TDump target in cm.");
  G4UIparameter* tlLengthParameter = new G4UIparameter("Length",'d',false);
  tlLengthParameter->SetParameterRange("Length > 0. && Length <= 10.");
  fSetTargetLengthCmd->SetParameter(tlLengthParameter);
  fSetTargetLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetFrontFaceZCmd = new G4UIcommand("/Detector/TDump/TargetFrontFaceZ",this);
  fSetTargetFrontFaceZCmd->SetGuidance("Set position along Z of Target front face wrt TDump front face in cm.");
  G4UIparameter* tffPosZParameter = new G4UIparameter("PosZ",'d',false);
  tffPosZParameter->SetParameterRange("PosZ >= 0. && PosZ <= 70.");
  fSetTargetFrontFaceZCmd->SetParameter(tffPosZParameter);
  fSetTargetFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTDumpFrontFaceZCmd = new G4UIcommand("/Detector/TDump/FrontFaceZ",this);
  fSetTDumpFrontFaceZCmd->SetGuidance("Set position along Z of TDump front face in cm.");
  G4UIparameter* dffPosZParameter = new G4UIparameter("PosZ",'d',false);
  dffPosZParameter->SetParameterRange("PosZ > -1000. && PosZ <= -180.");
  fSetTDumpFrontFaceZCmd->SetParameter(dffPosZParameter);
  fSetTDumpFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

TDumpMessenger::~TDumpMessenger()
{

  delete fTDumpDetectorDir;

  delete fSetTargetRadiusCmd;
  delete fSetTargetLengthCmd;

  delete fSetTargetFrontFaceZCmd;

  delete fSetTDumpFrontFaceZCmd;

}

void TDumpMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetTargetRadiusCmd ) {
    G4double r; std::istringstream is(par); is >> r;
    fTDumpGeometry->SetTargetRadius(r*cm);
  }

  if ( cmd == fSetTargetLengthCmd ) {
    G4double l; std::istringstream is(par); is >> l;
    fTDumpGeometry->SetTargetLength(l*cm);
  }

  if ( cmd == fSetTargetFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fTDumpGeometry->SetTargetFrontFacePosZ(z*cm);
  }

  if ( cmd == fSetTDumpFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fTDumpGeometry->SetTDumpFrontFacePosZ(z*cm);
  }

}
