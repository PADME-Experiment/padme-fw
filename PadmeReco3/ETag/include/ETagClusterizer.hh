#ifndef ETagClusterizer_h
#define ETagClusterizer_h 1

#include <Rtypes.h>

class PadmeVRecoConfig;
class ETagHit;
class ETagCluster;

class ETagClusterizer
{

public:

  ETagClusterizer(PadmeVRecoConfig*);
  ~ETagClusterizer();

  void Init();

  Int_t GetVerbose() { return fVerbose; }
  void  SetVerbose(Int_t v) { fVerbose = v; }

  Bool_t BuildClusters(vector<ETagHit*>&,vector<ETagCluster*>&);

private:

  Int_t fVerbose;

  PadmeVRecoConfig* fETagConfig;

};
#endif // ETagClusterizer_h
