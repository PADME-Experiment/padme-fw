#ifndef EVetoMonitor_H
#define EVetoMonitor_H

#include "TFile.h"
#include "TString.h"
#include "TH1D.h"

#include "utlConfigParser.hh"

#define EVETOMONITOR_PEAKS_MAX 100
#define EVETO_FINGERS 96

class Configuration;

class EVetoMonitor
{

public:

  EVetoMonitor(TString);
  ~EVetoMonitor();

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

  void ResetEVetoMap(Double_t map[EVETO_FINGERS]);
  void ResetEVetoMap(TH1D* map[EVETO_FINGERS]);
  void ResetEVetoWaveforms(Short_t map[EVETO_FINGERS][1024]);

  Configuration* fConfig;

  // Time calibration costants
  Double_t fEVeto_TCalib[EVETO_FINGERS];

  utl::ConfigParser* fConfigParser;

  Bool_t fIsBeam;
  Bool_t fIsOffBeam;

  UInt_t fBeamEventCount;
  UInt_t fBeamOutputRate;
  UInt_t fOffBeamEventCount;
  UInt_t fOffBeamOutputRate;

  // Pedestal and RMS of current channel
  UInt_t fPedestalSamples;
  Double_t fChannelPedestal;
  Double_t fChannelPedRMS;

  // Energy of current channel
  Double_t fChannelEnergy;

  // Number of peaks and their times for current channel
  UInt_t fNPeaks;
  Double_t fPeakTime[EVETOMONITOR_PEAKS_MAX];

  // SAC waveforms
  Short_t fBeamWF[EVETO_FINGERS][1024];
  Short_t fOffBeamWF[EVETO_FINGERS][1024];

  // Histograms
  TH1D* fHNPeaks[EVETO_FINGERS];
  TH1D* fHNPeaksOB[EVETO_FINGERS];

  // Map from Board[10,11,12]/[channel] to finger position
  Short_t fEVeto_map[3][32] = {
    {3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12,19,18,17,16,23,22,21,20,27,26,25,24,31,30,29,28},
    {35,34,33,32,39,38,37,36,43,42,41,40,47,46,45,44,51,50,49,48,55,54,53,52,59,58,57,56,63,62,61,60},
    {67,66,65,64,71,70,69,68,75,74,73,72,79,78,77,76,83,82,81,80,87,86,85,84,91,90,89,88,95,94,93,92}
  };

};
#endif
