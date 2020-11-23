#ifndef TriggerMonitor_H
#define TriggerMonitor_H

#include "TFile.h"
#include "TString.h"

#include "utlConfigParser.hh"

class Configuration;

class TriggerMonitor
{

public:

  TriggerMonitor(TString);
  ~TriggerMonitor();

  void Initialize();
  void Finalize();
  void StartOfEvent();
  void EndOfEvent();
  void Analyze(UChar_t,UChar_t,Short_t*);
  Double_t GetTriggerTime() { return fTriggerTime; }

private:

  // Estimate total charge of channel from samples
  void ComputeTriggerTime(UChar_t,UChar_t,Short_t*);

  Configuration* fConfig;

  utl::ConfigParser* fConfigParser;

  // Reconstruction parameters
  Double_t fVoltageBin; // Volt per count (1V = 4096 counts)
  Double_t fImpedance; // ADC input impedance in Ohm (50 Ohm)
  UInt_t fEventOutputScale; // Number of events between outputs

  // Array to store waveforms, charge and pedestals for a whole event
  Short_t fWaveform[1024] = {0};

  // Counters
  UInt_t fEventCounter = 0;

  // Trigger time (in bin units) of last processed trigger
  Double_t fTriggerTime;

};
#endif
