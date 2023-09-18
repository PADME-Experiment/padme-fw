#ifndef ECalCalib_h
#define ECalCalib_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"
#include "GeneralInfo.hh"

class ECalCalib {

protected:
ECalCalib(TString c = "config/UserAnalysis.conf", Int_t v = 0);

public:

  ~ECalCalib();
  static ECalCalib* GetInstance();


  Bool_t Init();
  Bool_t Process(PadmeAnalysisEvent*);
  Bool_t Finalize();


private:
  static ECalCalib* fInstance;
  Bool_t InitHistos();
  Double_t CorrectEScale();
  Double_t CorrectETimeSlope();
  Int_t NClusterPairSimpleSelection();

  Int_t fVerbose;
  PadmeAnalysisEvent* fEvent;
  HistoSvc* fHS;
  utl::ConfigParser* fCfgParser;
  GeneralInfo* fGeneralInfo;

  Bool_t   fisMC;
  Int_t fPairIndex[2]; // index of cluster pairs
};
#endif
