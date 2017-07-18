// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------

#include <sstream>
//#include <ctime>
#include <sys/time.h>

#include "RootIOManager.hh"

#include "ECalRootIO.hh"
#include "TargetRootIO.hh"
#include "PVetoRootIO.hh"
#include "EVetoRootIO.hh"
#include "HEPVetoRootIO.hh"
#include "SACRootIO.hh"
#include "TPixRootIO.hh"
//#include "LAVRootIO.hh"
//#include "MagnetRootIO.hh"
//#include "TDumpRootIO.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4Run.hh"

#include "TPadmeRun.hh"
#include "TMCEvent.hh"
#include "TDetectorInfo.hh"

#include "DetectorConstruction.hh"

#include "TString.h"
#include "TProcessID.h"

//#include "TargetGeometry.hh"
#include "BeamParameters.hh"
#include "DatacardManager.hh"

RootIOManager* RootIOManager::fInstance = 0;

RootIOManager::RootIOManager()
{

  // Create run and event objects
  fRun   = new TPadmeRun();
  fEvent = new TMCEvent();

  // Default output file parameters
  fBufSize = 64000; //size of output buffer
  fBranchStyle = 1; //new branch style by default
  fCompLevel = 1;   //fast compression by default

  // Default fVerbose level
  fVerbose = 0;

  // Default file name
  fFileName = DatacardManager::GetInstance()->GetOutputFileName(); 
  fFileNameHasChanged = true;

  // Make sure initial file pointer is empty (mandatory!)
  fFile = 0;

  TTree::SetBranchStyle(fBranchStyle);

  G4cout << "RootIOManager: Initialized" << G4endl;

  // Add subdetectors persistency managers
  fRootIOList.push_back(new ECalRootIO);
  fRootIOList.push_back(new TargetRootIO);
  fRootIOList.push_back(new PVetoRootIO);
  fRootIOList.push_back(new EVetoRootIO);
  fRootIOList.push_back(new HEPVetoRootIO);
  fRootIOList.push_back(new SACRootIO);
  fRootIOList.push_back(new TPixRootIO);
  //fRootIOList.push_back(new LAVRootIO);
  //fRootIOList.push_back(new MagnetRootIO);
  //fRootIOList.push_back(new TDumpRootIO);
  // Tell all RootIO handlers about new run

  // Pass some useful information to subdetectors
  //((SACRootIO*)FindRootIO("SAC"))->SetBeamStartZ(TargetGeometry::GetInstance()->GetTargetFrontFacePosZ());

  //fGVirtMem = new TGraph();
  //fGVirtMem->SetName("VirtualMemory");
  //fGVirtMem->SetTitle("Virtual Memory Usage");
}

RootIOManager::~RootIOManager()
{;}

RootIOManager* RootIOManager::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new RootIOManager(); }
  return fInstance;
}

void RootIOManager::Close()
{

  // Save latest data, clean run tree, and close file
  if ( fFile != 0 ) {
    fFile->cd();
    fRunTree->Write();
    if (fVerbose) fRunTree->Print();
    delete fRunTree;
    //fGVirtMem->GetYaxis()->SetTitle("Virtual Memory (MB)");
    //fGVirtMem->GetXaxis()->SetTitle("Processed Events");
    //fGVirtMem->Write();
    //delete fGVirtMem;
    fFile->Purge();
    fFile->Close();
    //if (fVerbose)
      G4cout << "RootIOManager: I/O file closed" << G4endl;
  }

}

void RootIOManager::EnableSubDetectorIO(G4String para)
{

  G4String detector,opts;
  G4bool saveHits = true;
  G4bool saveDigis = true;
  if (para.contains(":")) { // If the [:HD] option is used, parse it
    G4int idx = para.index(":");
    detector = para(0,idx);
    opts = para(idx+1,para.length()-idx-1);
    if (! opts.contains("H")) saveHits = false;
    if (! opts.contains("D")) saveDigis = false;
  } else {
    detector = para;
  }

  MCVRootIO* drio = FindRootIO(detector);
  if (drio) {
    printf("Enabling IO for subdetector %s:",detector.data());
    drio->SetEnabled(true);
    if (saveHits) { printf(" hits enabled,"); } else { printf(" hits disabled,"); }
    drio->SetHitsEnabled(saveHits);
    if (saveDigis) { printf(" digis enabled\n"); } else { printf(" digis disabled\n"); }
    drio->SetDigisEnabled(saveDigis);
  } else {
    printf("WARNING: request to enable IO for unknown subdetector %s\n",detector.data());
  }

}

void RootIOManager::DisableSubDetectorIO(G4String det)
{
  MCVRootIO* drio = FindRootIO(det);
  if (drio) {
    printf("Disabling IO for subdetector %s\n",det.data());
    drio->SetEnabled(false);
  } else {
    printf("WARNING: request to disable IO for unknown subdetector %s\n",det.data());
  }
}

void RootIOManager::SetFileName(G4String newName)
{
  //if (fVerbose)
    G4cout << "RootIOManager: Setting file name to " << newName << G4endl;
  fFileName = newName;
  fFileNameHasChanged = true;
}

void RootIOManager::NewRun(G4int nRun)
{

  //if (fVerbose)
    G4cout << "RootIOManager: Initializing I/O for run " << nRun << G4endl;

  if ( fFileNameHasChanged ) {

    // Close old file (if any)
    if ( fFile != 0 ) {
      if (fVerbose)
	G4cout << "RootIOManager: Closing old file" << G4endl;
      Close();
    }

    // Create new file to hold data
    //if (fVerbose)
      G4cout << "RootIOManager: Creating new file " << fFileName << G4endl;
    fFile = TFile::Open(fFileName.c_str(),"RECREATE","PadmeMC");
    fFileNameHasChanged = false;

    // Define basic file properties
    if (fVerbose>=2)
      G4cout << "RootIOManager: Setting file compression level to "
	     << fCompLevel << G4endl;
    fFile->SetCompressionLevel(fCompLevel);

    // Create tree to hold runs
    //if (fVerbose>=2)
      G4cout << "RootIOManager: Creating new Run tree" << G4endl;
    fRunTree = new TTree("Runs","List of runs");
    //fRunTree->SetAutoSave(1000000000);  // autosave when ~1 Gbyte written
    fRunTree->SetDirectory(fFile->GetDirectory("/"));

    // Create branch to hold the run content info
    fRunBranch = fRunTree->Branch("Run", &fRun, fBufSize);
    fRunBranch->SetAutoDelete(kFALSE);

  }

  // Create tree to hold all events in this run
  G4cout << "RootIOManager: Creating new MC Event tree" << G4endl;
  fEventTree = new TTree("MC","MC events tree");
  //fEventTree->SetAutoSave(1000000000);  // autosave when ~1 Gbyte written
  fEventTree->SetAutoSave(1000000);  // autosave when ~1 Mbyte written
  TTree::SetMaxTreeSize(19000000000);
  fEventTree->SetDirectory(fFile->GetDirectory("/"));

  // Create Stream object to hold run general information...
  fRun->Clear();
  fRun->SetRunNumber(G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID());
  //fRun->SetType(DatacardManager::GetInstance()->GetDecayType());
  fRun->SetRunType(-1); // TYpe<0 is MC, >0 is real data
  fRun->SetTimeStart(time(0)); // Get start of run time
  fRun->SetNEvents(0); // Reset number of events

  // Fill detector info section of run structure
  TDetectorInfo* detInfo = fRun->GetDetectorInfo();

  // Set some geometry dependent parameters in detectors' RootIO handlers
  //G4cout << "RootIOManager: Beam parameters " << BeamParameters::GetInstance()->GetBeamOriginPosZ()
  //	 << " " << BeamParameters::GetInstance()->GetBunchTimeLength() << G4endl;
  //((SACRootIO*)FindRootIO("SAC"))->SetBeamStartZ(BeamParameters::GetInstance()->GetBeamOriginPosZ());
  //((SACRootIO*)FindRootIO("SAC"))->SetBeamBunchLengthT(BeamParameters::GetInstance()->GetBunchTimeLength());

  // Tell all RootIO handlers about new run
  RootIOList::iterator iRootIO(fRootIOList.begin());
  RootIOList::iterator endRootIO(fRootIOList.end());
  while (iRootIO!=endRootIO) {
    //G4cout << "RootIOManager: Checking IO for " << (*iRootIO)->GetName() << G4endl;
    if ((*iRootIO)->GetEnabled()) {
      //G4cout << "RootIOManager: IO for " << (*iRootIO)->GetName() << " enabled" << G4endl;
      (*iRootIO)->NewRun(nRun,fFile,detInfo);
      //(*iRootIO)->NewRun(nRun,fFile);
    }
    iRootIO++;
  }

  // Fill run tree
  //if (fVerbose>=2) fRun->Print();
  //fRunTree->Fill();

  // Create branch to hold the generated event structure
  fEventBranch = fEventTree->Branch("Event", &fEvent, fBufSize);
  fEventBranch->SetAutoDelete(kFALSE);

}

void RootIOManager::EndRun()
{

  if (fVerbose)
    G4cout << "RootIOManager: Executing End-of-Run procedure" << G4endl;

  fRun->SetTimeStop(time(0)); // Get end of run time
  if (fVerbose>=2) fRun->Print();
  fRunTree->Fill(); // Fill run tree

  // Dump tree for this event to file and erase it
  if(fEventTree) {
    fFile->cd();
    fEventTree->Write();
    if (fVerbose>=2) fEventTree->Print();
    delete fEventTree;
  }

  // Same for each subdetetctor
  RootIOList::iterator iRootIO(fRootIOList.begin());
  RootIOList::iterator endRootIO(fRootIOList.end());
  while (iRootIO!=endRootIO){
    if((*iRootIO)->GetEnabled()) (*iRootIO)->EndRun();
    iRootIO++;
  }

}

void RootIOManager::SaveEvent(const G4Event* eventG4)
{
  if (fVerbose>=2)
    G4cout << "RootIOManager: Preparing event structure" << G4endl;

  // Save current Object count
  Int_t savedObjNumber = TProcessID::GetObjectCount();

  // Increase event counter for this run
  fRun->IncNEvents();

  // Get event id (run,event,date)
  G4int nRun = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();
  G4int nEvent = eventG4->GetEventID();
  //G4int date = time(0);
  struct timeval tp;
  gettimeofday(&tp,NULL);
  G4double now = tp.tv_sec*1.+tp.tv_usec/1000000.;

  if (fVerbose>=2)
    G4cout << "RootIOManager: Event " << nEvent
	   << " Run " << nRun << " at " << now << G4endl;

  // Fill Event info
  fEvent->SetRunNumber(nRun);
  fEvent->SetEventNumber(nEvent);
  fEvent->SetTime(now);

  //Restore Object count
  //To save space in the table keeping track of all referenced objects
  //we assume that our events do not address each other. We reset the
  //object count to what it was at the beginning of the event.
  TProcessID::SetObjectCount(savedObjNumber);

  if (fVerbose>=2)
    G4cout << "RootIOManager: Saving event structure to file" << G4endl;
  if (fVerbose>=8)
    fEvent->Print();
  //if (fVerbose>=9) fEvent->PrintAll();

  RootIOList::iterator iRootIO(fRootIOList.begin());
  RootIOList::iterator endRootIO(fRootIOList.end());
  while (iRootIO!=endRootIO) {
    if ((*iRootIO)->GetEnabled()) (*iRootIO)->SaveEvent(eventG4);
    iRootIO++;
  }

  // All data have been copied: write it to file
  fEventTree->Fill();

  //virtual memory usage monitoring
  //G4int DownscalingFactor = 1; //check virtual memory every N processed events
  //if(!(nEvent%DownscalingFactor)){
  //  fGVirtMem->SetPoint(nEvent/DownscalingFactor, nEvent, GetVirtualMemoryUsage());
  //}

}

//G4double RootIOManager::GetVirtualMemoryUsage(){
//  ifstream file;
//  file.open("/proc/self/stat");
//  G4String datum;
//  G4double VirtualMemory = 0.;
//  if(file.is_open()){
//    for(Int_t i = 0; i < 23; i++)
//      file >> datum;
//    VirtualMemory = atoll( (const char*) datum.c_str() )/(1024.*1024.);//datum.Atoll()/(1024.*1024.);
//  }
//  file.close();
//  return VirtualMemory;
//}

MCVRootIO* RootIOManager::FindRootIO(G4String name){

  RootIOList::iterator iRootIO(fRootIOList.begin());
  RootIOList::iterator endRootIO(fRootIOList.end());
  while (iRootIO!=endRootIO){
    if((*iRootIO)->GetName() == name) return (*iRootIO);
    iRootIO++;
  }
  return 0;

}
