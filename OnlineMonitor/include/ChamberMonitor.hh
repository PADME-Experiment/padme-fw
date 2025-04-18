#ifndef ChamberMonitor_H
#define ChamberMonitor_H

#include "TH1D.h"
#include "TFile.h"
#include "TString.h"

#include "utlConfigParser.hh"

class Configuration;
class ChamberEvent;

class ChamberMonitor
{

public:

  ChamberMonitor(TString);
  ~ChamberMonitor();

  void Initialize();
  void Finalize();
  void StartOfEvent();
  void EndOfEvent();
  void AnalyzeEvent(ChamberEvent*);

private:

  Int_t OutputBeam();

  Configuration* fConfig;
  utl::ConfigParser* fConfigParser;

  Bool_t fIsBeam;
  Bool_t fIsOffBeam;
  Bool_t fIsCosmics;
  Bool_t fIsRandom;

  UInt_t fBeamOutputRate;
  UInt_t fOffBeamOutputRate;
  UInt_t fCosmicsOutputRate;
  UInt_t fRandomOutputRate;

  UInt_t fBeamEventCount;
  UInt_t fOffBeamEventCount;
  UInt_t fCosmicsEventCount;
  UInt_t fRandomEventCount;

  // Histograms
  //TH1D* fHLGPedestalBM;

  // Trend vectors
  //std::vector<Double_t> fVLGTimeBM;

  // Trend support file
  TString fTFChTrendsBM;

  // Range for NPoTs and Bunch Length trend plots
  //Double_t fNPoTsRangeMin;
  //Double_t fNPoTsRangeMax;

};
#endif
