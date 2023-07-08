#ifndef UserAnalysis_h
#define UserAnalysis_h 1

#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"

class HistoSvc;
class ETagAnalysis;
class NPoTAnalysis;
class ECalCalib;
class IsGGAnalysis;
class Is22GGAnalysis;
class SinglePhoton;
class Is3GAnalysis;
class MCTruth;

class UserAnalysis {

public:

  UserAnalysis(TString c = "config/UserAnalysis.conf", Int_t v = 0);
  ~UserAnalysis();

  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();

private:

  Bool_t InitHistos();

  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;

  utl::ConfigParser* fCfgParser;

  HistoSvc* fHS;

  NPoTAnalysis* fNPoTAnalysis;
  ETagAnalysis* fETagAnalysis;

  IsGGAnalysis* fIsGGAnalysis;
  Is22GGAnalysis* fIs22GGAnalysis;
  SinglePhoton* fSinglePhoton;
  Is3GAnalysis* fIs3GAnalysis;
  MCTruth* fMCTruth;
  ECalCalib* fECalCalib;
};
#endif
