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
  Double_t GeNCrystals()  { return fNCrystals; }	

  Double_t GetXCenter() { return fXCenter; }
  Double_t GetYCenter() { return fYCenter; }

  Double_t ComputeTime();
  Double_t ComputeEnergy();
  Double_t ComputeCenter();

  void  Print();

 private:

  Int_t fNCrystals;
  ECalCrystal* fCrystalList[CLUSTER_N_MAX_CRYSTALS_IN_CLUSTER];

  Double_t fRawEnergy;
  Double_t fEnergy;
  Double_t fTime;	 
  Double_t fXCenter;	 
  Double_t fYCenter;	 

};
#endif
