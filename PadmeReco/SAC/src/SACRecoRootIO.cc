#include "SACRecoRootIO.hh"
#include "RecoVRootIO.hh"

#include "TSACRecoEvent.hh"
#include "SACReconstruction.hh"
#include "TRecoVCluster.hh"
#include "TSACClusCollection.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"
#include <stdio.h>
#include "Riostream.h"


SACRecoRootIO::SACRecoRootIO() 
  : RecoVRootIO(TString("SAC"))
{
  iev = 0;
  fEvent = new TSACRecoEvent();
  fClusColl = new TSACClusCollection();
  //fClusColl = new TRecoVClusCollection(TRecoVCluster::Class());

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  //  fHitsEnabled = true;
  std::cout << "SACRecoRootIO: Initialized" << std::endl;

}
SACRecoRootIO::~SACRecoRootIO()
{;}

/*
void SACRecoRootIO::SaveEvent()
{
  
  TSACRecoEvent* myEvt = (TSACRecoEvent*)fEvent;
  myEvt->Clear();
  fClusColl->Clear();
  
  RecoVRootIO::SaveEvent();

  PadmeVReconstruction* MyReco = (PadmeVReconstruction*) RecoRootIOManager::GetInstance()->GetReconstruction()->FindReco(this->GetName());
  SACReconstruction* Reco      = (SACReconstruction*)MyReco;

  vector<TRecoVCluster*> clVector = Reco->getClusters();
  for (unsigned int j=0; j<clVector.size(); ++j)
    {
      myEvt->getClusCollection()->AddElement(clVector[j]);
      fClusColl->AddElement(clVector[j]);
    }
}
*/


 /*
void SACRecoRootIO::NewRun(Int_t nRun, TFile* hfile){
  
  std::cout<<this->GetName()<<"::NewRun"<<std::endl;
  // Book  branch Target (collection of hits)
  RecoVRootIO::NewRun(nRun, hfile);
    
  std::cout<<this->GetName()<<"::NewRun - digit branch setup ... now moving to Target specific branches"<<std::endl;
  fEventTree = RecoRootIOManager::GetInstance()->GetEventTree();
  std::cout<<this->GetName()<<"::NewRun - pointer to the tree obtained: "<<fEventTree<<std::endl;


  // Book branch TargetFitEvent (WF fit parameters)
  fBranchClusColl = fEventTree->Branch("SACClusClollection", fClusColl->IsA()->GetName(), &fClusColl);
  std::cout<<this->GetName()<<"::NewRun - branch SACClusClollection obtained"<<std::endl;
  fBranchClusColl->SetAutoDelete(kFALSE);
  std::cout<<this->GetName()<<"::NewRun - branch SACClusClollection autodelete set to false"<<std::endl;
  
}
*/
