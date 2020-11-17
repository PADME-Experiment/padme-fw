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

  Double_t GetChannelCharge(UChar_t,UChar_t,Short_t*);

  Configuration* fConfig;

  utl::ConfigParser* fConfigParser;

  Bool_t fIsBeam;

  // Reconstruction parameters
  Double_t fVoltageBin; // Volt per count (1V = 4096 counts)
  Double_t fTimeBin; // Time per sample in ns (1 Msps)
  Double_t fImpedance; // ADC input impedance in Ohm (50 Ohm)
  UInt_t fPedestalSamples; // Number of samples to use for pedestals
  UInt_t fSignalSamplesStart; // Index of first sample of signal (included)
  UInt_t fSignalSamplesEnd;   // Index of last sample of signal (excluded)

  // Map from [board][channel] to position as yyxx
  Short_t fTarget_map[32] = {{0}};

  // Counters
  UInt_t fEventCounter = 0;
  Double_t fStrip_charge[32]; // Strip 1-32 to index 0-31

};
#endif
