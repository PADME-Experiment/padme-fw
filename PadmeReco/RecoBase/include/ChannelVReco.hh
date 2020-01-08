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
  
  virtual void SetDigis(UShort_t n,Short_t* arr) = 0;  
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray)=0;
  virtual void Init(PadmeVRecoConfig *cfg) = 0;
  virtual void Init(GlobalRecoConfigOptions *gOptions, PadmeVRecoConfig *cfg) = 0;

  Int_t fChID;
  Int_t fTrigMask;
  Int_t GetChID(){return fChID;};
  void  SetChID(Int_t ChID){fChID=ChID;};
  Int_t GetTrigMask(){return fTrigMask;};
  void  SetTrigMask(Int_t TrigMask){fTrigMask=TrigMask;};

};
#endif
