// HEPVetoMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "HEPVetoMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

#include "HEPVetoDetector.hh"
#include "HEPVetoGeometry.hh"

HEPVetoMessenger::HEPVetoMessenger(HEPVetoDetector* det)
:fHEPVetoDetector(det)
{

  fHEPVetoGeometry = HEPVetoGeometry::GetInstance();

  fHEPVetoDetectorDir = new G4UIdirectory("/Detector/HEPVeto/");
  fHEPVetoDetectorDir->SetGuidance("UI commands to control HEPVeto detector geometry");

  fSetNFingersCmd = new G4UIcmdWithAnInteger("/Detector/HEPVeto/NFingers",this);
  fSetNFingersCmd->SetGuidance("Set number of HEPVeto fingers.");
  fSetNFingersCmd->SetParameterName("NF",false);
  fSetNFingersCmd->SetRange("NF >= 1 && NF <= 100");
  fSetNFingersCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetFingerStepCmd = new G4UIcmdWithADoubleAndUnit("/Detector/HEPVeto/FingerStep",this);
  fSetFingerStepCmd->SetGuidance("Set step (distance between centers) of finger position.");
  fSetFingerStepCmd->SetParameterName("FSP",false);
  fSetFingerStepCmd->SetDefaultUnit("cm");
  fSetFingerStepCmd->SetRange("FSP > 0. && FSP <= 5.");
  fSetFingerStepCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetFingerSizeCmd = new G4UIcmdWithADoubleAndUnit("/Detector/HEPVeto/FingerSize",this);
  fSetFingerSizeCmd->SetGuidance("Set size (side of square) of finger front face.");
  fSetFingerSizeCmd->SetParameterName("FS",false);
  fSetFingerSizeCmd->SetDefaultUnit("cm");
  fSetFingerSizeCmd->SetRange("FS > 0. && FS <= 5.");
  fSetFingerSizeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetFingerLengthCmd = new G4UIcmdWithADoubleAndUnit("/Detector/HEPVeto/FingerLength",this);
  fSetFingerLengthCmd->SetGuidance("Set length of finger.");
  fSetFingerLengthCmd->SetParameterName("FL",false);
  fSetFingerLengthCmd->SetDefaultUnit("cm");
  fSetFingerLengthCmd->SetRange("FL > 0. && FS <= 50.");
  fSetFingerLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetDistToCornerCmd = new G4UIcmdWithADoubleAndUnit("/Detector/HEPVeto/DistanceToCorner",this);
  fSetDistToCornerCmd->SetGuidance("Set distance of HEPVeto to corner of vacuum chamber back wall.");
  fSetDistToCornerCmd->SetParameterName("DTC",false);
  fSetDistToCornerCmd->SetDefaultUnit("cm");
  fSetDistToCornerCmd->SetRange("DTC > 0. && DTC <= 100.");
  fSetDistToCornerCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

HEPVetoMessenger::~HEPVetoMessenger()
{

  delete fHEPVetoDetectorDir;

  delete fSetNFingersCmd;
  delete fSetFingerStepCmd;
  delete fSetFingerSizeCmd;
  delete fSetFingerLengthCmd;
  delete fSetDistToCornerCmd;
}

void HEPVetoMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetNFingersCmd )
    fHEPVetoGeometry->SetHEPVetoNFingers(fSetNFingersCmd->GetNewIntValue(par));

  if ( cmd == fSetFingerSizeCmd ) {
    fHEPVetoGeometry->SetFingerSizeX(fSetFingerSizeCmd->GetNewDoubleValue(par));
    fHEPVetoGeometry->SetFingerSizeZ(fSetFingerSizeCmd->GetNewDoubleValue(par));
  }

  if ( cmd == fSetFingerLengthCmd )
    fHEPVetoGeometry->SetFingerSizeY(fSetFingerLengthCmd->GetNewDoubleValue(par));

  if ( cmd == fSetDistToCornerCmd )
    fHEPVetoGeometry->SetHEPVetoDistanceToCorner(fSetDistToCornerCmd->GetNewDoubleValue(par));

}
