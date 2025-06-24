#ifndef ECalClusterHandler_h
#define ECalClusterHandler_h

#include "ECalCluster.hh"

#define CLUSTERHANDLER_N_MAX_CLUSTERS 20

class ECalClusterHandler {

 public :

  ECalClusterHandler();
  ~ECalClusterHandler();
  
 public :

  Int_t GetNClusters(){return fNClusters;};
  void Print();

  ECalCluster* NewCluster(); //Create a new cluster and return pointer
  ECalCluster* GetCluster(Int_t);

  // Erase results from previous cluster findings
  void ResetClusters();

 private:

  Int_t fNClusters; 
  ECalCluster* fClusterList[CLUSTERHANDLER_N_MAX_CLUSTERS];

};
#endif
