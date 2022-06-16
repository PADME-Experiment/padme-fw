#include "ETagRecoRootIO.hh"
#include "RecoVRootIO.hh"

#include "TETagRecoEvent.hh"
#include "ETagReconstruction.hh"
//#include "TETagClusCollection.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"
#include <stdio.h>
#include "Riostream.h"


ETagRecoRootIO::ETagRecoRootIO() 
  : RecoVRootIO(TString("ETag"))
{
  fEvent = new TETagRecoEvent();
  //fClusColl = new TETagClusCollection();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  //  fHitsEnabled = true;
  std::cout << "ETagRecoRootIO: Initialized" << std::endl;

}
ETagRecoRootIO::~ETagRecoRootIO()
{;}
