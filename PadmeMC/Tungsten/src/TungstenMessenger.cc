// TungstenMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2017-10-03 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "TungstenMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

#include "TungstenDetector.hh"
#include "TungstenGeometry.hh"

TungstenMessenger::TungstenMessenger(TungstenDetector* det)
:fTungstenDetector(det)
{

  fTungstenGeometry = TungstenGeometry::GetInstance();

  fTungstenDetectorDir = new G4UIdirectory("/Detector/Tungsten/");
  fTungstenDetectorDir->SetGuidance("UI commands to control Tungsten target dump geometry");

  fSetTungstenLengthCmd = new G4UIcmdWithADoubleAndUnit("/Detector/Tungsten/Lenght",this);
  fSetTungstenLengthCmd->SetGuidance("Set length of Tungsten target dump.");
  fSetTungstenLengthCmd->SetParameterName("Len",false);
  fSetTungstenLengthCmd->SetDefaultUnit("cm");
  fSetTungstenLengthCmd->SetRange("Len > 0. && Len <= 20.");
  fSetTungstenLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
 
  fSetTungstenFrontFaceZCmd = new G4UIcmdWithADoubleAndUnit("/Detector/Tungsten/FrontFaceZ",this);
  fSetTungstenFrontFaceZCmd->SetGuidance("Set position along Z of Tungsten target dump front face.");
  fSetTungstenFrontFaceZCmd->SetGuidance("Reference: center of magnet.");
  fSetTungstenFrontFaceZCmd->SetParameterName("PosZ",false);
  fSetTungstenFrontFaceZCmd->SetDefaultUnit("cm");
  fSetTungstenFrontFaceZCmd->SetRange("PosZ >= -85.5 && PosZ <= -57.");
  fSetTungstenFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

TungstenMessenger::~TungstenMessenger()
{

  delete fTungstenDetectorDir;

  delete fSetTungstenLengthCmd;
  delete fSetTungstenFrontFaceZCmd;

}

void TungstenMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetTungstenLengthCmd )
    fTungstenGeometry->SetTungstenSizeZ(fSetTungstenLengthCmd->GetNewDoubleValue(par));

  if ( cmd == fSetTungstenFrontFaceZCmd )
    fTungstenGeometry->SetTungstenFrontFacePosZ(fSetTungstenFrontFaceZCmd->GetNewDoubleValue(par));

}
