#ifndef _RecoBase_detVSubSystem_hh
#define _RecoBase_detVSubSystem_hh 1
#include<stdio.h>
#include<memory>
#include<iostream>
#include<vector>
#include<map>
#include<stdarg.h>
#include<typeinfo>   // operator typeid
#include<deque>

#include"det.hh"
#include"fwk.hh"
#include"detVPadmeElement.hh"

#include"detVDetectorLayer.hh"
#include"detVDetector.hh"



namespace det{
  class EVetoLayer;


  class VSubSystem: public VPadmeElement{
    public:
      VSubSystem(const std::string&n):VPadmeElement(n){
      }
      virtual VDetectorLayer& GetTopLayer(){return *fTopLayer;}
      virtual ~VSubSystem(){ }
    protected:
      VSubSystem(){}
    protected:
      std::deque<std::shared_ptr<VDetector>> fDetectors;
      det::eSubSystem fSubSystem;
      VDetectorLayer* fTopLayer;
  };



  class DetectorVChargedVeto {};
  class ChargedVetoVInMagnet : public DetectorVChargedVeto{};
  class ChargedVetoVOutMagnet: public DetectorVChargedVeto{};

 
 
 
 
 
 
  class EVetoSubSystem  :public VSubSystem,public ChargedVetoVInMagnet { public: ~EVetoSubSystem  (){delete fTopLayer;}EVetoSubSystem  (const std::string&n):VSubSystem(n){fSubSystem=eSubSystem::eEVeto  ;fTopLayer=new EVetoLayer  (nullptr);} };
  class PVetoSubSystem  :public VSubSystem,public ChargedVetoVInMagnet { public: ~PVetoSubSystem  (){delete fTopLayer;}PVetoSubSystem  (const std::string&n):VSubSystem(n){fSubSystem=eSubSystem::ePVeto  ;fTopLayer=new PVetoLayer  (nullptr);} };
  class HEPVetoSubSystem:public VSubSystem,public ChargedVetoVOutMagnet{ public: ~HEPVetoSubSystem(){delete fTopLayer;}HEPVetoSubSystem(const std::string&n):VSubSystem(n){fSubSystem=eSubSystem::eHEPVeto;fTopLayer=new HEPVetoLayer(nullptr);} };
  class SACSubSystem    :public VSubSystem                             { public: ~SACSubSystem    (){delete fTopLayer;}SACSubSystem    (const std::string&n):VSubSystem(n){fSubSystem=eSubSystem::eSAC    ;fTopLayer=new SACLayer    (nullptr);} };
  class ECALSubSystem   :public VSubSystem                             { public: ~ECALSubSystem   (){delete fTopLayer;}ECALSubSystem   (const std::string&n):VSubSystem(n){fSubSystem=eSubSystem::eECAL   ;fTopLayer=new ECALLayer   (nullptr);} };
  class TargetSubSystem :public VSubSystem                             { public: ~TargetSubSystem (){delete fTopLayer;}TargetSubSystem (const std::string&n):VSubSystem(n){fSubSystem=eSubSystem::eTarget ;fTopLayer=new TargetLayer (nullptr);} };


}

#endif
