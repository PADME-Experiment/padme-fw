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

  Double_t GetPedestal()    { return fLGPedestal;  }
  Double_t GetPedestalRMS() { return fLGPedRMS;    }
  Double_t GetCharge()      { return fLGCharge;    }
  Double_t GetEnergy()      { return fLGEnergy;    }
  Double_t GetNPoTs()       { return fLGNPoTs;     }
  Double_t GetBunchLength() { return fBunchLength; }

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

  // Results of pedestal and total charge evaluation
  Double_t fLGPedestal; // Pedestal level from the first fPedestalSamples samples
  Double_t fLGPedRMS;   // Pedestal RMS
  Double_t fLGCharge;   // Total charge between fSignalSamplesStart and fSignalSamplesEnd

  // Calibration parameter to convert Total Charge to Total Energy
  Double_t fChargeToEnergy;

  // Total energy released in LeadGlass
  Double_t fLGEnergy;

  // Parameter to convert Total Charge to NPoTs
  Double_t fChargeToNPoTs;

  // Number of Positrons on Target computed from total charge
  Double_t fLGNPoTs;

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
