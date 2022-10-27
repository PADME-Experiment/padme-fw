#ifndef CombinedMonitor_H
#define CombinedMonitor_H

#include "TH1D.h"
#include "TFile.h"
#include "TString.h"

#include "utlConfigParser.hh"

class Configuration;
class ECalMonitor;
class LeadGlassMonitor;
class ETagMonitor;
class EVetoMonitor;
class PVetoMonitor;
class HEPVetoMonitor;
class SACMonitor;
class TargetMonitor;
class TriggerMonitor;
class ADCMonitor;

class CombinedMonitor
{

public:

  CombinedMonitor(TString);
  ~CombinedMonitor();

  void Initialize();
  void Finalize();
  void StartOfEvent();
  void EndOfEvent();

  void SetECalMonitor(ECalMonitor* m) { fECalMon = m; }
  void SetLeadGlassMonitor(LeadGlassMonitor* m) { fLeadGlassMon = m; }
  void SetETagMonitor(ETagMonitor* m) { fETagMon = m; }
  void SetEVetoMonitor(EVetoMonitor* m) { fEVetoMon = m; }
  void SetPVetoMonitor(PVetoMonitor* m) { fPVetoMon = m; }
  void SetHEPVetoMonitor(HEPVetoMonitor* m) { fHEPVetoMon = m; }
  void SetSACMonitor(SACMonitor* m) { fSACMon = m; }
  void SetTargetMonitor(TargetMonitor* m) { fTargetMon = m; }
  void SetTriggerMonitor(TriggerMonitor* m) { fTriggerMon = m; }
  void SetADCMonitor(ADCMonitor* m) { fADCMon = m; }

private:

  Int_t OutputBeam();
  Int_t OutputOffBeam();
  Int_t OutputCosmics();
  Int_t OutputRandom();

  ECalMonitor*      fECalMon;
  LeadGlassMonitor* fLeadGlassMon;
  ETagMonitor*      fETagMon;
  EVetoMonitor*     fEVetoMon;
  PVetoMonitor*     fPVetoMon;
  HEPVetoMonitor*   fHEPVetoMon;
  SACMonitor*       fSACMon;
  TargetMonitor*    fTargetMon;
  TriggerMonitor*   fTriggerMon;
  ADCMonitor*       fADCMon;

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

  // Ratio between total energy in ECal and number of PoTs in LeadGlass
  Double_t fETotOnNPoTsBM;

  // Acceptable range for ETot/NPoTs
  Double_t fETotOnNPoTsMin;
  Double_t fETotOnNPoTsMax;

  // Histograms
  TH1D* fHETotOnNPoTsBM;

  // Trend vectors
  std::vector<Double_t> fVCmbTimeBM;
  std::vector<Double_t> fVETotOnNPoTsBM;

  // Trend support file
  TString fTFCmbTrendsBM;

};
#endif
