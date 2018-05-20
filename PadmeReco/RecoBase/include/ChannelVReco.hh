#ifndef CHANNEL_VRECO_H
#define CHANNEL_VRECO_H

#include "TRecoVHit.hh"
#include <vector>
#include "PadmeVRecoConfig.hh"

class ChannelVReco {

public:
  ChannelVReco(){;};
  ~ChannelVReco(){;};
  
  virtual void SetDigis(UShort_t n,Short_t* arr) = 0;  
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray)=0;
  virtual void Init(PadmeVRecoConfig *cfg) = 0;

};
#endif
