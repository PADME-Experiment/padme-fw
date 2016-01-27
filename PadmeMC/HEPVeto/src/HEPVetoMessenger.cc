// HEPVetoMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "HEPVetoMessenger.hh"

#include "HEPVetoDetector.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "HEPVetoGeometry.hh"

HEPVetoMessenger::HEPVetoMessenger(HEPVetoDetector* det)
:fHEPVetoDetector(det)
{

  fHEPVetoGeometry = HEPVetoGeometry::GetInstance();

  fHEPVetoDetectorDir = new G4UIdirectory("/Detector/HEPVeto/");
  fHEPVetoDetectorDir->SetGuidance("UI commands to control HEPVeto detector geometry");

  fSetHEPVetoNFingersCmd = new G4UIcommand("/Detector/HEPVeto/NFingers",this);
  fSetHEPVetoNFingersCmd->SetGuidance("Set number of HEPVeto fingers.");
  G4UIparameter* nfNFinParameter = new G4UIparameter("NFin",'i',false);
  nfNFinParameter->SetParameterRange("NFin >= 1 && NFin <= 1000");
  fSetHEPVetoNFingersCmd->SetParameter(nfNFinParameter);
  fSetHEPVetoNFingersCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetFingerSizeCmd = new G4UIcommand("/Detector/HEPVeto/FingerSize",this);
  fSetFingerSizeCmd->SetGuidance("Set size (side of square) of finger front face in cm.");
  G4UIparameter* fsSizeParameter = new G4UIparameter("Size",'d',false);
  fsSizeParameter->SetParameterRange("Size > 0. && Size <= 10.");
  fSetFingerSizeCmd->SetParameter(fsSizeParameter);
  fSetFingerSizeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetFingerLengthCmd = new G4UIcommand("/Detector/HEPVeto/FingerLength",this);
  fSetFingerLengthCmd->SetGuidance("Set length of finger in cm.");
  G4UIparameter* flLenParameter = new G4UIparameter("Len",'d',false);
  flLenParameter->SetParameterRange("Len > 0. && Len <= 100.");
  fSetFingerLengthCmd->SetParameter(flLenParameter);
  fSetFingerLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetHEPVetoPosXCmd = new G4UIcommand("/Detector/HEPVeto/PositionX",this);
  fSetHEPVetoPosXCmd->SetGuidance("Set position along X of HEPVeto center in cm.");
  G4UIparameter* pxPosXParameter = new G4UIparameter("PosX",'d',false);
  pxPosXParameter->SetParameterRange("PosX > 0. && PosX <= 100.");
  fSetHEPVetoPosXCmd->SetParameter(pxPosXParameter);
  fSetHEPVetoPosXCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  /*
  fSetHEPVetoPosYCmd = new G4UIcommand("/Detector/HEPVeto/PositionY",this);
  fSetHEPVetoPosYCmd->SetGuidance("Set position along Y of HEPVeto center in cm.");
  G4UIparameter* pyPosYParameter = new G4UIparameter("PosY",'d',false);
  pyPosYParameter->SetParameterRange("PosY >= -1000. && PosY <= -50.");
  fSetHEPVetoPosYCmd->SetParameter(pyPosYParameter);
  fSetHEPVetoPosYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  */

  fSetHEPVetoPosZCmd = new G4UIcommand("/Detector/HEPVeto/PositionZ",this);
  fSetHEPVetoPosZCmd->SetGuidance("Set position along Z of HEPVeto center in cm.");
  G4UIparameter* pzPosZParameter = new G4UIparameter("PosZ",'d',false);
  pzPosZParameter->SetParameterRange("PosZ > 100. && PosZ <= 1000.");
  fSetHEPVetoPosZCmd->SetParameter(pzPosZParameter);
  fSetHEPVetoPosZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  /*
  fSetHEPVetoRotXCmd = new G4UIcommand("/Detector/HEPVeto/RotationX",this);
  fSetHEPVetoRotXCmd->SetGuidance("Set HEPVeto rotation around X axis in rad.");
  G4UIparameter* rxRotXParameter = new G4UIparameter("RotX",'d',false);
  fSetHEPVetoRotXCmd->SetParameter(rxRotXParameter);
  fSetHEPVetoRotXCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  */

  fSetHEPVetoRotYCmd = new G4UIcommand("/Detector/HEPVeto/RotationY",this);
  fSetHEPVetoRotYCmd->SetGuidance("Set HEPVeto rotation around Y axis in rad.");
  G4UIparameter* ryRotYParameter = new G4UIparameter("RotY",'d',false);
  fSetHEPVetoRotYCmd->SetParameter(ryRotYParameter);
  fSetHEPVetoRotYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  /*
  fSetHEPVetoRotZCmd = new G4UIcommand("/Detector/HEPVeto/RotationZ",this);
  fSetHEPVetoRotZCmd->SetGuidance("Set HEPVeto rotation around Z axis in rad.");
  G4UIparameter* rzRotZParameter = new G4UIparameter("RotZ",'d',false);
  fSetHEPVetoRotZCmd->SetParameter(rzRotZParameter);
  fSetHEPVetoRotZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  */

}

HEPVetoMessenger::~HEPVetoMessenger()
{

  delete fHEPVetoDetectorDir;

  delete fSetHEPVetoNFingersCmd;
  delete fSetFingerSizeCmd;
  delete fSetFingerLengthCmd;

  delete fSetHEPVetoPosXCmd;
  //delete fSetHEPVetoPosYCmd;
  delete fSetHEPVetoPosZCmd;

  //delete fSetHEPVetoRotXCmd;
  delete fSetHEPVetoRotYCmd;
  //delete fSetHEPVetoRotZCmd;

}

void HEPVetoMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetHEPVetoNFingersCmd ) {
    G4int n; std::istringstream is(par); is >> n;
    fHEPVetoGeometry->SetHEPVetoNFingers(n);
  }

  if ( cmd == fSetFingerSizeCmd ) {
    G4double s; std::istringstream is(par); is >> s;
    fHEPVetoGeometry->SetFingerNominalSizeY(s*cm);
    fHEPVetoGeometry->SetFingerNominalSizeZ(s*cm);
  }

  if ( cmd == fSetFingerLengthCmd ) {
    G4double l; std::istringstream is(par); is >> l;
    fHEPVetoGeometry->SetFingerNominalSizeX(l*cm);
  }

  if ( cmd == fSetHEPVetoPosXCmd ) {
    G4double x; std::istringstream is(par); is >> x;
    fHEPVetoGeometry->SetHEPVetoPosX(x*cm);
  }

  /*
  if ( cmd == fSetHEPVetoPosYCmd ) {
    G4double y; std::istringstream is(par); is >> y;
    fHEPVetoGeometry->SetHEPVetoPosY(y*cm);
  }
  */

  if ( cmd == fSetHEPVetoPosZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fHEPVetoGeometry->SetHEPVetoPosZ(z*cm);
  }

  /*
  if ( cmd == fSetHEPVetoRotXCmd ) {
    G4double x; std::istringstream is(par); is >> x;
    fHEPVetoGeometry->SetHEPVetoRotX(x*rad);
  }
  */

  if ( cmd == fSetHEPVetoRotYCmd ) {
    G4double y; std::istringstream is(par); is >> y;
    fHEPVetoGeometry->SetHEPVetoRotY(y*rad);
  }

  /*
  if ( cmd == fSetHEPVetoRotZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fHEPVetoGeometry->SetHEPVetoRotZ(z*rad);
  }
  */

}
