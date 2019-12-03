// TPixMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2016-02-05 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "TPixMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

#include "TPixDetector.hh"
#include "TPixGeometry.hh"

TPixMessenger::TPixMessenger(TPixDetector* det)
:fTPixDetector(det)
{

  fTPixGeometry = TPixGeometry::GetInstance();

  fTPixDetectorDir = new G4UIdirectory("/Detector/TPix/");
  fTPixDetectorDir->SetGuidance("UI commands to control TPix detector geometry");

  fSetNColumnsCmd = new G4UIcmdWithAnInteger("/Detector/TPix/NColumns",this);
  fSetNColumnsCmd->SetGuidance("Set number of TPix columns.");
  fSetNColumnsCmd->SetParameterName("NC",false);
  fSetNColumnsCmd->SetRange("NF >= 1 && NF <= 10");
  fSetNColumnsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetDistToCornerCmd = new G4UIcmdWithADoubleAndUnit("/Detector/TPix/DistanceToCorner",this);
  fSetDistToCornerCmd->SetGuidance("Set distance of TPix to corner of vacuum chamber back wall.");
  fSetDistToCornerCmd->SetParameterName("DTC",false);
  fSetDistToCornerCmd->SetDefaultUnit("cm");
  fSetDistToCornerCmd->SetRange("DTC > 0. && DTC <= 100.");
  fSetDistToCornerCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetVerboseLevelCmd = new G4UIcmdWithAnInteger("/Detector/TPix/VerboseLevel",this);
  fSetVerboseLevelCmd->SetGuidance("Set verbose level for TPix code.");
  fSetVerboseLevelCmd->SetParameterName("VL",false);
  fSetVerboseLevelCmd->SetRange("VL >= 0");
  fSetVerboseLevelCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

TPixMessenger::~TPixMessenger()
{

  delete fSetNColumnsCmd;
  delete fSetDistToCornerCmd;

  delete fSetVerboseLevelCmd;

  delete fTPixDetectorDir;

}

void TPixMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetNColumnsCmd )
    fTPixGeometry->SetTPixNCols(fSetNColumnsCmd->GetNewIntValue(par));

  if ( cmd == fSetDistToCornerCmd )
    fTPixGeometry->SetTPixDistanceToCorner(fSetDistToCornerCmd->GetNewDoubleValue(par));
 
  if ( cmd == fSetVerboseLevelCmd )
    fTPixGeometry->SetVerboseLevel(fSetVerboseLevelCmd->GetNewIntValue(par));

}
