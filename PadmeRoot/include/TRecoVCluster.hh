// --------------------------------------------------------------
#ifndef TRecoVCluster_H
#define TRecoVCluster_H

#include "TObject.h"
#include "TVector3.h"
#include "TMCVHit.hh"


class TRecoVCluster : public TMCVHit
{

public:

  TRecoVCluster();
  virtual ~TRecoVCluster(){};
  void SetNHitsInClus(Int_t nh) { fNHitsInClus=nh; }
  Int_t GetNHitsInClus() { return fNHitsInClus; }
  Int_t GetSeed() { return fSeed; }
  void SetSeed(Int_t i) { fSeed=i; }
  void SetHitVecInClus(std::vector<Int_t> v) { fHitVecInClus=v; }
  std::vector<Int_t> GetHitVecInClus() { return fHitVecInClus; }
  
  void setStatus(ULong64_t status) { fStatus = status; }
  ULong64_t getStatus() { return fStatus; }

private: 
  Int_t fSeed; // index of hit selected as seed of this cluster
  Int_t fNHitsInClus;  // nHits in Cluster 
  std::vector<Int_t> fHitVecInClus; // vector of indices of hits belonging to this cluster
  ULong64_t fStatus; // status of cluster

public:
 
  ClassDef(TRecoVCluster,2);
};
#endif
