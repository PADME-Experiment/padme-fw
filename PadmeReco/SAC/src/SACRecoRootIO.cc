#include "SACRecoRootIO.hh"
#include "RecoVRootIO.hh"

#include "TSACRecoEvent.hh"
#include "SACReconstruction.hh"
//#include "TRecoVHit.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"
#include <stdio.h>
#include "Riostream.h"


SACRecoRootIO::SACRecoRootIO() 
  : RecoVRootIO(TString("SAC"))
{
  fEvent = new TSACRecoEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  //  fHitsEnabled = true;
  std::cout << "SACRecoRootIO: Initialized" << std::endl;

}
SACRecoRootIO::~SACRecoRootIO()
{;}

