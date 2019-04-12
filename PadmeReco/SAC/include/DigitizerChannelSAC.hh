#ifndef DIGITIZER_CHANNEL_RECO_H
#define DIGITIZER_CHANNEL_RECO_H

#include "ChannelVReco.hh"
#include "GlobalRecoConfigOptions.hh"
#include "utlConfigParser.hh"
#include "TObject.h"
#include "TVector3.h"
#include "TH1D.h"
#include "TFile.h"
#include "TList.h"

//ADC board 27, Ch0-25
typedef  GlobalRecoConfigOptions LocalRecoConfigOptions;

class DigitizerChannelSAC : public ChannelVReco {
public:
  DigitizerChannelSAC(){;};
  // ~DigitizerChannelSAC(){;};
  ~DigitizerChannelSAC();

  virtual void SetDigis(UShort_t n,Short_t* arr){fNSamples = n;fSamples = arr; };
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray);
  virtual void Init(PadmeVRecoConfig *cfg){return;}
  virtual void Init(GlobalRecoConfigOptions *gOptions, PadmeVRecoConfig *cfg);

  void ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray);
  void ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray);
  void PrintConfig();

  Short_t  CalcMaximum();
  Double_t CalcPedestal();
  Double_t CalcCharge(UShort_t);
  Double_t CalcTime(UShort_t,UShort_t);
  Double_t CalcChaTime(std::vector<TRecoVHit *> &hitArray,UShort_t);

  Short_t  GetMaximum(){return fMax;};
  Double_t GetPedestal(){return fPed;};

  Double_t CalcPosition(UShort_t);
  Double_t ReadPedFile();
  
  Int_t GetChID(){return fChID;};
  void  SetChID(Int_t ChID){fChID=ChID;};

  Int_t GetTrigMask(){return fTrigMask;};
  void  SetTrigMask(Int_t TrigMask){fTrigMask=TrigMask;};

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
  Double_t fCalibEnergy;

  Double_t fTime;
  UShort_t fNPedSamples;
  Int_t fChID;
  Int_t fTrigMask;

  Double_t fSigAvg[50];
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

  //mode variables
  GlobalRecoConfigOptions* fGlobalMode;
  LocalRecoConfigOptions*  fLocalMode;


//  // Robaccia da levare
//  TFile *fileOut;
  TH1D ** hPedCalo;
//  TH1D ** hPedMean;
//  TH1D ** hVMax;
//  TH1D ** hQCh;
//  TH1D ** hChTime;
//  TH1D *  hVPeak;
//  TH1D *  hMean;
//  TH1D *  hCharge;
//  TH1D *  hTime;
//
//
  TList* hListCal;  
  TH1D* histo;
  
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
