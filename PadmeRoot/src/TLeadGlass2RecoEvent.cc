#include "TLeadGlass2RecoEvent.hh"

ClassImp(TLeadGlass2RecoEvent)

TLeadGlass2RecoEvent::TLeadGlass2RecoEvent() : TRecoVObject(TRecoVHit::Class())
{
  Clear();
}

TLeadGlass2RecoEvent::~TLeadGlass2RecoEvent()
{;}

void TLeadGlass2RecoEvent::Clear()
{
  fLGPedestal = 0.;
  fLGPedestalRMS = 0.;
  fLGPedestal_v2 = 0.;
  fLGPedestalRMS_v2 = 0.;
  fLGStartIndexCell = 0;
  fLGTotalCharge = 0.;
  fLGTotalCharge_v2 = 0.;
  fLGTotalChargeWPed = 0.;
  fLGTotalEnergy = 0.;
  fLGNPoTs = 0.;
  fLGBunchLength = 0.;
  fLGBunchBBQ = 0.;
}
