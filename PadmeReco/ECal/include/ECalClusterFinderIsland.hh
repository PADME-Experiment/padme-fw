#ifndef ECalClusterFinderIsland_h
#define ECalClusterFinderIsland_h

#include "ECalCrystalHandler.hh"
#include "ECalClusterHandler.hh"

class ECalClusterFinderIsland {

 public :

  ECalClusterFinderIsland(ECalCrystalHandler*,ECalClusterHandler*);
  ~ECalClusterFinderIsland();

 public :

  Int_t FindClusters(); // Returns number of clusters found

  Double_t GetEThreshold() { return fEThreshold; }
  Double_t GetEThresholdSeed() { return fEThresholdSeed; }

  ECalCrystalHandler* GetCrystalHandler() { return fCrystalHandler; }
  ECalClusterHandler* GetClusterHandler() { return fClusterHandler; }

 private:

  void ExpandCluster(ECalCluster*,ECalCrystal*);

 private:

  Double_t fEThreshold;
  Double_t fEThresholdSeed;

  ECalCrystalHandler* fCrystalHandler;
  ECalClusterHandler* fClusterHandler;

};
#endif
