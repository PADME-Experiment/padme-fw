#ifndef LeadGlassMonitor_H
#define LeadGlassMonitor_H

#define LEADGLASS_BOARD   14
#define LEADGLASS_CHANNEL 31
#define LEADGLASS2_BOARD   21
#define LEADGLASS2_CHANNEL 31

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

  // Methods to access local results for combined analysis
  Double_t GetLeadGlassNPoTs() { return fLGNPoTs; }

private:

  void ComputeTotalCharge(Short_t*);
  void ComputeTotalChargeLED(Short_t*);

  void ComputeBunchLength(Short_t*);

  Int_t OutputBeam();
  Int_t OutputOffBeam();
  Int_t OutputCosmics();
  Int_t OutputRandom();
  Int_t OutputLED();

  Configuration* fConfig;
  utl::ConfigParser* fConfigParser;

  Bool_t fIsBeam;
  Bool_t fIsOffBeam;
  Bool_t fIsCosmics;
  Bool_t fIsRandom;
  Bool_t fIsLED;

  UInt_t fBeamOutputRate;
  UInt_t fOffBeamOutputRate;
  UInt_t fCosmicsOutputRate;
  UInt_t fRandomOutputRate;
  UInt_t fLEDOutputRate;

  UInt_t fBeamEventCount;
  UInt_t fOffBeamEventCount;
  UInt_t fCosmicsEventCount;
  UInt_t fRandomEventCount;
  UInt_t fLEDEventCount;

  // Define parameters for pedestal and total charge evaluation
  UInt_t fPedestalSamples;    // Number of samples to use for pedestals
  UInt_t fSignalSamplesStart; // Index of first sample of signal (included)
  UInt_t fSignalSamplesEnd;   // Index of last sample of signal (excluded)

  // Parameter to convert Total Charge to NPoTs
  Double_t fChargeToNPoTs;

  // Factor to take into account number of streams analyzed (F=N_streams_collected/N_streams_analyzed)
  // E.g. if 5 streams are collected and 1 is analyzed then F=5/1=5.
  Double_t fStreamsFactor;

  // Results of pedestal and total charge evaluation
  Double_t fChannelPedestal; // Pedestal level from the first fPedestalSamples samples
  Double_t fChannelPedRMS;   // Pedestal RMS
  Double_t fChannelCharge;   // Total charge for channel

  // Total charge of two blocks and ratio
  Double_t fLG1TotalChargeBM;
  Double_t fLG2TotalChargeBM;
  Double_t fTotalChargeRatioBM;
  Double_t fLG1TotalChargeLD;
  Double_t fLG2TotalChargeLD;
  Double_t fTotalChargeRatioLD;

  // Histograms
  TH1D* fHLGPedestalBM;
  TH1D* fHLGPedRMSBM;
  TH1D* fHLGTotChargeBM;
  TH1D* fHLG2TotChargeBM;
  TH1D* fHLGTotChargeRatioBM;
  TH1D* fHLGNPoTsBM;
  TH1D* fHLGNPoTsTotBM;
  TH1D* fHLGBunchLengthBM;
  TH1D* fHLGBunchLengthTotBM;
  TH1D* fHLGBunchBBQBM;
  TH1D* fHLGBunchBBQTotBM;
  TH1D* fHLGBunchDensityBM;
  TH1D* fHLGBunchDensityTotBM;

  TH1D* fHLGPedestalLD;
  TH1D* fHLG2PedestalLD;
  TH1D* fHLGPedRMSLD;
  TH1D* fHLG2PedRMSLD;
  TH1D* fHLGTotChargeLD;
  TH1D* fHLG2TotChargeLD;
  TH1D* fHLGTotChargeRatioLD;

  // Waveform vectors
  Short_t fLGWaveformBM[1024];
  Short_t fLGWaveformLD[1024];
  Short_t fLG2WaveformLD[1024];
  Int_t   fLGWaveSumBM[1024];
  Int_t   fLGWaveSumLD[1024];
  Int_t   fLG2WaveSumLD[1024];

  // Waveform saturation flag
  Bool_t fWFSaturated;

  // Threshold to use to evaluate bunch length
  Double_t fBunchLengthThreshold;

  // Bunch length, quality and density
  Double_t fBunchLength;
  Double_t fBunchBBQ;
  Double_t fBunchDensity;

  // Number of positrons on target (NPoTs)
  Double_t fLGNPoTs;

  // Trend vectors
  std::vector<Double_t> fVLGTimeBM;
  std::vector<Double_t> fVLGTotChargeBM;
  std::vector<Double_t> fVLG2TotChargeBM;
  std::vector<Double_t> fVLGTotChargeRatioBM;
  std::vector<Double_t> fVLGNPoTsBM;
  std::vector<Double_t> fVLGNPoTsTotBM;
  std::vector<Double_t> fVLGBunchLengthBM;
  std::vector<Double_t> fVLGBunchBBQBM;
  std::vector<Double_t> fVLGBunchDensityBM;

  std::vector<Double_t> fVLGTimeLD;
  std::vector<Double_t> fVLGTotChargeLD;
  std::vector<Double_t> fVLG2TotChargeLD;
  std::vector<Double_t> fVLGTotChargeRatioLD;

  // Trend support files
  TString fTFLGTrendsBM;
  TString fTFLGTrendsLD;

  // Range for NPoTs and Bunch Length trend plots
  Double_t fNPoTsRangeMin;
  Double_t fNPoTsRangeMax;
  Double_t fBunchLengthRangeMin;
  Double_t fBunchLengthRangeMax;

};
#endif
