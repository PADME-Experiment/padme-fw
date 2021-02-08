#ifndef MULTIHIT_HH
#define MULTIHIT_HH

#define MULTIHIT_MAXHITS 10
#define MULTIHIT_NSAMPLES 2014

#include "utlConfigParser.hh"

#include "TString.h"

class MultiHit
{
public:

  MultiHit(TString);
  ~MultiHit();

  void SetSmoothLength(UInt_t l) { fSmoothLen = l; }
  UInt_t GetSmoothLength() { return fSmoothLen; }

  void SetDerivativeLength(UInt_t l) { fDerivativeLen = l; }
  UInt_t GetDerivativeLength() { return fDerivativeLen; }

  void SetMaxNHits(UInt_t n) { fMaxNHits = n; }
  UInt_t GetMaxNHits() { return fMaxNHits; }

  Int_t SetWaveform(UShort_t,Double_t*);

  void SetWaveformSaturated() { fWorkIsSaturated = true; }
  void UnsetWaveformSaturated() { fWorkIsSaturated = false; }

  Int_t FindHits();

  UInt_t GetNHits() { return fNHits; }
  UInt_t GetHitBin(UInt_t);
  Double_t GetHitIntegral(UInt_t);

private:

  Int_t ReadTemplate();

  void SmoothWF();
  Int_t FindMax();
  Int_t SubtractTemplate();

  utl::ConfigParser *fConfigParser;

  // Multihit algorithm parameters
  UInt_t fSmoothLen;      // Smoothing length (number of bins)
  UInt_t fDerivativeLen;  // Derivative length (number of bins)
  Double_t fDerivativeThreshold; // Threshold (in mV) for derivative peak when finding maximum
  UInt_t fMaxNHits;       // Maximum number of hits to find

  // Arrays with hit quantities
  UInt_t fNHits;
  UInt_t fHitBin[MULTIHIT_MAXHITS];
  Double_t fHitIntegral[MULTIHIT_MAXHITS];

  // Working waveform data
  Bool_t fWorkIsSaturated;
  UInt_t fWorkIMax;
  Double_t fWorkMaxValue;
  Double_t fWorkWF[MULTIHIT_NSAMPLES];

  // Template waveform data
  UInt_t fTempIMax;
  Double_t fTempMaxValue;
  Double_t fTempIntegral;
  Double_t fTempWF[MULTIHIT_NSAMPLES];

};

#endif
