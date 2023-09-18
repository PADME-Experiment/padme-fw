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

  Double_t SetEScale();
  Double_t CorrectESlope();


private:
  static ECalCalib* fInstance;
  Bool_t InitHistos();

  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;

  HistoSvc* fHS;
  utl::ConfigParser* fCfgParser;
  GeneralInfo* fGeneralInfo;

  Bool_t   fisMC;

};
#endif
