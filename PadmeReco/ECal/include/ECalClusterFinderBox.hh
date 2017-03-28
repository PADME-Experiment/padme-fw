#ifndef ECalClusterFinderBox_h
#define ECalClusterFinderBox_h

#include "ECalCrystalHandler.hh"
#include "ECalClusterHandler.hh"

#define CLUSTERFINDERBOX_NEIGHBORS_MAX 900

#define CLUSTERFINDERBOX_DEFAULT_RADIUS 5.
#define CLUSTERFINDERBOX_DEFAULT_ETHRESHOLD 0.
#define CLUSTERFINDERBOX_DEFAULT_ETHRESHOLDSEED 0.

class ECalClusterFinderBox {

 public :

  ECalClusterFinderBox(ECalCrystalHandler*,ECalClusterHandler*);
  ~ECalClusterFinderBox();
  
 public :

  Int_t FindClusters(); // Returns number of clusters found

  Double_t GetEThreshold(){return fEThreshold;};
  Double_t GetEThresholdSeed(){return fEThresholdSeed;};
  Double_t GetBoxRadius(){return fBoxRadius;};

  void SetEThreshold(Double_t eThr){fEThreshold = eThr;};
  void SetEThresholdSeed(Double_t eThrS){fEThresholdSeed = eThrS;};
  void SetBoxRadius(Double_t);

  void PrintNeighborMap();

  ECalCrystalHandler* GetCrystalHandler(){return fCrystalHandler;};
  ECalClusterHandler* GetClusterHandler(){return fClusterHandler;};

 private:

  void ExpandCluster(ECalCluster*,ECalCrystal*);

  // Energy thresholds
  Double_t fEThreshold;
  Double_t fEThresholdSeed;

  // Box radius and related quantities
  Double_t fBoxRadius;
  Double_t fCryXSize;
  Double_t fCryYSize;
  Double_t fCryXSize2;
  Double_t fCryYSize2;

  // Map of neighbor crystals within given radius
  Int_t fNNeighbor;
  Int_t fXNeighbor[CLUSTERFINDERBOX_NEIGHBORS_MAX];
  Int_t fYNeighbor[CLUSTERFINDERBOX_NEIGHBORS_MAX];

  ECalCrystalHandler* fCrystalHandler;
  ECalClusterHandler* fClusterHandler;

};
#endif
