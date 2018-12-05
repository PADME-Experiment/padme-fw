
#include "TSACRecoEvent.hh"

ClassImp(TSACRecoEvent)

TSACRecoEvent::TSACRecoEvent() : TRecoVObject(TRecoVHit::Class())
{
  fClusColl = new TRecoVClusCollection(TRecoVCluster::Class());
}

TSACRecoEvent::~TSACRecoEvent()
{;}

void TSACRecoEvent::Clear(Option_t* Opt)
{
  fNHits = 0;
  if(fHits) fHits->Clear(Opt);
  fClusColl->Clear(Opt);
}

