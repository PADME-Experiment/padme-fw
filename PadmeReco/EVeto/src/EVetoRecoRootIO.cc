#include "EVetoRecoRootIO.hh"
#include "RecoVRootIO.hh"

#include "TEVetoRecoEvent.hh"
#include "EVetoReconstruction.hh"
//#include "TRecoVHit.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"
#include <stdio.h>
#include "Riostream.h"


EVetoRecoRootIO::EVetoRecoRootIO() 
  : RecoVRootIO(TString("EVeto"))
{
  fEvent = new TEVetoRecoEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  //  fHitsEnabled = true;
  std::cout << "EVetoRecoRootIO: Initialized" << std::endl;

}
EVetoRecoRootIO::~EVetoRecoRootIO()
{;}

