#include "HEPVetoRecoRootIO.hh"
#include "RecoVRootIO.hh"

#include "HEPVetoReconstruction.hh"
#include "THEPVetoRecoEvent.hh"
#include "THEPVetoClusCollection.hh"

//#include "TRecoVHit.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"
#include <stdio.h>
#include "Riostream.h"


HEPVetoRecoRootIO::HEPVetoRecoRootIO() 
  : RecoVRootIO(TString("HEPVeto"))
{
  fEvent = new THEPVetoRecoEvent();
  fClusColl = new THEPVetoClusCollection();
  //fClusColl = new TRecoVClusCollection(TRecoVCluster::Class());

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  //  fHitsEnabled = true;
  std::cout << "HEPVetoRecoRootIO: Initialized" << std::endl;

}
HEPVetoRecoRootIO::~HEPVetoRecoRootIO()
{;}

