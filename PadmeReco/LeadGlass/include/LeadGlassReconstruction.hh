// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2022-10-25
//
// --------------------------------------------------------------
#ifndef LeadGlassReconstruction_H
#define LeadGlassReconstruction_H

#include "PadmeVReconstruction.hh"

#define N_LEADGLASS 2

class LeadGlassReconstruction : public PadmeVReconstruction
{

public:

  LeadGlassReconstruction(TFile*, TString);
  ~LeadGlassReconstruction();

  void ProcessEvent(TRawEvent*);
  virtual void HistoInit();

  Bool_t LeadGlassFound() { return fLeadGlassFound; }
  //UChar_t LeadGlassID() { return leadglassID; }

  Double_t GetPedestal(UChar_t leadglassID)    { return fLGPedestal[leadglassID];  }
  Double_t GetPedestalRMS(UChar_t leadglassID) { return fLGPedRMS[leadglassID];    }
  Int_t GetStartIndexCell(UChar_t leadglassID) { return fLGStartIndexCell[leadglassID];    }
  Double_t GetCharge(UChar_t leadglassID)      { return fLGCharge[leadglassID];    }
  Double_t GetEnergy(UChar_t leadglassID)      { return fLGEnergy[leadglassID];    }
  Double_t GetNPoTs()       { return fLGNPoTs;     }
  Double_t GetBunchLength() { return fBunchLength; }
  Double_t GetBunchBBQ()    { return fBunchBBQ;    }

private:

  Bool_t TriggerToBeSkipped();
  void AnalyzeEvent(TRawEvent*);
  void AnalyzeChannel(UChar_t leadglassID, Short_t*);
  Double_t ComputePedestal(UChar_t leadglassID, Short_t*);
  void ComputeTotalCharge(UChar_t leadglassID, Short_t*);
  void ComputeTotalChargeLED(UChar_t leadglassID, Short_t*);
  void ComputeBunchLength(Short_t*);

  // Flag to signal if LeadGlass channel was found in this event
  Bool_t fLeadGlassFound;

  UChar_t leadglassID;

  // Define parameters for pedestal and total charge evaluation
  UInt_t fPedestalSamples;    // Number of samples to use for pedestals
  UInt_t fSignalSamplesStart; // Index of first sample of signal (included)
  UInt_t fSignalSamplesEnd;   // Index of last sample of signal (excluded)

  UInt_t fLEDPedestalSamplesBefore;    // Index of first sample to use for pedestals before signal
  UInt_t fLEDPedestalSamplesAfter;    // Index of first sample to use for pedestals after signal
  UInt_t fLEDSamplesStart; // Index of first sample of LED signal (included)
  UInt_t fLEDSamplesEnd;   // Index of last sample of LED signal (excluded)

  Double_t fPed, fPedRMS;

  // Results of pedestal and total charge evaluation
  Double_t fLGPedestal[N_LEADGLASS]; // Pedestal level from the first fPedestalSamples samples
  Double_t fLGPedRMS[N_LEADGLASS];   // Pedestal RMS
  Double_t fLGPedestal_v2[N_LEADGLASS]; // Pedestal level 
  Double_t fLGPedRMS_v2[N_LEADGLASS];   // Pedestal RMS
  Int_t fLGStartIndexCell[N_LEADGLASS]; // Index of signal start
  Double_t fLGCharge[N_LEADGLASS];   // Total charge between fSignalSamplesStart and fSignalSamplesEnd
  Double_t fLGCharge_v2[N_LEADGLASS];   // Total charge between fSignalSamplesStart and fSignalSamplesEnd

  // Calibration parameter to convert Total Charge to Total Energy
  Double_t fChargeToEnergy;

  // Total energy released in LeadGlass
  Double_t fLGEnergy[N_LEADGLASS];

  // Parameter to convert Total Charge to NPoTs
  Double_t fChargeToNPoTs;

  // Number of Positrons on Target computed from total charge
  Double_t fLGNPoTs;

  // Threshold to use to evaluate bunch length
  Double_t fBunchLengthThreshold;

  // Bunch length and quality
  Double_t fBunchLength;
  Double_t fBunchBBQ;

  // Histograms
  TH1D* fHLGPedestal;
  TH1D* fHLGPedRMS;
  TH1D* fHLGTotCharge;
  TH1D* fHLGNPoTs;
  TH1D* fHLGBunchLength;
  TH1D* fHLGBunchBBQ;
  //TH1D* fHLGBunchBBQWF;
  //TH1D* fHLGBunchBBQWF2;

};
#endif
