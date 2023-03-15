#include "ECalRecoRootIO.hh"

#include <stdio.h>
#include "Riostream.h"

#include <TTree.h>
#include <TBranch.h>

#include "RecoRootIOManager.hh"
#include "ECalReconstruction.hh"
#include "ECalHit.hh"
#include "ECalCluster.hh"

ECalRecoRootIO::ECalRecoRootIO()
{

  fECalReconstruction = 0;

  fVerbose = 0;

  fHitsCollection = new TECalRecoEvent();
  fClusCollection = new TECalClusCollection();

  TTree::SetBranchStyle(1);

  std::cout << "ECalRecoRootIO::ECalRecoRootIO - ECal RootIO system initialized" << std::endl;

}

ECalRecoRootIO::~ECalRecoRootIO()
{
  if (fVerbose) std::cout << "ECalRecoRootIO::ECalRecoRootIO - Deleting ECal RootIO system" << std::endl;
  if (fHitsCollection) delete fHitsCollection;
  if (fClusCollection) delete fClusCollection;
}

void ECalRecoRootIO::NewRun()
{
  
  if (fVerbose) std::cout << "ECalRecoRootIO::NewRun - Preparing event structure" << std::endl;

  if (fVerbose>1) std::cout << "ECalRecoRootIO::NewRun - Preparing the branches in '" << fEventTree->GetName() << "' output TTree" << std::endl;
  if (fECalReconstruction->writeHits()){
     fBranchHitsColl = fEventTree->Branch("ECal_Hits","TECalRecoEvent",&fHitsCollection);
    fBranchHitsColl->SetAutoDelete(kFALSE);
    if (fVerbose>1) std::cout << "ECalRecoRootIO::NewRun - Branch named ECal_Hits prepared" << std::endl;
  }
  
  if (fECalReconstruction->writeClusters()){
    fBranchClusColl = fEventTree->Branch("ECal_Clusters","TECalClusCollection",&fClusCollection);
    fBranchClusColl->SetAutoDelete(kFALSE);
    if (fVerbose>1) std::cout << "ECalRecoRootIO::NewRun - Branch named ECal_Clusters prepared" << std::endl;
  }
  
}

void ECalRecoRootIO::EndRun()
{
  if (fVerbose) std::cout << "ECalRecoRootIO::EndRun - Executing End-of-Run procedure" << std::endl;  
}

void ECalRecoRootIO::SaveEvent()
{

  if (fECalReconstruction->writeHits()){
    fHitsCollection->Clear();
    vector<ECalHit*> Hits = fECalReconstruction->GetRecoHits();
    if (fVerbose>2) printf("ECalRecoRootIO::SaveEvent - Found %ld hit(s)\n",Hits.size());
    for(UInt_t iH = 0;iH < Hits.size(); ++iH) {
      if (fVerbose>3) {
	printf("ECalRecoRootIO::SaveEvent - Hit %d ",iH);
	Hits[iH]->Print();
      }
      TRecoVHit* pHit = new TRecoVHit();
      ExportHit(Hits[iH],pHit);
      fHitsCollection->AddHit(pHit); // This should keep hit numbering unchanged
    }
  }

  if (fECalReconstruction->writeClusters()){
    fClusCollection->Clear();
    vector<ECalCluster*> Clusters = fECalReconstruction->GetRecoClusters();
    if (fVerbose>2) printf("ECalRecoRootIO::SaveEvent - Found %ld cluster(s)\n",Clusters.size());
    for(UInt_t iC = 0;iC < Clusters.size(); ++iC){
      if (fVerbose>3) {
	printf("ECalRecoRootIO::SaveEvent - Cluster %d ",iC);
	Clusters[iC]->Print();
      }
      TRecoVCluster* pClus = new TRecoVCluster();
      ExportCluster(Clusters[iC],pClus);
      fClusCollection->AddElement(pClus);
    }
  }

}

Bool_t ECalRecoRootIO::ExportHit(ECalHit* rHit,TRecoVHit* pHit)
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

Bool_t ECalRecoRootIO::ExportCluster(ECalCluster* rClus,TRecoVCluster* pClus)
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
