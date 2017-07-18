// SACMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "SACMessenger.hh"

#include "SACDetector.hh"
#include "SACGeometry.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

SACMessenger::SACMessenger(SACDetector* det)
:fSACDetector(det)
{

  fSACGeometry = SACGeometry::GetInstance();

  fSACDetectorDir = new G4UIdirectory("/Detector/SAC/");
  fSACDetectorDir->SetGuidance("UI commands to control SAC detector geometry");

  fSetSACNRowsCmd = new G4UIcmdWithAnInteger("/Detector/SAC/NRows",this);
  fSetSACNRowsCmd->SetGuidance("Set number of crystal rows in SAC.");
  fSetSACNRowsCmd->SetParameterName("NR",false);
  fSetSACNRowsCmd->SetRange("NR >= 1 && NR <= 10");
  fSetSACNRowsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetSACNColsCmd = new G4UIcmdWithAnInteger("/Detector/SAC/NCols",this);
  fSetSACNColsCmd->SetGuidance("Set number of crystal columns in SAC.");
  fSetSACNColsCmd->SetParameterName("NC",false);
  fSetSACNColsCmd->SetRange("NC >= 1 && NC <= 10");
  fSetSACNColsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetCrystalSizeCmd = new G4UIcmdWithADoubleAndUnit("/Detector/SAC/CrystalSize",this);
  fSetCrystalSizeCmd->SetGuidance("Set size (side of square) of SAC crystal front face.");
  fSetCrystalSizeCmd->SetParameterName("S",false);
  fSetCrystalSizeCmd->SetDefaultUnit("cm");
  fSetCrystalSizeCmd->SetRange("S > 0. && S <= 10.");
  fSetCrystalSizeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetCrystalLengthCmd = new G4UIcmdWithADoubleAndUnit("/Detector/SAC/CrystalLength",this);
  fSetCrystalLengthCmd->SetGuidance("Set length of SAC crystal.");
  fSetCrystalLengthCmd->SetParameterName("L",false);
  fSetCrystalLengthCmd->SetDefaultUnit("cm");
  fSetCrystalLengthCmd->SetRange("L > 0. && L <= 30.");
  fSetCrystalLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetCrystalGapCmd = new G4UIcmdWithADoubleAndUnit("/Detector/SAC/CrystalGap",this);
  fSetCrystalGapCmd->SetGuidance("Set size of gap between crystals.");
  fSetCrystalGapCmd->SetParameterName("G",false);
  fSetCrystalGapCmd->SetDefaultUnit("um");
  fSetCrystalGapCmd->SetRange("G > 0. && G <= 1000.");
  fSetCrystalGapCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetCrystalCoatingCmd = new G4UIcmdWithADoubleAndUnit("/Detector/SAC/CrystalCoating",this);
  fSetCrystalCoatingCmd->SetGuidance("Set thickness of paint coating around crystal.");
  fSetCrystalCoatingCmd->SetParameterName("C",false);
  fSetCrystalCoatingCmd->SetDefaultUnit("um");
  fSetCrystalCoatingCmd->SetRange("C > 0. && C <= 1000.");
  fSetCrystalCoatingCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetSACFrontFaceZCmd = new G4UIcmdWithADoubleAndUnit("/Detector/SAC/FrontFaceZ",this);
  fSetSACFrontFaceZCmd->SetGuidance("Set position along Z of SAC front face.");
  fSetSACFrontFaceZCmd->SetParameterName("Z",false);
  fSetSACFrontFaceZCmd->SetDefaultUnit("cm");
  fSetSACFrontFaceZCmd->SetRange("Z > 100. && Z <= 1000.");
  fSetSACFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

SACMessenger::~SACMessenger()
{

  delete fSACDetectorDir;

  delete fSetSACNRowsCmd;
  delete fSetSACNColsCmd;

  delete fSetCrystalSizeCmd;
  delete fSetCrystalLengthCmd;

  delete fSetCrystalGapCmd;
  delete fSetCrystalCoatingCmd;

  delete fSetSACFrontFaceZCmd;

}

void SACMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetSACNRowsCmd )
    fSACGeometry->SetSACNRows(fSetSACNRowsCmd->GetNewIntValue(par));
  if ( cmd == fSetSACNColsCmd )
    fSACGeometry->SetSACNCols(fSetSACNColsCmd->GetNewIntValue(par));

  if ( cmd == fSetCrystalSizeCmd ) {
    fSACGeometry->SetCrystalSizeX(fSetCrystalSizeCmd->GetNewDoubleValue(par));
    fSACGeometry->SetCrystalSizeY(fSetCrystalSizeCmd->GetNewDoubleValue(par));
  }
  if ( cmd == fSetCrystalLengthCmd )
    fSACGeometry->SetCrystalSizeZ(fSetCrystalLengthCmd->GetNewDoubleValue(par));

  if ( cmd == fSetCrystalGapCmd )
    fSACGeometry->SetCrystalGap(fSetCrystalGapCmd->GetNewDoubleValue(par));
  if ( cmd == fSetCrystalCoatingCmd )
    fSACGeometry->SetCrystalCoating(fSetCrystalCoatingCmd->GetNewDoubleValue(par));

  if ( cmd == fSetSACFrontFaceZCmd )
    fSACGeometry->SetSACFrontFacePosZ(fSetSACFrontFaceZCmd->GetNewDoubleValue(par));

}
