#include "LeadGlassRecoRootIO.hh"

#include "RecoRootIOManager.hh"
#include "LeadGlassReconstruction.hh"

#include "TLeadGlassRecoEvent.hh"

#include <stdio.h>
#include "Riostream.h"

LeadGlassRecoRootIO::LeadGlassRecoRootIO() 
  : RecoVRootIO(TString("LeadGlass"))
{
  fEvent = new TLeadGlassRecoEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  std::cout << "LeadGlassRecoRootIO: Initialized" << std::endl;

  fLGReco = 0;
}

LeadGlassRecoRootIO::~LeadGlassRecoRootIO()
{;}

void LeadGlassRecoRootIO::SaveEvent()
{

  ((TLeadGlassRecoEvent*)fEvent)->Clear(); // Clear all LeadGlass info

  if (! fLGReco) fLGReco = (LeadGlassReconstruction*)RecoRootIOManager::GetInstance()->GetReconstruction()->FindReco("LeadGlass");

  if (fLGReco->LeadGlassFound()) {
    ((TLeadGlassRecoEvent*)fEvent)->SetPedestal(fLGReco->GetPedestal());
    ((TLeadGlassRecoEvent*)fEvent)->SetPedestalRMS(fLGReco->GetPedestalRMS());
    ((TLeadGlassRecoEvent*)fEvent)->SetTotalCharge(fLGReco->GetCharge());
    ((TLeadGlassRecoEvent*)fEvent)->SetTotalEnergy(fLGReco->GetEnergy());
    ((TLeadGlassRecoEvent*)fEvent)->SetNPoTs(fLGReco->GetNPoTs());
    ((TLeadGlassRecoEvent*)fEvent)->SetBunchLength(fLGReco->GetBunchLength());
    ((TLeadGlassRecoEvent*)fEvent)->SetBunchBBQ(fLGReco->GetBunchBBQ());
  }

  RecoVRootIO::SaveEvent();

}
