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
    for (UChar_t lgID = 0; lgID < N_LEADGLASS; lgID++){
    //UChar_t lgID = fLGReco->LeadGlassID();
    ((TLeadGlassRecoEvent*)fEvent)->SetPedestal(lgID,fLGReco->GetPedestal(lgID));
    ((TLeadGlassRecoEvent*)fEvent)->SetPedestalRMS(lgID,fLGReco->GetPedestalRMS(lgID));
    ((TLeadGlassRecoEvent*)fEvent)->SetTotalCharge(lgID,fLGReco->GetCharge(lgID));
    ((TLeadGlassRecoEvent*)fEvent)->SetTotalEnergy(lgID,fLGReco->GetEnergy(lgID));
    }
    ((TLeadGlassRecoEvent*)fEvent)->SetNPoTs(fLGReco->GetNPoTs());
    ((TLeadGlassRecoEvent*)fEvent)->SetBunchLength(fLGReco->GetBunchLength());
    ((TLeadGlassRecoEvent*)fEvent)->SetBunchBBQ(fLGReco->GetBunchBBQ());
  }

  RecoVRootIO::SaveEvent();

}
