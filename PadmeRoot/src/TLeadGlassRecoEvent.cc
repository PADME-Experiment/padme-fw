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
  for (UChar_t lgID = 0; lgID < N_LEADGLASS; lgID++){
    fLGPedestal[lgID] = 0.;
    fLGPedestalRMS[lgID] = 0.;
    fLGStartIndexCell[lgID] = 0;
    fLGTotalCharge[lgID] = 0.;
    fLGTotalEnergy[lgID] = 0.;
  }
  fLGNPoTs = 0.;
  fLGBunchLength = 0.;
  fLGBunchBBQ = 0.;
}
