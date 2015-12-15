// TargetMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "TargetMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "TargetDetector.hh"
#include "TargetGeometry.hh"

TargetMessenger::TargetMessenger(TargetDetector* det)
:fTargetDetector(det)
{

  fTargetGeometry = TargetGeometry::GetInstance();

  fTargetDetectorDir = new G4UIdirectory("/Detector/Target/");
  fTargetDetectorDir->SetGuidance("UI commands to control Target detector geometry");

  fSetTargetSizeCmd = new G4UIcommand("/Detector/Target/Size",this);
  fSetTargetSizeCmd->SetGuidance("Set size (side of squared plaque) of Target detector in cm.");
  G4UIparameter* tsSizeParameter = new G4UIparameter("Size",'d',false);
  tsSizeParameter->SetParameterRange("Size >= 0.1 && Size <= 5.");
  fSetTargetSizeCmd->SetParameter(tsSizeParameter);
  fSetTargetSizeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetThicknessCmd = new G4UIcommand("/Detector/Target/Thickness",this);
  fSetTargetThicknessCmd->SetGuidance("Set thickness of Target detector in um.");
  G4UIparameter* ttThickParameter = new G4UIparameter("Thick",'d',false);
  ttThickParameter->SetParameterRange("Thick >= 10. && Thick <= 500.");
  fSetTargetThicknessCmd->SetParameter(ttThickParameter);
  fSetTargetThicknessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetFrontFaceZCmd = new G4UIcommand("/Detector/Target/FrontFaceZ",this);
  fSetTargetFrontFaceZCmd->SetGuidance("Set position along Z of Target front face in cm.");
  G4UIparameter* tffPosZParameter = new G4UIparameter("PosZ",'d',false);
  tffPosZParameter->SetParameterRange("PosZ >= -100. && PosZ <= 100.");
  fSetTargetFrontFaceZCmd->SetParameter(tffPosZParameter);
  fSetTargetFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

TargetMessenger::~TargetMessenger()
{

  delete fTargetDetectorDir;

  delete fSetTargetSizeCmd;
  delete fSetTargetThicknessCmd;
  delete fSetTargetFrontFaceZCmd;

}

void TargetMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetTargetSizeCmd ) {
    G4double s; std::istringstream is(par); is >> s;
    fTargetGeometry->SetTargetSizeX(s*cm);
    fTargetGeometry->SetTargetSizeY(s*cm);
  }

  if ( cmd == fSetTargetThicknessCmd ) {
    G4double s; std::istringstream is(par); is >> s;
    fTargetGeometry->SetTargetSizeZ(s*um);
  }

  if ( cmd == fSetTargetFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    printf("Setting TargetFrontFacePosZ to %f cm\n",z);
    fTargetGeometry->SetTargetFrontFacePosZ(z*cm);
  }

}
