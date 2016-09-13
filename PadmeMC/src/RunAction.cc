#include "RunAction.hh"
#include "Run.hh"
#include "G4Run.hh"

#include "G4RegionStore.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"

#include "G4VSteppingVerbose.hh"
#include "SteppingVerbose.hh"

#include "RootIOManager.hh"
#include "HistoManager.hh"
#include "Constants.hh"

#ifdef  G4MULTITHREADED
#include "G4MTHepRandom.hh"
#else
#endif



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4Run* RunAction::GenerateRun()
{ return new Run; }

//RunAction::RunAction(HistoManager* histo)
//:fHistoManager(histo)
//{}

RunAction::RunAction()
{
  fHistoManager = HistoManager::GetInstance();
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

  // automatic (time-based) random seeds for each run
  if ( fAutoSeed ) {
    //    aRun->SetRandomNumberStore(true);
    //    aRun->SetRandomNumberStoreDir("random/");
    
    G4cout << "*******************" << G4endl;
    G4cout << "*** AUTOSEED ON ***" << G4endl;
    G4cout << "*******************" << G4endl;
    
    long seeds[2];
    time_t systime = time(NULL);
    seeds[0] = (long) systime;
    seeds[1] = (long) (systime*G4UniformRand());

#ifdef  G4MULTITHREADED
    G4MTHepRandom::setTheSeeds(seeds);
    G4MTHepRandom::showEngineStatus();
#else
    CLHEP::HepRandom::setTheSeeds(seeds);
    CLHEP::HepRandom::showEngineStatus();
#endif
  } else {
    G4cout << "*******************" << G4endl;
    G4cout << "*** AUTOSEED OFF***" << G4endl;
    G4cout << "*******************" << G4endl;

    long seeds[2];

    seeds[0] = atol(getenv("PADME_SEED1"));
    seeds[1] = atol(getenv("PADME_SEED2"));

    G4cout << "SEEDS:  " << seeds[0] << "\t" << seeds[1] << G4endl;


#ifdef  G4MULTITHREADED
    G4MTHepRandom::setTheSeeds(seeds);
    G4MTHepRandom::showEngineStatus();
#else
    CLHEP::HepRandom::setTheSeeds(seeds);
    CLHEP::HepRandom::showEngineStatus();
#endif


  }

  if(IsTurboOn){
	  G4cout << "*******************" << G4endl;
	  G4cout << "** TURBO MODE ON **" << G4endl;
	  G4cout << "*******************" << G4endl;
  }


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* aRun)
{
  const Run* theRun = (const Run*)aRun;
  G4double nEvt   = (G4double)(theRun->GetNumberOfEvent());
  G4double nGamma = theRun->GetNGamma(0);
  G4cout<<"Nevent Run Action "<<nEvt<<" "<<nGamma<<G4endl;
  RootIOManager::GetInstance()->EndRun();
  //save histograms
  //
  fHistoManager->PrintStatistic();
  fHistoManager->save();   
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

