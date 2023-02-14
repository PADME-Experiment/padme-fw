#ifndef PADMEV_CLUSTERIZATION_H
#define PADMEV_CLUSTERIZATION_H

#include "ChannelVReco.hh"
#include "utlConfigParser.hh"

class TRecoVHit;
class TRecoVCluster;

class PadmeVClusterization {
public:
  PadmeVClusterization(){;};
  virtual ~PadmeVClusterization(){;};

  virtual void Init(PadmeVRecoConfig *cfg);
  virtual void PrintConfig();
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray, std::vector<TRecoVCluster *> &clArray);
  std::vector<TRecoVHit *> removeOutOfTimeHits(std::vector<TRecoVHit *> &Hits);
  virtual Int_t IsSeedNeig(Int_t seedID, Int_t cellID);
  virtual Int_t findSeed(std::vector<TRecoVHit *> &hitArray);

  
protected:
  // configuration parameters
  Double_t fClusterDtMax;
  Int_t    fClusterDcellMax;
  Double_t fClusterEthrForHit;
  Double_t fClusterEthrForSeed;
  Double_t fClusterTLowForHit; 
  Double_t fClusterTHighForHit;
  
  std::vector<Int_t> fhUsed;

};
#endif
