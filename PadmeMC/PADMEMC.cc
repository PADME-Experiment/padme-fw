#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "SteppingVerbose.hh"
#include "HistoManager.hh"
#include "StackingAction.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "DatacardManager.hh"
#include "Constants.hh"

#include "PADME_PHYS.hh"


//#include "MyEvent.hh"

#ifdef  G4MULTITHREADED
#include "G4MTHepRandom.hh"
#else
#endif

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif
#include <signal.h>

double NNeutrons;
double Npionc;
double Npi0;
double Nmuons;

double NNeutrons_aft;
double Npionc_aft;
double Npi0_aft;
double Nmuons_aft;

void sighandler(int sig){
    G4cerr << G4endl << "********************************************************************************" << G4endl;
    G4cerr << "Killed with Signal " << sig << G4endl << "Closing ROOT files ..." << G4endl; 
    G4RunManager::GetRunManager()->AbortRun();
    //    histo->save();
    //    RootIOManager::GetInstance()->Close();
    G4cerr << "... Done" << G4endl;
    G4cerr << G4endl << "********************************************************************************" << G4endl;
    //  G4GeometryManager::GetInstance()->OpenGeometry();

#ifdef G4VIS_USE
    delete G4VVisManager::GetConcreteInstance();
#endif
    delete G4RunManager::GetRunManager();
    exit(0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  //  int SeedNum = -1;
  //  if (argc == 3) SeedNum = atoi(argv[2]);
  
  //choose the Random engine
  //  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  //G4MTHepRandom::setTheEngine(new CLHEP::Ranlux64Engine);  
#ifdef  G4MULTITHREADED
  G4MTHepRandom::setTheEngine(new CLHEP::RanecuEngine);
#else 
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
#endif

  // User Verbose output class
  G4VSteppingVerbose* verbosity = new SteppingVerbose;
  G4VSteppingVerbose::SetInstance(verbosity);

  //  G4String fileNameOut= argv[2];
  //  G4cout<<"Output File Name "<<fileNameOut<<G4endl;

  //Get the event storage. Should be instantiated before the HISTO!
  //MyEvent *TheEvent = MyEvent::GetInstance(); //

  // set an HistoManager for Root IO
  HistoManager*  histo = new HistoManager();

  // Run manager
  G4RunManager * runManager = new G4RunManager;
  
#ifdef G4VIS_USE
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
#endif    

  // Set inputs from datacard (macro file)
  DatacardManager::GetInstance()->SetMessenger();

  // User Initialization classes (mandatory)
  //
  DetectorConstruction* detector = new DetectorConstruction;
  runManager->SetUserInitialization(detector);

  if ( UseNewPhysList == 1) {
    // Physics list based on "official" physics models
    // Mainly QGSP_BERT or QGSP_BIC + Optical
    G4VModularPhysicsList* physicsList = new PADME_PHYS;
    runManager->SetUserInitialization(physicsList);
  } else {
    //Custom made user physics list, old style
    G4VUserPhysicsList* physics = new PhysicsList;
    runManager->SetUserInitialization(physics);
  }
  


  // User Action classes
  G4cout << "MAIN: Constructing the generator action" << G4endl;
  G4VUserPrimaryGeneratorAction* gen_action = new PrimaryGeneratorAction(detector);
  runManager->SetUserAction(gen_action);
  //
  //  G4UserRunAction* run_action = new ExN02RunAction(histo);
  RunAction* run_action = new RunAction(histo);
  runManager->SetUserAction(run_action); 
  //
  EventAction* myevent_action = new EventAction(run_action,histo);
  G4UserEventAction* event_action  = myevent_action;
  //  ExN02EventAction* event_action = new ExN02EventAction(run_action,histo);
  runManager->SetUserAction(event_action);
  //
  SteppingAction* mystepping_action = new SteppingAction;
  G4UserSteppingAction* stepping_action = mystepping_action ;
  runManager->SetUserAction(stepping_action);
  myevent_action->myStepping = mystepping_action; //setting the pointer to stepping action into Event action

  //Invoca la classe di Stacking Action
  runManager->SetUserAction(new StackingAction);

  // Initialize G4 kernel
  //
  // POstponed to macro level with command /run/initialize
  //runManager->Initialize();
  //  physics->DumpList();

 // trap signals to close datafiles in case of abnormal termination
//  void (*prev_fn)(int);
//  signal(SIGUSR1,sighandler);
//  signal(SIGXCPU,sighandler);
//  signal(SIGINT,sighandler);
//  signal(SIGTERM,sighandler);
//  signal(127,sighandler);

//  if (prev_fn==SIGTERM) run_action::EndOfRunAction();
      
  // Get the pointer to the User Interface manager
  //
  G4UImanager * UImanager = G4UImanager::GetUIpointer();  
  
  if (argc!=1)   // batch mode  
    {
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UImanager->ApplyCommand(command+fileName);
      //      UImanager->ApplyCommand(command+fileName+fileNameOut);
    }
  else           // interactive mode : define UI session
    { 
#ifdef G4UI_USE
      G4UIExecutive * ui = new G4UIExecutive(argc,argv);
#ifdef G4VIS_USE
      UImanager->ApplyCommand("/control/execute vis.mac");     
#endif
      ui->SessionStart();
      delete ui;
#endif
     
#ifdef G4VIS_USE
      delete visManager;
#endif     
    }

  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !
  delete runManager;
  delete verbosity;
  delete histo;
  //  G4cout<<"n= "<<NNeutrons<<" pic "<<Npionc<<" pi0 "<<Npi0<<" mu "<<Nmuons<<G4endl;
  //  G4cout<<"n_aft= "<<NNeutrons_aft<<" pic "<<Npionc_aft<<" pi0 "<<Npi0_aft<<" mu "<<Nmuons_aft<<G4endl;
  //delete TheEvent;
  return 0;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
