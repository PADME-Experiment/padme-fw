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
  Double_t chi2;  // if fit is done, otherwise it is a nominal value [-999]
  Double_t chi2IP;
  Double_t pchi2;
  Double_t pars[5];// x0,y0,x1,y1,dz: for mode = 0, fit x0,x1 or y0,y1 depending on the view and fix the other pair of parameters
  TVector3 lambda; // cosines of track directions
  vector<TVector3> vres; //vector of residuals
  
  TVector3 BestTrackExtrapolationAtZ(double z_in) {
    TVector3 ext_pos;

    ext_pos.SetX(pars[0]+lambda.X()/lambda.Z()*(z_in-GeneralInfo::GetInstance()->GetMMPosPlaneZ(0)));
    ext_pos.SetY(pars[1]+lambda.Y()/lambda.Z()*(z_in-GeneralInfo::GetInstance()->GetMMPosPlaneZ(0)));
    ext_pos.SetZ(z_in);
    
    return ext_pos;
  }
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
  
private:
  static MMFindBestTrack* fInstance;
  GeneralInfo* fGeneralInfo;
  MMClustering* fMMClusteringInstance;
  Bool_t InitHistos(Int_t);
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
};
#endif
