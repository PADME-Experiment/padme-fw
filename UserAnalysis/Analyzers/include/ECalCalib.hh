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
  Double_t FixPosition();
  Double_t CorrectESlope();

private:

  Bool_t InitHistos();

  //  TRandom3* fRndm;

  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;

  utl::ConfigParser* fCfgParser;

  HistoSvc* fHS;

  Int_t fNClusters;

  Int_t fNRun;	     
  Int_t fCurrentRun;     
  Int_t fCurrentRunIndex;


  Bool_t   fisMC=false;
  Double_t fGlobalEScale = 1.11398; //used if no RUN dependent value is found
  Double_t fGlobalESlope = 5E-5;    //used if no RUN dependent value is found
  //  Double_t fGlobalEScale = 1.; //can be elimiated 

  //Calibration constants Runs
  std::vector<int> vNRun;
  std::vector<int> vEBeam;
  std::vector<double> vEAvgRun;
  std::vector<double> vE0Run;
  std::vector<double> vSlopeRun;

  std::vector<double> vCOGX;
  std::vector<double> vCOGY;


};
#endif
