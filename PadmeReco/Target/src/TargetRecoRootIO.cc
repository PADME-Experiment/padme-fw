#include "TargetRecoRootIO.hh"
#include "RecoVRootIO.hh"

#include "TTargetRecoEvent.hh"
#include "TargetReconstruction.hh"
//#include "TRecoVHit.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"
#include <stdio.h>
#include "Riostream.h"


TargetRecoRootIO::TargetRecoRootIO() 
  : RecoVRootIO(TString("Target"))
{
  fEvent = new TTargetRecoEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  //  fHitsEnabled = true;
  std::cout << "TargetRecoRootIO: Initialized" << std::endl;

}
TargetRecoRootIO::~TargetRecoRootIO()
{;}

