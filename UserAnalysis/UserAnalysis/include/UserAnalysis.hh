#ifndef UserAnalysis_h
#define UserAnalysis_h 1

#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"

class HistoSvc;
class NPoTAnalysis;
class ECalCalib;
class ECalCalib22;
class IsGGAnalysis;
class Is22GGAnalysis;
class Is3GAnalysis;
class GeneralInfo;
class ECalSel;
class ETagAn;
class ETagAnalysis;
class MCTruth;
class MCTruthECal;
class DataQuality;

class UserAnalysis {

public:

  UserAnalysis(TString c = "config/UserAnalysis.conf", Int_t v = 0);
  ~UserAnalysis();

  //Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Init(PadmeAnalysisEvent* event, Bool_t fHistoMode, TString InputHistofile, Int_t DBRunNumber);
  Bool_t Process();
  Bool_t Finalize();

private:

  Bool_t InitHistos();

  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;

  utl::ConfigParser* fCfgParser;

  HistoSvc* fHS;

  NPoTAnalysis* fNPoTAnalysis;
  IsGGAnalysis* fIsGGAnalysis;
  Is22GGAnalysis* fIs22GGAnalysis;
  Is3GAnalysis* fIs3GAnalysis;
  GeneralInfo* fGeneralInfo;
  ECalSel* fECalSel;
  ECalCalib22 *fECalCalib22;
  ETagAn* fETagAn;
  ETagAnalysis* fETagAnalysis;
  MCTruth* fMCTruth;
  MCTruthECal* fMCTruthECal;
  ECalCalib* fECalCalib;
  DataQuality *fDataQuality;

  bool fETagHitsAvail;
  bool fETagClusAvail;
};
#endif
