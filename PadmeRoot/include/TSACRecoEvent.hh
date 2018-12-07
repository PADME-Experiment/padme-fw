#ifndef TSACRecoEvent_H
#define TSACRecoEvent_H

#include "TRecoVObject.hh"
#include "TRecoVClusCollection.hh"

class TSACRecoEvent : public TRecoVObject {

public:

  TSACRecoEvent(); 
  ~TSACRecoEvent();

  
  void setClusCollection(TRecoVClusCollection* clusColl){fClusColl = clusColl;}
  TRecoVClusCollection* getClusCollection(){return fClusColl;}
  void Clear(Option_t* = "");
  Int_t GetNClusters(){return fClusColl->GetNElements();}
  TRecoVCluster* Cluster(Int_t i){return fClusColl->Element(i);} 
  
private:
  
  TRecoVClusCollection* fClusColl;

  ClassDef(TSACRecoEvent,1);
};
#endif
