#include "EVetoBLRecoRootIO.hh"
#include "RecoVRootIO.hh"

#include "TEVetoBLRecoEvent.hh"
#include "EVetoBLReconstruction.hh"
#include "TEVetoBLClusCollection.hh"

//#include "TRecoVHit.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"
#include <stdio.h>
#include "Riostream.h"


EVetoBLRecoRootIO::EVetoBLRecoRootIO() 
  : RecoVRootIO(TString("EVetoBL"))
{
  fEvent = new TEVetoBLRecoEvent();
  fClusColl = new TEVetoBLClusCollection();
  //  fClusColl = new TRecoVClusCollection(TRecoVCluster::Class());


  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  //  fHitsEnabled = true;
  std::cout << "EVetoBLRecoRootIO: Initialized" << std::endl;

}
EVetoBLRecoRootIO::~EVetoBLRecoRootIO()
{;}

