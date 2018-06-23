#include "DatacardMessenger.hh"
#include "DatacardManager.hh"

#include "G4RunManager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"

#include "RootIOManager.hh"
#include "SteppingAction.hh"
#include "EventAction.hh" //M. Raggi

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

  //M. Raggi addded flags for Nt save options.
  fEnableSaveEcalCmd = new G4UIcmdWithABool("/analysis/SaveEcal",this);
  fEnableSaveEcalCmd->SetGuidance("Enable (true) or disable (false) save based on ECal Energy.");
  fEnableSaveEcalCmd->SetParameterName("SEC",true);
  fEnableSaveEcalCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  //M. Raggi addded flags for Nt save options.
  fEnableSaveVetoCmd = new G4UIcmdWithABool("/analysis/SaveVeto",this);
  fEnableSaveVetoCmd->SetGuidance("Enable (true) or disable (false) save based on Veto hits.");
  fEnableSaveVetoCmd->SetParameterName("SVE",true);
  fEnableSaveVetoCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  //M. Raggi addded flags for Nt save options.
  fEnableSaveSACCmd = new G4UIcmdWithABool("/analysis/SaveSAC",this);
  fEnableSaveSACCmd->SetGuidance("Enable (true) or disable (false) save based on SAC Energy.");
  fEnableSaveSACCmd->SetParameterName("SSA",true);
  fEnableSaveSACCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

DatacardMessenger::~DatacardMessenger() {

    delete fOutNameCmd;
    delete fHistoNameCmd;

    delete fEnableDetectorIOCmd;
    delete fDisableDetectorIOCmd;

    delete fDisableSACAnalysisCmd;
    delete fDisableECalAnalysisCmd;

    delete fEnableSaveSACCmd;
    delete fEnableSaveVetoCmd;
    delete fEnableSaveEcalCmd;
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


    //M. Raggi 23/06/2018
    if (command == fEnableSaveEcalCmd) {
      EventAction* evtAct = (EventAction*)G4RunManager::GetRunManager()->GetUserEventAction();
      if (fEnableSaveEcalCmd->GetNewBoolValue(newValue)) {
	evtAct->EnableSaveEcal();
      } else {
	evtAct->DisableSaveEcal();
      }
    }

    if (command == fEnableSaveSACCmd) {
      EventAction* evtAct = (EventAction*)G4RunManager::GetRunManager()->GetUserEventAction();
      if (fEnableSaveSACCmd->GetNewBoolValue(newValue)) {
	evtAct->EnableSaveSAC();
      } else {
	evtAct->DisableSaveSAC();
      }
    }

    if (command == fEnableSaveVetoCmd) {
      EventAction* evtAct = (EventAction*)G4RunManager::GetRunManager()->GetUserEventAction();
      if (fEnableSaveVetoCmd->GetNewBoolValue(newValue)) {
	//	G4cout<<"Enable "<<G4endl;
	evtAct->EnableSaveVeto();
      } else {
	evtAct->DisableSaveVeto();
	//	G4cout<<"Disable "<<G4endl;
      }
    }

}
