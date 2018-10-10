#include "HEPVetoRecoRootIO.hh"
#include "RecoVRootIO.hh"

#include "THEPVetoRecoEvent.hh"
#include "HEPVetoReconstruction.hh"
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

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  //  fHitsEnabled = true;
  std::cout << "HEPVetoRecoRootIO: Initialized" << std::endl;

}
HEPVetoRecoRootIO::~HEPVetoRecoRootIO()
{;}

