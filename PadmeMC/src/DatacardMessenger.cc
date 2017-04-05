#include "DatacardMessenger.hh"
#include "DatacardManager.hh"

#include "G4RunManager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"

#include "RootIOManager.hh"
#include "SteppingAction.hh"

DatacardMessenger::DatacardMessenger(DatacardManager* datacardMng):fDatacardManager(datacardMng)
{

  fOutputDir = new G4UIdirectory("/output/");
  fOutputDir->SetGuidance("UI commands to manage data output.");

  fOutputDir = new G4UIdirectory("/analysis/");
  fOutputDir->SetGuidance("UI commands to manage MC-based analysis.");

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

  fDisableSACAnalysisCmd = new G4UIcmdWithABool("/analysis/DisableSACAnalysis",this);
  fDisableSACAnalysisCmd->SetGuidance("If -true- disable analysis of SAC inside MC.");
  fDisableSACAnalysisCmd->SetGuidance("WARNING! If -false-, SAC will not be simulated and no SAC hits/digis will be produced.");
  fDisableSACAnalysisCmd->SetParameterName("DSA",true);
  fDisableSACAnalysisCmd->SetDefaultValue(true);
  fDisableSACAnalysisCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fDisableECalAnalysisCmd = new G4UIcmdWithABool("/analysis/DisableECalAnalysis",this);
  fDisableECalAnalysisCmd->SetGuidance("If -true- disable analysis of ECal inside MC.");
  fDisableECalAnalysisCmd->SetParameterName("DEA",true);
  fDisableECalAnalysisCmd->SetDefaultValue(true);
  fDisableECalAnalysisCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

DatacardMessenger::~DatacardMessenger() {

    delete fOutNameCmd;
    delete fHistoNameCmd;

    delete fEnableDetectorIOCmd;
    delete fDisableDetectorIOCmd;

    delete fDisableSACAnalysisCmd;
    delete fDisableECalAnalysisCmd;
}

void DatacardMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {

    G4cout << command->GetCommandName() << " " << newValue << G4endl;

    if (command == fOutNameCmd)   fDatacardManager->SetOutputFileName(newValue);
    if (command == fHistoNameCmd) fDatacardManager->SetHistoFileName(newValue);

    if (command == fEnableDetectorIOCmd) RootIOManager::GetInstance()->EnableSubDetectorIO(newValue);
    if (command == fDisableDetectorIOCmd) RootIOManager::GetInstance()->DisableSubDetectorIO(newValue);

    if (command == fDisableSACAnalysisCmd) {
      SteppingAction* stepAct = (SteppingAction*)G4RunManager::GetRunManager()->GetUserSteppingAction();
      if (fDisableSACAnalysisCmd->GetNewBoolValue(newValue)) {
	stepAct->DisableSACAnalysis();
      } else {
	stepAct->EnableSACAnalysis();
      }
    }

    if (command == fDisableECalAnalysisCmd) {
      SteppingAction* stepAct = (SteppingAction*)G4RunManager::GetRunManager()->GetUserSteppingAction();
      if (fDisableECalAnalysisCmd->GetNewBoolValue(newValue)) {
	stepAct->DisableECalAnalysis();
      } else {
	stepAct->EnableECalAnalysis();
      }
    }

}
