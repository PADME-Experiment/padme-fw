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
  void AnalyzeBoard(UChar_t);
  void AnalyzeGroup(UChar_t,UChar_t,Short_t*);
  Double_t GetTriggerTime() { return fTriggerTime; }

private:

  // Estimate trigger time from set of samples
  void ComputeTriggerTime(UChar_t,UChar_t,Short_t*);

  void ResetWaveforms(Short_t map[4][1024]);

  Int_t OutputData();

  Configuration* fConfig;

  utl::ConfigParser* fConfigParser;

  // Reconstruction parameters
  Double_t fVoltageBin; // Volt per count (1V = 4096 counts)
  Double_t fImpedance; // ADC input impedance in Ohm (50 Ohm)
  UInt_t fEventOutputRate; // Number of events between outputs

  // Array to store waveforms of four groups in an event
  Short_t fEventWF[4][1024];

  // Counters
  UInt_t fEventCounter;

  // Board to show on monitor
  UInt_t fBoardToShow;

  // Trigger time (in bin units) of last processed trigger
  Double_t fTriggerTime;

};
#endif
