#ifndef DIGITIZER_CHANNEL_RECO_H
#define DIGITIZER_CHANNEL_RECO_H

#include "ChannelVReco.hh"
#include "utlConfigParser.hh"

class DigitizerChannelReco : public ChannelVReco {
public:
  DigitizerChannelReco(){;};
  ~DigitizerChannelReco(){;};

  virtual void SetDigis(UShort_t n,Short_t* arr){fNSamples = n;fSamples = arr; };
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray);
  virtual void Init(PadmeVRecoConfig *cfg);

  void ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray);
  void ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray);
  void PrintConfig();

  Short_t CalcMaximum();
  Double_t CalcPedestal();
  Double_t CalcCharge(UShort_t);
  Double_t CalcTime(UShort_t);

  Double_t ZSupHit(Float_t thr,UShort_t NAvg);  //M. Raggi 30/10/2018

  Short_t GetMaximum(){return fMax;};
  Double_t GetPedestal(){return fPed;};

  void SetAbsSignals();
  
private:
  //What do we operate
  UShort_t fNSamples;
  Short_t *fSamples;
  Short_t fMax; //Short_t = Signed Short integer 2 bytes (short)
  Short_t fIMax;
  Double_t fPed;
  Double_t fCharge;
  Double_t fTime;
  UShort_t fNPedSamples;

  //Beth 14/5/19
  Double_t fWaveform[1024];

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

  Double_t fChargeCut; //added by Beth 19/4/19

  Bool_t fMultihit;
  Bool_t fUseAbsSignals;
  

};
#endif
