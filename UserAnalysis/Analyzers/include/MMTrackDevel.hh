#ifndef MMTrackDevel_h
#define MMTrackDevel_h 1

#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"
#include "MMClustering.hh"
#include "GeneralInfo.hh"

struct MMTrack{
  int view  = -999;   //y=0, x=1
  int quad  = -999;   //0,1,2,3
  int level = -999;  //0 -> one segment per plane, 1-> two segments per plane

  MMTracklet tracklet;

  void Initialize() {
    view = -999;
    quad = -999;
    level = -999;
  }
};


class MMTrackDevel {

public:

  ~MMTrackDevel();
  static MMTrackDevel* GetInstance();
  Bool_t Init(PadmeAnalysisEvent* event,Bool_t fHistoMode,TString InputHistofile);
  Bool_t Process();
  Bool_t Finalize();
  vector<MMTrack*> GetVectorTracks() {return fvTracks;};
  MMTrack* GetTrack(int i) {if(i>=0 || i<(int)fvTracks.size()) return fvTracks.at(i); return nullptr;};
  Int_t GetNTracks() {return (int) fvTracks.size();};
  
private:
  static MMTrackDevel* fInstance;
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
  vector<MMTrack*> fvTracks;
};
#endif
