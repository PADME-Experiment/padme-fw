#ifndef TargetMonitor_H
#define TargetMonitor_H

#define TARGETMONITOR_TIMELINE_SIZE 1000

#include "TFile.h"
#include "TString.h"
#include "TH1D.h"

#include "utlConfigParser.hh"

class Configuration;

class TargetMonitor
{

public:

  TargetMonitor(TString);
  ~TargetMonitor();

  void Initialize();
  void Finalize();
  void StartOfEvent();
  void EndOfEvent();
  void AnalyzeBoard(UChar_t);
  void AnalyzeChannel(UChar_t,UChar_t,Short_t*);

private:

  // Estimate total charge of channel from samples
  void ComputeChannelCharge(UChar_t,UChar_t,Short_t*);
  void ComputeChannelCumulativeCharge(UChar_t,UChar_t,Double_t*);

  // Compute total charge along X and Y
  void ComputeTotalChargeX();
  void ComputeTotalChargeY();

  // Estimate number of positrons on target for this event
  void ComputePoTs();
  void ComputeCumulativePoTs();

  Int_t OutputBeam();

  Configuration* fConfig;

  utl::ConfigParser* fConfigParser;

  Bool_t fIsBeam;

  // Reconstruction parameters
  Double_t fVoltageBin; // Volt per count (1V = 4096 counts)
  Double_t fTimeBin; // Time per sample in ns (1 Msps)
  Double_t fImpedance; // ADC input impedance in Ohm (50 Ohm)
  Double_t fChargeToPoTs; // Charge to PoTs conversion factor
  Bool_t fUseAbsSignal; // Enable (s<2048 -> s=4096-s) technique to get positive signal
  UInt_t fPedestalSamples; // Number of samples to use for pedestals
  UInt_t fSignalSamplesStart; // Index of first sample of signal (included)
  UInt_t fSignalSamplesEnd;   // Index of last sample of signal (excluded)
  UInt_t fBeamOutputRate; // Number of beam events between outputs

  // Map from [channel] to position X (1-16) and Y (17-32)
  Short_t fTarget_Map[32] = { 16,14,12,10,8,6,4,2,1,3,5,7,9,11,13,15,23,21,19,17,18,20,22,24,26,28,30,32,31,29,27,25 };

  // Array to store waveforms, charge and pedestals for a whole event
  Double_t fCharge[32];
  Double_t fCumulCharge[32];
  Double_t fPedestal[32];
  Short_t fWaveform[32][1024] = {{0}};

  // Array to store cumulative pedestal-subtracted waveforms for each channel
  Double_t fWF_Cumulative[32][1024] = {{0}};

  // Counters
  UInt_t fBeamEventCount = 0;
  //Double_t fStrip_Charge[32]; // Strips 1-32 are mapped to index 0-31
  Double_t fStrip_CumulativeCharge[32]; // Strips 1-32 are mapped to index 0-31

  // Total charge along X and Y
  Double_t fTotalChargeX;
  Double_t fTotalChargeY;
  Double_t fTotalCumulChargeX;
  Double_t fTotalCumulChargeY;

  // Histograms
  TH1D* fHTargetChargeX;
  TH1D* fHTargetChargeY;

  Double_t fCumulPoTs; // Average event PoTs from cumulative waveforms
  Double_t fEventPoTs; // Number of positrons on target for current event
  Double_t fEventPoTsTotal; // Number of PoTs for last 500 events
  Double_t fRunPoTsTotal; // Total number of PoTs for this run

  Double_t fTL_RunPoTs[TARGETMONITOR_TIMELINE_SIZE] = {0.}; // Total PoTs in this run
  Double_t fTL_EventPoTs[TARGETMONITOR_TIMELINE_SIZE] = {0.}; // Average values of PoTs in last 500 events
  Double_t fTL_CumulPoTs[TARGETMONITOR_TIMELINE_SIZE] = {0.}; // Average cumulative values of PoTs in last 500 events
  Double_t fTL_Time[TARGETMONITOR_TIMELINE_SIZE] = {0.}; // Event time of last event in average
  UInt_t   fTL_Current = 0; // Store current position in timeline (round-robin)

};
#endif
