#ifndef MMCluster_h
#define MMCluster_h 1
#include "MMSoftHit.hh"
#include <Fit/Fitter.h>
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "Math/ProbFunc.h"
#include "MMTrackFcn.hh"
#include "GeneralInfo.hh"

#define IPSINCUT 0.05
#define YES_CALIB true
#define NO_CALIB false

struct MMTracklet{
  Double_t slope; // dv/dz
  Double_t inter; // v at mesh plane
  Double_t chi2;  // if fit is done, otherwise it is a nominal value [-999]
  Double_t pars[5];// x0,y0,x1,y1,dz: for mode = 0, fit x0,x1 or y0,y1 depending on the view and fix the other pair of parameters
  TVector3 lambda; // cosines of track directions
  vector<TVector3> vres; //vector of residuals
  
  TVector3 ExtrapolationAtZ(double z_in) {
    TVector3 ext_pos;

    ext_pos.SetX(pars[0]+lambda.X()/lambda.Z()*(z_in-GeneralInfo::GetInstance()->GetMMPosPlaneZ(0)));
    ext_pos.SetY(pars[1]+lambda.Y()/lambda.Z()*(z_in-GeneralInfo::GetInstance()->GetMMPosPlaneZ(0)));
    ext_pos.SetZ(z_in);
    
    return ext_pos;
  }
}; 

class MMCluster {
public:
  MMCluster(Int_t ipmode, Int_t clumode);  // set ipmode, clumode, clear private vectors, set isolation flag default
  ~MMCluster();

  void Import(MMCluster* oldclu);
  bool AddHit(MMSoftHit* softhit); // ONLY USED AT LEVEL ZERO
  bool MergeAcrossPlanes(MMCluster* inputclus); // If possible, merge present cluster with given input cluster
  bool ReFitWithClusterTime(bool ipused, double refTimeZ); // re-fit level-1 cluster with fixed DZ
  bool HitRejectionAlgorithm(double v_new_hit, double z_new_hit, double mt_avg, double ct_avg);
  
  //  TCLuster* MergeCluster(TCluster* tomerge); // is able to merge with another cluster built with the same mode
  void RemoveHit(Int_t i); // remove a hit if necessary
    
  void SetSeedSlope(Double_t slope){fSeedSlope = slope;};
  void SetECalClusIndex(Int_t ecalidx){fEcalClusIndex = ecalidx;};
  
  Int_t GetIPmode() {return fIpmode;};
  Int_t GetClumode() {return fClumode;};
  UInt_t GetHitsVectorSize() {return fMMHitsInClu.size();};
  MMSoftHit* GetHit(Int_t i){if (i >= 0 && i < (int) fMMHitsInClu.size()) return fMMHitsInClu.at(i); return nullptr;};
  Double_t GetSeedSlope() {return fSeedSlope;};
  Int_t GetNHitsPerPlane(Int_t plane) {if(plane>0 && plane<2) return fNHitsPerPlane[plane]; return -1;};
  Int_t GetIsolationFlag() {return fIsolationFlag;};
  MMTracklet GetTracklet(){return fTracos;};
  Int_t GetEcalClusIndex() {return fEcalClusIndex;};
  Double_t GetIPPhaseAngle(){return fIPPhaseAngle;};
  Double_t GetDvAtMeshPlane(){return fDvAtMeshPlane;};
  // some set/Get methods to be added.. getIPMode, getCluMode, etc.

  void Print();
  
private:
  void evaluateStraightLineTwoD(vector<double>vhits,vector<double>zhits, double* v_avgout, double* z_avgout, double* mt_avgout, double* ct_avgout,double* cosvout,double* coszout, double* chi2);
  
  
  Int_t fIpmode; // 0/1 if the ip connection is enfored/not
  Int_t fClumode; // 0--4: Double_tt of hits, Double_tt+hits in the same {plane,board}, match with other board in the same view [DeltaT fit], match with opposite view [3d fit]
  vector<MMSoftHit*> fMMHitsInClu; // vector of MMSoftHit objects filled in Clustering::Init
  Double_t fSeedSlope;  // slope of the first Double_tt which generated the cluster in mode = 0
  Int_t fNHitsPerPlane[2]; // useful to make cluster categories
  Int_t fIsolationFlag; // the cluster is isolated in some sense
  MMTracklet fTracos; // tracklet of the current cluster
  Int_t fEcalClusIndex; // index of the ecal cluster (an object of type TRecoVCluster in a TECalClusCollection object) to which the MMcluster is connected
  Double_t fIPPhaseAngle; // angle wrt IP: for any ipmode=0, level zero cluster
  Double_t fDvAtMeshPlane; // available for level one clusters

  vector<TVector3> fPositions;
  vector<TVector3> fErrors;
  vector<int> fBoardIds;
  MMTrackFcn fMMTrackFcn;

  ROOT::Fit::Fitter fFitter;  
  void InitFit(vector<MMSoftHit*> hitArray);
  void InitFit(vector<MMSoftHit*> hitArray, double x, double y, double z);
  void InitFit(vector<MMSoftHit*> hitArray, double dz);
  void setPoints(vector<MMSoftHit*> hitArray);
  void setAdditionalPoint(double x, double y, double z);
};

#endif
