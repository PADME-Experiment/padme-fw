#ifndef ECalClusterFinderRadius_h
#define ECalClusterFinderRadius_h

#include "ECalCrystalHandler.hh"
#include "ECalClusterHandler.hh"

#define CLUSTERFINDERRADIUS_NEIGHBORS_MAX 900

class ECalClusterFinderRadius {

 public :

  ECalClusterFinderRadius(ECalCrystalHandler*,ECalClusterHandler*);
  ~ECalClusterFinderRadius();
  
 public :

  Int_t FindClusters(); // Find clusters and return number of clusters found

  Double_t GetEThreshold(){return fEThreshold;};
  Double_t GetEThresholdSeed(){return fEThresholdSeed;};
  Double_t GetRadius(){return fRadius;};

  void PrintNeighborMap();

  ECalCrystalHandler* GetCrystalHandler(){return fCrystalHandler;};
  ECalClusterHandler* GetClusterHandler(){return fClusterHandler;};

 private:

  void SetRadius(Double_t);

  void ExpandCluster(ECalCluster*,ECalCrystal*);

  // Energy thresholds
  Double_t fEThreshold;
  Double_t fEThresholdSeed;

  // Radius and related quantities
  Double_t fRadius;
  Double_t fCryXSize;
  Double_t fCryYSize;

  // Map of neighbor crystals within given radius
  Int_t fNNeighbor;
  Int_t fXNeighbor[CLUSTERFINDERRADIUS_NEIGHBORS_MAX];
  Int_t fYNeighbor[CLUSTERFINDERRADIUS_NEIGHBORS_MAX];

  ECalCrystalHandler* fCrystalHandler;
  ECalClusterHandler* fClusterHandler;

};
#endif
