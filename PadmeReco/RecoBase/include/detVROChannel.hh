#ifndef _RecoBase_detVROChannel_HH_
#define _RecoBase_detVROChannel_HH_ 1
#include"det.hh"
#include"fwk.hh"
#include"detVPadmeElement.hh"
//#include"detVRODevice.hh"
namespace det{
  class VRODevice;
  class VROChannel: public VPadmeElement{
    public:
      VROChannel(const std::string&n,VRODevice* d):fParentRODevice(d),VPadmeElement(n){}
      VROChannel(VRODevice* d):fParentRODevice(d){
      }
      virtual VRODevice& GetParentRODevice() {return *fParentRODevice;}
      void SetROChannelNumber(unsigned int i){fROChannelNumber=i;}
      unsigned int GetROChannelNumber()const {return fROChannelNumber;}
    protected:
      VRODevice* fParentRODevice;
      eRODeviceTypes fROChannelType;
      unsigned int fROChannelNumber;
  };

  class HamamatsuH9500;

  class PhotonisChannel:public VROChannel{
  };

}
#endif
