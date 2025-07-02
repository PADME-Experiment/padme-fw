#include "MMRecoRootIO.hh"

#include "RecoRootIOManager.hh"
#include "MMReconstruction.hh"

#include "TMMRecoEvent.hh"

#include <stdio.h>
#include "Riostream.h"

MMRecoRootIO::MMRecoRootIO() 
  : RecoVRootIO(TString("MM"))
{
  fEvent = new TMMRecoEvent();
  //  fClusColl = new TMMClusCollection();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  std::cout << "MMRecoRootIO: Initialized" << std::endl;

  fMMReco = 0;
}

MMRecoRootIO::~MMRecoRootIO()
{;}

void MMRecoRootIO::SaveEvent()
{


  // IF GLOBAL RECO QUANTITIES ARE TO BE KEPT, ADD HERE
  //  if (! fMMReco) fMMReco = (MMReconstruction*)RecoRootIOManager::GetInstance()->GetReconstruction()->FindReco("MM");
  //  if (fMMReco->MMFound()) {
  //    ((TMMRecoEvent*)fEvent)->SetPedestal(fMMReco->GetPedestal()); 
  //  }

  RecoVRootIO::SaveEvent();

}
