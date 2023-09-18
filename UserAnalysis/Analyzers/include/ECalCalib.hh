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
  Double_t CorrectETimeSlope(Int_t);  // input is correction level: 0->no correction, 1->default, 2->enhanced
  Double_t CorrectEScale(Int_t);      // input is correction level: 0->no correction, 1->default, 2->enhanced
  Int_t NClusterPairSimpleSelection();

  Int_t fVerbose;
  PadmeAnalysisEvent* fEvent;
  HistoSvc* fHS;
  utl::ConfigParser* fCfgParser; // should handle the correction level for energy scale and energy-time slope
  GeneralInfo* fGeneralInfo;

  Bool_t   fisMC;
  Int_t fNPairs;
  Int_t fPairIndex[2]; // index of cluster pairs
};
#endif
