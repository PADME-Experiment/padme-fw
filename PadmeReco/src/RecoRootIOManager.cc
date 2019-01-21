#include <sstream>
//#include <ctime>
#include <sys/time.h>
//#include "PadmeReconstruction.hh" // M. Raggi 6/12/18
#include "RecoRootIOManager.hh"
#include "PVetoRecoRootIO.hh"
#include "EVetoRecoRootIO.hh"
#include "HEPVetoRecoRootIO.hh"
#include "SACRecoRootIO.hh"
#include "TargetRecoRootIO.hh"
#include "ECalRecoRootIO.hh"

RecoRootIOManager* RecoRootIOManager::fInstance = 0;


RecoRootIOManager::RecoRootIOManager()
{
  // Create run and event objects
  fEvent = new TRecoEvent();
  ReadConf *readconf = ReadConf::GetInstance();
  //  std::cout<<"Read conf"<<readconf<<std::endl;

  //connecto to Reco to retrieve the configuration
  // PadmeReconstruction* PadmeReco; 
  // Default output file parameters
  fBufSize = 64000; //size of output buffer
  fBranchStyle = 1; //new branch style by default
  fCompLevel = 1;   //fast compression by default

  // Default fVerbose level
  fVerbose = 0;
  
  // Default file name
  
  fFileName = "";
  fFileNameHasChanged = true;

  // Make sure initial file pointer is empty (mandatory!)
  fFile = 0;

  TTree::SetBranchStyle(fBranchStyle);
  std::cout << "RecoRootIOManager: Initialized" << std::endl;
    
  // Add subdetectors persistency managers   !! Need to apply flags for different recosntructions
  fRootIOList.push_back(new PVetoRecoRootIO);
  fRootIOList.push_back(new EVetoRecoRootIO);
  fRootIOList.push_back(new HEPVetoRecoRootIO);
  fRootIOList.push_back(new SACRecoRootIO);
  fRootIOList.push_back(new TargetRecoRootIO);
  fRootIOList.push_back(new ECalRecoRootIO);
  RootIOList::iterator iRootIO(fRootIOList.begin());
  RootIOList::iterator endRootIO(fRootIOList.end());
  std::cout << "Enabling branches  AAAAAAAAAAAAAAAA" <<readconf->IsSACON()<<std::endl;
  while (iRootIO!=endRootIO) {
    //    std::cout << "RootIOManager: Checking IO for " << (*iRootIO)->GetName() << std::endl;
    if ((*iRootIO)->GetName()=="PVeto" && ! readconf->IsPVetoON()) {
      (*iRootIO)->SetEnabled(false);
      std::cout << "RootIOManager: " << (*iRootIO)->GetName() << "Off "<<std::endl;
    }

    if ((*iRootIO)->GetName()=="EVeto" && ! readconf->IsEVetoON()) {
      (*iRootIO)->SetEnabled(false);
      std::cout << "RootIOManager: " << (*iRootIO)->GetName() << "Off "<<std::endl;
    }

    if ((*iRootIO)->GetName()=="HEPVeto" && ! readconf->IsHEPVetoON()) {
      (*iRootIO)->SetEnabled(false);
      std::cout << "RootIOManager: " << (*iRootIO)->GetName() << "Off "<<std::endl;
    }

    if ((*iRootIO)->GetName()=="SAC" && ! readconf->IsSACON()) {
      (*iRootIO)->SetEnabled(false);
      std::cout << "RootIOManager: " << (*iRootIO)->GetName() << "Off "<<std::endl;
    }

    if ((*iRootIO)->GetName()=="ECal" && ! readconf->IsECalON()) {
      (*iRootIO)->SetEnabled(false);
      std::cout << "RootIOManager: " << (*iRootIO)->GetName() << "Off "<<std::endl;
    }

    if ((*iRootIO)->GetName()=="Target" && ! readconf->IsTargetON()) {
      (*iRootIO)->SetEnabled(false);
      std::cout << "RootIOManager: " << (*iRootIO)->GetName() << "Off "<<std::endl;
    }

    iRootIO++;
  }
  
    
  
}

RecoRootIOManager::~RecoRootIOManager()
{;}

RecoRootIOManager* RecoRootIOManager::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new RecoRootIOManager(); }
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
  
  int nRun=0;
  int nEvent=0;
  
  struct timeval tp;
  gettimeofday(&tp,NULL);
  double now = tp.tv_sec*1.+tp.tv_usec/1000000.;
  
  fEvent->SetRunNumber(nRun);
  fEvent->SetEventNumber(nEvent);
  fEvent->SetTime(now);
  

  RootIOList::iterator iRootIO(fRootIOList.begin());
  RootIOList::iterator endRootIO(fRootIOList.end());
  while (iRootIO!=endRootIO) {
    if ((*iRootIO)->GetEnabled()) {
      //      std::cout << "Saving event for RECO: " << (*iRootIO)->GetName() << std::endl;
      (*iRootIO)->SaveEvent();
      
    }
    iRootIO++;
  }

  // All data have been copied: write it to file
  fEventTree->Fill();
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


