#ifndef PVetoClustering_h
#define PVetoClustering_h 1
#define MAXHIT 100

#include "PVetoClusterHits.hh"
#include <iostream>

class PVetoCluster{
public:
  PVetoCluster();
  ~PVetoCluster(){};

  int InsertHit(PVetoClusterHits hit, int ihit, double seedtime);
  std::vector<int> GetHitIndex(){
    return hitIndexVector;
  }
  int GetMostUpstreamChannel(){return mostUpstreamChannel;}
  int GetMostDownstreamChannel(){return mostDownstreamChannel;}
  double GetAverageTime(){return averagetime;}
  double GetEarlyHitTime(){return earlyhittime;}
  double GetLateHitTime(){return latehittime;}
  int GetNHits(){return nhits;}
  int AddCluster(PVetoCluster* cluster);
  void Print(){std::cout<<"Most Upstream "<<mostUpstreamChannel<<" Most Downstream "<<mostDownstreamChannel<<" average time "<<averagetime<<std::endl;}
  double GetEnergy(){return energy;}
  
  private:
  std::vector<int> hitIndexVector;
  //  int hitIndex[MAXHIT];
  int mostUpstreamChannel;
  int mostDownstreamChannel;
  double averagetime;
  double earlyhittime;
  double latehittime;
  double energy;
  double seedtime;
  double seedch;
  double seedenergy;
  int nhits;
  
};

class PVetoClusterStructure{
  
public:
  PVetoClusterStructure();
  ~PVetoClusterStructure(){};

  void Clear();
  
  void AddHit(PVetoClusterHits MyHit, int hitindex){    
    HitIndexVec.push_back(hitindex);
    HitVec.push_back(MyHit);
  }

  void HitSort();

  void Clusterise();

  void MergeClusters();

  std::vector<PVetoCluster*> GetClusters(){return ClusVec;}
  std::vector<PVetoClusterHits> GetHitVec(){return HitVec;}
  
private:
  std::vector<PVetoClusterHits> HitVec;
  std::vector<PVetoCluster*> ClusVec;
  std::vector<int> HitIndexVec;
  double ClusterDeltaT=2;//maximum time difference between hits in the same cluster, as of May 2022 with Beth's digitizer the resolution of PVeto wrt SAC is 0.4 ns
};
 
#endif

