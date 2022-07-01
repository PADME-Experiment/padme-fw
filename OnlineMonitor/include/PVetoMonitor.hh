#ifndef PVetoMonitor_H
#define PVetoMonitor_H

#include "TFile.h"
#include "TString.h"
#include "TH1D.h"

#include "utlConfigParser.hh"

#define PVETOMONITOR_PEAKS_MAX 100
#define PVETO_FINGERS 96

class Configuration;

class PVetoMonitor
{

public:

  PVetoMonitor(TString);
  ~PVetoMonitor();

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

  void ResetPVetoMap(Double_t map[PVETO_FINGERS]);
  void ResetPVetoMap(TH1D* map[PVETO_FINGERS]);
  void ResetPVetoWaveforms(Short_t map[PVETO_FINGERS][1024]);

  Configuration* fConfig;

  // Time calibration costants
  Double_t fPVeto_TCalib[PVETO_FINGERS];

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
  Double_t fPeakTime[PVETOMONITOR_PEAKS_MAX];

  // PVeto waveforms
  Short_t fBeamWF[PVETO_FINGERS][1024];
  Short_t fOffBeamWF[PVETO_FINGERS][1024];

  // Histograms
  TH1D* fHNPeaks[PVETO_FINGERS];
  TH1D* fHNPeaksOB[PVETO_FINGERS];

  // Map from Board[10,11,12]/[channel] to finger position
  Short_t fPVeto_map[3][32] = {
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31},
    {32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63},
    {64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95}
  };

};
#endif
