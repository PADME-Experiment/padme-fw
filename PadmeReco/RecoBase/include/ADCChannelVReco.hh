#ifndef ADCChannelVReco_H
#define ADCChannelVReco_H

#include "PadmeVRecoConfig.hh"
#include "VPMTChannel.hh"
#include <vector>
#include <string>
#include "TRecoVHit.hh"
#include "ChannelVReco.hh"

class ADCChannelVReco : public VPMTChannel, public ChannelVReco {

public:
  ADCChannelVReco();
  ADCChannelVReco(PadmeVRecoConfig *cfg);
  ~ADCChannelVReco(){;};
  virtual void Init(PadmeVRecoConfig *cfg);
  virtual void Reconstruct();
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray);
  virtual void ReconstructSinglePeak();
  virtual void ReconstructMultiPeak(){;};
  virtual void SetDigis( UShort_t n,Short_t* arr  ){ SetSamples(n,arr);};
  Double_t GetCharge(){return fCharge;};  
  void SetCharge(Double_t q) { fCharge = q;};
  unsigned int GetChannelID(int bID, int chID){ return 32*bID + chID; }
  Bool_t CheckSingleHit();

  Double_t GetMaxAmplitude(){return fMax;}
  void CalcMaxAmplitude(){};
  


private:
  //Reco parameters
  Bool_t fUseMultipeak;
  
  Double_t fCharge;
  std::vector<TRecoVHit *> fHits;
  
  Double_t fThreshold;
  Double_t fMax;
  Double_t fMin;
  
  PadmeVRecoConfig *fConfig;
  //Reconstruction configuration variables
  


};


#endif
