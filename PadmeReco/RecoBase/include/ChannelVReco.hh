#ifndef CHANNEL_VRECO_H
#define CHANNEL_VRECO_H

#include "TRecoVHit.hh"
#include <vector>
#include "PadmeVRecoConfig.hh"
#include "GlobalRecoConfigOptions.hh"

class ChannelVReco {

public:
  ChannelVReco(){;};
  virtual  ~ChannelVReco(){;};
  
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray)=0;
  virtual void Init(PadmeVRecoConfig *cfg) = 0;
  virtual void Init(GlobalRecoConfigOptions *gOptions, PadmeVRecoConfig *cfg);
  void ReconstructMultiHitSignalDeconvolution(std::vector<TRecoVHit *> &hitArray);
  void SetDigis(UShort_t n,Short_t* arr){fNSamples = n;fSamples = arr; };


  Int_t GetChID(){return fChID;};
  void  SetChID(Int_t ChID){fChID=ChID;};
  Int_t GetTrigMask(){return fTrigMask;};
  void  SetTrigMask(Int_t TrigMask){fTrigMask=TrigMask;};

  
protected:
  Int_t fChID;
  Int_t fTrigMask;

  UShort_t fNSamples;
  Short_t *fSamples;

  //Configuration variables
  Int_t fPreSamples;
  Int_t fPostSamples;
  Int_t fPedMaxNSamples;
  Int_t fSignalWidth;
  Int_t fPedOffset;
  Int_t fMinAmplitude;

  Double_t fTimeBin;
  Double_t fVoltageBin;
  Double_t fImpedance;
  Double_t fmVtoMeV;

  Double_t fAmpThresholdLow;
  Double_t fAmpThresholdHigh;

  Bool_t fUseAbsSignals;

  int fMultihit;
  std::vector<double> fDeconvMultiplierRe;
  std::vector<double> fDeconvMultiplierIm;
  float fDeconvThreshold_mV;
  float fDeconvIntegralAmplitudeRatio;

  
};
#endif
