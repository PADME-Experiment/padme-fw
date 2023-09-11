
#ifndef SinglePhoton_h
#define SinglePhoton_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"

class SinglePhoton {

public:

  SinglePhoton(TString c = "config/UserAnalysis.conf", Int_t v = 0);
  ~SinglePhoton();

  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();

private:

  Bool_t InitHistos();

  //  TRandom3* fRndm;

  std::vector<double> vNPoT;
  std::vector<double> vRatio;
  std::vector<double> vETotECal;
  std::vector<double> vNEvt;
  std::vector<double> vETotOvNPot;


 //Defining Cut variables
  Double_t ClRadMin; 
  Double_t ClRadMax;

  Double_t MinECluster; 
  Double_t MaxECluster;

  Int_t Navg;
  Double_t NPoTAvg;
  Double_t ETotECal;
  Double_t EToTAvg;
  Double_t HitEToTAvg;

  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;
  utl::ConfigParser* fCfgParser;
  HistoSvc* fHS;

  Int_t Neve;
  Double_t fCurrentRun;
  Int_t fNRun;

};
#endif
