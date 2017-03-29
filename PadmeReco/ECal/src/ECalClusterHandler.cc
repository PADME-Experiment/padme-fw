#include "ECalClusterHandler.hh"

#include <iostream>
#include "cstdlib"
#include "math.h"

ECalClusterHandler::ECalClusterHandler()
{
  fNClusters=0;
}

ECalClusterHandler::~ECalClusterHandler()
{
  ResetClusters();
}

void ECalClusterHandler::ResetClusters()
{
  // Kill all existing clusters and reset counter
  for(Int_t kk=0; kk<fNClusters;kk++) delete fClusterList[kk];
  fNClusters = 0;
}

ECalCluster* ECalClusterHandler::NewCluster()
{
  if(fNClusters<CLUSTERHANDLER_N_MAX_CLUSTERS){
    ECalCluster* clu = new ECalCluster();
    fClusterList[fNClusters++] = clu;
    return clu;
  } else {
    std::cout << "WARNING - ClusterHandler::NewCluster - Too many clusters in event: " << fNClusters << std::endl;
    return 0;
  }
}

ECalCluster* ECalClusterHandler::GetCluster(Int_t i)
{
  if (i<0 || i>=fNClusters) {
    std::cout << "WARNING - ClusterHandler::GetCluster - Attept to access cluster outside list: " << i << std::endl;
    return 0;
  }
  return fClusterList[i];
}

void ECalClusterHandler::Print()
{
  std::cout << "Found " << fNClusters << " clusters:" << std::endl;
  for(Int_t kk=0;kk<fNClusters;kk++) fClusterList[kk]->Print();
}
