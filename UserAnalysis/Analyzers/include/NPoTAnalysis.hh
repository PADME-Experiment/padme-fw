#ifndef NPoTAnalysis_h
#define NPoTAnalysis_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"

class NPoTAnalysis {

public:

  NPoTAnalysis(TString c = "config/UserAnalysis.conf", Int_t v = 0);
  ~NPoTAnalysis();

  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();

  Double_t GetNPoT() { return fNPoT; }

private:

  Bool_t InitHistos();

  //  TRandom3* fRndm;

  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;

  utl::ConfigParser* fCfgParser;

  HistoSvc* fHS;

  Double_t fNPoT;
  Double_t fXPos;
  Double_t fYPos;

  std::vector<double> vNPoT;
  std::vector<double> vTotPoT;
  std::vector<double> vNEvt;
  std::vector<double> vNPoTRun;
  std::vector<double> vNRun;

  Int_t Neve;
  Double_t fNRun;
  Double_t fTimeStamp;
  Double_t fCurrentRun;
  Double_t TotPoT;
  Double_t TotPoTRun;
  Double_t TotPoTPhys;
  Double_t TotPoTNoPhys;
  Double_t TotPoTOver30K;
  Double_t TotPoTOver5K;

};
#endif
