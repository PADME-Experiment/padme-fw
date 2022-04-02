#ifndef ECalCalib_h
#define ECalCalib_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"

class ECalCalib {

public:

  ECalCalib(TString c = "config/UserAnalysis.conf", Int_t v = 0);
  ~ECalCalib();

  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();

  Double_t SetEScale();
  Double_t CorrectESlope();

private:

  Bool_t InitHistos();

  //  TRandom3* fRndm;

  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;

  utl::ConfigParser* fCfgParser;

  HistoSvc* fHS;

  Double_t fNRun;
  Bool_t   fisMC=false;
  Double_t fGlobalEScale = 1.11398; //can be elimiated 

  //Calibration constants Runs
  std::vector<int> vNRun;
  std::vector<double> vEAvgRun;
  std::vector<double> vSlopeRun;

};
#endif
