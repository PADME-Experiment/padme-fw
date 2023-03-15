#include "Riostream.h"

#include "PadmeVRecoConfig.hh"

#include "ECalHit.hh"
#include "ECalCluster.hh"

#include "ECalClusterizer.hh"

ECalClusterizer::ECalClusterizer(PadmeVRecoConfig* cfg)
  : fECalConfig(cfg)
{

  // Define verbose level
  fVerbose = fECalConfig->GetParOrDefault("Output", "Verbose", 0);

  if (fVerbose) printf("ECalClusterizer::ECalClusterizer - ECal clusterization system created\n");
}

ECalClusterizer::~ECalClusterizer()
{
  if (fVerbose) printf("ECalClusterizer::~ECalClusterizer - Deleting ECal clusterization system\n");
}

void ECalClusterizer::Init()
{
  if (fVerbose) printf("ECalClusterizer::Init - Initilizing ECal clusterizer\n");
}

Bool_t ECalClusterizer::BuildClusters(vector<ECalHit*>& hits,vector<ECalCluster*>& clusters)
{
  if (fVerbose>1) printf("ECalClusterizer::BuildClusters - Building ECal clusters from ECal hits\n");
  return true;
}
