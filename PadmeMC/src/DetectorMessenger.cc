#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction* myDet)
:fDetector(myDet)
{ 
  
  fDetectorDir = new G4UIdirectory("/Detector/");
  fDetectorDir->SetGuidance("UI commands to manage detector construction.");

  fEnableSubDetCmd = new G4UIcmdWithAString("/Detector/EnableSubDetector",this);
  fEnableSubDetCmd->SetGuidance("Enable sub detector in simulation.");
  fEnableSubDetCmd->SetParameterName("Det",false);
  fEnableSubDetCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fDisableSubDetCmd = new G4UIcmdWithAString("/Detector/DisableSubDetector",this);
  fDisableSubDetCmd->SetGuidance("Disable sub detector in simulation.");
  fDisableSubDetCmd->SetParameterName("Det",false);
  fDisableSubDetCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEnableStructCmd = new G4UIcmdWithAString("/Detector/EnableStructure",this);
  fEnableStructCmd->SetGuidance("Enable structure in simulation.");
  fEnableStructCmd->SetParameterName("Struct",false);
  fEnableStructCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fDisableStructCmd = new G4UIcmdWithAString("/Detector/DisableStructure",this);
  fDisableStructCmd->SetGuidance("Disable structure in simulation.");
  fDisableStructCmd->SetParameterName("Struct",false);
  fDisableStructCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEnableMagFieldCmd = new G4UIcmdWithoutParameter("/Detector/EnableMagneticField",this);
  fEnableMagFieldCmd->SetGuidance("Enable magnetic field in simulation.");
  fEnableMagFieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fDisableMagFieldCmd = new G4UIcmdWithoutParameter("/Detector/DisableMagneticField",this);
  fDisableMagFieldCmd->SetGuidance("Disable magnetic field in simulation.");
  fDisableMagFieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fMagVolVisibleCmd = new G4UIcmdWithoutParameter("/Detector/SetMagneticVolumeVisible",this);
  fMagVolVisibleCmd->SetGuidance("Make magnetic volume visible.");
  fMagVolVisibleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fMagVolInvisibleCmd = new G4UIcmdWithoutParameter("/Detector/SetMagneticVolumeInvisible",this);
  fMagVolInvisibleCmd->SetGuidance("Make magnetic volume invisible.");
  fMagVolInvisibleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetMagFieldValueCmd = new G4UIcmdWithADoubleAndUnit("/Detector/SetMagneticFieldValue",this);
  fSetMagFieldValueCmd->SetGuidance("Set value of constant part of magnetic field.");
  fSetMagFieldValueCmd->SetParameterName("MFV",false);
  fSetMagFieldValueCmd->SetDefaultUnit("tesla");
  fSetMagFieldValueCmd->SetRange("MFV >= -2. && MFV <= 2.");
  fSetMagFieldValueCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fChamberVisibleCmd = new G4UIcmdWithoutParameter("/Detector/SetChamberVisible",this);
  fChamberVisibleCmd->SetGuidance("Make vacuum chamber visible.");
  fChamberVisibleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fChamberInvisibleCmd = new G4UIcmdWithoutParameter("/Detector/SetChamberInvisible",this);
  fChamberInvisibleCmd->SetGuidance("Make vacuum chamber invisible.");
  fChamberInvisibleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fWorldIsAirCmd = new G4UIcmdWithoutParameter("/Detector/WorldIsAir",this);
  fWorldIsAirCmd->SetGuidance("Fill world (and magnetic volume) with air.");
  fWorldIsAirCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fWorldIsVacuumCmd = new G4UIcmdWithoutParameter("/Detector/WorldIsVacuum",this);
  fWorldIsVacuumCmd->SetGuidance("Fill world (and magnetic volume) with vacuum (i.e. low pressure air).");
  fWorldIsVacuumCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
  delete fDetectorDir;
  delete fEnableSubDetCmd;
  delete fDisableSubDetCmd;
  delete fEnableStructCmd;
  delete fDisableStructCmd;
  delete fEnableMagFieldCmd;
  delete fDisableMagFieldCmd;
  delete fMagVolVisibleCmd;
  delete fMagVolInvisibleCmd;
  delete fSetMagFieldValueCmd;
  delete fChamberVisibleCmd;
  delete fChamberInvisibleCmd;
  delete fWorldIsAirCmd;
  delete fWorldIsVacuumCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  if( command == fEnableSubDetCmd )  fDetector->EnableSubDetector(newValue);
  if( command == fDisableSubDetCmd ) fDetector->DisableSubDetector(newValue);

  if( command == fEnableStructCmd )  fDetector->EnableStructure(newValue);
  if( command == fDisableStructCmd ) fDetector->DisableStructure(newValue);

  if( command == fEnableMagFieldCmd )  fDetector->EnableMagneticField();
  if( command == fDisableMagFieldCmd ) fDetector->DisableMagneticField();

  if( command == fMagVolVisibleCmd )   fDetector->MagneticVolumeIsVisible();
  if( command == fMagVolInvisibleCmd ) fDetector->MagneticVolumeIsInvisible();

  if ( command == fSetMagFieldValueCmd )
    fDetector->SetMagFieldValue(fSetMagFieldValueCmd->GetNewDoubleValue(newValue));

  if( command == fChamberVisibleCmd )   fDetector->ChamberIsVisible();
  if( command == fChamberInvisibleCmd ) fDetector->ChamberIsInvisible();

  if( command == fWorldIsAirCmd )    fDetector->WorldIsAir();
  if( command == fWorldIsVacuumCmd ) fDetector->WorldIsVacuum();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
