// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------

#include "TMCVEvent.hh"

ClassImp(TMCVEvent)

TMCVEvent::TMCVEvent()
{

    fRunID   = -1;
    fEventID = -1;

    fNHits = 0;
    fHits  = 0;

}

TMCVEvent::TMCVEvent(TClass* cls)
{

    fRunID   = -1;
    fEventID = -1;

    fNHits = 0;
    fHits  = new TClonesArray(cls,1000);

}

TMCVEvent::~TMCVEvent()
{
  if (fHits) delete fHits;
}

void TMCVEvent::Clear(Option_t * option)
{
  fNHits = 0;
  if(fHits) fHits->Clear(option);
}

TMCVHit* TMCVEvent::AddHit()
{
  return (TMCVHit*)(fHits->New(fNHits++));
}

TMCVHit* TMCVEvent::AddHit(TMCVHit* mcHit)
{
  TMCVHit* hit = (TMCVHit*)(fHits->New(fNHits++));
  *hit = *mcHit;
  return hit;
}

TMCVHit* TMCVEvent::Hit(Int_t iHit)
{
  return (TMCVHit*)fHits->At(iHit);
}

TMCVHit* TMCVEvent::LastHit()
{
  return (TMCVHit*)fHits->At(fNHits-1);
}

void TMCVEvent::RemoveHit(Int_t iHit)
{
    fHits->RemoveAt(iHit);
    fHits->Compress();
    fNHits--;
}
