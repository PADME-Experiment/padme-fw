#ifndef MMStudy_h
#define MMStudy_h 1

#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"
#include "GeneralInfo.hh"
#include "NPoTAnalysis.hh"
#include "MMTracker.hh"

class MMStudy {


public:

  ~MMStudy();
  static MMStudy* GetInstance();


  Bool_t Init(PadmeAnalysisEvent* event,Bool_t fHistoMode,TString InputHistofile);

  Bool_t Process();
  Bool_t Finalize();


private:
  static MMStudy* fInstance;
  GeneralInfo* fGeneralInfo;
  Bool_t InitHistos(Int_t);
  NPoTAnalysis *fNPoTAnalysis;
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
  MMTracker* fTracker[3];//y view, x view, 3d view fits
};
#endif
