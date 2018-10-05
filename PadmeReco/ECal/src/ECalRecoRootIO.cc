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


ECalRecoRootIO::ECalRecoRootIO() 
  : RecoVRootIO(TString("ECal"))
{
  fEvent = new TECalRecoEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  //  fHitsEnabled = true;
  std::cout << "ECalRecoRootIO: Initialized" << std::endl;

}
ECalRecoRootIO::~ECalRecoRootIO()
{;}

