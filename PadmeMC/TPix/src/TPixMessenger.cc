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

  fSetNColumnsCmd = new G4UIcmdWithAnInteger("/Detector/TPix/Columns",this);
  fSetNColumnsCmd->SetGuidance("Set number of TPix columns.");
  fSetNColumnsCmd->SetParameterName("NC",false);
  fSetNColumnsCmd->SetRange("NF >= 1 && NF <= 10");
  fSetNColumnsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

TPixMessenger::~TPixMessenger()
{

  delete fTPixDetectorDir;

  delete fSetNColumnsCmd;
  delete fSetDistToCornerCmd;

}

void TPixMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetNColumnsCmd )
    fTPixGeometry->SetTPixNCols(fSetNColumnsCmd->GetNewIntValue(par));

  if ( cmd == fSetDistToCornerCmd )
    fTPixGeometry->SetTPixDistanceToCorner(fSetDistToCornerCmd->GetNewDoubleValue(par));

}
