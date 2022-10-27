#include "TLeadGlassRecoEvent.hh"

ClassImp(TLeadGlassRecoEvent)

TLeadGlassRecoEvent::TLeadGlassRecoEvent() : TRecoVObject(TRecoVHit::Class())
{
  fLGPedestal = 0.;
  fLGPedestalRMS = 0.;
  fLGTotalCharge = 0.;
  fLGTotalEnergy = 0.;
  fLGNPoTs = 0.;
  fLGBunchLength = 0.;
}

TLeadGlassRecoEvent::~TLeadGlassRecoEvent()
{;}
