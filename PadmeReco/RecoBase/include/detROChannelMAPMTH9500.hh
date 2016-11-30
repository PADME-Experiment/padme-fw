#ifndef _RecoBase_detROChannelMAPMTH9500_hh_
#define _RecoBase_detROChannelMAPMTH9500_hh_ 1
#include"detVROChannel.hh"
namespace det{
  class ROChannelMAPMTH9500:public VROChannel{
    public:
      ROChannelMAPMTH9500(const std::string&n,VRODevice* d):VROChannel(n,d){fROChannelType=eRODeviceTypes::eMAPMTH9500;}
      //HamamatsuH9500& GetParentRODevice(){return *(std::dynamic_pointer_cast<HamamatsuH9500,VRODevice>(fParentRODevice));}
  };
}
#endif
