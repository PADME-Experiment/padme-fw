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


SACRecoRootIO::SACRecoRootIO(RecoRootIOManager* rootMgr) 
  : RecoVRootIO(TString("SAC"),rootMgr)
{
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
