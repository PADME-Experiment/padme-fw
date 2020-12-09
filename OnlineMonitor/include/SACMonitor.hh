#ifndef SACMonitor_H
#define SACMonitor_H

#include "TFile.h"
#include "TString.h"

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
  void Analyze(UChar_t,UChar_t,Short_t*);

private:

  Double_t GetChannelEnergy(UChar_t,UChar_t,Short_t*);

  void GetChannelPeaks(UChar_t,UChar_t,Short_t*);

  void ResetSACMap(Double_t map[5][5]);
  void ResetSACWaveforms(Short_t map[5][5][1024]);

  Configuration* fConfig;

  // Channel map of calibration constants for cosmics
  Double_t   fSAC_CosmClb[32];

  utl::ConfigParser* fConfigParser;

  Bool_t fIsCosmics;

  Double_t fSAC_CosmSum[5][5]; // 100 events map
  Double_t fSAC_CosmEvt[5][5]; // Last event map

  UInt_t fCosmicsEventCount;
  UInt_t fCosmicsOutputRate;

  // Number of peaks and their times for current channel
  UInt_t fNPeaks;
  Double_t fPeakTime[SACMONITOR_PEAKS_MAX];

  // SAC waveforms
  Short_t fCosmWF[5][5][1024];

  // Map from Board27/[channel] to position as yx
  Short_t fSAC_map[32] = { 00,10,20,30,40,01,11,21,31,41,02,12,22,32,42,03,13,23,33,43,04,14,24,34,44,-1,-1,-1,-1,-1,-1,-1 };

};
#endif
