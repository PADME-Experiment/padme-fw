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
    ((TLeadGlassRecoEvent*)fEvent)->SetPedestal(fLGReco->GetPedestal(0));
    ((TLeadGlassRecoEvent*)fEvent)->SetPedestalRMS(fLGReco->GetPedestalRMS(0));    
    ((TLeadGlassRecoEvent*)fEvent)->SetPedestalV2(fLGReco->GetPedestalV2(0));
    ((TLeadGlassRecoEvent*)fEvent)->SetPedestalRMSV2(fLGReco->GetPedestalRMSV2(0));
    ((TLeadGlassRecoEvent*)fEvent)->SetStartIndexCell(fLGReco->GetStartIndexCell(0));
    ((TLeadGlassRecoEvent*)fEvent)->SetTotalCharge(fLGReco->GetCharge(0));
    ((TLeadGlassRecoEvent*)fEvent)->SetTotalChargeV2(fLGReco->GetChargeV2(0));
    ((TLeadGlassRecoEvent*)fEvent)->SetTotalChargeWPed(fLGReco->GetChargeWPed(0));
    ((TLeadGlassRecoEvent*)fEvent)->SetTotalEnergy(fLGReco->GetEnergy(0));
    ((TLeadGlassRecoEvent*)fEvent)->SetNPoTs(fLGReco->GetNPoTs());
    ((TLeadGlassRecoEvent*)fEvent)->SetBunchLength(fLGReco->GetBunchLength());
    ((TLeadGlassRecoEvent*)fEvent)->SetBunchBBQ(fLGReco->GetBunchBBQ());
  }

  RecoVRootIO::SaveEvent();

}
