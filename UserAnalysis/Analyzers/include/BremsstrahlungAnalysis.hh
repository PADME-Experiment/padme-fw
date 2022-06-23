#ifndef BremsstrahlungAnalysis_h
#define BremsstrahlungAnalysis_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"

class BremsstrahlungAnalysis {

public:

  BremsstrahlungAnalysis(TString c = "config/UserAnalysis.conf", Int_t v = 0);
  ~BremsstrahlungAnalysis();

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
