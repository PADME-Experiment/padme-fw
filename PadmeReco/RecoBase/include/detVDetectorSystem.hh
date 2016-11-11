#ifndef _RECOBASE_DETVSubSystem
#define _RECOBASE_DETVSubSystem 1
#include<stdio.h>
#include<memory>
#include<iostream>
#include<vector>
#include<map>
#include<stdarg.h>
#include <typeinfo>   // operator typeid
#include<deque>

#include"det.hh"
#include"fwk.hh"
#include"detVPadmeElement.hh"
#include"detVDigitizerChannel.hh"
#include"detVDigitizer.hh"
#include"detVRODevice.hh"
#include"detVDetector.hh"

namespace det{
  class VSubSystem:public VPadmeElement{
    protected:
      VSubSystem(){}
      virtual ~VSubSystem(){}
    protected:
      std::deque<std::shared_ptr<VDetector>> fDetectors;
      //std::deque<std::shared_ptr<VVetoLayer>> fLayers;
      det::eSubSystem fSubSystem;
  };
}


namespace det{
  class ChargedVetoVLayer :public VPadmeElement{};
  class EVetoLayer        :public ChargedVetoVLayer{};
  class PVetoLayer        :public ChargedVetoVLayer{};
  class HEPVetoLayer      :public ChargedVetoVLayer{};
  class DetectorVChargedVeto {};
  class ChargedVetoVInMagnet : public DetectorVChargedVeto{};
  class ChargedVetoVOutMagnet: public DetectorVChargedVeto{};

  class EVetoSubSystem  :public VSubSystem,public ChargedVetoVInMagnet{
    public:
      EVetoSubSystem  (){fSubSystem=eSubSystem::eEVeto  ;};

    private:
      std::map<unsigned, std::shared_ptr<EVetoLayer>> fLayers;
  };
  class PVetoSubSystem  :public VSubSystem,public ChargedVetoVInMagnet {
    public:
      PVetoSubSystem  (){fSubSystem=eSubSystem::ePVeto  ;};
    private:
      std::map<unsigned, std::shared_ptr<PVetoLayer>> fLayers;
  };
  class HEPVetoSubSystem:public VSubSystem,public ChargedVetoVOutMagnet{
    public:
      HEPVetoSubSystem(){fSubSystem=eSubSystem::eHEPVeto;};
    private:
      std::map<unsigned, std::shared_ptr<HEPVetoLayer>> fLayers;
  };
  class SACSubSystem    :public VSubSystem{ public:
    SACSubSystem    (){fSubSystem=eSubSystem::eSAC    ;};
  };
  class ECALSubSystem   :public VSubSystem{ public:
    ECALSubSystem   (){fSubSystem=eSubSystem::eECAL   ;};
  };
  class TargetSubSystem :public VSubSystem{ public:
    TargetSubSystem (){fSubSystem=eSubSystem::eTarget ;};
  };


}








namespace det{
}


#endif
