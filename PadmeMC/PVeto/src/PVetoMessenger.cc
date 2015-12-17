// PVetoMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "PVetoMessenger.hh"

#include "PVetoDetector.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "PVetoGeometry.hh"

PVetoMessenger::PVetoMessenger(PVetoDetector* det)
:fPVetoDetector(det)
{

  fPVetoGeometry = PVetoGeometry::GetInstance();

  fPVetoDetectorDir = new G4UIdirectory("/Detector/PVeto/");
  fPVetoDetectorDir->SetGuidance("UI commands to control PVeto detector geometry");

  fSetPVetoNFingersCmd = new G4UIcommand("/Detector/PVeto/NFingers",this);
  fSetPVetoNFingersCmd->SetGuidance("Set number of PVeto fingers.");
  G4UIparameter* nfNFinParameter = new G4UIparameter("NFin",'i',false);
  nfNFinParameter->SetParameterRange("NFin >= 1 && NFin <= 1000");
  fSetPVetoNFingersCmd->SetParameter(nfNFinParameter);
  fSetPVetoNFingersCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetFingerSizeCmd = new G4UIcommand("/Detector/PVeto/FingerSize",this);
  fSetFingerSizeCmd->SetGuidance("Set size (side of square) of finger front face in cm.");
  G4UIparameter* fsSizeParameter = new G4UIparameter("Size",'d',false);
  fsSizeParameter->SetParameterRange("Size > 0. && Size <= 10.");
  fSetFingerSizeCmd->SetParameter(fsSizeParameter);
  fSetFingerSizeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  /*
  fSetPVetoFrontFaceZCmd = new G4UIcommand("/Detector/PVeto/FrontFaceZ",this);
  fSetPVetoFrontFaceZCmd->SetGuidance("Set position along Z of PVeto front face in cm.");
  G4UIparameter* effPosZParameter = new G4UIparameter("PosZ",'d',false);
  effPosZParameter->SetParameterRange("PosZ >= -100. && PosZ <= 100.");
  fSetPVetoFrontFaceZCmd->SetParameter(effPosZParameter);
  fSetPVetoFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  */

  fSetPVetoInnerFaceYCmd = new G4UIcommand("/Detector/PVeto/InnerFaceY",this);
  fSetPVetoInnerFaceYCmd->SetGuidance("Set position along Y of PVeto inner face in cm.");
  G4UIparameter* pifPosYParameter = new G4UIparameter("PosY",'d',false);
  pifPosYParameter->SetParameterRange("PosY >= -100. && PosY <= -10.");
  fSetPVetoInnerFaceYCmd->SetParameter(pifPosYParameter);
  fSetPVetoInnerFaceYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

PVetoMessenger::~PVetoMessenger()
{

  delete fPVetoDetectorDir;

  delete fSetPVetoNFingersCmd;
  delete fSetFingerSizeCmd;
  //delete fSetPVetoFrontFaceZCmd;
  delete fSetPVetoInnerFaceYCmd;

}

void PVetoMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetPVetoNFingersCmd ) {
    G4int n; std::istringstream is(par); is >> n;
    fPVetoGeometry->SetPVetoNFingers(n);
  }

  if ( cmd == fSetFingerSizeCmd ) {
    G4double s; std::istringstream is(par); is >> s;
    fPVetoGeometry->SetFingerNominalSizeY(s*cm);
    fPVetoGeometry->SetFingerNominalSizeZ(s*cm);
  }

  /*
  if ( cmd == fSetPVetoFrontFaceZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fPVetoGeometry->SetPVetoFrontFacePosZ(z*cm);
  }
  */

  if ( cmd == fSetPVetoInnerFaceYCmd ) {
    G4double y; std::istringstream is(par); is >> y;
    fPVetoGeometry->SetPVetoInnerFacePosY(y*cm);
  }

}
