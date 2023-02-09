#ifndef ECalMLClusterFinderIsland_h
#define ECalMLClusterFinderIsland_h

#include "ECalMLCrystalHandler.hh"
#include "ECalMLClusterHandler.hh"

class ECalMLClusterFinderIsland {

 public :

  ECalMLClusterFinderIsland(ECalMLCrystalHandler*,ECalMLClusterHandler*);
  ~ECalMLClusterFinderIsland();

 public :

  Int_t FindClusters(); // Returns number of clusters found

  Double_t GetEThreshold() { return fEThreshold; }
  Double_t GetEThresholdSeed() { return fEThresholdSeed; }

  ECalMLCrystalHandler* GetCrystalHandler() { return fCrystalHandler; }
  ECalMLClusterHandler* GetClusterHandler() { return fClusterHandler; }

 private:

  void ExpandCluster(ECalMLCluster*,ECalMLCrystal*);

 private:

  Double_t fEThreshold;
  Double_t fEThresholdSeed;

  ECalMLCrystalHandler* fCrystalHandler;
  ECalMLClusterHandler* fClusterHandler;

};
#endif
