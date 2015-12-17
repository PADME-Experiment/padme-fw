// EVetoMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "EVetoMessenger.hh"

#include "EVetoDetector.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

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

  /*
  fSetEVetoFrontFaceZCmd = new G4UIcommand("/Detector/EVeto/FrontFaceZ",this);
  fSetEVetoFrontFaceZCmd->SetGuidance("Set position along Z of EVeto front face in cm.");
  G4UIparameter* effPosZParameter = new G4UIparameter("PosZ",'d',false);
  effPosZParameter->SetParameterRange("PosZ >= -100. && PosZ <= 100.");
  fSetEVetoFrontFaceZCmd->SetParameter(effPosZParameter);
  fSetEVetoFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  */

  fSetEVetoInnerFaceYCmd = new G4UIcommand("/Detector/EVeto/InnerFaceY",this);
  fSetEVetoInnerFaceYCmd->SetGuidance("Set position along Y of EVeto inner face in cm.");
  G4UIparameter* pifPosYParameter = new G4UIparameter("PosY",'d',false);
  pifPosYParameter->SetParameterRange("PosY >= -100. && PosY <= -10.");
  fSetEVetoInnerFaceYCmd->SetParameter(pifPosYParameter);
  fSetEVetoInnerFaceYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

EVetoMessenger::~EVetoMessenger()
{

  delete fEVetoDetectorDir;

  delete fSetEVetoNFingersCmd;
  delete fSetFingerSizeCmd;
  //delete fSetEVetoFrontFaceZCmd;
  delete fSetEVetoInnerFaceYCmd;

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

  /*
  if ( cmd == fSetEVetoFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fEVetoGeometry->SetEVetoFrontFacePosZ(z*cm);
  }
  */

  if ( cmd == fSetEVetoInnerFaceYCmd ) {
    G4double y; std::istringstream is(par); is >> y;
    fEVetoGeometry->SetEVetoInnerFacePosY(y*cm);
  }

}
