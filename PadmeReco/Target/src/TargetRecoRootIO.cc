#include "TargetRecoRootIO.hh"
#include "RecoVRootIO.hh"

#include "TTargetRecoEvent.hh"
#include "TTargetRecoBeam.hh"
#include "TTargetFitEvent.hh"
#include "TargetReconstruction.hh"
//#include "TRecoVHit.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"
#include <stdio.h>
#include "Riostream.h"


TargetRecoRootIO::TargetRecoRootIO() 
  : RecoVRootIO(TString("Target"))
{
  fEvent = new TTargetRecoEvent();
  fBeam  = new TTargetRecoBeam();
  fFitEvent = new TTargetFitEvent();
  fClusColl = NULL;

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  std::cout << this->GetName()<<"::TargetRecoRootIO Initialized" << std::endl;

}
TargetRecoRootIO::~TargetRecoRootIO()
{;}

void TargetRecoRootIO::SaveEvent()
{
  //std::cout<<" in TargetRootIO::SaveEvent"<<std::endl;

  // Save  branch Target (collection of hits)
  RecoVRootIO::SaveEvent();

  PadmeVReconstruction* MyReco = (PadmeVReconstruction*) RecoRootIOManager::GetInstance()->GetReconstruction()->FindReco(this->GetName());
  TargetReconstruction* Reco = (TargetReconstruction*)MyReco;
  
  // Save branch TargetBeam (beam parameters from the Target)
  TTargetRecoBeam* tBeam=Reco->getRecoBeam();
  fBeam=tBeam;
  //std::cout<<" in TargetRootIO::SaveEvent after beam "<<std::endl;

  /*
  TTargetRecoEvent* myEvent = (TTargetRecoEvent*)fEvent;
  myEvent->setTargetRecoBeam(tBeam);  
  */

  // Save branch TargetFitEvent (WF fit parameters)
  SaveTargetFitEvent();
  //std::cout<<" in TargetRootIO::SaveEvent after fit"<<std::endl;

}
void TargetRecoRootIO::SaveTargetFitEvent(){
  
  //  std::cout<<this->GetName()<<"::SaveTargetFitEvent"<<std::endl;
  fFitEvent->Clear();
  //std::cout<<"fFitEvent  at <"<<fFitEvent<<"> in now cleared"<<std::endl;

  PadmeVReconstruction* MyReco = (PadmeVReconstruction*) RecoRootIOManager::GetInstance()->GetReconstruction()->FindReco(this->GetName());
  TargetReconstruction* Reco = (TargetReconstruction*)MyReco;
  //std::cout<<this->GetName()<<"::SaveTargetFitEvent ... pointer to TargetReconstruction retrieved"<<std::endl;
  
  vector<TTargetSignalFitParams *> fitPars = Reco->getSignalFitParams();
  //std::cout<<this->GetName()<<"::SaveTargetFitEvent ... from TargetReconstruction the size of fitParams is "<<fitPars.size()<<std::endl;
  for(unsigned int iHit = 0;iHit < fitPars.size();iHit++){
    fFitEvent->AddFitParSet(fitPars[iHit]);
    //std::cout<<this->GetName()<<"::SaveTargetFitEvent - transient version of TTargetSignalFitParams pushed back to fitEvent "<<iHit<<std::endl;
  }
}


void TargetRecoRootIO::NewRun(Int_t nRun, TFile* hfile){
  
  std::cout<<this->GetName()<<"::NewRun"<<std::endl;
  // Book  branch Target (collection of hits)
  RecoVRootIO::NewRun(nRun, hfile);
    
  std::cout<<this->GetName()<<"::NewRun - digit branch setup ... now moving to Target specific branches"<<std::endl;
  fEventTree = RecoRootIOManager::GetInstance()->GetEventTree();
  std::cout<<this->GetName()<<"::NewRun - pointer to the tree obtained: "<<fEventTree<<std::endl;

  
  // Book branch TargetBeam (beam parameters from the Target)
  fBranchTargetRecoBeam = fEventTree->Branch("TargetBeam", fBeam->IsA()->GetName(), &fBeam);
  std::cout<<this->GetName()<<"::NewRun - branch TargetBeam obtained"<<std::endl;
  fBranchTargetRecoBeam->SetAutoDelete(kFALSE);
  std::cout<<this->GetName()<<"::NewRun - branch TargetBeam autodelete set to false"<<std::endl;
  

  // Book branch TargetFitEvent (WF fit parameters)
  fBranchTargetFitEvent = fEventTree->Branch("TargetFitEvent", fFitEvent->IsA()->GetName(), &fFitEvent);
  std::cout<<this->GetName()<<"::NewRun - branch TargetFitEvent obtained"<<std::endl;
  fBranchTargetFitEvent->SetAutoDelete(kFALSE);
  std::cout<<this->GetName()<<"::NewRun - branch TargetFitEvent autodelete set to false"<<std::endl;
  
}
