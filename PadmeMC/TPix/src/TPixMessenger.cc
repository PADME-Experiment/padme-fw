// TPixMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2016-02-05 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "TPixMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "TPixDetector.hh"
#include "TPixGeometry.hh"

TPixMessenger::TPixMessenger(TPixDetector* det)
:fTPixDetector(det)
{

  fTPixGeometry = TPixGeometry::GetInstance();

  fTPixDetectorDir = new G4UIdirectory("/Detector/TPix/");
  fTPixDetectorDir->SetGuidance("UI commands to control TPix detector geometry");

  fSetTPixNColumnsCmd = new G4UIcommand("/Detector/TPix/NColumns",this);
  fSetTPixNColumnsCmd->SetGuidance("Set number of TPix columns of chips.");
  G4UIparameter* nfNColParameter = new G4UIparameter("NCol",'i',false);
  nfNColParameter->SetParameterRange("NCol >= 1 && NFin <= 5");
  fSetTPixNColumnsCmd->SetParameter(nfNColParameter);
  fSetTPixNColumnsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTPixPosXCmd = new G4UIcommand("/Detector/TPix/PositionX",this);
  fSetTPixPosXCmd->SetGuidance("Set position along X of TPix center in cm.");
  G4UIparameter* pxPosXParameter = new G4UIparameter("PosX",'d',false);
  pxPosXParameter->SetParameterRange("PosX > 0. && PosX <= 200.");
  fSetTPixPosXCmd->SetParameter(pxPosXParameter);
  fSetTPixPosXCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  /*
  fSetTPixPosYCmd = new G4UIcommand("/Detector/TPix/PositionY",this);
  fSetTPixPosYCmd->SetGuidance("Set position along Y of TPix center in cm.");
  G4UIparameter* pyPosYParameter = new G4UIparameter("PosY",'d',false);
  pyPosYParameter->SetParameterRange("PosY >= -1000. && PosY <= -50.");
  fSetTPixPosYCmd->SetParameter(pyPosYParameter);
  fSetTPixPosYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  */

  fSetTPixPosZCmd = new G4UIcommand("/Detector/TPix/PositionZ",this);
  fSetTPixPosZCmd->SetGuidance("Set position along Z of TPix center in cm.");
  G4UIparameter* pzPosZParameter = new G4UIparameter("PosZ",'d',false);
  pzPosZParameter->SetParameterRange("PosZ > 100. && PosZ <= 1000.");
  fSetTPixPosZCmd->SetParameter(pzPosZParameter);
  fSetTPixPosZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  /*
  fSetTPixRotXCmd = new G4UIcommand("/Detector/TPix/RotationX",this);
  fSetTPixRotXCmd->SetGuidance("Set TPix rotation around X axis in rad.");
  G4UIparameter* rxRotXParameter = new G4UIparameter("RotX",'d',false);
  fSetTPixRotXCmd->SetParameter(rxRotXParameter);
  fSetTPixRotXCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  */

  fSetTPixRotYCmd = new G4UIcommand("/Detector/TPix/RotationY",this);
  fSetTPixRotYCmd->SetGuidance("Set TPix rotation around Y axis in rad.");
  G4UIparameter* ryRotYParameter = new G4UIparameter("RotY",'d',false);
  fSetTPixRotYCmd->SetParameter(ryRotYParameter);
  fSetTPixRotYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  /*
  fSetTPixRotZCmd = new G4UIcommand("/Detector/TPix/RotationZ",this);
  fSetTPixRotZCmd->SetGuidance("Set TPix rotation around Z axis in rad.");
  G4UIparameter* rzRotZParameter = new G4UIparameter("RotZ",'d',false);
  fSetTPixRotZCmd->SetParameter(rzRotZParameter);
  fSetTPixRotZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  */

}

TPixMessenger::~TPixMessenger()
{

  delete fTPixDetectorDir;

  delete fSetTPixNColumnsCmd;

  delete fSetTPixPosXCmd;
  //delete fSetTPixPosYCmd;
  delete fSetTPixPosZCmd;

  //delete fSetTPixRotXCmd;
  delete fSetTPixRotYCmd;
  //delete fSetTPixRotZCmd;

}

void TPixMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetTPixNColumnsCmd ) {
    G4int n; std::istringstream is(par); is >> n;
    fTPixGeometry->SetTPixNCols(n);
  }

  if ( cmd == fSetTPixPosXCmd ) {
    G4double x; std::istringstream is(par); is >> x;
    fTPixGeometry->SetTPixPosX(x*cm);
  }

  /*
  if ( cmd == fSetTPixPosYCmd ) {
    G4double y; std::istringstream is(par); is >> y;
    fTPixGeometry->SetTPixPosY(y*cm);
  }
  */

  if ( cmd == fSetTPixPosZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fTPixGeometry->SetTPixPosZ(z*cm);
  }

  /*
  if ( cmd == fSetTPixRotXCmd ) {
    G4double x; std::istringstream is(par); is >> x;
    fTPixGeometry->SetTPixRotX(x*rad);
  }
  */

  if ( cmd == fSetTPixRotYCmd ) {
    G4double y; std::istringstream is(par); is >> y;
    fTPixGeometry->SetTPixRotY(y*rad);
  }

  /*
  if ( cmd == fSetTPixRotZCmd ) {
    G4double z; std::istringstream is(par); is >> z;
    fTPixGeometry->SetTPixRotZ(z*rad);
  }
  */

}
