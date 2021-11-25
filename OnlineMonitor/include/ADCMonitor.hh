#ifndef ADCMonitor_H
#define ADCMonitor_H

#include "TFile.h"
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"

#include "utlConfigParser.hh"

#define ADCMONITOR_NBOARDS 29

class Configuration;

class ADCMonitor
{

public:

  ADCMonitor(TString);
  ~ADCMonitor();

  void Initialize();
  void Finalize();
  void StartOfEvent();
  void EndOfEvent();
  void Analyze(UChar_t,UChar_t,Short_t*);

private:

  void ComputeChannelRMS(UChar_t,UChar_t,Short_t*);

  Int_t OutputBeam();
  Int_t OutputOffBeam();
  Int_t OutputRandom();
  Int_t OutputCosmics();

  Configuration* fConfig;

  utl::ConfigParser* fConfigParser;

  Bool_t fIsCosmics;
  Bool_t fIsBeam;
  Bool_t fIsOffBeam;
  Bool_t fIsRandom;

  UInt_t fBeamEventCount;
  UInt_t fBeamOutputRate;
  UInt_t fOffBeamEventCount;
  UInt_t fOffBeamOutputRate;
  UInt_t fCosmicsEventCount;
  UInt_t fCosmicsOutputRate;
  UInt_t fRandomEventCount;
  UInt_t fRandomOutputRate;

  // RMS of current channel
  Double_t fChannelRMS;

  // Histograms
  TH2D* fHChanRMSOB;
  TH2D* fHChanRMSRM;

  // Map of ADC to analyze (all 0 by default)
  Short_t fADC_map[ADCMONITOR_NBOARDS] = { };

};
#endif
