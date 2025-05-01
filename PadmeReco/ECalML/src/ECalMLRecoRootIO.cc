#include "ECalMLRecoRootIO.hh"
#include "RecoVRootIO.hh"

#include "TECalMLRecoEvent.hh"
#include "ECalMLReconstruction.hh"
//#include "TRecoVHit.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"
#include <stdio.h>
#include "Riostream.h"
//#include "TRecoVClusCollection.hh"
#include "TECalMLClusCollection.hh"
#include "TRecoVCluster.hh"


ECalMLRecoRootIO::ECalMLRecoRootIO() 
  : RecoVRootIO(TString("ECalML"))
{
  fEvent = new TECalMLRecoEvent();
  fClusColl = new TECalMLClusCollection();
  //fClusColl = new TRecoVClusCollection(TRecoVCluster::Class());

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  //  fHitsEnabled = true;
  std::cout << "ECalMLRecoRootIO: Initialized" << std::endl;

}
ECalMLRecoRootIO::~ECalMLRecoRootIO()
{;}

