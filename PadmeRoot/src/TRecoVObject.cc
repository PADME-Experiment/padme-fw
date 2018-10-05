#include "TRecoVObject.hh"

#include "Riostream.h"

ClassImp(TRecoVObject)

TRecoVObject::TRecoVObject(){
    fNHits = 0;
    fHits  = 0;
  ;
}

TRecoVObject::TRecoVObject(TClass* hCls) {
  fNHits = 0;
  fHits  = new TClonesArray(hCls,1000);
}


void TRecoVObject::Print(Option_t *) const {
  ;
}
TRecoVObject::~TRecoVObject(){
  if (fHits) delete fHits;
}


void TRecoVObject::Clear(Option_t * option)
{

  fNHits = 0;
  if(fHits) fHits->Clear(option);
}

TRecoVHit* TRecoVObject::AddHit()
{
  return (TRecoVHit*)(fHits->New(fNHits++));
}

TRecoVHit* TRecoVObject::AddHit(TRecoVHit* mcHit)
{
  TRecoVHit* hit = (TRecoVHit*)(fHits->New(fNHits++));
  *hit = *mcHit;
  return hit;
}

TRecoVHit* TRecoVObject::Hit(Int_t iHit)
{
  return (TRecoVHit*)fHits->At(iHit);
}

TRecoVHit* TRecoVObject::LastHit()
{
  return (TRecoVHit*)fHits->At(fNHits-1);
}

void TRecoVObject::RemoveHit(Int_t iHit)
{
    fHits->RemoveAt(iHit);
    fHits->Compress();
    fNHits--;
}
