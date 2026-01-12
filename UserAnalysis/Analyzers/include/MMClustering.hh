#ifndef MMClustering_h
#define MMClustering_h 1
#include "TMMRecoEvent.hh"
#include "MMSoftHit.hh"

class MMClustering {
  // 
public:
  static MMClustering* GetInstance(); // want to have a singleton class
  
  Bool_t Init(TMMRecoEvent* mmevent,Bool_t fHistoMode); // pass mm reconstructed event so it can access the TRecoVHit objects of mmevent, copy them to a vector of MMSoftHit objects
  void Clusterize();    // loop over MMSoftHit objects, build clusters. Build progressively 4 cluster modes, both with IP connection enfored / not.
  void Clear();         // clear cluster list, destroys contents and free memory
  MMCluster* getMMCluster(int i, int ipmode, int clumode){if (i >= 0 && i < mmClusters[ipmode][clumode].size()) return mmClusters[ipmode][clumode].at(i); return nullptr;}
  MMCluster* getLastMMCluster(int ipmode, int clumode){return mmClusters[ipmode][clumode].back();}
  int insertNewMMCluster(int ipmode, int clumode){mmClusters[ipmode][clumode].push_back(new MMCluster(ipmode,clumode)); return mmClusters[ipmode][clumode].size()-1;}
  void ClusterMerger(); // loop over cluster pairs, merge the cluster if needed and create a new cluster of higher mode

private:
  static MMClustering* fInstance;
  vector<MMCluster*> mmClusters[2][CLUSTERMODES];
  
};

class MMCluster {
public:
  MMCluster(int ipmode, int clumode);  // set ipmode, clumode, clear private vectors, set isolation flag default
  int getECalClusIndex(){return ecalclusindex;}
  void setECalClusIndex(int ecalidx){ecalclusindex = ecalidx;}

  double getSeedSlope(){return seedslope;}
  void setSeedSlope(){double slope){seedslope = slope;}

  void addHit(int i){mmhitsindex.push_back(i);}
  TCLuster* mergeCluster(TCluster* tomerge); // is able to merge with another cluster built with the same mode
  void removeHit(int i); // remove a hit if necessary
  void Fit(); // depending on the mode used, the ip mode used, evaluates the track parameters
  
  MMTracklet getTracklet(){return tracos;}

  // some set/Get methods to be added.. getIPMode, getCluMode, etc.
  
private:
  int ipmode; // 0/1 if the ip connection is enfored/not
  int clumode; // 0--4: doublet of hits, doublet+hits in the same {plane,board}, match with other board in the same view [DeltaT fit], match with opposite view [3d fit]
  vector<int> mmhitsindex; // points to the vector of MMSoftHit objects filled in Clustering::Init
  double seedslope;  // slope of the first doublet which generated the cluster in mode = 0
  int nhitPerPlane[2]; // useful to make cluster categories
  int isolationflag; // the cluster is isolated in some sense
  MMTracklet tracos; // tracklet of the current cluster
  int ecalclusindex; // index of the ecal cluster (an object of type TRecoVCluster in a TECalClusCollection object) to which the MMcluster is connected
};

struct MMTracklet{
  double slope; // dv/dz
  double inter; // v at mesh plane
  double chi2;  // if fit is done, otherwise it is a nominal value [-999]
  double pars[4];// x0,y0,x1,y1: for mode = 0, fit x0,x1 or y0,y1 depending on the view and fix the other pair of parameters
  TVector3 lambda; // cosines of track directions
}; 
#endif

  
