#ifndef _RecoBase_detRODeviceMAPMTH9500_hh_
#define _RecoBase_detRODeviceMAPMTH9500_hh_ 1
#include"detVRODevice.hh"
namespace det{
  class RODeviceMAPMTH9500:public VRODevice{
    public:
      ~RODeviceMAPMTH9500();
      RODeviceMAPMTH9500(const std::string&n);
      private:
      const unsigned int fkNChannels=256;
  };
}
#endif
