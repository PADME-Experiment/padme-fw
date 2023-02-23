#include "RecoVRootIO.hh"
#include <iostream>
#include "RecoRootIOManager.hh"
#include <vector>
#include "PadmeVReconstruction.hh"
#include "TRecoVObject.hh"
#include "TRecoVClusCollection.hh"


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

  //std::cout<<this->GetName()<<" in RecoVRootIO::SaveEvent"<<std::endl;
  PadmeVReconstruction* MyReco = (PadmeVReconstruction*) RecoRootIOManager::GetInstance()->GetReconstruction()->FindReco(this->GetName());

  if (MyReco->writeHits()){
    fEvent->Clear();
    vector<TRecoVHit *> Hits = MyReco->GetRecoHits();
    for(unsigned int iHit = 0;iHit < Hits.size(); ++iHit){
      fEvent->AddHit(Hits[iHit]);
    }
  }
  //std::cout<<" hits done "<<std::endl;

  if (MyReco->writeClusters()){
    if (fClusColl){
      fClusColl->Clear();
      vector<TRecoVCluster *> Clusters = MyReco->GetClusters();
      for(unsigned int iC = 0;iC < Clusters.size(); ++iC){
	//std::cout<<" adding cluster  "<<iC<<std::endl;
	fClusColl->AddElement(Clusters[iC]);
      }
    }
  }
  //std::cout<<" in RecoVRootIO::SaveEvent ... out "<<std::endl;

}

void RecoVRootIO::NewRun(Int_t nRun, TFile* hfile){
  fRunNumber = nRun;
  
  //if (fVerbose>=2)

  std::cout<<this->GetName()<<"  Preparing event structure" <<std::endl;
  // Create branch to hold PVeto Hits and Digis for this run
  fEventTree = (RecoRootIOManager::GetInstance())->GetEventTree();

  PadmeVReconstruction* MyReco = (PadmeVReconstruction*) RecoRootIOManager::GetInstance()->GetReconstruction()->FindReco(this->GetName());
  if (MyReco->writeHits()){
    std::cout << "Preparing the branches in  " << fEventTree << std::endl;
    std::string brHname = std::string(this->GetName())+"_Hits";
    fBranch = fEventTree->Branch(brHname.c_str(), fEvent->IsA()->GetName(), &fEvent);
    std::cout << "Branch named "<<brHname<<" prepared" << std::endl;
    fBranch->SetAutoDelete(kFALSE);
  }
  
  if (MyReco->writeClusters()){
    if (fClusColl){
      std::string brCname = std::string(this->GetName())+"_Clusters";
      fBranchClusColl = fEventTree->Branch(brCname.c_str(), fClusColl->IsA()->GetName(), &fClusColl);
      std::cout << "Branch named "<<brCname<<" prepared" << std::endl;
      fBranchClusColl->SetAutoDelete(kFALSE);
    }
  }
  
}

void RecoVRootIO::EndRun(){
  std::cout << this->GetName() << "RootIO: Executing End-of-Run procedure" << std::endl;  
}

void RecoVRootIO::Close()
{;}
