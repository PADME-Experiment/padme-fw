#ifndef _RECOBASE_DETVRODEVICE_HH_
#define _RECOBASE_DETVRODEVICE_HH_ 1
#include<stdio.h>
#include<memory>
#include<iostream>
#include<vector>
#include<map>
#include<deque>
#include<stdarg.h>
#include<typeinfo>   // operator typeid

#include"det.hh"
#include"fwk.hh"
#include"detVPadmeElement.hh"
#//include"detVDigitizerChannel.hh"
#//include"detVDigitizer.hh"
//#include"detVSubSystem.hh"
#include"detVDetector.hh"
//#include"detVROChannel.hh"

namespace det{
  class VROChannel;
  class VRODevice       :public VPadmeElement{
    public:
      VRODevice(){}
      VRODevice(const std::string&n):VPadmeElement(n){}
      virtual ~VRODevice       (){}
    protected:
      eRODeviceTypes fRODeviceType;
    protected:
      std::deque<VDigitizerChannel*> fDigitizerChannels;
      std::deque<VDetector*>  fDetectors;
      std::map<int,VROChannel *> fROChannels;
  };


  class RODevicePhotonis:public VRODevice{
    public:
      RODevicePhotonis(const std::string&n)
        : VRODevice(n)
      {fRODeviceType=eRODeviceTypes::ePhotonis;}
  };

}
#endif
