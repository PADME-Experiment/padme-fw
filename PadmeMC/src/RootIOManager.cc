// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------

#include <sstream>

#include "RootIOManager.hh"

//#include "ECalRootIO.hh"
//#include "TargetRootIO.hh"
//#include "PVetoRootIO.hh"
//#include "EVetoRootIO.hh"
//#include "HEPVetoRootIO.hh"
//#include "SACRootIO.hh"
//#include "LAVRootIO.hh"
//#include "MagnetRootIO.hh"
//#include "TDumpRootIO.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4Run.hh"

#include "DetectorConstruction.hh"

//#include "Stream.hh"
//#include "Event.hh"

#include "TString.h"
#include "TProcessID.h"

#include "DatacardManager.hh"

RootIOManager* RootIOManager::fInstance = 0;

RootIOManager::RootIOManager()
{

  // Create run and event objects
  //fStream = new Stream();
  //fEvent = new Event();

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
  //fRootIOList.push_back(new ECalRootIO);
  //fRootIOList.push_back(new TargetRootIO);
  //fRootIOList.push_back(new PVetoRootIO);
  //fRootIOList.push_back(new EVetoRootIO);
  //fRootIOList.push_back(new HEPVetoRootIO);
  //fRootIOList.push_back(new SACRootIO);
  //fRootIOList.push_back(new LAVRootIO);
  //fRootIOList.push_back(new MagnetRootIO);
  //fRootIOList.push_back(new TDumpRootIO);

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
    //fFile->cd();
    fStreamTree->Write();
    if (fVerbose) fStreamTree->Print();
    delete fStreamTree;
    //fGVirtMem->GetYaxis()->SetTitle("Virtual Memory (MB)");
    //fGVirtMem->GetXaxis()->SetTitle("Processed Events");
    //fGVirtMem->Write();
    //delete fGVirtMem;
    fFile->Purge();
    fFile->Close();
    if (fVerbose) G4cout << "RootIOManager: I/O file closed" << G4endl;

  }
}


void RootIOManager::SetFileName(G4String newName)
{
  if (fVerbose) G4cout << "RootIOManager: Setting file name to "
    << newName << G4endl;
  fFileName = newName;
  fFileNameHasChanged = true;
}

void RootIOManager::NewRun(G4int nRun)
{


  if (fVerbose) G4cout << "RootIOManager: Initializing I/O for run " << nRun << G4endl;

  if ( fFileNameHasChanged ) {

    // Close old file (if any)
    if ( fFile != 0 ) {
      if (fVerbose) G4cout << "RootIOManager: Closing old file" << G4endl;
      Close();
    }

    // Create new file to hold data
    if (fVerbose) G4cout << "RootIOManager: Creating new file " << fFileName << G4endl;
    fFile = TFile::Open(fFileName.c_str(),"RECREATE","NA62MC");
    fFileNameHasChanged = false;

    // Define basic file properties
    if (fVerbose>=2) G4cout << "RootIOManager: Setting file compression level to "
      << fCompLevel << G4endl;
    fFile->SetCompressionLevel(fCompLevel);

    // Create tree to hold runs
    if (fVerbose>=2) G4cout << "RootIOManager: Creating new Streams tree" << G4endl;
    fStreamTree = new TTree("Streams","List of streams");
    //fStreamTree->SetAutoSave(1000000000);  // autosave when ~1 Gbyte written
    fStreamTree->SetDirectory(fFile->GetDirectory("/"));

    // Create branch to hold the run content info
    //fStreamBranch = fStreamTree->Branch("Stream", &fStream, fBufSize);
    //fStreamBranch->SetAutoDelete(kFALSE);

  }

  // Create tree to hold all events in this run
  fEventTree = new TTree("Generated","Generated events tree");
  //fEventTree->SetAutoSave(1000000000);  // autosave when ~1 Gbyte written
  TTree::SetMaxTreeSize(19000000000);
  fEventTree->SetDirectory(fFile->GetDirectory("/"));

  // Create Stream object to hold run general information...
  //fStream->Clear();
  //fStream->SetType(DatacardManager::GetInstance()->GetDecayType());

  // Fill detector info section of run structure
  //DetectorInfo* detInfo = fStream->GetDetectorInfo();

  RootIOList::iterator iRootIO(fRootIOList.begin());
  RootIOList::iterator endRootIO(fRootIOList.end());

  while (iRootIO!=endRootIO){
    if((*iRootIO)->GetEnabled())
      //(*iRootIO)->NewRun(nRun,fFile,detInfo);
      (*iRootIO)->NewRun(nRun,fFile);
    iRootIO++;
  }

  //if (fVerbose>=2) fStream->Print();

  // Fill run tree
  fStreamTree->Fill();

  // Create branch to hold the whole event structure
  //fEventBranch = fEventTree->Branch("event", &fEvent, fBufSize);
  //fEventBranch->SetAutoDelete(kFALSE);

}

void RootIOManager::EndRun()
{
  if (fVerbose) G4cout << "RootIOManager: Executing End-of-Run procedure" << G4endl;
  // Dump tree for this run to file and erase it
  if(fEventTree){
    fEventTree->Write();
    if (fVerbose) fEventTree->Print();
    delete fEventTree;
  }
  RootIOList::iterator iRootIO(fRootIOList.begin());
  RootIOList::iterator endRootIO(fRootIOList.end());

  while (iRootIO!=endRootIO){
    if((*iRootIO)->GetEnabled())
      (*iRootIO)->EndRun();
    iRootIO++;
  }
}

void RootIOManager::SaveEvent(const G4Event* eventG4)
{
  if (fVerbose>=2) G4cout << "RootIOManager: Preparing event structure" << G4endl;

  // Save current Object count
  Int_t savedObjNumber = TProcessID::GetObjectCount();

  // Get event id (run,event,date)
  G4int nRun = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();
  G4int nEvent = eventG4->GetEventID();
  G4int date = 0; // To be replaced with current time

  if (fVerbose>=2) G4cout << "RootIOManager: Event " << nEvent << " Run " << nRun
    << " at " << date << G4endl;

  // Fill Event Header
  //EventHeader* evtHead = fEvent->GetHeader();
  //evtHead->SetEvtNum(nEvent);
  //evtHead->SetRunNum(nRun);
  //evtHead->SetDate(date);

  //Restore Object count
  //To save space in the table keeping track of all referenced objects
  //we assume that our events do not address each other. We reset the
  //object count to what it was at the beginning of the event.
  TProcessID::SetObjectCount(savedObjNumber);

  //if (fVerbose>=2) G4cout << "RootIOManager: Saving event structure to file" << G4endl;
  //if (fVerbose>=8) fEvent->Print();
  //if (fVerbose>=9) fEvent->PrintAll();

  fEventTree->Fill();

  RootIOList::iterator iRootIO(fRootIOList.begin());
  RootIOList::iterator endRootIO(fRootIOList.end());

  while(iRootIO!=endRootIO){
    if((*iRootIO)->GetEnabled())
      (*iRootIO)->SaveEvent(eventG4);
    iRootIO++;
  }

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
    if((*iRootIO)->GetName() == name)
      return (*iRootIO);
    iRootIO++;
  }
  return 0;
}
