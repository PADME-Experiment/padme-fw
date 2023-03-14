#ifndef ReversedBFieldBremsstrahlungAnalysis_h
#define ReversedBFieldBremsstrahlungAnalysis_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"

class ReversedBFieldBremsstrahlungAnalysis {

public:

  ReversedBFieldBremsstrahlungAnalysis(TString c = "config/UserAnalysis.conf", Int_t v = 0);
  ~ReversedBFieldBremsstrahlungAnalysis();

  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();

  
private:

  Bool_t InitHistos(Bool_t isMC);

  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;

  utl::ConfigParser* fCfgParser;

  HistoSvc* fHS;
};
#endif
