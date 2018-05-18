#ifndef ADCChannelVReco_H
#define ADCChannelVReco_H

#include "PadmeVRecoConfig.hh"
#include "VPMTChannel.hh"
#include <vector>
#include <string>
#include "TRecoVHit.hh"

class ADCChannelVReco : public VPMTChannel {

public:
  ADCChannelVReco();
  ADCChannelVReco(PadmeVRecoConfig *cfg);
  ~ADCChannelVReco(){;};
  void Init(PadmeVRecoConfig *cfg);
  virtual void Reconstruct();
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray);
  virtual void ReconstructSinglePeak();

  virtual void ReconstructMultiPeak(){;};
 
  Double_t GetCharge(){return fCharge;};
  
  void SetCharge(Double_t q) { fCharge = q;};

  unsigned int GetChannelID(int bID, int chID){ return 32*bID + chID; }
  
private:
  //Reco parameters
  Bool_t fUseMultipeak;
  
  Double_t fCharge;
  std::vector<TRecoVHit *> fHits;
  
  Double_t fThreshold;
  
  

};


#endif
