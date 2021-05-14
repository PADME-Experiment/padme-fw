#ifndef UserAnalysis_h
#define UserAnalysis_h 1

#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"

class UserAnalysis {

public:

  UserAnalysis();
  UserAnalysis(TString, Int_t);
  ~UserAnalysis();

  Bool_t InitHistos();
  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();

private:

  int fVerbose;

  PadmeAnalysisEvent* fEvent;

  utl::ConfigParser* fCfgParser;

};
#endif
