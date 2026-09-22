#ifndef MMFindBestTrack_h
#define MMFindBestTrack_h 1

#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"
#include "MMClustering.hh"
#include "GeneralInfo.hh"

struct MMTrack{
  int view  = -999;   //y=0, x=1
  int quad  = -999;   //0,1,2,3
  int level = -999;  //0 -> one segment per plane, 1-> two segments per plane

  int nhit  = -999;
  MMTracklet tracklet;

  void Initialize() {
    view = -999;
    quad = -999;
    level = -999;
  }
};

struct MMBestTrack{

  int view  = -999;   //y=0, x=1
  int quad  = -999;   //0,1,2,3
  int level = -999;  //0 -> one segment per plane, 1-> two segments per plane
  int nhit  = -999;
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
  Double_t purity=-999; //default, assigned only when calling track-clupos association
  TVector3 BestTrackExtrapolationAtZ(double z_in) {
    TVector3 ext_pos;

    ext_pos.SetX(pars[0]+lambda.X()/lambda.Z()*(z_in-GeneralInfo::GetInstance()->GetMMPosPlaneZ(0)));
    ext_pos.SetY(pars[1]+lambda.Y()/lambda.Z()*(z_in-GeneralInfo::GetInstance()->GetMMPosPlaneZ(0)));
    ext_pos.SetZ(z_in);
    
    return ext_pos;
  }
};


struct MMBestTrack3D{
  MMBestTrack* tracks[2]={nullptr, nullptr}; //one per view y=0, x=1
  // Double_t slope; // dv/dz
  // Double_t inter; // v at mesh plane
  // Double_t chi2;  // if fit is done, otherwise it is a nominal value [-999]
  // Double_t chi2IP;
  // Double_t pchi2;
  // Double_t pars[5];// x0,y0,x1,y1,dz: for mode = 0, fit x0,x1 or y0,y1 depending on the view and fix the other pair of parameters
  // TVector3 lambda; // cosines of track directions
  // vector<TVector3> vres; //vector of residuals

  Double_t purity=-999; //default, assigned only when calling track-clupos association

  // TVector3 BestTrackExtrapolationAtZ(double z_in) {
  //   TVector3 ext_pos;

  //   ext_pos.SetX(pars[0]+lambda.X()/lambda.Z()*(z_in-GeneralInfo::GetInstance()->GetMMPosPlaneZ(0)));
  //   ext_pos.SetY(pars[1]+lambda.Y()/lambda.Z()*(z_in-GeneralInfo::GetInstance()->GetMMPosPlaneZ(0)));
  //   ext_pos.SetZ(z_in);
    
  //   return ext_pos;
  // }
};

class MMFindBestTrack {

public:

  ~MMFindBestTrack();
  static MMFindBestTrack* GetInstance();
  Bool_t Init(PadmeAnalysisEvent* event);
  Bool_t Process();
  Bool_t Finalize();
  vector<MMBestTrack*> GetVectorTracks() {return fvTracks;};
  MMBestTrack* GetTrack(int i) {if(i>=0 || i<(int)fvTracks.size()) return fvTracks.at(i); return nullptr;};
  Int_t GetNTracks() {return (int) fvTracks.size();};
  MMBestTrack3D* GetBestTrackFromCluPos(TVector3 cluPos, Double_t extrZ); //
  Double_t PurityFunc(Double_t x, Double_t y, Double_t *p);
  Double_t PurityFunc_4HitL1(Double_t x, Double_t *p);
  Int_t QualityBin(Double_t purity, Int_t type) {
    //        purity_TAG < 0      --> quality_bin_TAG = 0 (no track considered)
    //    0 < purity_TAG < 0.02   --> quality_bin_TAG = 1 (bad purity)
    // 0.02 < purity_TAG < 0.2    --> quality_bin_TAG = 2
    //  0.2 < purity_TAG < 0.6    --> quality_bin_TAG = 3
    //  0.6 < purity_TAG < 1    --> quality_bin_TAG = 4
    //        purity_PROBE < 0      --> quality_bin_PROBE = 0 (no track considered)
    //    0 < purity_PROBE < 0.02   --> quality_bin_PROBE = 1 (bad purity)
    // 0.02 < purity_PROBE < 0.2    --> quality_bin_PROBE = 2
    //  0.2 < purity_PROBE < 0.6    --> quality_bin_PROBE = 3
    //  0.6 < purity_PROBE < 1    --> quality_bin_PROBE = 4
    
    if(type == 0) { //TAG
      if(purity < 0)     return 0;
      if(purity < 0.02)  return 1;
      if(purity < 0.2)   return 2;
      if(purity < 0.6)   return 3;
      return 4;
    }
    else { //PROBE
      if(purity < 0)     return 0;
      if(purity < 0.02)  return 1;
      if(purity < 0.2)   return 2;
      if(purity < 0.6)   return 3;
      return 4;
    }
  };

private:
  static MMFindBestTrack* fInstance;
  GeneralInfo* fGeneralInfo;
  MMClustering* fMMClusteringInstance;
  Bool_t InitHistos(Int_t);
  void AssignPurity(MMBestTrack* track, TVector3 cluPos,Double_t extrZ);//

  Int_t fNRun;
  Bool_t fHistoMode;
  PadmeAnalysisEvent* fEvent;
  utl::ConfigParser* fCfgParser;
  HistoSvc* fHS;
  Int_t fVerbose;
  TString InputHistofile;
  TString InputHistofileName;
  int fEventCounter;
  bool fSaveEvent;
  vector<MMBestTrack*> fvTracks;

  //purity fit parameters

  double pars_3h_lvl0[8] = {7.56008, 58.3746, -1.2901, 7.28396, 16.0513, 105.037, 3.37634, 0.263186};

  double pars_4h_lvl0[8] = {0.0265705, 92.0291, -1.24075, 6.90274, 19.2264, 104.307, 8.19723, 1.84315};

  double pars_4h_lvl1[6] = {1819.62, 0.294837, 9.89586, 1664.43, 10.6183, 83.4185};
  double err_pars_4h_lvl1[6] = {16.1599, 0.093497, 0.084371, 4.18318, 0.142544, 0.102222};
  
  double pars_5h_lvl1[8] = {18.5897, 34.5641, 0.906092, 11.324, 5.74614, 87.1964, 2.09614, 0.914284};

  double pars_6ph_lvl1[8] = {204.73, 306.454, 1.26521, 11.1982, 4.69843, 99.2033, -0.781994, 4.14681};

   
};
#endif
