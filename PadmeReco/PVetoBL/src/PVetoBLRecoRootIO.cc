#include "PVetoBLRecoRootIO.hh"
#include "RecoVRootIO.hh"

#include "TPVetoBLRecoEvent.hh"
#include "PVetoBLReconstruction.hh"
#include "TPVetoBLClusCollection.hh"
//#include "TRecoVHit.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"
#include <stdio.h>
#include "Riostream.h"


PVetoBLRecoRootIO::PVetoBLRecoRootIO() 
  : RecoVRootIO(TString("PVetoBL"))
{
  fEvent = new TPVetoBLRecoEvent();
  fClusColl = new TPVetoBLClusCollection();
  //  fClusColl = new TRecoVClusCollection(TRecoVCluster::Class());

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  //  fHitsEnabled = true;
  std::cout << "PVetoBLRecoRootIO: Initialized" << std::endl;

}
PVetoBLRecoRootIO::~PVetoBLRecoRootIO()
{;}

