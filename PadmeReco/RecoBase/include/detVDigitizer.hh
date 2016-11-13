#ifndef _RECOBASE_DETVDIGITIZER_HH_
#define _RECOBASE_DETVDIGITIZER_HH_ 1
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
#include"detVDetectorSystem.hh"
#include"detVRODevice.hh"
#include"detVDetector.hh"
namespace det{
  class VDigitizer      :public VPadmeElement{
    protected:
      ~VDigitizer      (){}
    protected:
      std::map<unsigned, std::shared_ptr<VDigitizerChannel>> fChannels;
      std::map<unsigned, std::shared_ptr<VRODevice>> fRODevices;
      det::eDigitizerTypes fDigitizerType;
  };
  class CAENv1742:public VDigitizer{
    public:
      CAENv1742(){fDigitizerType=eDigitizerTypes::eCAENv1742;}
  };

  //  class DigitizerVCAEN:public VDigitizer {public: typedef unsigned nChannels_t;protected: int fSignalChannalMask, fTriggerChannelMask; nChannels_t fNSignalChannels, fNTriggerChannels;};
  //  class CAENv1742     : public DigitizerVCAEN {public:CAENv1742(){fNTriggerChannels=4;fNSignalChannels=32;}};
  //  class CAENv1751     : public DigitizerVCAEN {public:CAENv1751(){fNTriggerChannels=0;fNSignalChannels=8;}};
  //  class TEL62         : public VDigitizer     {public:TEL62    (){}};

}
#endif
