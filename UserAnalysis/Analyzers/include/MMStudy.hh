#ifndef MMStudy_h
#define MMStudy_h 1

#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"
#include "GeneralInfo.hh"
#include "NPoTAnalysis.hh"

class MMStudy {


public:

  ~MMStudy();
  static MMStudy* GetInstance();


  Bool_t Init(PadmeAnalysisEvent* event,Bool_t fHistoMode,TString InputHistofile);

  Bool_t Process();
  Bool_t Finalize();


private:
  static MMStudy* fInstance;
  Bool_t InitHistos(Int_t);
  NPoTAnalysis *fNPoTAnalysis;
  Int_t fNRun;
  Bool_t fHistoMode;
  PadmeAnalysisEvent* fEvent;
  utl::ConfigParser* fCfgParser;
  HistoSvc* fHS;
  GeneralInfo* fGeneralInfo;
  Int_t fVerbose;
  TString InputHistofile;
  TString InputHistofileName;
  
};
#endif
