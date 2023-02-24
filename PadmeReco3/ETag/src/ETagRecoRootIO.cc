#include "ETagRecoRootIO.hh"

#include <stdio.h>
#include "Riostream.h"

#include <TTree.h>
#include <TBranch.h>

#include "RecoRootIOManager.hh"
#include "ETagReconstruction.hh"
#include "ETagHit.hh"
#include "ETagCluster.hh"

ETagRecoRootIO::ETagRecoRootIO() 
{

  fETagReconstruction = 0;

  fVerbose = 0;

  fHitsCollection = new TETagRecoEvent();
  fClusCollection = new TETagClusCollection();

  TTree::SetBranchStyle(1);

  std::cout << "ETagRecoRootIO::ETagRecoRootIO - ETag RootIO system initialized" << std::endl;

}

ETagRecoRootIO::~ETagRecoRootIO()
{
  if (fVerbose) std::cout << "ETagRecoRootIO::ETagRecoRootIO - Deleting ETag RootIO system" << std::endl;
}

void ETagRecoRootIO::NewRun()
{
  
  if (fVerbose) std::cout << "ETagRecoRootIO::NewRun - Preparing event structure" << std::endl;

  fEventTree = (RecoRootIOManager::GetInstance())->GetEventTree();

  //PadmeVReconstruction* MyReco = (PadmeVReconstruction*) RecoRootIOManager::GetInstance()->GetReconstruction()->FindReco(this->GetName());
  if (fVerbose>1) std::cout << "ETagRecoRootIO::NewRun - Preparing the branches in '" << fEventTree->GetName() << "' output TTree" << std::endl;
  if (fETagReconstruction->writeHits()){
     fBranchHitsColl = fEventTree->Branch("ETag_Hits","TETagRecoEvent",&fHitsCollection);
    fBranchHitsColl->SetAutoDelete(kFALSE);
    if (fVerbose>1) std::cout << "ETagRecoRootIO::NewRun - Branch named ETag_Hits prepared" << std::endl;
  }
  
  if (fETagReconstruction->writeClusters()){
    fBranchClusColl = fEventTree->Branch("ETag_Clusters","TETagClusCollection",&fClusCollection);
    fBranchClusColl->SetAutoDelete(kFALSE);
    if (fVerbose>1) std::cout << "ETagRecoRootIO::NewRun - Branch named ETag_Clusters prepared" << std::endl;
  }
  
}

void ETagRecoRootIO::EndRun()
{
  if (fVerbose) std::cout << "ETagRecoRootIO::EndRun - Executing End-of-Run procedure" << std::endl;  
}

void ETagRecoRootIO::SaveEvent()
{

  if (fETagReconstruction->writeHits()){
    fHitsCollection->Clear();
    vector<ETagHit*> Hits = fETagReconstruction->GetRecoHits();
    if (fVerbose>2) printf("ETagRecoRootIO::SaveEvent - Found %ld hit(s)\n",Hits.size());
    for(UInt_t iH = 0;iH < Hits.size(); ++iH) {
      if (fVerbose>3) {
	printf("ETagRecoRootIO::SaveEvent - Hit %d ",iH);
	Hits[iH]->Print();
      }
      TRecoVHit* pHit = new TRecoVHit();
      ExportHit(Hits[iH],pHit);
      fHitsCollection->AddHit(pHit); // This should keep hit numbering unchanged
    }
  }

  if (fETagReconstruction->writeClusters()){
    fClusCollection->Clear();
    vector<ETagCluster*> Clusters = fETagReconstruction->GetRecoClusters();
    if (fVerbose>2) printf("ETagRecoRootIO::SaveEvent - Found %ld cluster(s)\n",Clusters.size());
    for(UInt_t iC = 0;iC < Clusters.size(); ++iC){
      if (fVerbose>3) {
	printf("ETagRecoRootIO::SaveEvent - Cluster %d ",iC);
	Clusters[iC]->Print();
      }
      TRecoVCluster* pClus = new TRecoVCluster();
      ExportCluster(Clusters[iC],pClus);
      fClusCollection->AddElement(pClus);
    }
  }

}

Bool_t ETagRecoRootIO::ExportHit(ETagHit* rHit,TRecoVHit* pHit)
{

  // Export content of reconstructed hit to persistent hit structure
  pHit->setBDCHid(rHit->GetADCBoard(),rHit->GetADCChannel());
  pHit->setStatus(rHit->GetStatus());
  pHit->SetChannelId(rHit->GetChannelId());
  pHit->SetPosition(rHit->GetPosition());
  pHit->SetEnergy(rHit->GetEnergy());
  pHit->SetTime(rHit->GetTime());

  return true;
}

Bool_t ETagRecoRootIO::ExportCluster(ETagCluster* rClus,TRecoVCluster* pClus)
{

  // Export content of reconstructed cluster to persistent cluster structure
  pClus->setStatus(rClus->GetStatus());
  pClus->SetPosition(rClus->GetPosition());
  pClus->SetEnergy(rClus->GetEnergy());
  pClus->SetTime(rClus->GetTime());
  pClus->SetSeed(rClus->GetSeed());
  pClus->SetNHitsInClus(rClus->GetNHits());
  pClus->SetHitVecInClus(rClus->GetHitsVector());

  return true;
}
