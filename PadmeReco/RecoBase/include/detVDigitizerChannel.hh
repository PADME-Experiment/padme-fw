#ifndef _RecoBase_detVDigitizerChannel_hh_
#define _RecoBase_detVDigitizerChannel_hh_ 1
#include<stdio.h>
#include<memory>
#include<iostream>
#include<vector>
#include<map>
#include<stdarg.h>
#include<typeinfo>   // operator typeid

#include"det.hh"
#include"fwk.hh"
#include"detVPadmeElement.hh"
#include"detVDigitizer.hh"
#include"detVSubSystem.hh"
#include"detVRODevice.hh"
#include"detVDetector.hh"
namespace det{
  class VDigitizerChannel:public VPadmeElement{
    protected:
      virtual~VDigitizerChannel(){}
    protected:
      std::shared_ptr<VDigitizer> fParent;
      std::shared_ptr<VRODevice> fRODevice;

      det::eDigitizerTypes fDigitizerChannelType;
  };
  class CAENv1742Channel:public VDigitizerChannel{
    public:
      CAENv1742Channel(){fDigitizerChannelType=eDigitizerTypes::eCAENv1742;}
  };
}
#endif
