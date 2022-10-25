// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2022-10-25
//
// --------------------------------------------------------------
#ifndef LeadGlassReconstruction_H
#define LeadGlassReconstruction_H

#include "PadmeVReconstruction.hh"

class LeadGlassReconstruction : public PadmeVReconstruction
{

public:

  LeadGlassReconstruction(TFile*, TString);
  ~LeadGlassReconstruction();

  void ProcessEvent(TRawEvent*);
  virtual void HistoInit();

private:

  Bool_t TriggerToBeSkipped();
  void AnalyzeEvent(TRawEvent*);
  void AnalyzeChannel(Short_t*);
  void ComputeTotalCharge(Short_t*);
  void ComputeBunchLength(Short_t*);

  // Define parameters for pedestal and total charge evaluation
  UInt_t fPedestalSamples;    // Number of samples to use for pedestals
  UInt_t fSignalSamplesStart; // Index of first sample of signal (included)
  UInt_t fSignalSamplesEnd;   // Index of last sample of signal (excluded)

  // Parameter to convert Total Charge to NPoTs
  Double_t fChargeToNPoTs;

  // Results of pedestal and total charge evaluation
  Double_t fLGPedestal; // Pedestal level from the first fPedestalSamples samples
  Double_t fLGPedRMS;   // Pedestal RMS
  Double_t fLGCharge;   // Total charge between fSignalSamplesStart and fSignalSamplesEnd
  Double_t fLGNPoTs;    // Number of Positrons on Target computed from total charge

  // Threshold to use to evaluate bunch length
  Double_t fBunchLengthThreshold;

  // Bunch length
  Double_t fBunchLength;

  // Histograms
  TH1D* fHLGPedestal;
  TH1D* fHLGPedRMS;
  TH1D* fHLGTotCharge;
  TH1D* fHLGNPoTs;
  TH1D* fHLGBunchLength;

};
#endif
