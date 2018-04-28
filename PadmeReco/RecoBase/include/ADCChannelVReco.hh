#ifndef ADCChannelVReco_H
#define ADCChannelVReco_H

#include "PadmeVRecoConfig.hh"
#include "VPMTChannel.hh"

class ADCChannelVReco : public VPMTChannel{

public:
  ADCChannelVReco();
  ADCChannelVReco(PadmeVRecoConfig *cfg);
  ~ADCChannelVReco(){;};
  void Init(PadmeVRecoConfig *cfg);
  virtual void Reconstruct();
  virtual void ReconstructSinglePeak(){;};

  virtual void ReconstructMultiPeak(){;};

  
private:
  //Reco parameters
  Bool_t fUseMultipeak;
  
  
};


#endif
