#ifndef NPoTAnalysis_h
#define NPoTAnalysis_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"
#include "GeneralInfo.hh"


class NPoTAnalysis {

public:

  static NPoTAnalysis* GetInstance();



  ~NPoTAnalysis();

  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();

  Double_t GetNPoT() { return fNPoT; }
  Double_t GetNPoTLG() { return fNPoTLG; }
  Double_t GetNPoTLGCorr() { return fNPoTLGCorr; }


private:

  Bool_t InitHistos();

  static NPoTAnalysis* fInstance;
  //  TRandom3* fRndm;

  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;

  utl::ConfigParser* fCfgParser;

  HistoSvc* fHS;
  GeneralInfo* fGeneralInfo;


  Double_t fEvtCount;
  Double_t fNPoT;
  Double_t fXPos;
  Double_t fYPos;

  Double_t fNPoTLG;
  Double_t fNPoTLGCorr;
  Double_t fNPoTBL;

  std::vector<double> vNRun;
  std::vector<double> vNEvt;

  std::vector<double> vNPoT;
  std::vector<double> vNPoTLG;

  std::vector<double> vTotPoT;
  std::vector<double> vTotPoTLG;

  std::vector<double> vNPoTRun;
  std::vector<double> vNPoTRunLG;

  Int_t Neve;

  Double_t fNRun;
  Double_t fTimeStamp;
  Double_t fCurrentRun;
  Double_t TotPoT;
  Double_t TotPoTLG;
  Double_t TotPoTRun;
  Double_t TotPoTPhys;
  Double_t TotPoTNoPhys;
  Double_t TotPoTOver30K;
  Double_t TotPoTOver5K;

};
#endif
