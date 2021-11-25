#ifndef NPoTAnalysis_h
#define NPoTAnalysis_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"

class NPoTAnalysis {

public:

  NPoTAnalysis(TString c = "config/UserAnalysis.conf", Int_t v = 0);
  ~NPoTAnalysis();

  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();

  Double_t GetNPoT() { return fNPoT; }

private:

  Bool_t InitHistos();

  TRandom3* fRndm;

  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;

  utl::ConfigParser* fCfgParser;

  HistoSvc* fHS;

  Double_t fNPoT;

};
#endif
