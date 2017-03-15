#include "DatacardMessenger.hh"
#include "DatacardManager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
//#include "G4UIcmdWithADouble.hh"
//#include "G4UIcmdWithAnInteger.hh"
//#include "G4UIcmdWith3Vector.hh"
#include "RootIOManager.hh"

DatacardMessenger::DatacardMessenger(DatacardManager* datacardMng):fDatacardManager(datacardMng)
{

  fOutputDir = new G4UIdirectory("/output/");
  fOutputDir->SetGuidance("UI commands to manage data output.");

  fOutNameCmd = new G4UIcmdWithAString("/output/DataFileName",this);
  fOutNameCmd->SetGuidance("Define name to use for hits/digis output file.");
  fOutNameCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fHistoNameCmd = new G4UIcmdWithAString("/output/HistoFileName",this);
  fHistoNameCmd->SetGuidance("Define name to use for histos/ntuples output file.");
  fHistoNameCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEnableDetectorIOCmd = new G4UIcmdWithAString("/output/EnableDetectorIO",this);
  fEnableDetectorIOCmd->SetGuidance("Enable hits/digis output for a specific detector.");
  fEnableDetectorIOCmd->SetGuidance("Argument must be in the form <DetectorName>[:HD].");
  fEnableDetectorIOCmd->SetGuidance("If [:HD] is not specified, both hits and digis are written.");
  fEnableDetectorIOCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fDisableDetectorIOCmd = new G4UIcmdWithAString("/output/DisableDetectorIO",this);
  fDisableDetectorIOCmd->SetGuidance("Disable hits/digis output for a specific detector.");
  fDisableDetectorIOCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

DatacardMessenger::~DatacardMessenger() {
    delete fOutNameCmd;
    delete fHistoNameCmd;
    delete fEnableDetectorIOCmd;
    delete fDisableDetectorIOCmd;
}

void DatacardMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {

    G4cout << command->GetCommandName() << " " << newValue << G4endl;

    if (command == fOutNameCmd)   fDatacardManager->SetOutputFileName(newValue);
    if (command == fHistoNameCmd) fDatacardManager->SetHistoFileName(newValue);
    if (command == fEnableDetectorIOCmd) RootIOManager::GetInstance()->EnableSubDetectorIO(newValue);
    if (command == fDisableDetectorIOCmd) RootIOManager::GetInstance()->DisableSubDetectorIO(newValue);

}
