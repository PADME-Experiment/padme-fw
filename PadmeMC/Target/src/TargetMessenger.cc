// TargetMessenger.cc
// --------------------------------------------------------------
// History:
//
// 2015-12-14 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "TargetMessenger.hh"

#include "G4UIdirectory.hh"
//#include "G4UIcommand.hh"
//#include "G4UIparameter.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

#include "TargetDetector.hh"
#include "TargetGeometry.hh"

TargetMessenger::TargetMessenger(TargetDetector* det)
:fTargetDetector(det)
{

  fTargetGeometry = TargetGeometry::GetInstance();

  fTargetDetectorDir = new G4UIdirectory("/Detector/Target/");
  fTargetDetectorDir->SetGuidance("UI commands to control Target detector geometry");

  fSetTargetSizeCmd = new G4UIcmdWithADoubleAndUnit("/Detector/Target/Size",this);
  fSetTargetSizeCmd->SetGuidance("Set size (side of squared plaque) of Target detector.");
  fSetTargetSizeCmd->SetParameterName("Size",false);
  fSetTargetSizeCmd->SetDefaultUnit("cm");
  fSetTargetSizeCmd->SetRange("Size >= 0.1 && Size <= 5.");
  fSetTargetSizeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
 
  fSetTargetThicknessCmd = new G4UIcmdWithADoubleAndUnit("/Detector/Target/Thickness",this);
  fSetTargetThicknessCmd->SetGuidance("Set thickness of Target detector.");
  fSetTargetThicknessCmd->SetParameterName("Thick",false);
  fSetTargetThicknessCmd->SetDefaultUnit("um");
  fSetTargetThicknessCmd->SetRange("Thick >= 10. && Thick <= 500.");
  fSetTargetThicknessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetTargetFrontFaceZCmd = new G4UIcmdWithADoubleAndUnit("/Detector/Target/FrontFaceZ",this);
  fSetTargetFrontFaceZCmd->SetGuidance("Set position along Z of Target front face (reference: center of magnet).");
  fSetTargetFrontFaceZCmd->SetParameterName("PosZ",false);
  fSetTargetFrontFaceZCmd->SetDefaultUnit("cm");
  fSetTargetFrontFaceZCmd->SetRange("PosZ >= -100. && PosZ <= -56."); // Upper limit is close to back face of vacuum chamber
  fSetTargetFrontFaceZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEnableFastDigitizationCmd = new G4UIcmdWithoutParameter("/Detector/Target/EnableFastDigitization",this);
  fEnableFastDigitizationCmd->SetGuidance("Enable fast digitization for Target.");
  fEnableFastDigitizationCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fDisableFastDigitizationCmd = new G4UIcmdWithoutParameter("/Detector/Target/DisableFastDigitization",this);
  fDisableFastDigitizationCmd->SetGuidance("Disable fast digitization for Target.");
  fDisableFastDigitizationCmd->SetGuidance("WARNING: full digitization is VERY time consuming!");
  fDisableFastDigitizationCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEnableSaveWaveformToDigiCmd = new G4UIcmdWithoutParameter("/Detector/Target/EnableSaveWaveformToDigi",this);
  fEnableSaveWaveformToDigiCmd->SetGuidance("Enable saving of Target digitized waveforms to persistent digis.");
  fEnableSaveWaveformToDigiCmd->SetGuidance("WARNING: saving waveforms adds O(300KB) of data to each event!");
  fEnableSaveWaveformToDigiCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fDisableSaveWaveformToDigiCmd = new G4UIcmdWithoutParameter("/Detector/Target/DisableSaveWaveformToDigi",this);
  fDisableSaveWaveformToDigiCmd->SetGuidance("Disable saving of Target digitized waveforms to persistent digis.");
  fDisableSaveWaveformToDigiCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

TargetMessenger::~TargetMessenger()
{

  delete fTargetDetectorDir;

  delete fSetTargetSizeCmd;
  delete fSetTargetThicknessCmd;
  delete fSetTargetFrontFaceZCmd;

  delete fEnableFastDigitizationCmd;
  delete fDisableFastDigitizationCmd;
  delete fEnableSaveWaveformToDigiCmd;
  delete fDisableSaveWaveformToDigiCmd;

}

void TargetMessenger::SetNewValue(G4UIcommand* cmd, G4String par)
{

  if ( cmd == fSetTargetSizeCmd ) {
    fTargetGeometry->SetTargetSizeX(fSetTargetSizeCmd->GetNewDoubleValue(par));
    fTargetGeometry->SetTargetSizeY(fSetTargetSizeCmd->GetNewDoubleValue(par));
  }

  if ( cmd == fSetTargetThicknessCmd )
    fTargetGeometry->SetTargetSizeZ(fSetTargetThicknessCmd->GetNewDoubleValue(par));

  if ( cmd == fSetTargetFrontFaceZCmd )
    fTargetGeometry->SetTargetFrontFacePosZ(fSetTargetFrontFaceZCmd->GetNewDoubleValue(par));

  if ( cmd == fEnableFastDigitizationCmd )  fTargetGeometry->EnableFastDigitization();
  if ( cmd == fDisableFastDigitizationCmd ) fTargetGeometry->DisableFastDigitization();

  if ( cmd == fEnableSaveWaveformToDigiCmd )  fTargetGeometry->EnableSaveWaveformToDigi();
  if ( cmd == fDisableSaveWaveformToDigiCmd ) fTargetGeometry->DisableSaveWaveformToDigi();
}
