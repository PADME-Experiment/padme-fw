#ifndef PADMEV_CLUSTERIZATION_H
#define PADMEV_CLUSTERIZATION_H

#include "ChannelVReco.hh"
#include "utlConfigParser.hh"

class TRecoVHit;
class TRecoVCluster;

class ETagSimpleClusterization{
public:
  ETagSimpleClusterization(){;};
  virtual ~ETagSimpleClusterization(){;};

  virtual void Init(PadmeVRecoConfig *cfg);
  virtual void PrintConfig();
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray, std::vector<TRecoVCluster *> &clArray);
  std::vector<TRecoVHit *> removeOutOfTimeHits(std::vector<TRecoVHit *> &Hits);
  
protected:
  // configuration parameters
  Double_t fClusterDtMax;
  Int_t    fClusterDcellMax;
  Double_t fClusterEthrForHit;
  Double_t fClusterTLowForHit; 
  Double_t fClusterTHighForHit;
  
};
#endif
