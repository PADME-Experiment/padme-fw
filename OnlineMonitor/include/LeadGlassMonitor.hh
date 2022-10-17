#ifndef LeadGlassMonitor_H
#define LeadGlassMonitor_H

#define LEADGLASS_BOARD   14
#define LEADGLASS_CHANNEL 31

#include "TH1D.h"
#include "TFile.h"
#include "TString.h"

#include "utlConfigParser.hh"

class Configuration;

class LeadGlassMonitor
{

public:

  LeadGlassMonitor(TString);
  ~LeadGlassMonitor();

  void Initialize();
  void Finalize();
  void StartOfEvent();
  void EndOfEvent();
  void AnalyzeBoard(UChar_t);
  void AnalyzeChannel(UChar_t,UChar_t,Short_t*);

private:

  void ComputeTotalCharge(Short_t*);

  void ComputeBunchLength(Short_t*);

  Int_t OutputBeam();
  Int_t OutputOffBeam();
  Int_t OutputCosmics();
  Int_t OutputRandom();

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

  // Define parameters for pedestal and total charge evaluation
  UInt_t fPedestalSamples;    // Number of samples to use for pedestals
  UInt_t fSignalSamplesStart; // Index of first sample of signal (included)
  UInt_t fSignalSamplesEnd;   // Index of last sample of signal (excluded)

  // Parameter to convert Total Charge to NPoTs
  Double_t fChargeToNPoTs;

  // Results of pedestal and total charge evaluation
  Double_t fChannelPedestal; // Pedestal level from the first fPedestalSamples samples
  Double_t fChannelPedRMS;   // Pedestal RMS
  Double_t fChannelCharge;   // Total charge between fSignalSamplesStart and fSignalSamplesEnd

  // Histograms
  TH1D* fHLGPedestalBM;
  TH1D* fHLGPedRMSBM;
  TH1D* fHLGTotChargeBM;
  TH1D* fHLGNPoTsBM;
  TH1D* fHLGBunchLengthBM;

  // Waveform vectors
  Short_t fLGWaveformBM[1024];
  Int_t   fLGWaveSumBM[1024];

  // Waveform saturation flag
  Bool_t fWFSaturated;

  // Threshold to use to evaluate bunch length
  Double_t fBunchLengthThreshold;

  // Bunch length
  Double_t fBunchLength;

  // Trend vectors
  std::vector<Double_t> fVLGTimeBM;
  std::vector<Double_t> fVLGNPoTsBM;
  std::vector<Double_t> fVLGBunchLengthBM;

  // Trend support file
  TString fTFLGTrendsBM;

  // Range for NPoTs and Bunch Length trend plots
  Double_t fNPoTsRangeMin;
  Double_t fNPoTsRangeMax;
  Double_t fBunchLengthRangeMin;
  Double_t fBunchLengthRangeMax;

};
#endif
