#include "TargetRecoRootIO.hh"

#include <stdio.h>
#include "Riostream.h"

#include <TTree.h>
#include <TBranch.h>

#include "RecoRootIOManager.hh"
#include "TargetReconstruction.hh"
//#include "TargetHit.hh"
//#include "TargetCluster.hh"
#include "TargetStripCharge.hh"

//Beth 21/3/24: modified to make hits->strip charge

TargetRecoRootIO::TargetRecoRootIO()
{

  fTargetReconstruction = 0;

  fVerbose = 0;

  //  fHitsCollection = new TTargetRecoEvent();
  //fClusCollection = new TTargetClusCollection();
  //Beth 21/3/24 is this a good thing to assing to the strip charge collection?
  fStripChargeCollection = new TTargetRecoEvent();

  TTree::SetBranchStyle(1);

  std::cout << "TargetRecoRootIO::TargetRecoRootIO - Target RootIO system initialized" << std::endl;

}

TargetRecoRootIO::~TargetRecoRootIO()
{
  if (fVerbose) std::cout << "TargetRecoRootIO::TargetRecoRootIO - Deleting Target RootIO system" << std::endl;
  //  if (fHitsCollection) delete fHitsCollection;
  if (fStripChargeCollection) delete fStripChargeCollection;
}

void TargetRecoRootIO::NewRun()
{
  
  if (fVerbose) std::cout << "TargetRecoRootIO::NewRun - Preparing event structure" << std::endl;

  if (fVerbose>1) std::cout << "TargetRecoRootIO::NewRun - Preparing the branches in '" << fEventTree->GetName() << "' output TTree" << std::endl;
  /*  if (fTargetReconstruction->writeHits()){
    fBranchHitsColl = fEventTree->Branch("Target_Hits","TTargetRecoEvent",&fHitsCollection);
    fBranchHitsColl->SetAutoDelete(kFALSE);
    if (fVerbose>1) std::cout << "TargetRecoRootIO::NewRun - Branch named Target_Hits prepared" << std::endl;
    }*/
  
    if (fTargetReconstruction->writeStripCharge()){
      fBranchStripChargeColl = fEventTree->Branch("Target_StripCharge","TTargetRecoEvent",&fStripChargeCollection);
      fBranchStripChargeColl->SetAutoDelete(kFALSE);
      if (fVerbose>1) std::cout << "TargetRecoRootIO::NewRun - Branch named Target_StripCharge prepared" << std::endl;
    }

  //if (fTargetReconstruction->writeClusters()){
  //  fBranchClusColl = fEventTree->Branch("Target_Clusters","TTargetClusCollection",&fClusCollection);
  //  fBranchClusColl->SetAutoDelete(kFALSE);
  //  if (fVerbose>1) std::cout << "TargetRecoRootIO::NewRun - Branch named Target_Clusters prepared" << std::endl;
  //}
  
}

void TargetRecoRootIO::EndRun()
{
  if (fVerbose) std::cout << "TargetRecoRootIO::EndRun - Executing End-of-Run procedure" << std::endl;  
}

void TargetRecoRootIO::SaveEvent()
{

  /*  if (fTargetReconstruction->writeHits()){
    fHitsCollection->Clear();
    vector<TargetHit*> Hits = fTargetReconstruction->GetRecoHits();
    if (fVerbose>2) printf("TargetRecoRootIO::SaveEvent - Found %ld hit(s)\n",Hits.size());
    for(UInt_t iH = 0;iH < Hits.size(); ++iH) {
      if (fVerbose>3) {
	printf("TargetRecoRootIO::SaveEvent - Hit %d ",iH);
	Hits[iH]->Print();
      }
      TRecoVHit* pHit = new TRecoVHit();
      ExportHit(Hits[iH],pHit);
      fHitsCollection->AddHit(pHit); // This should keep hit numbering unchanged
    }
    }*/

  if (fTargetReconstruction->writeStripCharge()){
    fStripChargeCollection->Clear();
    vector<TargetStripCharge*> StripCharge = fTargetReconstruction->GetRecoStripCharge();
    if (fVerbose>2) printf("TargetRecoRootIO::SaveEvent - Found %ld hit(s)\n",StripCharge.size());
    for(UInt_t iH = 0;iH < StripCharge.size(); ++iH) {
      if (fVerbose>3) {
	printf("TargetRecoRootIO::SaveEvent - Hit %d ",iH);
	StripCharge[iH]->Print();
      }

      //Beth 21/3/24: Is this necessary? What does it do? It doesn't exist in the LeadGlass reconstruction... if it's only to protect numbering that doesn't matter because there aren't any hits to be numbered
      TRecoVHit* pHit = new TRecoVHit();
      ExportHit(StripCharge[iH],pHit);
      fStripChargeCollection->AddHit(pHit); // This should keep hit numbering unchanged
    }
  }

  //if (fTargetReconstruction->writeClusters()){
  //  fClusCollection->Clear();
  //  vector<TargetCluster*> Clusters = fTargetReconstruction->GetRecoClusters();
  //  if (fVerbose>2) printf("TargetRecoRootIO::SaveEvent - Found %ld cluster(s)\n",Clusters.size());
  //  for(UInt_t iC = 0;iC < Clusters.size(); ++iC){
  //    if (fVerbose>3) {
  //	printf("TargetRecoRootIO::SaveEvent - Cluster %d ",iC);
  //	Clusters[iC]->Print();
  //    }
  //    TRecoVCluster* pClus = new TRecoVCluster();
  //    ExportCluster(Clusters[iC],pClus);
  //    fClusCollection->AddElement(pClus);
  //  }
  //}

}
/* //commented by Beth 21/3/24: The concept of "hits" in the target doesn't really have any meaning. It would be better to compute the charge per strip
Bool_t TargetRecoRootIO::ExportHit(TargetHit* rHit,TRecoVHit* pHit)
{

  // Export content of reconstructed hit to persistent hit structure
  pHit->setBDCHid(rHit->GetADCBoard(),rHit->GetADCChannel());
  pHit->setStatus(rHit->GetStatus());
  pHit->SetChannelId(rHit->GetChannelId());
  pHit->SetPosition(rHit->GetPosition());
  pHit->SetEnergy(rHit->GetEnergy());
  pHit->SetTime(rHit->GetTime());

  return true;
  }*/

Bool_t TargetRecoRootIO::ExportHit(TargetStripCharge* rHit,TRecoVHit* pHit)
{

  // Export content of reconstructed hit to persistent hit structure
  pHit->setBDCHid(rHit->GetADCBoard(),rHit->GetADCChannel());
  pHit->setStatus(rHit->GetStatus());
  pHit->SetChannelId(rHit->GetChannelId());
  pHit->SetPosition(rHit->GetPosition());
  pHit->SetEnergy(rHit->GetCharge());
  //  pHit->SetTime(rHit->GetTime());

  return true;
}
/*
Bool_t TargetRecoRootIO::ExportCluster(TargetCluster* rClus,TRecoVCluster* pClus)
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
*/