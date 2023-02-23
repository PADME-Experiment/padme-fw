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

  fHitsCollection = new TETagRecoEvent();
  fClusCollection = new TETagClusCollection();

  TTree::SetBranchStyle(1);

  fEnabled = true;

  std::cout << "ETagRecoRootIO: Initialized" << std::endl;

}

ETagRecoRootIO::~ETagRecoRootIO()
{;}

void ETagRecoRootIO::NewRun()
{
  
  std::cout << "ETagRecoRootIO::NewRun - Preparing event structure" << std::endl;

  fEventTree = (RecoRootIOManager::GetInstance())->GetEventTree();

  //PadmeVReconstruction* MyReco = (PadmeVReconstruction*) RecoRootIOManager::GetInstance()->GetReconstruction()->FindReco(this->GetName());
  if (fETagReconstruction->writeHits()){
    std::cout << "ETagRecoRootIO::NewRun - Preparing the branches in " << fEventTree << std::endl;
    std::string brHname = "ETag_Hits";
    fBranchHitsColl = fEventTree->Branch("ETag_Hits","TETagRecoEvent",&fHitsCollection);
    fBranchHitsColl->SetAutoDelete(kFALSE);
    std::cout << "ETagRecoRootIO::NewRun - Branch named ETag_Hits prepared" << std::endl;
  }
  
  if (fETagReconstruction->writeClusters()){
    fBranchClusColl = fEventTree->Branch("ETag_Clusters","TETagClusCollection",&fClusCollection);
    fBranchClusColl->SetAutoDelete(kFALSE);
    std::cout << "ETagRecoRootIO::NewRun - Branch named ETag_Clusters prepared" << std::endl;
  }
  
}

void ETagRecoRootIO::EndRun()
{
  std::cout << "ETagRecoRootIO::EndRun - Executing End-of-Run procedure" << std::endl;  
}

void ETagRecoRootIO::SaveEvent()
{

  //PadmeVReconstruction* MyReco = (PadmeVReconstruction*) RecoRootIOManager::GetInstance()->GetReconstruction()->FindReco(this->GetName());

  if (fETagReconstruction->writeHits()){
    fHitsCollection->Clear();
    vector<ETagHit*> Hits = fETagReconstruction->GetRecoHits();
    printf("ETagRecoRootIO::SaveEvent - Found %d hit(s)\n",Hits.size());
    for(unsigned int iH = 0;iH < Hits.size(); ++iH) {
      TRecoVHit* pHit = new TRecoVHit();
      ExportHit(Hits[iH],pHit);
      fHitsCollection->AddHit(pHit); // This should keep hit numbering unchanged
    }
  }

  if (fETagReconstruction->writeClusters()){
    fClusCollection->Clear();
    vector<ETagCluster*> Clusters = fETagReconstruction->GetRecoClusters();
    for(unsigned int iC = 0;iC < Clusters.size(); ++iC){
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
