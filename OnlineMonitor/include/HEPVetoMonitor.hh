#ifndef HEPVetoMonitor_H
#define HEPVetoMonitor_H

#include "TFile.h"
#include "TString.h"
#include "TH1D.h"

#include "utlConfigParser.hh"

#define HEPVETOMONITOR_PEAKS_MAX 100
#define HEPVETO_FINGERS 32

class Configuration;

class HEPVetoMonitor
{

public:

  HEPVetoMonitor(TString);
  ~HEPVetoMonitor();

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

  void ResetHEPVetoMap(Double_t map[HEPVETO_FINGERS]);
  void ResetHEPVetoMap(TH1D* map[HEPVETO_FINGERS]);
  void ResetHEPVetoWaveforms(Short_t map[HEPVETO_FINGERS][1024]);

  Configuration* fConfig;

  // Time calibration costants
  Double_t fHEPVeto_TCalib[HEPVETO_FINGERS];

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
  Double_t fPeakTime[HEPVETOMONITOR_PEAKS_MAX];

  // HEPVeto waveforms
  Short_t fBeamWF[HEPVETO_FINGERS][1024];
  Short_t fOffBeamWF[HEPVETO_FINGERS][1024];

  // Histograms
  TH1D* fHNPeaks[HEPVETO_FINGERS];
  TH1D* fHNPeaksOB[HEPVETO_FINGERS];

  // Map from Board13/[channel] to finger position
  Short_t fHEPVeto_map[32] = {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,28,29,30,31,24,25,26,27,20,21,22,23,16,17,18,19};

};
#endif
