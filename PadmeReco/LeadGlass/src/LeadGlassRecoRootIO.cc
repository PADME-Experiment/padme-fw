#include "LeadGlassRecoRootIO.hh"

//#include "LeadGlassReconstruction.hh"

#include <stdio.h>
#include "Riostream.h"

LeadGlassRecoRootIO::LeadGlassRecoRootIO() 
  : RecoVRootIO(TString("LeadGlass"))
{
  fEvent = new TLeadGlassRecoEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  std::cout << "LeadGlassRecoRootIO: Initialized" << std::endl;
}

LeadGlassRecoRootIO::~LeadGlassRecoRootIO()
{;}
