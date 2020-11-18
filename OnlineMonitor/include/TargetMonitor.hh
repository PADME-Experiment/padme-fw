#ifndef TargetMonitor_H
#define TargetMonitor_H

#include "TFile.h"
#include "TString.h"

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
  void Analyze(UChar_t,UChar_t,Short_t*);

private:

  // Estimate total charge of channel from samples
  void ComputeChannelCharge(UChar_t,UChar_t,Short_t*);

  Configuration* fConfig;

  utl::ConfigParser* fConfigParser;

  Bool_t fIsBeam;

  // Reconstruction parameters
  Double_t fVoltageBin; // Volt per count (1V = 4096 counts)
  Double_t fTimeBin; // Time per sample in ns (1 Msps)
  Double_t fImpedance; // ADC input impedance in Ohm (50 Ohm)
  Bool_t fUseAbsSignal; // Enable (s<2048 -> s=4096-s) technique to get positive signal
  UInt_t fPedestalSamples; // Number of samples to use for pedestals
  UInt_t fSignalSamplesStart; // Index of first sample of signal (included)
  UInt_t fSignalSamplesEnd;   // Index of last sample of signal (excluded)

  // Map from [channel] to position X (1-16) and Y (17-32)
  Short_t fTarget_map[32] = { 16,14,12,10,8,6,4,2,1,3,5,7,9,11,13,15,23,21,19,17,18,20,22,24,26,28,30,32,31,29,27,25 };

  // Array to store waveforms, charge and pedestals for a whole event
  Double_t fCharge[32];
  Double_t fPedestal[32];
  Short_t fWaveform[32][1024] = {{0}};

  // Counters
  UInt_t fEventCounter = 0;
  Double_t fStrip_charge[32]; // Strip 1-32 to index 0-31

};
#endif
