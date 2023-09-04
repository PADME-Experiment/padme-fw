#ifndef ECalCalib_h
#define ECalCalib_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"

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
  Double_t FixPosition();
  Double_t CorrectESlope();

  Double_t GetBeamEnergy(){return fBeamEnergy;};
  Double_t GetCOGX(){return fCOGX;};
  Double_t GetCOGY(){return fCOGY;};


private:
  static ECalCalib* fInstance;
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

  // variable to be returned on request
  Double_t fBeamEnergy;
  Double_t fCOGX;
  Double_t fCOGY;

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
