#ifndef SACMonitor_H
#define SACMonitor_H

#include "TFile.h"
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"

#include "utlConfigParser.hh"

#define SACMONITOR_PEAKS_MAX 100

class Configuration;

class SACMonitor
{

public:

  SACMonitor(TString);
  ~SACMonitor();

  void Initialize();
  void Finalize();
  void StartOfEvent();
  void EndOfEvent();
  void AnalyzeBoard(UChar_t);
  void AnalyzeChannel(UChar_t,UChar_t,Short_t*);

private:

  void ComputeChannelEnergy(UChar_t,UChar_t,Short_t*);
  void ComputeChannelPedestal(UChar_t,UChar_t,Short_t*);
  void FindChannelPeaks(UChar_t,UChar_t,Short_t*);

  Int_t OutputBeam();
  Int_t OutputOffBeam();
  Int_t OutputCosmics();
  Int_t OutputRandom();

  void ResetSACMap(Double_t map[5][5]);
  void ResetSACMap(TH1D* map[5][5]);
  void ResetSACWaveforms(Short_t map[5][5][1024]);

  Configuration* fConfig;

  // Channel map of calibration constants for cosmics
  Double_t   fSAC_CosmClb[32];

  utl::ConfigParser* fConfigParser;

  Bool_t fIsCosmics;
  Bool_t fIsBeam;
  Bool_t fIsOffBeam;
  Bool_t fIsRandom;

  Double_t fSAC_CosmSum[5][5]; // 100 events map
  Double_t fSAC_CosmEvt[5][5]; // Last event map

  Double_t fSAC_BeamSum[5][5]; // 100 events map
  Double_t fSAC_BeamEvt[5][5]; // Last event map

  UInt_t fBeamEventCount;
  UInt_t fBeamOutputRate;
  UInt_t fOffBeamEventCount;
  UInt_t fOffBeamOutputRate;
  UInt_t fCosmicsEventCount;
  UInt_t fCosmicsOutputRate;
  UInt_t fRandomEventCount;
  UInt_t fRandomOutputRate;

  // Pedestal and RMS of current channel
  UInt_t fPedestalSamples;
  Double_t fChannelPedestal;
  Double_t fChannelPedRMS;
  Double_t fChannelRMS;

  // Energy of current channel
  Double_t fChannelEnergy;

  // Number of peaks and their times for current channel
  UInt_t fNPeaks;
  Double_t fPeakTime[SACMONITOR_PEAKS_MAX];

  // SAC waveforms
  Short_t fCosmWF[5][5][1024];
  Short_t fBeamWF[5][5][1024];
  Short_t fOffBeamWF[5][5][1024];

  // Histograms
  TH1D* fHNPeak[5][5];
  TH1D* fHTPeak[5][5];

  // Map from Board27/[channel] to position as yx
  Short_t fSAC_map[32] = { 00,10,20,30,40,01,11,21,31,41,02,12,22,32,42,03,13,23,33,43,04,14,24,34,44,-1,-1,-1,-1,-1,-1,-1 };

};
#endif
