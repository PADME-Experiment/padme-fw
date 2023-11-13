#ifndef ECalCluster_h
#define ECalCluster_h

#include "ECalCrystal.hh"

#define CLUSTER_N_MAX_CRYSTALS_IN_CLUSTER 150

class ECalCluster {

public :
  ECalCluster();
  ~ECalCluster();
  
 public :

  Int_t AddCrystal(ECalCrystal*); // Returns number of crystals currently in cluster

  Double_t GetRawEnergy() { return fRawEnergy; }
  Double_t GetEnergy()    { return fEnergy; }
  Double_t GetTime()      { return fTime; }
  Double_t GetNCrystals() { return fNCrystals; }
  ECalCrystal* GetCrystal(Int_t i){if (i<fNCrystals) return fCrystalList[i]; else return NULL;}
  Int_t    GetSeed()      { return fSeed; }
  void     SetSeed(Int_t i) {fSeed = i;}

  Double_t GetXCenter() { return fXCenter; }
  Double_t GetYCenter() { return fYCenter; }

  Double_t ComputeTime();
  Double_t ComputeEnergy();
  Double_t ComputeCenter();

  void  Print();

 private:

  Int_t fNCrystals;
  ECalCrystal* fCrystalList[CLUSTER_N_MAX_CRYSTALS_IN_CLUSTER];

  Int_t    fSeed;
  Double_t fRawEnergy;
  Double_t fEnergy;
  Double_t fTime;	 
  Double_t fXCenter;	 
  Double_t fYCenter;	 

};
#endif
