// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2022-10-25
//
// --------------------------------------------------------------
#ifndef LeadGlassReconstruction_H
#define LeadGlassReconstruction_H

#define LEADGLASS_BOARD 14
#define LEADGLASS_CHANNEL 31

class TString;
class TH1D;

class TRawEvent;

class HistoSvc;
class TrigTimeSvc;
class RunConditionSvc;
class RunConfigurationSvc;

class LeadGlassReconstruction
{

public:

  LeadGlassReconstruction(TString);
  ~LeadGlassReconstruction();

  void Init();
  void ProcessEvent(TRawEvent*);
  void EndProcessing();

  Bool_t writeHits() { return fWriteHits; }

  Bool_t LeadGlassFound() { return fLeadGlassFound; }

  Double_t GetPedestal()    { return fLGPedestal;  }
  Double_t GetPedestalRMS() { return fLGPedRMS;    }
  Double_t GetCharge()      { return fLGCharge;    }
  Double_t GetEnergy()      { return fLGEnergy;    }
  Double_t GetNPoTs()       { return fLGNPoTs;     }
  Double_t GetBunchLength() { return fBunchLength; }
  Double_t GetBunchBBQ()    { return fBunchBBQ;    }

  Int_t GetVerbose() { return fVerbose; }

private:

  HistoSvc* fHistoSvc;
  TrigTimeSvc* fTrigTimeSvc;
  RunConditionSvc* fRunConditionSvc;
  RunConfigurationSvc* fRunConfigurationSvc;

  PadmeVRecoConfig* fLeadGlassConfig;

  void AnalyzeEvent(TRawEvent*);
  void AnalyzeChannel(Short_t*);
  void ComputeTotalCharge(Short_t*);
  void ComputeBunchLength(Short_t*);

  // Flag to signal if LeadGlass channel was found in this event
  Bool_t fLeadGlassFound;

  // Define parameters for pedestal and total charge evaluation
  UInt_t fPedestalSamples;    // Number of samples to use for pedestals
  UInt_t fSignalSamplesStart; // Index of first sample of signal (included)
  UInt_t fSignalSamplesEnd;   // Index of last sample of signal (excluded)

  // Results of pedestal and total charge evaluation
  Double_t fLGPedestal; // Pedestal level from the first fPedestalSamples samples
  Double_t fLGPedRMS;   // Pedestal RMS
  Double_t fLGCharge;   // Total charge between fSignalSamplesStart and fSignalSamplesEnd

  // Energy of current run
  Double_t fRunEnergy;

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

  // Bunch length and quality
  Double_t fBunchLength;
  Double_t fBunchBBQ;

  Bool_t fWriteHits;

  // Histograms
  TH1D* fHLGPedestal;
  TH1D* fHLGPedRMS;
  TH1D* fHLGTotCharge;
  TH1D* fHLGNPoTs;
  TH1D* fHLGBunchLength;
  TH1D* fHLGBunchBBQ;
  //TH1D* fHLGBunchBBQWF;
  //TH1D* fHLGBunchBBQWF2;

  Int_t fVerbose;

};
#endif
