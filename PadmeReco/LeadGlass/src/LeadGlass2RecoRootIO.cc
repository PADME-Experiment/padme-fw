#include "LeadGlass2RecoRootIO.hh"

#include "RecoRootIOManager.hh"
#include "LeadGlassReconstruction.hh"

#include "TLeadGlass2RecoEvent.hh"

#include <stdio.h>
#include "Riostream.h"

LeadGlass2RecoRootIO::LeadGlass2RecoRootIO() 
  : RecoVRootIO(TString("LeadGlass2"))
{
  fEvent = new TLeadGlass2RecoEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  std::cout << "LeadGlass2RecoRootIO: Initialized" << std::endl;

  fLGReco = 0;
}

LeadGlass2RecoRootIO::~LeadGlass2RecoRootIO()
{;}

void LeadGlass2RecoRootIO::SaveEvent()
{

  ((TLeadGlass2RecoEvent*)fEvent)->Clear(); // Clear all LeadGlass2 info

  if (! fLGReco) fLGReco = (LeadGlassReconstruction*)RecoRootIOManager::GetInstance()->GetReconstruction()->FindReco("LeadGlass");

  if (fLGReco->LeadGlass2Found()) {
    ((TLeadGlass2RecoEvent*)fEvent)->SetPedestal(fLGReco->GetPedestal(1));
    ((TLeadGlass2RecoEvent*)fEvent)->SetPedestalRMS(fLGReco->GetPedestalRMS(1));
    ((TLeadGlass2RecoEvent*)fEvent)->SetStartIndexCell(fLGReco->GetStartIndexCell(1));
    ((TLeadGlass2RecoEvent*)fEvent)->SetTotalCharge(fLGReco->GetCharge(1));
    ((TLeadGlass2RecoEvent*)fEvent)->SetTotalEnergy(fLGReco->GetEnergy(1));
    ((TLeadGlass2RecoEvent*)fEvent)->SetNPoTs(0);
    ((TLeadGlass2RecoEvent*)fEvent)->SetBunchLength(0.);
    ((TLeadGlass2RecoEvent*)fEvent)->SetBunchBBQ(0.);
  }

  RecoVRootIO::SaveEvent();

}
