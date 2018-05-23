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
  void PrintConfig();

  Short_t CalcMaximum();
  Double_t CalcPedestal();
  

private:
  //What do we operate
  UShort_t fNSamples;
  Short_t *fSamples;
  Short_t fMax;
  Double_t fPed;
  Double_t fCharge;
  Double_t fTime;
  

  //Configuration variables
  Int_t fSignalWidth;
  Int_t fPreSamples;
  Int_t fPostSamples;
  Int_t fPedOffset; 
  Int_t fPedMaxNSamples;

  Double_t fSignalThreshold;
  Double_t fSignalPercentage;
  
  

};
#endif
