#ifndef ECalClusterFinder_h
#define ECalClusterFinder_h

#include "ECalCrystalHandler.hh"
#include "ECalClusterHandler.hh"

class ECalClusterFinder {

 public :

  ECalClusterFinder(ECalCrystalHandler*,ECalClusterHandler*);
  ~ECalClusterFinder();

 public :

  Int_t FindClusters(); // Returns number of clusters found

  Double_t GetEThreshold() { return fEThreshold; }
  Double_t GetEThresholdSeed() { return fEThresholdSeed; }

  void SetEThreshold(Double_t eThr) { fEThreshold = eThr; }
  void SetEThresholdSeed(Double_t eThrS) { fEThresholdSeed = eThrS; }

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
