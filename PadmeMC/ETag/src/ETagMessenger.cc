// ETagMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "ETagMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"

#include "ETagDetector.hh"
#include "ETagGeometry.hh"

ETagMessenger::ETagMessenger(ETagDetector* det)
:fETagDetector(det)
{

  fETagGeometry = ETagGeometry::GetInstance();

  fETagDetectorDir = new G4UIdirectory("/Detector/ETag/");
  fETagDetectorDir->SetGuidance("UI commands to control ETag detector geometry");

//  fSetETagInnerRadiusCmd = new G4UIcommand("/Detector/ETag/InnerRadius",this);
//  fSetETagInnerRadiusCmd->SetGuidance("Set ETag inner radius in cm.");
//  G4UIparameter* lirParameter = new G4UIparameter("InnR",'d',false);
//  lirParameter->SetParameterRange("InnR > 0. && InnR <= 20.");
//  fSetETagInnerRadiusCmd->SetParameter(lirParameter);
//  fSetETagInnerRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
//
//  fSetETagOuterRadiusCmd = new G4UIcommand("/Detector/ETag/OuterRadius",this);
//  fSetETagOuterRadiusCmd->SetGuidance("Set ETag inner radius in cm.");
//  G4UIparameter* lorParameter = new G4UIparameter("OutR",'d',false);
//  lorParameter->SetParameterRange("OutR >= 5. && OutR <= 50.");
//  fSetETagOuterRadiusCmd->SetParameter(lorParameter);
//  fSetETagOuterRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetETagZLengthCmd = new G4UIcommand("/Detector/ETag/ZLength",this);
  fSetETagZLengthCmd->SetGuidance("Set ETag length along Z in cm.");
  G4UIparameter* llParameter = new G4UIparameter("ZLen",'d',false);
  llParameter->SetParameterRange("ZLen > 0. && ZLen <= 50.");
  fSetETagZLengthCmd->SetParameter(llParameter);
  fSetETagZLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetETagFrontFaceZCmd = new G4UIcommand("/Detector/ETag/FrontFaceZ",this);
  fSetETagFrontFaceZCmd->SetGuidance("Set position along Z of ETag front face in cm.");
  G4UIparameter* effPosZParameter = new G4UIparameter("PosZ",'d',false);
  effPosZParameter->SetParameterRange("PosZ >= -100. && PosZ <= -50.");
  fSetETagFrontFaceZCmd->SetParameter(effPosZParameter);
  fSetETagFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

ETagMessenger::~ETagMessenger()
{

  delete fETagDetectorDir;

  delete fSetETagFrontFaceZCmd;

  delete fSetETagInnerRadiusCmd;
  delete fSetETagOuterRadiusCmd;
  delete fSetETagZLengthCmd;

}

void ETagMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetETagFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fETagGeometry->SetETagFrontFacePosZ(z*cm);
  }

//  if ( cmd == fSetETagInnerRadiusCmd ) {
//    G4double r; std::istringstream is(par); is >> r;
//    fETagGeometry->SetETagInnerRadius(r*cm);
//  }
//
//  if ( cmd == fSetETagOuterRadiusCmd ) {
//    G4double r; std::istringstream is(par); is >> r;
//    fETagGeometry->SetETagOuterRadius(r*cm);
//  }

//  if ( cmd == fSetETagZLengthCmd ) {
//    G4double z; std::istringstream is(par); is >> z;
//    fETagGeometry->SetETagZLength(z*cm);
//  }

}
