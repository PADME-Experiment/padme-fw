// MMegaMessenger.cc
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
//
// --------------------------------------------------------------

#include "MMegaMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"

#include "MMegaDetector.hh"
#include "MMegaGeometry.hh"

MMegaMessenger::MMegaMessenger(MMegaDetector* det)
:fMMegaDetector(det)
{

  fMMegaGeometry = MMegaGeometry::GetInstance();

  fMMegaDetectorDir = new G4UIdirectory("/Detector/MMega/");
  fMMegaDetectorDir->SetGuidance("UI commands to control MMega detector geometry");

//  fSetMMegaInnerRadiusCmd = new G4UIcommand("/Detector/MMega/InnerRadius",this);
//  fSetMMegaInnerRadiusCmd->SetGuidance("Set MMega inner radius in cm.");
//  G4UIparameter* lirParameter = new G4UIparameter("InnR",'d',false);
//  lirParameter->SetParameterRange("InnR > 0. && InnR <= 20.");
//  fSetMMegaInnerRadiusCmd->SetParameter(lirParameter);
//  fSetMMegaInnerRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
//
//  fSetMMegaOuterRadiusCmd = new G4UIcommand("/Detector/MMega/OuterRadius",this);
//  fSetMMegaOuterRadiusCmd->SetGuidance("Set MMega inner radius in cm.");
//  G4UIparameter* lorParameter = new G4UIparameter("OutR",'d',false);
//  lorParameter->SetParameterRange("OutR >= 5. && OutR <= 50.");
//  fSetMMegaOuterRadiusCmd->SetParameter(lorParameter);
//  fSetMMegaOuterRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetMMegaZLengthCmd = new G4UIcommand("/Detector/MMega/ZLength",this);
  fSetMMegaZLengthCmd->SetGuidance("Set MMega length along Z in cm.");
  G4UIparameter* llParameter = new G4UIparameter("ZLen",'d',false);
  llParameter->SetParameterRange("ZLen > 0. && ZLen <= 50.");
  fSetMMegaZLengthCmd->SetParameter(llParameter);
  fSetMMegaZLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetMMegaFrontFaceZCmd = new G4UIcommand("/Detector/MMega/FrontFaceZ",this);
  fSetMMegaFrontFaceZCmd->SetGuidance("Set position along Z of MMega front face in cm.");
  G4UIparameter* effPosZParameter = new G4UIparameter("PosZ",'d',false);
  effPosZParameter->SetParameterRange("PosZ >= -100. && PosZ <= -50.");
  fSetMMegaFrontFaceZCmd->SetParameter(effPosZParameter);
  fSetMMegaFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

MMegaMessenger::~MMegaMessenger()
{

  delete fMMegaDetectorDir;

  delete fSetMMegaFrontFaceZCmd;
  // D. Not defined for MMega so i comment them
  //delete fSetMMegaInnerRadiusCmd;
  //delete fSetMMegaOuterRadiusCmd;
  delete fSetMMegaZLengthCmd;

}

void MMegaMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetMMegaFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fMMegaGeometry->SetMMegaFrontFacePosZ(z*cm);
  }

//  if ( cmd == fSetMMegaInnerRadiusCmd ) {
//    G4double r; std::istringstream is(par); is >> r;
//    fMMegaGeometry->SetMMegaInnerRadius(r*cm);
//  }
//
//  if ( cmd == fSetMMegaOuterRadiusCmd ) {
//    G4double r; std::istringstream is(par); is >> r;
//    fMMegaGeometry->SetMMegaOuterRadius(r*cm);
//  }

//  if ( cmd == fSetMMegaZLengthCmd ) {
//    G4double z; std::istringstream is(par); is >> z;
//    fMMegaGeometry->SetMMegaZLength(z*cm);
//  }

}
