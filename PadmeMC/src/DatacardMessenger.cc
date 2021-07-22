#include "DatacardMessenger.hh"
#include "DatacardManager.hh"

#include "G4RunManager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

#include "RootIOManager.hh"
#include "SteppingAction.hh"
#include "StackingAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "MCTruthManager.hh"

DatacardMessenger::DatacardMessenger(DatacardManager* datacardMng):fDatacardManager(datacardMng)
{

  fOutputDir = new G4UIdirectory("/output/");
  fOutputDir->SetGuidance("UI commands to manage data output.");

  fAnalysisDir = new G4UIdirectory("/analysis/");
  fAnalysisDir->SetGuidance("UI commands to manage MC-based analysis.");

  fSettingsDir = new G4UIdirectory("/settings/");
  fSettingsDir->SetGuidance("UI commands to manage general behaviour of program.");

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

  fPrintOutFrequencyCmd = new G4UIcmdWithAnInteger("/settings/PrintOutFrequency",this);
  fPrintOutFrequencyCmd->SetGuidance("Set after how many events some info printout is shown");
  fPrintOutFrequencyCmd->SetParameterName("POF",false);
  fPrintOutFrequencyCmd->SetRange("POF > 0");
  fPrintOutFrequencyCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fAutomaticRandomSeedCmd = new G4UIcmdWithABool("/settings/AutomaticRandomSeed",this);
  fAutomaticRandomSeedCmd->SetGuidance("Enable (true) or disable (false) automatic generation of random seeds");
  fAutomaticRandomSeedCmd->SetParameterName("ARS",true);
  fAutomaticRandomSeedCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTurboModeCmd = new G4UIcmdWithABool("/settings/TurboModeEnable",this);
  fTurboModeCmd->SetGuidance("Enable (true) or disable (false) turbo mode in StackingAction");
  fTurboModeCmd->SetParameterName("TM",true);
  fTurboModeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTurboModeDir = new G4UIdirectory("/settings/TurboMode/");
  fTurboModeDir->SetGuidance("Commands to define TurboMode behaviour.");

  fTurboModeVerboseCmd = new G4UIcmdWithAnInteger("/settings/TurboMode/Verbose",this);
  fTurboModeVerboseCmd->SetGuidance("Define TurboMode verbose level.");
  fTurboModeVerboseCmd->SetParameterName("TMV",false);
  fTurboModeVerboseCmd->SetRange("TMV >= 0");
  fTurboModeVerboseCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTurboPositronKillEnergyCmd = new G4UIcmdWithADoubleAndUnit("/settings/TurboMode/KillEnergyPositron",this);
  fTurboPositronKillEnergyCmd->SetGuidance("Set energy threshold to kill positrons in turbo mode");
  fTurboPositronKillEnergyCmd->SetParameterName("TPKE",false);
  fTurboPositronKillEnergyCmd->SetDefaultUnit("MeV");
  fTurboPositronKillEnergyCmd->SetRange("TPKE >= 0.");
  fTurboPositronKillEnergyCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTurboElectronKillEnergyCmd = new G4UIcmdWithADoubleAndUnit("/settings/TurboMode/KillEnergyElectron",this);
  fTurboElectronKillEnergyCmd->SetGuidance("Set energy threshold to kill electrons in turbo mode");
  fTurboElectronKillEnergyCmd->SetParameterName("TEKE",false);
  fTurboElectronKillEnergyCmd->SetDefaultUnit("MeV");
  fTurboElectronKillEnergyCmd->SetRange("TEKE >= 0.");
  fTurboElectronKillEnergyCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTurboGammaKillEnergyCmd = new G4UIcmdWithADoubleAndUnit("/settings/TurboMode/KillEnergyGamma",this);
  fTurboGammaKillEnergyCmd->SetGuidance("Set energy threshold to kill gammas in turbo mode");
  fTurboGammaKillEnergyCmd->SetParameterName("TGKE",false);
  fTurboGammaKillEnergyCmd->SetDefaultUnit("MeV");
  fTurboGammaKillEnergyCmd->SetRange("TGKE >= 0.");
  fTurboGammaKillEnergyCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTurboNeutronKillEnergyCmd = new G4UIcmdWithADoubleAndUnit("/settings/TurboMode/KillEnergyNeutron",this);
  fTurboNeutronKillEnergyCmd->SetGuidance("Set energy threshold to kill neutrons in turbo mode");
  fTurboNeutronKillEnergyCmd->SetParameterName("TNKE",false);
  fTurboNeutronKillEnergyCmd->SetDefaultUnit("MeV");
  fTurboNeutronKillEnergyCmd->SetRange("TNKE >= 0.");
  fTurboNeutronKillEnergyCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fEnableMCTruthCmd = new G4UIcmdWithABool("/settings/EnableMCTruth",this);
  fEnableMCTruthCmd->SetGuidance("Enable (true) or disable (false) MCTruth generation");
  fEnableMCTruthCmd->SetParameterName("EMCT",true);
  fEnableMCTruthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

DatacardMessenger::~DatacardMessenger() {

  delete fOutNameCmd;
  delete fHistoNameCmd;
  delete fEnableDetectorIOCmd;
  delete fDisableDetectorIOCmd;
  delete fOutputDir;

  delete fDisableSACAnalysisCmd;
  delete fDisableECalAnalysisCmd;
  delete fEnableSaveSACCmd;
  delete fEnableSaveVetoCmd;
  delete fEnableSaveEcalCmd;
  delete fAnalysisDir;

  delete fTurboModeVerboseCmd;
  delete fTurboPositronKillEnergyCmd;
  delete fTurboElectronKillEnergyCmd;
  delete fTurboGammaKillEnergyCmd;
  delete fTurboNeutronKillEnergyCmd;
  delete fTurboModeDir;

  delete fTurboModeCmd;
  delete fPrintOutFrequencyCmd;
  delete fAutomaticRandomSeedCmd;
  delete fSettingsDir;

  delete fEnableMCTruthCmd;
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

  if (command == fPrintOutFrequencyCmd) {
    EventAction* evtAct = (EventAction*)G4RunManager::GetRunManager()->GetUserEventAction();
    evtAct->SetPrintoutfrequency(fPrintOutFrequencyCmd->GetNewIntValue(newValue));
  }

  if (command == fAutomaticRandomSeedCmd) {
    RunAction* runAct = (RunAction*)G4RunManager::GetRunManager()->GetUserRunAction();
    if (fEnableSaveVetoCmd->GetNewBoolValue(newValue)) {
      runAct->EnableAutomaticRandomSeed();
    } else {
      runAct->DisableAutomaticRandomSeed();
    }
  }

  if (command == fTurboModeCmd) {
    StackingAction* stkAct = (StackingAction*)G4RunManager::GetRunManager()->GetUserStackingAction();
    if (fTurboModeCmd->GetNewBoolValue(newValue)) {
      stkAct->EnableTurboMode();
    } else {
      stkAct->DisableTurboMode();
    }
  }

  if (command == fTurboModeVerboseCmd)
    ((StackingAction*)G4RunManager::GetRunManager()->GetUserStackingAction())->SetTurboModeVerboseLevel(fTurboModeVerboseCmd->GetNewIntValue(newValue));

  if ( command == fTurboPositronKillEnergyCmd )
    ((StackingAction*)G4RunManager::GetRunManager()->GetUserStackingAction())->SetEpKillEnergy(fTurboPositronKillEnergyCmd->GetNewDoubleValue(newValue));

  if ( command == fTurboElectronKillEnergyCmd )
    ((StackingAction*)G4RunManager::GetRunManager()->GetUserStackingAction())->SetEmKillEnergy(fTurboElectronKillEnergyCmd->GetNewDoubleValue(newValue));

  if ( command == fTurboGammaKillEnergyCmd )
    ((StackingAction*)G4RunManager::GetRunManager()->GetUserStackingAction())->SetGKillEnergy(fTurboGammaKillEnergyCmd->GetNewDoubleValue(newValue));

  if ( command == fTurboNeutronKillEnergyCmd )
    ((StackingAction*)G4RunManager::GetRunManager()->GetUserStackingAction())->SetNKillEnergy(fTurboNeutronKillEnergyCmd->GetNewDoubleValue(newValue));

  if (command == fEnableMCTruthCmd) {
    MCTruthManager* mctMgr = MCTruthManager::GetInstance();
    if (fEnableMCTruthCmd->GetNewBoolValue(newValue)) {
      mctMgr->Enable();
    } else {
      mctMgr->Disable();
    }
  }

}
