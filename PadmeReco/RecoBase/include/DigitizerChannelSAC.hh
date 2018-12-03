#ifndef DIGITIZER_CHANNEL_RECO_H
#define DIGITIZER_CHANNEL_RECO_H

#include "ChannelVReco.hh"
#include "utlConfigParser.hh"
#include "TObject.h"
#include "TVector3.h"
#include "TH1D.h"


//ADC board 27, Ch0-25

class DigitizerChannelSAC : public ChannelVReco {
public:
  DigitizerChannelSAC(){;};
  ~DigitizerChannelSAC(){;};

  virtual void SetDigis(UShort_t n,Short_t* arr){fNSamples = n;fSamples = arr; };
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray);
  virtual void Init(PadmeVRecoConfig *cfg);

  void ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray);
  void ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray);
  void PrintConfig();

  Short_t  CalcMaximum();
  Double_t CalcPedestal();
  Double_t CalcCharge(UShort_t);
  Double_t CalcTime(UShort_t,UShort_t);
  Double_t CalcChaTime(std::vector<TRecoVHit *> &hitArray,UShort_t,UShort_t);

  Short_t  GetMaximum(){return fMax;};
  Double_t GetPedestal(){return fPed;};

  Double_t CalcPosition(UShort_t);
  Double_t ReadPedFile();
  
  void SetAbsSignals();
  void SetAnalogOffSets();
private:
  //What do we operate
  UShort_t fNSamples;
  Short_t *fSamples;
  Short_t  fMax;
  Short_t  fIMax;
  Double_t fPed;
  Double_t fCharge;
  Double_t fEnergy;
  Double_t fTime;
  UShort_t fNPedSamples;

  TH1D *H1;

  //Configuration variables
  Int_t fSignalWidth;
  Int_t fPreSamples;
  Int_t fPostSamples;
  Int_t fPedOffset; 
  Int_t fPedMaxNSamples;
  
  Int_t fMinAmplitude;

  Double_t fSignalThreshold;
  Double_t fSignalPercentage;
  
  Double_t fTimeBin;
  Double_t fVoltageBin;
  Double_t fImpedance;

  Double_t fAmpThresholdLow;
  Double_t fAmpThresholdHigh;

  Bool_t fMultihit;
  Bool_t fUseAbsSignals;

  //SAC variables 
  Double_t fCh        [25];
  Double_t fCry       [25];//fCrystal mapping
  Double_t fMeasure   [25];//fCrystal position in mm
  Double_t fPedCh     [25];//Adc channel pedestals
  Double_t fQCh       [25];//Channel charge
  Double_t fCalibCh   [25];//Calibration Constant fStrips
  Double_t fEConvConst[25];//Calibration Constant fStrips
  Double_t fQtot        ;
  Double_t fXbeam       ;
  Double_t fYbeam       ;
};

#endif
