// LAVMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "LAVMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"

#include "LAVDetector.hh"
#include "LAVGeometry.hh"

LAVMessenger::LAVMessenger(LAVDetector* det)
:fLAVDetector(det)
{

  fLAVGeometry = LAVGeometry::GetInstance();

  fLAVDetectorDir = new G4UIdirectory("/Detector/LAV/");
  fLAVDetectorDir->SetGuidance("UI commands to control LAV detector geometry");

  fSetLAVInnerRadiusCmd = new G4UIcommand("/Detector/LAV/InnerRadius",this);
  fSetLAVInnerRadiusCmd->SetGuidance("Set LAV inner radius in cm.");
  G4UIparameter* lirParameter = new G4UIparameter("InnR",'d',false);
  lirParameter->SetParameterRange("InnR > 0. && InnR <= 20.");
  fSetLAVInnerRadiusCmd->SetParameter(lirParameter);
  fSetLAVInnerRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetLAVOuterRadiusCmd = new G4UIcommand("/Detector/LAV/OuterRadius",this);
  fSetLAVOuterRadiusCmd->SetGuidance("Set LAV inner radius in cm.");
  G4UIparameter* lorParameter = new G4UIparameter("OutR",'d',false);
  lorParameter->SetParameterRange("OutR >= 5. && OutR <= 50.");
  fSetLAVOuterRadiusCmd->SetParameter(lorParameter);
  fSetLAVOuterRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetLAVZLengthCmd = new G4UIcommand("/Detector/LAV/ZLength",this);
  fSetLAVZLengthCmd->SetGuidance("Set LAV length along Z in cm.");
  G4UIparameter* llParameter = new G4UIparameter("ZLen",'d',false);
  llParameter->SetParameterRange("ZLen > 0. && ZLen <= 50.");
  fSetLAVZLengthCmd->SetParameter(llParameter);
  fSetLAVZLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetLAVFrontFaceZCmd = new G4UIcommand("/Detector/LAV/FrontFaceZ",this);
  fSetLAVFrontFaceZCmd->SetGuidance("Set position along Z of LAV front face in cm.");
  G4UIparameter* effPosZParameter = new G4UIparameter("PosZ",'d',false);
  effPosZParameter->SetParameterRange("PosZ >= -100. && PosZ <= -50.");
  fSetLAVFrontFaceZCmd->SetParameter(effPosZParameter);
  fSetLAVFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

LAVMessenger::~LAVMessenger()
{

  delete fLAVDetectorDir;

  delete fSetLAVFrontFaceZCmd;

  delete fSetLAVInnerRadiusCmd;
  delete fSetLAVOuterRadiusCmd;
  delete fSetLAVZLengthCmd;

}

void LAVMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetLAVFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fLAVGeometry->SetLAVFrontFacePosZ(z*cm);
  }

  if ( cmd == fSetLAVInnerRadiusCmd ) {
    G4double r; std::istringstream is(par); is >> r;
    fLAVGeometry->SetLAVInnerRadius(r*cm);
  }

  if ( cmd == fSetLAVOuterRadiusCmd ) {
    G4double r; std::istringstream is(par); is >> r;
    fLAVGeometry->SetLAVOuterRadius(r*cm);
  }

  if ( cmd == fSetLAVZLengthCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fLAVGeometry->SetLAVZLength(z*cm);
  }

}
