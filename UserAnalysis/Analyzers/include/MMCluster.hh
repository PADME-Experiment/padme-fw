#ifndef MMCluster_h
#define MMCluster_h 1
#include "MMSoftHit.hh"
#include <Fit/Fitter.h>
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "Math/ProbFunc.h"
#include "MMTrackFcn.hh"
#include "GeneralInfo.hh"
#include <TMinuit.h>

#define IPSINCUT 0.05 //Run IV 
//#define IPSINCUT 0.2  //BField on

#define CALIBRATION false
#define IPMODE false

struct MMTracklet{           //NOTE THAT PARS[4] è dZ!!!!
  Double_t slope; // dv/dz
  Double_t inter; // v at mesh plane
  Double_t errSlope = 0.010; //mrad
  Double_t errInter = 0.300; //mm
  Double_t slope_lvl0[2];
  Double_t inter_lvl0[2];
  Double_t chi2;  // if fit is done, otherwise it is a nominal value [-999]
  Double_t chi2IP;
  Double_t pchi2;
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

  void RefitWithTarget()  {

    double zNew = GeneralInfo::GetInstance()->GetTargetPos().Z();
    double vNew;
    if(fabs(lambda.X()) < 1e-15) vNew = GeneralInfo::GetInstance()->GetTargetPos().Y();
    else                         vNew = GeneralInfo::GetInstance()->GetTargetPos().X();
    double errV = 1.; //mm BEAM SPOT DA CAMBIARE!!!
    
    const double m0 = slope;
    const double c0 = inter;

    const double wm = 1.0/(errSlope*errSlope);
    const double wc = 1.0/(errInter*errInter);
    const double wp = 1.0/(errV*errV);

    // Sistema lineare ottenuto da d(chi2)/dm = 0 e d(chi2)/dc = 0

    const double A11 = wm + wp*zNew*zNew;
    const double A12 = wp*zNew;

    const double A21 = wp*zNew;
    const double A22 = wc + wp;

    const double B1 = wm*m0 + wp*zNew*vNew;
    const double B2 = wc*c0 + wp*vNew;

    const double det = A11*A22 - A12*A21;

    if(std::abs(det) < 1e-20) return;

    slope = ( B1*A22 - B2*A12 )/det;
    inter = ( A11*B2 - A21*B1 )/det;

    // chi2 aggiornato

    chi2IP =
      std::pow((slope-m0)/errSlope,2) +
      std::pow((inter-c0)/errInter,2) +
      std::pow((vNew-(slope*zNew+inter))/errV,2);

    double cosv = slope/TMath::Sqrt(1+slope*slope);
    double cosz = 1./TMath::Sqrt(1+slope*slope);

    lambda.SetZ(cosz);
    if(fabs(lambda.X()) < 1e-15) lambda.SetY(cosv);
    else                         lambda.SetX(cosv);
  }
}; 

class MMCluster {
public:
  MMCluster(Int_t ipmode, Int_t clumode);  // set ipmode, clumode, clear private vectors, set isolation flag default
  ~MMCluster();

  void Import(MMCluster* oldclu);
  bool AddHit(MMSoftHit* softhit); // ONLY USED AT LEVEL ZERO
  bool L0SimpleFitWithClusterTime(double xEcal, double yEcal, double tEcal);
  bool MergeAcrossPlanes(MMCluster* inputclus); // If possible, merge present cluster with given input cluster
  bool MergeAcrossPlanesWithdZ(MMCluster* inputclus); //Use dZ for matching
  bool SimpleFitWithClusterTime(double xEcal, double yEcal, double tEcal);
  bool FitWithClusterTime(double xEcal, double yEcal, double tEcal); // re-fit level-1 cluster with fixed DZ
  //bool HitRejectionAlgorithm(double v_new_hit, double z_new_hit, double mt_avg, double ct_avg);
  bool HitRejectionAlgorithm();
  
  //  TCLuster* MergeCluster(TCluster* tomerge); // is able to merge with another cluster built with the same mode
  void RemoveHit(Int_t i); // remove a hit if necessary
    
  void SetSeedSlope(Double_t slope){fSeedSlope = slope;};
  void SetECalClusIndex(Int_t ecalidx){fEcalClusIndex = ecalidx;};
  
  Int_t GetIPmode() {return fIpmode;};
  Int_t GetClumode() {return fClumode;};
  UInt_t GetHitsVectorSize() {return fMMHitsInClu.size();};
  MMSoftHit* GetHit(Int_t i){if (i >= 0 && i < (int) fMMHitsInClu.size()) return fMMHitsInClu.at(i); return nullptr;};
  UInt_t GetHitsHRVectorSize() {return fMMHitsInClu_HR.size();};
  MMSoftHit* GetHitHR(Int_t i){if (i >= 0 && i < (int) fMMHitsInClu_HR.size()) return fMMHitsInClu_HR.at(i); return nullptr;};
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
  void SimpleFitWithDz(vector<int> plane, vector<double>vhits, vector<double>zhits, double dz, double* v_avgout, double* z_avgout, double* mt_avgout, double* ct_avgout,double* cosvout, double* coszout, double* chi2out, double *dz_fitout);
  
  
  Int_t fIpmode; // 0/1 if the ip connection is enfored/not
  Int_t fClumode; // 0--4: Double_tt of hits, Double_tt+hits in the same {plane,board}, match with other board in the same view [DeltaT fit], match with opposite view [3d fit]
  vector<MMSoftHit*> fMMHitsInClu; // vector of MMSoftHit objects filled in Clustering::Init
  vector<MMSoftHit*> fMMHitsInClu_HR; // vector of MMSoftHit objects filled in Clustering::Init
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
  
  void setPoints(vector<MMSoftHit*> hitArray);
  void setAdditionalPoint(double x, double y, double z);
};

#endif
