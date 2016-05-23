// EVetoMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "EVetoMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "EVetoDetector.hh"
#include "EVetoGeometry.hh"

EVetoMessenger::EVetoMessenger(EVetoDetector* det)
:fEVetoDetector(det)
{

  fEVetoGeometry = EVetoGeometry::GetInstance();

  fEVetoDetectorDir = new G4UIdirectory("/Detector/EVeto/");
  fEVetoDetectorDir->SetGuidance("UI commands to control EVeto detector geometry");

  fSetEVetoNFingersCmd = new G4UIcommand("/Detector/EVeto/NFingers",this);
  fSetEVetoNFingersCmd->SetGuidance("Set number of EVeto fingers.");
  G4UIparameter* nfNFinParameter = new G4UIparameter("NFin",'i',false);
  nfNFinParameter->SetParameterRange("NFin >= 1 && NFin <= 1000");
  fSetEVetoNFingersCmd->SetParameter(nfNFinParameter);
  fSetEVetoNFingersCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetFingerSizeCmd = new G4UIcommand("/Detector/EVeto/FingerSize",this);
  fSetFingerSizeCmd->SetGuidance("Set size (side of square) of finger front face in cm.");
  G4UIparameter* fsSizeParameter = new G4UIparameter("Size",'d',false);
  fsSizeParameter->SetParameterRange("Size > 0. && Size <= 10.");
  fSetFingerSizeCmd->SetParameter(fsSizeParameter);
  fSetFingerSizeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetFingerLengthCmd = new G4UIcommand("/Detector/EVeto/FingerLength",this);
  fSetFingerLengthCmd->SetGuidance("Set vertical length of finger in cm.");
  G4UIparameter* fsLengthParameter = new G4UIparameter("Length",'d',false);
  fsLengthParameter->SetParameterRange("Length > 0. && Length <= 23.");
  fSetFingerLengthCmd->SetParameter(fsLengthParameter);
  fSetFingerLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetEVetoInnerFaceXCmd = new G4UIcommand("/Detector/EVeto/InnerFaceX",this);
  fSetEVetoInnerFaceXCmd->SetGuidance("Set position along X of EVeto inner face in cm.");
  G4UIparameter* pifPosXParameter = new G4UIparameter("PosX",'d',false);
  pifPosXParameter->SetParameterRange("PosX > 0. && PosX <= 55.");
  fSetEVetoInnerFaceXCmd->SetParameter(pifPosXParameter);
  fSetEVetoInnerFaceXCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetEVetoFrontFaceZCmd = new G4UIcommand("/Detector/EVeto/FrontFaceZ",this);
  fSetEVetoFrontFaceZCmd->SetGuidance("Set position along Z of EVeto front face in cm.");
  G4UIparameter* pifPosZParameter = new G4UIparameter("PosZ",'d',false);
  pifPosZParameter->SetParameterRange("PosZ >= -50. && PosZ <= 50.");
  fSetEVetoFrontFaceZCmd->SetParameter(pifPosZParameter);
  fSetEVetoFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

EVetoMessenger::~EVetoMessenger()
{

  delete fEVetoDetectorDir;

  delete fSetEVetoNFingersCmd;
  delete fSetFingerSizeCmd;
  delete fSetFingerLengthCmd;

  delete fSetEVetoInnerFaceXCmd;
  delete fSetEVetoFrontFaceZCmd;

}

void EVetoMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetEVetoNFingersCmd ) {
    G4int n; std::istringstream is(par); is >> n;
    fEVetoGeometry->SetEVetoNFingers(n);
  }

  if ( cmd == fSetFingerSizeCmd ) {
    G4double s; std::istringstream is(par); is >> s;
    fEVetoGeometry->SetFingerNominalSizeY(s*cm);
    fEVetoGeometry->SetFingerNominalSizeZ(s*cm);
  }

  if ( cmd == fSetEVetoInnerFaceXCmd ) {
    G4double x; std::istringstream is(par); is >> x;
    fEVetoGeometry->SetEVetoInnerFacePosX(x*cm);
  }

  if ( cmd == fSetEVetoFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fEVetoGeometry->SetEVetoFrontFacePosZ(z*cm);
  }

}
