// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
// Modified by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-21
//   - added MCDigi structure
//
// --------------------------------------------------------------

#include "TMCVEvent.hh"

ClassImp(TMCVEvent)

TMCVEvent::TMCVEvent()
{

    fRunNumber   = -1;
    fEventNumber = -1;

    fNHits = 0;
    fHits  = 0;

    fNDigi = 0;
    fDigi  = 0;

}

TMCVEvent::TMCVEvent(TClass* hCls, TClass* dCls)
{

    fRunNumber   = -1;
    fEventNumber = -1;

    fNHits = 0;
    fHits  = new TClonesArray(hCls,1000);

    fNDigi = 0;
    fDigi  = new TClonesArray(dCls,1000);

}

TMCVEvent::~TMCVEvent()
{
  if (fHits) delete fHits;
  if (fDigi) delete fDigi;
}

void TMCVEvent::Clear(Option_t * option)
{

  fNHits = 0;
  if(fHits) fHits->Clear(option);

  fNDigi = 0;
  if(fDigi) fDigi->Clear(option);

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

TMCVDigi* TMCVEvent::AddDigi()
{
  return (TMCVDigi*)(fDigi->New(fNDigi++));
}

TMCVDigi* TMCVEvent::AddDigi(TMCVDigi* mcDigi)
{
  TMCVDigi* digi = (TMCVDigi*)(fDigi->New(fNDigi++));
  *digi = *mcDigi;
  return digi;
}

TMCVDigi* TMCVEvent::Digi(Int_t iDigi)
{
  return (TMCVDigi*)fDigi->At(iDigi);
}

TMCVDigi* TMCVEvent::LastDigi()
{
  return (TMCVDigi*)fDigi->At(fNDigi-1);
}

void TMCVEvent::RemoveDigi(Int_t iDigi)
{
    fDigi->RemoveAt(iDigi);
    fDigi->Compress();
    fNDigi--;
}
