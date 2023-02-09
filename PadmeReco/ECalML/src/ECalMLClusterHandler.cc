#include "ECalMLClusterHandler.hh"

#include <iostream>
#include "cstdlib"
#include "math.h"

ECalMLClusterHandler::ECalMLClusterHandler()
{
  fNClusters=0;
}

ECalMLClusterHandler::~ECalMLClusterHandler()
{
  ResetClusters();
}

void ECalMLClusterHandler::ResetClusters()
{
  // Kill all existing clusters and reset counter
  for(Int_t kk=0; kk<fNClusters;kk++) delete fClusterList[kk];
  fNClusters = 0;
}

ECalMLCluster* ECalMLClusterHandler::NewCluster()
{
  if(fNClusters<CLUSTERHANDLER_N_MAX_CLUSTERS){
    ECalMLCluster* clu = new ECalMLCluster();
    fClusterList[fNClusters++] = clu;
    return clu;
  } else {
    std::cout << "WARNING - ClusterHandler::NewCluster - Too many clusters in event: " << fNClusters << std::endl;
    return 0;
  }
}

ECalMLCluster* ECalMLClusterHandler::GetCluster(Int_t i)
{
  if (i<0 || i>=fNClusters) {
    std::cout << "WARNING - ClusterHandler::GetCluster - Attept to access cluster outside list: " << i << std::endl;
    return 0;
  }
  return fClusterList[i];
}

void ECalMLClusterHandler::Print()
{
  std::cout << "Found " << fNClusters << " clusters:" << std::endl;
  for(Int_t kk=0;kk<fNClusters;kk++) fClusterList[kk]->Print();
}
