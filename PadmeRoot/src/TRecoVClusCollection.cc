#include "TRecoVClusCollection.hh"

#include "TRecoVCluster.hh"
#include "Riostream.h"

ClassImp(TRecoVClusCollection)

TRecoVClusCollection::TRecoVClusCollection(){
    fNClus = 0;
    fClus  = 0;
  ;
}

TRecoVClusCollection::TRecoVClusCollection(TClass* hCls) {
  fNClus = 0;
  fClus  = new TClonesArray(hCls,1000);
}


void TRecoVClusCollection::Print(Option_t *) const {
  TRecoVCluster* hit = NULL;
  for (Int_t ih=0; ih<fNClus; ++ih)
	 { 
	   //hit = this->Hit(ih);
	    hit = (TRecoVCluster*)fClus->At(ih);
	    std::cout<<GetName()<<" hit id, posx, posy, posz = "<<hit->GetChannelId()<<" "<<hit->GetPosition().x()<<" "<<hit->GetPosition().y()<<" "<<hit->GetPosition().z()<<" energy "<<hit->GetEnergy()<<" time "<<hit->GetTime()<<std::endl;
	 }
;
}
TRecoVClusCollection::~TRecoVClusCollection(){
  if (fClus) delete fClus;
}


void TRecoVClusCollection::Clear(Option_t * option)
{
  fNClus = 0;
  if(fClus) fClus->Clear(option);
}

TRecoVCluster* TRecoVClusCollection::AddElement()
{
  return (TRecoVCluster*)(fClus->New(fNClus++));
}

TRecoVCluster* TRecoVClusCollection::AddElement(TRecoVCluster* mcHit)
{
  TRecoVCluster* hit = (TRecoVCluster*)(fClus->New(fNClus++));
  *hit = *mcHit;
  return hit;
}

TRecoVCluster* TRecoVClusCollection::Element(Int_t iEl)
{
  return (TRecoVCluster*)fClus->At(iEl);
}

TRecoVCluster* TRecoVClusCollection::LastElement()
{
  return (TRecoVCluster*)fClus->At(fNClus-1);
}

void TRecoVClusCollection::RemoveElement(Int_t iEl)
{
    fClus->RemoveAt(iEl);
    fClus->Compress();
    fNClus--;
}
