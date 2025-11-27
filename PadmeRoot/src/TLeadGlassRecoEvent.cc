#include "TLeadGlassRecoEvent.hh"

ClassImp(TLeadGlassRecoEvent)

TLeadGlassRecoEvent::TLeadGlassRecoEvent() : TRecoVObject(TRecoVHit::Class())
{
  Clear();
}

TLeadGlassRecoEvent::~TLeadGlassRecoEvent()
{;}

void TLeadGlassRecoEvent::Clear()
{
  fLGPedestal[N_LEADGLASS] = 0.;
  fLGPedestalRMS[N_LEADGLASS] = 0.;
  fLGTotalCharge[N_LEADGLASS] = 0.;
  fLGTotalEnergy[N_LEADGLASS] = 0.;
  fLGNPoTs = 0.;
  fLGBunchLength = 0.;
  fLGBunchBBQ = 0.;
}
