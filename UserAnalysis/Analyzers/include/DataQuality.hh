#ifndef DataQuality_h
#define DataQuality_h 1

#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"
#include "GeneralInfo.hh"

struct observable{
  TString name;
  Double_t* valueAvg; // avg value vs time 
  Double_t* valueAvgCoarse; // avg value vs time [coarse bin]
  Int_t* nCounts; // number vs time
  Int_t* nCountsCoarse; // number vs time [coarse bin]
};

class DataQuality {

public:

  static DataQuality* GetInstance();



  ~DataQuality();

  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();

  Int_t GetDataQualityLevel() { return fDataQualityLevel; }


private:
  Double_t fTimeBin; // maybe should allow external conf
  Double_t fTimeBinCoarse; // maybe should allow external conf
  Int_t fNTimeBins; //number of time bins
  Int_t fNTimeBinsCoarse; // number of coarse time bins
  std::vector<observable> fObservables;

  Bool_t InitHistos(Int_t);
  static DataQuality* fInstance;


  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;
  utl::ConfigParser* fCfgParser;
  HistoSvc* fHS;
  GeneralInfo* fGeneralInfo;


  Int_t fDataQualityLevel; // 0->perfect 1->middle 2->bad
};
#endif
