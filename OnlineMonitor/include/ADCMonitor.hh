#ifndef ADCMonitor_H
#define ADCMonitor_H

#include "TFile.h"
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"

#include "utlConfigParser.hh"

#define ADCMONITOR_NBOARDS 32

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
  void AnalyzeBoard(UChar_t);
  void AnalyzeChannel(UChar_t,UChar_t,Short_t*);

private:

  void ComputeChannelMeanRMS(UChar_t,UChar_t,Short_t*);
  void ComputeChannelPedestal(UChar_t,UChar_t,Short_t*);

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

  // Mean and RMS of current channel
  Double_t fChannelMean;
  Double_t fChannelRMS;

  // Pedestal and pedestal RMS of current channel
  Double_t fPedestal;
  Double_t fPedRMS;

  // Flags to choose which pedestals to show
  Bool_t fShowPedBeam;
  Bool_t fShowPedOffBeam;
  Bool_t fShowPedCosmics;
  Bool_t fShowPedRandom;

  // Histograms

  TH2D* fHChanMeanOB; // Channel mean of off-beam events
  TH2D* fHChanMeanRM; // Channel mean of random events

  TH2D* fHChanRMSOB; // Channel RMS of off-beam events
  TH2D* fHChanRMSRM; // Channel RMS of random events

  TH2D* fHGroupSICOB; // Group SIC errors of off-beam events
  TH2D* fHGroupSICRM; // Group SIC errors of random events

  TH2D* fHPedestalBM; // Channel pedestal of beam events
  TH2D* fHPedestalCS; // Channel pedestal of cosmics events
  TH2D* fHPedestalOB; // Channel pedestal of off-beam events
  TH2D* fHPedestalRM; // Channel pedestal of random events

  TH2D* fHPedRMSBM; // Channel pedestal RMS of beam events
  TH2D* fHPedRMSCS; // Channel pedestal RMS of cosmics events
  TH2D* fHPedRMSOB; // Channel pedestal RMS of off-beam events
  TH2D* fHPedRMSRM; // Channel pedestal RMS of random events

  TH1D* fHChPedestal[ADCMONITOR_NBOARDS][32]; // Channel pedestal of beam events
  TH1D* fHChPedRMS[ADCMONITOR_NBOARDS][32]; // Channel pedestal RMS of beam events

  // Number of samples to use for pedestal for each board
  UShort_t fPedestalSamples[ADCMONITOR_NBOARDS];

  // Counters for Mean and pedestals of random events (needed to compute ratio)
  Double_t fCountMeanRM[ADCMONITOR_NBOARDS*32];
  Double_t fCountPedRM[ADCMONITOR_NBOARDS*32];

  // Map of ADC to analyze (all 0 by default)
  Short_t fADC_map[ADCMONITOR_NBOARDS] = { };

};
#endif
