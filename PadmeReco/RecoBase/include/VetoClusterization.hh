#ifndef VetoClustering_h
#define VetoClustering_h 1
#define MAXHIT 100

#include "VetoClusterHits.hh"
#include <iostream>

class VetoCluster{
public:
  VetoCluster();
  ~VetoCluster(){};

  int InsertHit(VetoClusterHits hit, int ihit, double seedtime);
  std::vector<int> GetHitIndex(){
    return hitIndexVector;
  }
  int GetMostUpstreamChannel(){return mostUpstreamChannel;}
  int GetMostDownstreamChannel(){return mostDownstreamChannel;}
  double GetAverageTime(){return averagetime;}
  double GetEarlyHitTime(){return earlyhittime;}
  double GetLateHitTime(){return latehittime;}
  int GetNHits(){return nhits;}
  int AddCluster(VetoCluster* cluster);
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

class VetoClusterStructure{
  
public:
  VetoClusterStructure();
  ~VetoClusterStructure(){};

  void Clear();
  
  void AddHit(VetoClusterHits MyHit, int hitindex){    
    HitIndexVec.push_back(hitindex);
    HitVec.push_back(MyHit);
  }

  void HitSort();

  void Clusterise();

  void MergeClusters();

  std::vector<VetoCluster*> GetClusters(){return ClusVec;}
  std::vector<VetoClusterHits> GetHitVec(){return HitVec;}
  
private:
  std::vector<VetoClusterHits> HitVec;
  std::vector<VetoCluster*> ClusVec;
  std::vector<int> HitIndexVec;
  double ClusterDeltaT=2;//maximum time difference between hits in the same cluster, as of May 2022 with Beth's digitizer the resolution of Veto wrt SAC is 0.4 ns
};
 
#endif

