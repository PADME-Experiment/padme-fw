#ifndef MMClustering_h
#define MMClustering_h 1
#include "TMMRecoEvent.hh"
#include "MMSoftHit.hh"
#include "MMCluster.hh"
#include "HistoSvc.hh"

class MMClustering {
 
protected:
  MMClustering();

public:
  ~MMClustering();
  static MMClustering* GetInstance(); // want to have a singleton class

private:
  static MMClustering* fInstance;

public:
  void Init(TMMRecoEvent* mmevent); // pass mm reconstructed event so it can access the TRecoVHit objects of mmevent, copy them to a vector of MMSoftHit objects
  void Clusterize();    // loop over MMSoftHit objects, build clusters. Build progressively 4 cluster modes, both with IP connection enforced / not.
  // modes: 0 -> within a single board (single view)
  // modes: 1 -> within the same view and including 2 boards (same quadrant): DeltaT fit
  // modes: 2 -> eventual xy matching
  void Clear();         // clear cluster list, destroys contents and free memory
  void Print();
  Int_t InsertNewMMCluster(Int_t ipmode, Int_t clumode){fMMClusters[ipmode][clumode].push_back(new MMCluster(ipmode,clumode)); return fMMClusters[ipmode][clumode].size()-1;}
  //  void ClusterMerger(); // loop over cluster pairs, merge the cluster if needed and create a new cluster of higher mode

  MMCluster* GetMMCluster(Int_t i, Int_t ipmode, Int_t clumode){if (i >= 0 && i < (int) fMMClusters[ipmode][clumode].size()) return fMMClusters[ipmode][clumode].at(i); return nullptr;}
  UInt_t GetMMClusterLength(Int_t ipmode, Int_t clumode) {return fMMClusters[ipmode][clumode].size();}
  MMCluster* GetLastMMCluster(Int_t ipmode, Int_t clumode){return fMMClusters[ipmode][clumode].back();}
  MMSoftHit* GetHit(Int_t hit_idx) {if(hit_idx>=0 && hit_idx< (int)fMMSoftHits.size()) return fMMSoftHits.at(hit_idx); return nullptr;}
  vector<int> GetHitComposition(int idx, int ipmode, int clumode){return fIndicesOfMergedClusters[ipmode][clumode].at(idx);}
private:
  Bool_t InitHistos();
  
  vector<MMCluster*> fMMClusters[IPMODES][CLUSTERMODES];
  vector<bool> fMergedMMClusters[IPMODES][CLUSTERMODES]; // keep track of clusters merged to other clusters
  vector<vector<int>> fIndicesOfMergedClusters[IPMODES][CLUSTERMODES];
  vector<MMSoftHit*> fMMSoftHits;
  HistoSvc* fHS;
};

#endif
  
