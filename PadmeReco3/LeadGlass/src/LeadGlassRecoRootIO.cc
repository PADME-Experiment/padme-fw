#include <stdio.h>
#include "Riostream.h"

#include "RecoRootIOManager.hh"
#include "LeadGlassReconstruction.hh"
#include "TLeadGlassRecoEvent.hh"

#include "LeadGlassRecoRootIO.hh"

LeadGlassRecoRootIO::LeadGlassRecoRootIO() 
{

  fLeadGlassReconstruction = 0;

  fVerbose = 0;

  fLeadGlassEvent = new TLeadGlassRecoEvent();

  TTree::SetBranchStyle(1);

  std::cout << "LeadGlassRecoRootIO::LeadGlassRecoRootIO - LeadGlass RootIO system initialized" << std::endl;

}

LeadGlassRecoRootIO::~LeadGlassRecoRootIO()
{
  if (fVerbose) std::cout << "LeadGlassRecoRootIO::~LeadGlassRecoRootIO - Deleting LeadGlass RootIO system" << std::endl;
  if (fLeadGlassEvent) delete fLeadGlassEvent;
}

void LeadGlassRecoRootIO::NewRun()
{
  
  if (fVerbose) std::cout << "LeadGlassRecoRootIO::NewRun - Preparing event structure" << std::endl;

  if (fVerbose>1) std::cout << "LeadGlassRecoRootIO::NewRun - Preparing the branches in '" << fEventTree->GetName() << "' output TTree" << std::endl;
  if (fLeadGlassReconstruction->writeHits()){
    fBranchHitsColl = fEventTree->Branch("LeadGlass_Hits","TLeadGlassRecoEvent",&fLeadGlassEvent);
    fBranchHitsColl->SetAutoDelete(kFALSE);
    if (fVerbose>1) std::cout << "LeadGlassRecoRootIO::NewRun - Branch named LeadGlass_Hits prepared" << std::endl;
  }
  
}

void LeadGlassRecoRootIO::EndRun()
{
  if (fVerbose) std::cout << "LeadGlassRecoRootIO::EndRun - Executing End-of-Run procedure" << std::endl;  
}

void LeadGlassRecoRootIO::SaveEvent()
{

  if (fLeadGlassReconstruction->writeHits()){

    ((TLeadGlassRecoEvent*)fLeadGlassEvent)->Clear(); // Clear all LeadGlass info

    if (fLeadGlassReconstruction->LeadGlassFound()) {
      ((TLeadGlassRecoEvent*)fLeadGlassEvent)->SetPedestal(fLeadGlassReconstruction->GetPedestal());
      ((TLeadGlassRecoEvent*)fLeadGlassEvent)->SetPedestalRMS(fLeadGlassReconstruction->GetPedestalRMS());
      ((TLeadGlassRecoEvent*)fLeadGlassEvent)->SetTotalCharge(fLeadGlassReconstruction->GetCharge());
      ((TLeadGlassRecoEvent*)fLeadGlassEvent)->SetTotalEnergy(fLeadGlassReconstruction->GetEnergy());
      ((TLeadGlassRecoEvent*)fLeadGlassEvent)->SetNPoTs(fLeadGlassReconstruction->GetNPoTs());
      ((TLeadGlassRecoEvent*)fLeadGlassEvent)->SetBunchLength(fLeadGlassReconstruction->GetBunchLength());
      ((TLeadGlassRecoEvent*)fLeadGlassEvent)->SetBunchBBQ(fLeadGlassReconstruction->GetBunchBBQ());
    }

  }

}
