#include <sstream>
//#include <ctime>
#include <sys/time.h>

#include "RecoRootIOManager.hh"
#include "PVetoRecoRootIO.hh"
#include "EVetoRecoRootIO.hh"
#include "HEPVetoRecoRootIO.hh"
#include "SACRecoRootIO.hh"
#include "ETagRecoRootIO.hh"
#include "TargetRecoRootIO.hh"
#include "ECalRecoRootIO.hh"
#include "LeadGlassRecoRootIO.hh"

RecoRootIOManager* RecoRootIOManager::fInstance = 0;

RecoRootIOManager::RecoRootIOManager(TString ConfFileName)
{
  // Create run and event objects
  fEvent = new TRecoEvent();

  // Default output file parameters
  fBufSize = 64000; //size of output buffer
  fBranchStyle = 1; //new branch style by default
  fCompLevel = 1;   //fast compression by default

  // Default fVerbose level
  fVerbose = 0;
  firstTime = true;
  
  // Default file name
  
  fFileName = "";
  fFileNameHasChanged = true;

  // Make sure initial file pointer is empty (mandatory!)
  fFile = 0;

  TTree::SetBranchStyle(fBranchStyle);
  std::cout << "RecoRootIOManager: Initialized" << std::endl;

  fConfigParser = new utl::ConfigParser((const std::string)ConfFileName);
  fConfig = new PadmeVRecoConfig(fConfigParser,"PadmeRecoIOConfiguration");

  // Add subdetectors persistency managers
  if (fConfig->GetParOrDefault("RECOOutput", "PVeto"   ,1)*fConfig->GetParOrDefault("RECOALGORITHMS", "PVeto"   ,1)) 
    fRootIOList.push_back(new PVetoRecoRootIO);
  if (fConfig->GetParOrDefault("RECOOutput", "EVeto"   ,1)*fConfig->GetParOrDefault("RECOALGORITHMS", "EVeto"   ,1))
    fRootIOList.push_back(new EVetoRecoRootIO);
  if (fConfig->GetParOrDefault("RECOOutput", "HEPVeto" ,1)*fConfig->GetParOrDefault("RECOALGORITHMS", "HEPVeto" ,1))
    fRootIOList.push_back(new HEPVetoRecoRootIO);
  if (fConfig->GetParOrDefault("RECOOutput", "SAC"     ,1)*fConfig->GetParOrDefault("RECOALGORITHMS", "SAC"     ,1))
    fRootIOList.push_back(new SACRecoRootIO);
  if (fConfig->GetParOrDefault("RECOOutput", "ETag"    ,1)*fConfig->GetParOrDefault("RECOALGORITHMS", "ETag"    ,1))
    fRootIOList.push_back(new ETagRecoRootIO);
  if (fConfig->GetParOrDefault("RECOOutput", "Target"  ,1)*fConfig->GetParOrDefault("RECOALGORITHMS", "Target"  ,1))
    fRootIOList.push_back(new TargetRecoRootIO);
  if (fConfig->GetParOrDefault("RECOOutput", "ECal"    ,1)*fConfig->GetParOrDefault("RECOALGORITHMS", "ECal"    ,1))
    fRootIOList.push_back(new ECalRecoRootIO);
  if (fConfig->GetParOrDefault("RECOOutput", "LeadGlass", 1)*fConfig->GetParOrDefault("RECOALGORITHMS", "LeadGlass" ,1))
    fRootIOList.push_back(new LeadGlassRecoRootIO);
  //if (fConfig->GetParOrDefault("RECOOutput", "TPix"    ,0))fRootIOList.push_back(new ECalRecoRootIO);
  std::cout<<"************************** "<<fRootIOList.size()<<" RecoIO Tools built"<<std::endl;

}

RecoRootIOManager::~RecoRootIOManager()
{;}

RecoRootIOManager* RecoRootIOManager::GetInstance()
{
  if ( fInstance == 0 ) { std::cout<<"ERROR RecoRootIOManager not yet built/initialized"<<std::endl;}
//fInstance = new RecoRootIOManager(); }
  return fInstance;
}

RecoRootIOManager* RecoRootIOManager::GetInstance(TString confFile)
{
  if ( fInstance == 0 ) { fInstance = new RecoRootIOManager(confFile); }
  return fInstance;
}


void RecoRootIOManager::Close()
{

  // Save latest data, clean run tree, and close file
  if ( fFile != 0 ) {
    fFile->cd();
    if(fEventTree!= 0) {
      fEventTree->Write();
      if (fVerbose) fEventTree->Print();
      delete fEventTree;
      fEventTree = 0;
    }
    //fGVirtMem->GetYaxis()->SetTitle("Virtual Memory (MB)");
    //fGVirtMem->GetXaxis()->SetTitle("Processed Events");
    //fGVirtMem->Write();
    //delete fGVirtMem;
    fFile->Purge();
    fFile->Close();
    //if (fVerbose)
    std::cout << "RecoRootIOManager: I/O file closed" << std::endl;
  }

}


void RecoRootIOManager::SetFileName(TString newName)
{
  //if (fVerbose)
  std::cout << "RecoRootIOManager: Setting file name to " << newName << std::endl;
  fFileName = newName;
  fFileNameHasChanged = true;
  if (firstTime)
    {
      // Close old file (if any)
      if ( fFile != 0 ) {
	if (fVerbose)
	  std::cout << "RecoRootIOManager: Closing old file" << std::endl;
	Close();
      }
      
      // Create new file to hold data
      //if (fVerbose)
      std::cout << "RecoRootIOManager: Creating new file " << fFileName << std::endl;
      fFile = TFile::Open(fFileName,"RECREATE","PadmeMC");
      fFileNameHasChanged = false;
      
      // Define basic file properties
      if (fVerbose>=2)
	std::cout << "RecoRootIOManager: Setting file compression level to "
		  << fCompLevel << std::endl;
      fFile->SetCompressionLevel(fCompLevel);
    }
}


void RecoRootIOManager::NewRun(Int_t nRun)
{
  
  //if (fVerbose)
  std::cout << "RecoRootIOManager: Initializing I/O for run " << nRun << std::endl;
  
  if ( fFileNameHasChanged ) {
    
    // Close old file (if any)
    if ( fFile != 0 ) {
      if (fVerbose)
	std::cout << "RecoRootIOManager: Closing old file" << std::endl;
      Close();
    }
    
    // Create new file to hold data
    //if (fVerbose)
      std::cout << "RecoRootIOManager: Creating new file " << fFileName << std::endl;
    fFile = TFile::Open(fFileName,"RECREATE","PadmeMC");
    fFileNameHasChanged = false;

    // Define basic file properties
    if (fVerbose>=2)
      std::cout << "RecoRootIOManager: Setting file compression level to "
             << fCompLevel << std::endl;
    fFile->SetCompressionLevel(fCompLevel);
  }

  if ( fFileNameHasChanged || firstTime ) {
    firstTime=false;
    // Create tree to hold runs
    //if (fVerbose>=2)
    std::cout << "RecoRootIOManager: Creating new Event tree" << std::endl;
    fEventTree = new TTree("Events","List of events");
    //fEventTree->SetAutoSave(1000000000);  // autosave when ~1 Gbyte written
    fEventTree->SetDirectory(fFile->GetDirectory("/"));
    
    // Create branch to hold the run content info
    fEventBranch = fEventTree->Branch("RecoEvent", &fEvent, fBufSize);
    fEventBranch->SetAutoDelete(kFALSE);

    RootIOList::iterator iRootIO(fRootIOList.begin());
    RootIOList::iterator endRootIO(fRootIOList.end());
    std::cout << "Preparing the branches in  " << fEventTree << std::endl;
    while (iRootIO!=endRootIO) {
      std::cout << "RootIOManager: Checking IO for " << (*iRootIO)->GetName() << std::endl;
      if ((*iRootIO)->GetEnabled()) {
	std::cout << "RootIOManager: IO for " << (*iRootIO)->GetName() << " enabled" << std::endl;
	(*iRootIO)->NewRun(nRun,fFile);
	//(*iRootIO)->NewRun(nRun,fFile);
      }
      iRootIO++;
    }

    // If present, replicate MCTruth info to output file
    if (fReco->GetMCTruthEvent()) 
      fEventTree->Branch("MCTruth",fReco->GetMCTruthEvent());

  }

}

void RecoRootIOManager::EndRun()
{

  if (fVerbose)
    std::cout << "RootIOManager: Executing End-of-Run procedure" << std::endl;

  // Dump tree for this event to file and erase it
  if(fEventTree) {
    fFile->cd();
    fEventTree->Write();
    if (fVerbose>=2) fEventTree->Print();
    delete fEventTree; 
    fEventTree = 0;
  }

  // Same for each subdetetctor
  RootIOList::iterator iRootIO(fRootIOList.begin());
  RootIOList::iterator endRootIO(fRootIOList.end());
  while (iRootIO!=endRootIO){
    if((*iRootIO)->GetEnabled()) (*iRootIO)->EndRun();
    iRootIO++;
  }

}

void RecoRootIOManager::SaveEvent(){
  
  //int nRun=0;
  //int nEvent=0;
  //fEvent->SetRunNumber(nRun);
  //fEvent->SetEventNumber(nEvent);
  
  // This is obsolete and will be removed
  struct timeval tp;
  gettimeofday(&tp,NULL);
  double now = tp.tv_sec*1.+tp.tv_usec/1000000.;
  fEvent->SetTime(now);

  fEvent->SetRunNumber(fReco->GetRunNumber());
  fEvent->SetEventNumber(fReco->GetEventNumber());
  fEvent->SetEventTime(fReco->GetEventTime());
  fEvent->SetRunClock(fReco->GetRunClock());
  fEvent->SetEventStatus(fReco->GetEventStatus());
  fEvent->SetTriggerMask(fReco->GetTriggerMask());

  // Save current time as event reconstruction time
  fEvent->SetRecoTime(TTimeStamp());

  // Show event header information (for debug purposes)
  //printf("RecoRootIOManager::SaveEvent() - Run nr %7d Event nr %7d Event time %8d-%06d.%09d Run clock %12lld Reco time %8d-%06d.%09d Status 0x%08x Trigger 0x%08x\n",fEvent->GetRunNumber(),fEvent->GetEventNumber(),fEvent->GetEventTime().GetDate(),fEvent->GetEventTime().GetTime(),fEvent->GetEventTime().GetNanoSec(),fEvent->GetRunClock(),fEvent->GetRecoTime().GetDate(),fEvent->GetRecoTime().GetTime(),fEvent->GetRecoTime().GetNanoSec(),fEvent->GetEventStatus(),fEvent->GetTriggerMask());

  RootIOList::iterator iRootIO(fRootIOList.begin());
  RootIOList::iterator endRootIO(fRootIOList.end());
  while (iRootIO!=endRootIO) {
    if ((*iRootIO)->GetEnabled()) {
      (*iRootIO)->SaveEvent();
      //std::cout << "Saving event for RECO: " << (*iRootIO)->GetName() << std::endl;
    }
    iRootIO++;
  }

  // All data have been copied: write it to file
  fEventTree->Fill();
  //std::cout<<" tree filled ... "<<std::endl;
  
}

RecoVRootIO* RecoRootIOManager::FindRecoRootIO(TString name){

  RootIOList::iterator iRootIO(fRootIOList.begin());
  RootIOList::iterator endRootIO(fRootIOList.end());
  while (iRootIO!=endRootIO){
    if((*iRootIO)->GetName() == name) return (*iRootIO);
    iRootIO++;
  }
  return 0;

}
