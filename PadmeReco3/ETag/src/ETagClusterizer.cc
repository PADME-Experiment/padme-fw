#include "Riostream.h"

#include "PadmeVRecoConfig.hh"

#include "ETagHit.hh"
#include "ETagCluster.hh"

#include "ETagClusterizer.hh"

ETagClusterizer::ETagClusterizer(PadmeVRecoConfig* cfg)
  : fETagConfig(cfg)
{

  // Define verbose level
  fVerbose = fETagConfig->GetParOrDefault("Output", "Verbose", 0);

  if (fVerbose) printf("ETagClusterizer::ETagClusterizer - ETag clusterization system created\n");
}

ETagClusterizer::~ETagClusterizer()
{
  if (fVerbose) printf("ETagClusterizer::~ETagClusterizer - Deleting ETag clusterization system\n");
}

void ETagClusterizer::Init()
{
  if (fVerbose) printf("ETagClusterizer::Init - Initilizing ETag clusterizer\n");
}

Bool_t ETagClusterizer::BuildClusters(vector<ETagHit*>& hits,vector<ETagCluster*>& clusters)
{
  if (fVerbose>1) printf("ETagClusterizer::BuildClusters - Building ETag clusters from ETag hits\n");

  ETagCluster* clus = new ETagCluster();
  clus->SetStatus(0x0000000000000003);
  clus->SetPosition(TVector3(1.,1.,2.));
  clus->SetEnergy(324.3);
  clus->SetTime(23456.456);
  clus->SetSeed(0);
  clus->SetNHits(1);
  std::vector<Int_t> v;
  v.push_back(0);
  clus->SetHitsVector(v);
  clusters.push_back(clus);
  if (fVerbose>3) clus->Print();

  return true;
}
