#ifndef ECalClusterizer_h
#define ECalClusterizer_h 1

#include <Rtypes.h>

class PadmeVRecoConfig;
class ECalHit;
class ECalCluster;

class ECalClusterizer
{

public:

  ECalClusterizer(PadmeVRecoConfig*);
  ~ECalClusterizer();

  void Init();

  Int_t GetVerbose() { return fVerbose; }
  void  SetVerbose(Int_t v) { fVerbose = v; }

  Bool_t BuildClusters(vector<ECalHit*>&,vector<ECalCluster*>&);

private:

  Int_t fVerbose;

  PadmeVRecoConfig* fECalConfig;

};
#endif // ECalClusterizer_h
