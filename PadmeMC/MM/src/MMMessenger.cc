// MMMessenger.cc
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
//
// --------------------------------------------------------------

#include "MMMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"

#include "MMDetector.hh"
#include "MMGeometry.hh"

MMMessenger::MMMessenger(MMDetector* det)
:fMMDetector(det)
{

  fMMGeometry = MMGeometry::GetInstance();

  fMMDetectorDir = new G4UIdirectory("/Detector/MM/");
  fMMDetectorDir->SetGuidance("UI commands to control MM detector geometry");

//  fSetMMInnerRadiusCmd = new G4UIcommand("/Detector/MM/InnerRadius",this);
//  fSetMMInnerRadiusCmd->SetGuidance("Set MM inner radius in cm.");
//  G4UIparameter* lirParameter = new G4UIparameter("InnR",'d',false);
//  lirParameter->SetParameterRange("InnR > 0. && InnR <= 20.");
//  fSetMMInnerRadiusCmd->SetParameter(lirParameter);
//  fSetMMInnerRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
//
//  fSetMMOuterRadiusCmd = new G4UIcommand("/Detector/MM/OuterRadius",this);
//  fSetMMOuterRadiusCmd->SetGuidance("Set MM inner radius in cm.");
//  G4UIparameter* lorParameter = new G4UIparameter("OutR",'d',false);
//  lorParameter->SetParameterRange("OutR >= 5. && OutR <= 50.");
//  fSetMMOuterRadiusCmd->SetParameter(lorParameter);
//  fSetMMOuterRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetMMZLengthCmd = new G4UIcommand("/Detector/MM/ZLength",this);
  fSetMMZLengthCmd->SetGuidance("Set MM length along Z in cm.");
  G4UIparameter* llParameter = new G4UIparameter("ZLen",'d',false);
  llParameter->SetParameterRange("ZLen > 0. && ZLen <= 50.");
  fSetMMZLengthCmd->SetParameter(llParameter);
  fSetMMZLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetMMFrontFaceZCmd = new G4UIcommand("/Detector/MM/FrontFaceZ",this);
  fSetMMFrontFaceZCmd->SetGuidance("Set position along Z of MM front face in cm.");
  G4UIparameter* effPosZParameter = new G4UIparameter("PosZ",'d',false);
  effPosZParameter->SetParameterRange("PosZ >= -100. && PosZ <= -50.");
  fSetMMFrontFaceZCmd->SetParameter(effPosZParameter);
  fSetMMFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

MMMessenger::~MMMessenger()
{

  delete fMMDetectorDir;

  delete fSetMMFrontFaceZCmd;
  // D. Not defined for MM so i comment them
  //delete fSetMMInnerRadiusCmd;
  //delete fSetMMOuterRadiusCmd;
  delete fSetMMZLengthCmd;

}

void MMMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetMMFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fMMGeometry->SetMMFrontFacePosZ(z*cm);
  }

//  if ( cmd == fSetMMInnerRadiusCmd ) {
//    G4double r; std::istringstream is(par); is >> r;
//    fMMGeometry->SetMMInnerRadius(r*cm);
//  }
//
//  if ( cmd == fSetMMOuterRadiusCmd ) {
//    G4double r; std::istringstream is(par); is >> r;
//    fMMGeometry->SetMMOuterRadius(r*cm);
//  }

//  if ( cmd == fSetMMZLengthCmd ) {
//    G4double z; std::istringstream is(par); is >> z;
//    fMMGeometry->SetMMZLength(z*cm);
//  }

}
