#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction* myDet)
:fDetector(myDet)
{ 
  
  fDetectorDir = new G4UIdirectory("/Detector/");
  fDetectorDir->SetGuidance("UI commands to manage detector construction.");

  fDetectorSetupCmd = new G4UIcmdWithAnInteger("/Detector/Setup",this);
  fDetectorSetupCmd->SetGuidance("Set main setup of detector.");
  fDetectorSetupCmd->SetGuidance("10: 2019");
  fDetectorSetupCmd->SetGuidance("20: 2020");
  fDetectorSetupCmd->SetGuidance("30: 2021");
  fDetectorSetupCmd->SetGuidance("40: 2022 - Add ETag, move ECal, remove SAC, switch off Vetoes, TimePix behind ECal");
  fDetectorSetupCmd->SetParameterName("DS",false);
  fDetectorSetupCmd->SetRange("DS >= 0");
  fDetectorSetupCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

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

  fCrossMagVolCmd = new G4UIcmdWithAString("/Detector/CrossMagneticVolume",this);
  fCrossMagVolCmd->SetGuidance("Define magnetic volume to be used in the Cross (Target) zone.");
  fCrossMagVolCmd->SetGuidance("Possible choices are:");
  fCrossMagVolCmd->SetGuidance("internal     magnetic field only inside the beam pipe (old style).");
  fCrossMagVolCmd->SetGuidance("external     the full cross region is included in the magnetic field.");
  fCrossMagVolCmd->SetParameterName("CMV",false);
  fCrossMagVolCmd->SetCandidates("internal external");
  fCrossMagVolCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

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

  fMagnetVisibleCmd = new G4UIcmdWithoutParameter("/Detector/SetMagnetVisible",this);
  fMagnetVisibleCmd->SetGuidance("Make dipole magnet structure visible.");
  fMagnetVisibleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fMagnetInvisibleCmd = new G4UIcmdWithoutParameter("/Detector/SetMagnetInvisible",this);
  fMagnetInvisibleCmd->SetGuidance("Make dipole magnet structure invisible.");
  fMagnetInvisibleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fChamberVisibleCmd = new G4UIcmdWithoutParameter("/Detector/SetChamberVisible",this);
  fChamberVisibleCmd->SetGuidance("Make vacuum chamber visible.");
  fChamberVisibleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fChamberInvisibleCmd = new G4UIcmdWithoutParameter("/Detector/SetChamberInvisible",this);
  fChamberInvisibleCmd->SetGuidance("Make vacuum chamber invisible.");
  fChamberInvisibleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  //M. Raggi 07/03/2019
  fBeamLineVisibleCmd = new G4UIcmdWithoutParameter("/Detector/SetBeamLineVisible",this);
  fBeamLineVisibleCmd->SetGuidance("Make BeamLine visible.");
  fBeamLineVisibleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  //M. Raggi 07/03/2019
  fBeamLineInvisibleCmd = new G4UIcmdWithoutParameter("/Detector/SetBeamLineInvisible",this);
  fBeamLineInvisibleCmd->SetGuidance("Make Beam Line invisible.");
  fBeamLineInvisibleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fMMegaReadoutCmd = new G4UIcmdWithAString("/Detector/MMegaReadoutType",this);
  fMMegaReadoutCmd->SetGuidance("Defines layout of MMega Readout Planes.");
  fMMegaReadoutCmd->SetGuidance("Possible choices are:");
  fMMegaReadoutCmd->SetGuidance("strips   readout electrode is segmented in strips.");
  fMMegaReadoutCmd->SetGuidance("pads     readout electrode is segmented in pads");
  fMMegaReadoutCmd->SetParameterName("Readout Type",false);
  fMMegaReadoutCmd->SetCandidates("strips pads");
  fMMegaReadoutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fWorldIsAirCmd = new G4UIcmdWithoutParameter("/Detector/WorldIsAir",this);
  fWorldIsAirCmd->SetGuidance("Fill world (and magnetic volume) with air.");
  fWorldIsAirCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fWorldIsVacuumCmd = new G4UIcmdWithoutParameter("/Detector/WorldIsVacuum",this);
  fWorldIsVacuumCmd->SetGuidance("Fill world (and magnetic volume) with vacuum (i.e. low pressure air).");
  fWorldIsVacuumCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSetVerboseLevelCmd = new G4UIcmdWithAnInteger("/Detector/VerboseLevel",this);
  fSetVerboseLevelCmd->SetGuidance("Set verbose level for detector code.");
  fSetVerboseLevelCmd->SetParameterName("VL",false);
  fSetVerboseLevelCmd->SetRange("VL >= 0");
  fSetVerboseLevelCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
  delete fDetectorSetupCmd;
  delete fEnableSubDetCmd;
  delete fDisableSubDetCmd;
  delete fEnableStructCmd;
  delete fDisableStructCmd;
  delete fEnableMagFieldCmd;
  delete fDisableMagFieldCmd;
  delete fCrossMagVolCmd;
  delete fMagVolVisibleCmd;
  delete fMagVolInvisibleCmd;
  delete fMagnetVisibleCmd;
  delete fMagnetInvisibleCmd;
  delete fSetMagFieldValueCmd;
  delete fChamberVisibleCmd;
  delete fChamberInvisibleCmd;
  delete fBeamLineVisibleCmd;   //M. Raggi 07/03/2019
  delete fBeamLineInvisibleCmd; //M. Raggi 07/03/2019
  delete fMMegaReadoutCmd; // D.Quaranta 09/04/2024
  delete fWorldIsAirCmd;
  delete fWorldIsVacuumCmd;
  delete fSetVerboseLevelCmd;
  delete fDetectorDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  if( command == fDetectorSetupCmd )  fDetector->SetDetectorSetup(fDetectorSetupCmd->GetNewIntValue(newValue));

  if( command == fEnableSubDetCmd )  fDetector->EnableSubDetector(newValue);
  if( command == fDisableSubDetCmd ) fDetector->DisableSubDetector(newValue);

  if( command == fEnableStructCmd )  fDetector->EnableStructure(newValue);
  if( command == fDisableStructCmd ) fDetector->DisableStructure(newValue);

  if( command == fEnableMagFieldCmd )  fDetector->EnableMagneticField();
  if( command == fDisableMagFieldCmd ) fDetector->DisableMagneticField();

  if ( command == fCrossMagVolCmd ) fDetector->SetCrossMagneticVolume(newValue);

  if( command == fMagVolVisibleCmd )   fDetector->MagneticVolumeIsVisible();
  if( command == fMagVolInvisibleCmd ) fDetector->MagneticVolumeIsInvisible();

  if ( command == fSetMagFieldValueCmd )
    fDetector->SetMagFieldValue(fSetMagFieldValueCmd->GetNewDoubleValue(newValue));

  if( command == fMagnetVisibleCmd )   fDetector->MagnetIsVisible();
  if( command == fMagnetInvisibleCmd ) fDetector->MagnetIsInvisible();

  if( command == fChamberVisibleCmd )   fDetector->ChamberIsVisible();
  if( command == fChamberInvisibleCmd ) fDetector->ChamberIsInvisible();

  if( command == fBeamLineVisibleCmd )   fDetector->BeamLineIsVisible(); //M. Raggi 07/03/2019
  if( command == fBeamLineInvisibleCmd ) fDetector->BeamLineIsInvisible(); //M. Raggi 07/03/2019

  if( command == fMMegaReadoutCmd)    fDetector->SetMMegaReadoutType(newValue); //D. Quaranta 09/04/2024

  if( command == fWorldIsAirCmd )    fDetector->WorldIsAir();
  if( command == fWorldIsVacuumCmd ) fDetector->WorldIsVacuum();

  if ( command == fSetVerboseLevelCmd )
    fDetector->SetVerboseLevel(fSetVerboseLevelCmd->GetNewIntValue(newValue));

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
