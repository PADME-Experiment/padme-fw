#include "ECalRecoRootIO.hh"
#include "RecoVRootIO.hh"

#include "TECalRecoEvent.hh"
#include "ECalReconstruction.hh"
//#include "TRecoVHit.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"
#include <stdio.h>
#include "Riostream.h"
//#include "TRecoVClusCollection.hh"
#include "TECalClusCollection.hh"
#include "TRecoVCluster.hh"


ECalRecoRootIO::ECalRecoRootIO() 
  : RecoVRootIO(TString("ECal"))
{
  fEvent = new TECalRecoEvent();
  fClusColl = new TECalClusCollection();
  //fClusColl = new TRecoVClusCollection(TRecoVCluster::Class());

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  //  fHitsEnabled = true;
  std::cout << "ECalRecoRootIO: Initialized" << std::endl;

}
ECalRecoRootIO::~ECalRecoRootIO()
{;}

