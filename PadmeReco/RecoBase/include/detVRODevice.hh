#ifndef _RECOBASE_DETVRODEVICE_HH_
#define _RECOBASE_DETVRODEVICE_HH_ 1
#include<stdio.h>
#include<memory>
#include<iostream>
#include<vector>
#include<map>
#include<stdarg.h>
#include <typeinfo>   // operator typeid

#include"det.hh"
#include"fwk.hh"
#include"detVPadmeElement.hh"
#include"detVDigitizerChannel.hh"
#include"detVDigitizer.hh"
#include"detVDetectorSystem.hh"
#include"detVDetector.hh"
namespace det{
  class VRODevice       :public VPadmeElement{
    protected:
      virtual ~VRODevice       (){}
      eRODeviceTypes fRODeviceType;
    protected:
      std::deque<std::shared_ptr<VDigitizerChannel>> fDigitizerChannels;
      std::deque<std::shared_ptr<VDetector>>  fDetectors;
  };
  class HamamatsuH9500:public VRODevice{
    public:
      HamamatsuH9500(){fRODeviceType=eRODeviceTypes::eHamamatsuH9500;}
  };
  class Photonis:public VRODevice{
    public:
      Photonis(){fRODeviceType=eRODeviceTypes::ePhotonis;}
  };
}
#endif
