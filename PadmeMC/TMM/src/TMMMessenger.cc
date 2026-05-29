// TMMMessenger.cc
// --------------------------------------------------------------
// History:
//
// Created by  Elisa Di Meco (elisa.dimeco@lnf.infn.it) 2026-05-27
//
// --------------------------------------------------------------

#include "TMMMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"

#include "TMMDetector.hh"
#include "TMMGeometry.hh"

TMMMessenger::TMMMessenger(TMMDetector* det)
:fTMMDetector(det)
{

  fTMMGeometry = TMMGeometry::GetInstance();

  fTMMDetectorDir = new G4UIdirectory("/Detector/TMM/");
  fTMMDetectorDir->SetGuidance("UI commands to control TMM detector geometry");

//  fSetTMMInnerRadiusCmd = new G4UIcommand("/Detector/TMM/InnerRadius",this);
//  fSetTMMInnerRadiusCmd->SetGuidance("Set TMM inner radius in cm.");
//  G4UIparameter* lirParameter = new G4UIparameter("InnR",'d',false);
//  lirParameter->SetParameterRange("InnR > 0. && InnR <= 20.");
//  fSetTMMInnerRadiusCmd->SetParameter(lirParameter);
//  fSetTMMInnerRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
//
//  fSetTMMOuterRadiusCmd = new G4UIcommand("/Detector/TMM/OuterRadius",this);
//  fSetTMMOuterRadiusCmd->SetGuidance("Set TMM inner radius in cm.");
//  G4UIparameter* lorParameter = new G4UIparameter("OutR",'d',false);
//  lorParameter->SetParameterRange("OutR >= 5. && OutR <= 50.");
//  fSetTMMOuterRadiusCmd->SetParameter(lorParameter);
//  fSetTMMOuterRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTMMZLengthCmd = new G4UIcommand("/Detector/TMM/ZLength",this);
  fSetTMMZLengthCmd->SetGuidance("Set TMM length along Z in cm.");
  G4UIparameter* llParameter = new G4UIparameter("ZLen",'d',false);
  llParameter->SetParameterRange("ZLen > 0. && ZLen <= 50.");
  fSetTMMZLengthCmd->SetParameter(llParameter);
  fSetTMMZLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTMMFrontFaceZCmd = new G4UIcommand("/Detector/TMM/FrontFaceZ",this);
  fSetTMMFrontFaceZCmd->SetGuidance("Set position along Z of TMM front face in cm.");
  G4UIparameter* effPosZParameter = new G4UIparameter("PosZ",'d',false);
  effPosZParameter->SetParameterRange("PosZ >= -100. && PosZ <= -50.");
  fSetTMMFrontFaceZCmd->SetParameter(effPosZParameter);
  fSetTMMFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

TMMMessenger::~TMMMessenger()
{

  delete fTMMDetectorDir;

  delete fSetTMMFrontFaceZCmd;
  // D. Not defined for TMM so i comment them
  //delete fSetTMMInnerRadiusCmd;
  //delete fSetTMMOuterRadiusCmd;
  delete fSetTMMZLengthCmd;

}

void TMMMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetTMMFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fTMMGeometry->SetTMMFrontFacePosZ(z*cm);
  }

//  if ( cmd == fSetTMMInnerRadiusCmd ) {
//    G4double r; std::istringstream is(par); is >> r;
//    fTMMGeometry->SetTMMInnerRadius(r*cm);
//  }
//
//  if ( cmd == fSetTMMOuterRadiusCmd ) {
//    G4double r; std::istringstream is(par); is >> r;
//    fTMMGeometry->SetTMMOuterRadius(r*cm);
//  }

//  if ( cmd == fSetTMMZLengthCmd ) {
//    G4double z; std::istringstream is(par); is >> z;
//    fTMMGeometry->SetTMMZLength(z*cm);
//  }

}
