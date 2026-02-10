#ifndef MMTrackDevel_h
#define MMTrackDevel_h 1

#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"
#include "MMClustering.hh"
#include "GeneralInfo.hh"


class MMTrackDevel {

public:

  ~MMTrackDevel();
  static MMTrackDevel* GetInstance();
  Bool_t Init(PadmeAnalysisEvent* event,Bool_t fHistoMode,TString InputHistofile);
  Bool_t Process();
  Bool_t Finalize();


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
};
#endif
