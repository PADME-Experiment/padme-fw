#include "EVetoRecoRootIO.hh"
#include "RecoVRootIO.hh"

#include "TEVetoRecoEvent.hh"
#include "EVetoReconstruction.hh"
#include "TEVetoClusCollection.hh"

//#include "TRecoVHit.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"
#include <stdio.h>
#include "Riostream.h"

EVetoRecoRootIO::EVetoRecoRootIO(RecoRootIOManager* rootMgr) 
  : RecoVRootIO(TString("EVeto"),rootMgr)
{
  fEvent = new TEVetoRecoEvent();
  fClusColl = new TEVetoClusCollection();
  //  fClusColl = new TRecoVClusCollection(TRecoVCluster::Class());


  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  //  fHitsEnabled = true;
  std::cout << "EVetoRecoRootIO: Initialized" << std::endl;

}
EVetoRecoRootIO::~EVetoRecoRootIO()
{;}

