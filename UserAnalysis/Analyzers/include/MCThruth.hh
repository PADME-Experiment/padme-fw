#ifndef MCThruth_h
#define MCThruth_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"

class MCThruth {

public:

  MCThruth(TString c = "config/UserAnalysis.conf", Int_t v = 0);
  ~MCThruth();

  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();

private:
  Bool_t InitHistos();
  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;
  utl::ConfigParser* fCfgParser;
  HistoSvc* fHS;

};
#endif
