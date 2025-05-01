#ifndef ECalMLClusterHandler_h
#define ECalMLClusterHandler_h

#include "ECalMLCluster.hh"

#define CLUSTERHANDLER_N_MAX_CLUSTERS 20

class ECalMLClusterHandler {

 public :

  ECalMLClusterHandler();
  ~ECalMLClusterHandler();
  
 public :

  Int_t GetNClusters(){return fNClusters;};
  void Print();

  ECalMLCluster* NewCluster(); //Create a new cluster and return pointer
  ECalMLCluster* GetCluster(Int_t);

  // Erase results from previous cluster findings
  void ResetClusters();

 private:

  Int_t fNClusters; 
  ECalMLCluster* fClusterList[CLUSTERHANDLER_N_MAX_CLUSTERS];

};
#endif
