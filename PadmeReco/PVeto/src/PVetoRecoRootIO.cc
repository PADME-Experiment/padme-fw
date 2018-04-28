#include "PVetoRecoRootIO.hh"
#include "RecoVRootIO.hh"

#include "TPVetoRecoEvent.hh"
#include "PVetoReconstruction.hh"
//#include "TRecoVHit.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"
#include <stdio.h>
#include "Riostream.h"


PVetoRecoRootIO::PVetoRecoRootIO() 
  : RecoVRootIO(TString("PVeto"))
{
  fEvent = new TPVetoRecoEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  //  fHitsEnabled = true;
  std::cout << "PVetoRecoRootIO: Initialized" << std::endl;

}
PVetoRecoRootIO::~PVetoRecoRootIO()
{;}

void PVetoRecoRootIO::Close()
{;}


void PVetoRecoRootIO::NewRun(Int_t nRun, TFile* hfile)
{
  fRunNumber = nRun;

  if (fVerbose>=2) 
    std::cout << "PVetoRootIO: Preparing event structure" << std::endl;
  // Create branch to hold PVeto Hits and Digis for this run
  fEventTree = RecoRootIOManager::GetInstance()->GetEventTree();
  fPVetoBranch = fEventTree->Branch("PVeto", fEvent->IsA()->GetName(), &fEvent);
  fPVetoBranch->SetAutoDelete(kFALSE);

}

void PVetoRecoRootIO::EndRun()
{
  std::cout << "PVetoRecoRootIO: Executing End-of-Run procedure" << std::endl;
}



void PVetoRecoRootIO::SaveEvent()
{
  //Clear the event before saving anything
  fEvent->Clear();
  PVetoReconstruction* MyReco = (PVetoReconstruction*) RecoRootIOManager::GetInstance()->GetReconstruction()->FindReco(this->GetName());
  
  vector<TRecoVHit *> Hits = MyReco->GetRecoHits();
  for(int iHit = 0;iHit < Hits.size();iHit++){
    fEvent->AddHit(Hits[iHit]);
  }
}


