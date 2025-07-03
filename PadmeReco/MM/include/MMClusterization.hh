// --------------------------------------------------------------
// History:
//
// --------------------------------------------------------------
#ifndef MMClusterization_H
#define MMClusterization_H

#include "PadmeVClusterization.hh"

class MMClusterization : public PadmeVClusterization
{
public:
  //  virtual void Init(PadmeVRecoConfig *cfg); // also called in PadmeVReconstruction once fClusterization in allocated
  void PrintConfig();
  void Reconstruct(std::vector<TRecoVHit *> &hitArray, std::vector<TRecoVCluster *> &clArray); // called in PadmeVReconstruction once fClusterization in allocated in BuildHits and in BuildClusters
  //  virtual Int_t IsSeedNeig(Int_t seedID, Int_t cellID);
private:
// double fClusterDtMax        ;IN PadmeVClusterization is used in Reconstruct
// double fClusterDcellMax     ;IN PadmeVClusterization is used in IsSeedNeig
// double fClusterEthrForHit   ;IN PadmeVClusterization is used in Reconstruct, removeOutOfTimeHits
// double fClusterEthrForSeed  ;IN PadmeVClusterization is used in findSeed called from Reconstruct
// double fClusterTLowForHit   ;IN PadmeVClusterization is used in removeOutOfTimeHits
// double fClusterTHighForHit  ;IN PadmeVClusterization is used in removeOutOfTimeHits
};
#endif
