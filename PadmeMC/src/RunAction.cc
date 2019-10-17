#include "RunAction.hh"
//#include "Run.hh"
#include "G4Run.hh"

#include "G4RegionStore.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"

#include "G4VSteppingVerbose.hh"
#include "SteppingVerbose.hh"

#include "RootIOManager.hh"
#include "HistoManager.hh"
#include "Constants.hh"

#include "BeamParameters.hh"

#ifdef  G4MULTITHREADED
#include "G4MTHepRandom.hh"
#else
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//G4Run* RunAction::GenerateRun()
//{ return new Run; }

//RunAction::RunAction(HistoManager* histo)
//:fHistoManager(histo)
//{}

RunAction::RunAction()
{

  fHistoManager = HistoManager::GetInstance();

  fAutomaticRandomSeed = true;

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* aRun)
{

  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
  RootIOManager::GetInstance()->NewRun(aRun->GetRunID());
  fHistoManager->book();

  long seeds[2];
  seeds[0] = 0;
  seeds[1] = 0;
  time_t systime = time(NULL);

  //if ( fAutoSeed ) {
  if ( fAutomaticRandomSeed ) {

    // Automatic (time-based) random seeds for each run

    //    aRun->SetRandomNumberStore(true);
    //    aRun->SetRandomNumberStoreDir("random/");
    
    G4cout << "*******************" << G4endl;
    G4cout << "*** AUTOSEED ON ***" << G4endl;
    G4cout << "*******************" << G4endl;
    
    seeds[0] = (long) systime;
    seeds[1] = (long) (systime*G4UniformRand());

  } else {

    // Use PADME_SEED1 and PADME_SEED2 environment variables to set random number generator seeds

    G4cout << "*******************" << G4endl;
    G4cout << "*** AUTOSEED OFF***" << G4endl;
    G4cout << "*******************" << G4endl;

    char* ps1 = getenv("PADME_SEED1");
    if (ps1 != NULL) {
      seeds[0] = (long) atol(ps1);
    } else {
      G4cout << "WARNING: unable to extract random seed from PADME_SEED1: using systime" << G4endl;
      seeds[0] = (long) systime;
    }

    char* ps2 = getenv("PADME_SEED2");
    if (ps2 != NULL) {
      seeds[1] = (long) atol(ps2);
    } else {
      G4cout << "WARNING: unable to extract random seed from PADME_SEED2: using systime" << G4endl;
      seeds[1] = (long) (systime*G4UniformRand());
    }

  }

  G4cout << "PadmeMCInfo - RANDOM SEEDS: " << seeds[0] << "\t" << seeds[1] << G4endl;

#ifdef  G4MULTITHREADED
  G4MTHepRandom::setTheSeeds(seeds);
  G4MTHepRandom::showEngineStatus();
#else
  CLHEP::HepRandom::setTheSeeds(seeds);
  CLHEP::HepRandom::showEngineStatus();
#endif

  if(IsTurboOn){
    G4cout << "*******************" << G4endl;
    G4cout << "** TURBO MODE ON **" << G4endl;
    G4cout << "*******************" << G4endl;
  }

  if ( ((G4VisManager*)G4VisManager::GetConcreteInstance())->GetCurrentScene() ) {
    // Show some info about current run on display
    char cmd[100];
    BeamParameters* bpar = BeamParameters::GetInstance();
    if (bpar->CalibrationRun()) {
      sprintf(cmd,"/vis/scene/add/text2D -0.95 -0.95 20 ! ! gamma @ %.0f MeV",bpar->GetCalibRunEnergy()/MeV);
    } else {
      sprintf(cmd,"/vis/scene/add/text2D -0.95 -0.95 20 ! ! %d e+ @ %.0f MeV",bpar->GetNPositronsPerBunch(),bpar->GetBeamMomentum()/MeV);
    }
    G4UImanager* UI = G4UImanager::GetUIpointer();
    UI->ApplyCommand("/vis/set/textColour cyan");
    UI->ApplyCommand(cmd);
    UI->ApplyCommand("/vis/set/textColour");
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* aRun)
{

  //const Run* theRun = (const Run*)aRun;
  //G4double nEvt   = (G4double)(theRun->GetNumberOfEvent());
  //G4double nGamma = theRun->GetNGamma(0);
  //G4cout<<"Nevent Run Action "<<nEvt<<" "<<nGamma<<G4endl;

  RootIOManager::GetInstance()->EndRun();

  // Print some summary info
  G4cout << "PadmeMCInfo - Run Number " << aRun->GetRunID() << G4endl;
  G4cout << "PadmeMCInfo - Total Events " << aRun->GetNumberOfEvent() << G4endl;

  //save histograms
  //
  fHistoManager->PrintStatistic();
  fHistoManager->save();   
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

