#include "RecoVRootIO.hh"
#include <iostream>
#include "RecoRootIOManager.hh"
#include <vector>
#include "PadmeVReconstruction.hh"


RecoVRootIO::RecoVRootIO(TString name)
  : PadmeVNamedModule(name)
{ 
  // Default output file parameters
  fBufSize = 64000; //size of output buffer
  fBranchStyle = 1; //new branch style by default

  // Default run number
  fRunNumber = 0;


}

RecoVRootIO::~RecoVRootIO(){;
}


void RecoVRootIO::SaveEvent(){
  
    fEvent->Clear();
  //  PVetoReconstruction* MyReco = (PVetoReconstruction*) RecoRootIOManager::GetInstance()->GetReconstruction()->FindReco(this->GetName());
  PadmeVReconstruction* MyReco = (PadmeVReconstruction*) RecoRootIOManager::GetInstance()->GetReconstruction()->FindReco(this->GetName());
  
  vector<TRecoVHit *> Hits = MyReco->GetRecoHits();
  for(unsigned int iHit = 0;iHit < Hits.size();iHit++){
    fEvent->AddHit(Hits[iHit]);
    //    std::cout << "Adding a hit " << iHit << std::endl;
    //    delete Hits[iHit];
  }
  //  Hits.clear();
}

void RecoVRootIO::NewRun(Int_t nRun, TFile* hfile){
  fRunNumber = nRun;
  
  if (fVerbose>=2) 
    std::cout << this->GetName() << "  Preparing event structure" << std::endl;
  // Create branch to hold PVeto Hits and Digis for this run
  fEventTree = RecoRootIOManager::GetInstance()->GetEventTree();

  std::cout << "Preparing the branch in  " << fEventTree << std::endl;
  fBranch = fEventTree->Branch(this->GetName(), fEvent->IsA()->GetName(), &fEvent);
   std::cout << "Branch prepared" << std::endl;
 
   fBranch->SetAutoDelete(kFALSE);
  
}

void RecoVRootIO::EndRun(){
  std::cout << this->GetName() << "RootIO: Executing End-of-Run procedure" << std::endl;  
}

void RecoVRootIO::Close()
{;}
