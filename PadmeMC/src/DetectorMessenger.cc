#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction* myDet)
:fDetector(myDet)
{ 
  //N02Dir = new G4UIdirectory("/N02/");
  //N02Dir->SetGuidance("UI commands specific to this example.");
  
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

  /*
  TargMatCmd = new G4UIcmdWithAString("/N02/det/setTargetMate",this);
  TargMatCmd->SetGuidance("Select Material of the Target.");
  TargMatCmd->SetParameterName("choice",false);
  TargMatCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  ChamMatCmd = new G4UIcmdWithAString("/N02/det/setChamberMate",this);
  ChamMatCmd->SetGuidance("Select Material of the Target.");
  ChamMatCmd->SetParameterName("choice",false);
  ChamMatCmd->AvailableForStates(G4State_PreInit,G4State_Idle);  
  
  FieldCmd = new G4UIcmdWithADoubleAndUnit("/N02/det/setField",this);  
  FieldCmd->SetGuidance("Define magnetic field.");
  FieldCmd->SetGuidance("Magnetic field will be in X direction.");
  FieldCmd->SetParameterName("Bx",false);
  FieldCmd->SetUnitCategory("Magnetic flux density");
  FieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
        
  StepMaxCmd = new G4UIcmdWithADoubleAndUnit("/N02/det/stepMax",this);  
  StepMaxCmd->SetGuidance("Define a step max");
  StepMaxCmd->SetParameterName("stepMax",false);
  StepMaxCmd->SetUnitCategory("Length");
  StepMaxCmd->AvailableForStates(G4State_Idle);
  */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
  //delete TargMatCmd;
  //delete ChamMatCmd;
  //delete FieldCmd;
  //delete StepMaxCmd;  
  delete fDetectorDir;
  delete fEnableSubDetCmd;
  delete fDisableSubDetCmd;
  delete fEnableStructCmd;
  delete fDisableStructCmd;
  //delete N02Dir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  if( command == fEnableSubDetCmd )  fDetector->EnableSubDetector(newValue);
  if( command == fDisableSubDetCmd ) fDetector->DisableSubDetector(newValue);

  if( command == fEnableStructCmd )  fDetector->EnableStructure(newValue);
  if( command == fDisableStructCmd ) fDetector->DisableStructure(newValue);

  //if( command == TargMatCmd )
  // { myDetector->setTargetMaterial(newValue);}
   
  //  if( command == ChamMatCmd )
  //   { myDetector->setChamberMaterial(newValue);}  
  
  //  if( command == FieldCmd )
  //   { myDetector->SetMagField(FieldCmd->GetNewDoubleValue(newValue));}
      
  //if( command == StepMaxCmd )
  // { myDetector->SetMaxStep(StepMaxCmd->GetNewDoubleValue(newValue));}   
 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
