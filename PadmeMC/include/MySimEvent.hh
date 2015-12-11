#ifndef _MY_SIM_EVENT_
#define _MY_SIM_EVENT_

#include "MyParticle.hh"
#include <iostream>
#include <vector>
#include "MyCaloCluster.hh"
#include "MyTrackerCluster.hh"
#include "TObject.h"

class MySimEvent : public TObject {

public:
  MySimEvent();
  ~MySimEvent(){;};

  void ClearEvent();
  
  void SetCrystalEnergies(double *en, int n);
  double * GetCrystalEnergies(){return ecry;};
  
  void AddCluster(double x, double y,double e);
  std::vector<MyCaloCluster>* GetClusters(){return &clusters;};

  void AddTrHit(int layer, double x, double y,double z,double e,double cut);
  std::vector<MyTrackerCluster>* GetTrackerClusters(){return &trClus;};

  void AddParticle(MyParticle part){particles.push_back(part);};
  std::vector<MyParticle>* GetParticles(){return &particles;};

  double ecry[2600];

  std::vector<MyCaloCluster> clusters;
  std::vector<MyTrackerCluster> trClus;


  double ETotCal;

  int pid;
  
  //Particles generated from the primary track
  std::vector<MyParticle> particles;
  

};
#endif
