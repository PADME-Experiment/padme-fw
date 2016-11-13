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
  class DetectorVLayer : public VPadmeElement,public std::enable_shared_from_this<DetectorVLayer>{
    public:
      virtual ~DetectorVLayer(){}
      DetectorVLayer(std::shared_ptr<DetectorVLayer> parent):fParentLayer(parent){}
      std::shared_ptr<DetectorVLayer> GetThisLayerPtr(){ return shared_from_this(); }
      const DetectorVLayer& GetParent(){return *fParentLayer;}
      const DetectorVLayer& GetLayer(unsigned i){
#warning throw
        return *fLayers.at(i); }
      void AddDetector(unsigned i,std::shared_ptr<VDetector> d){
#warning if exists throw
      }
      virtual std::shared_ptr<DetectorVLayer> GetLayerPtr(unsigned i){
#warning try cast
        return fLayers.at(i);
      }
      virtual DetectorVLayer& EmplaceLayer(unsigned)=0;
      //      DetectorVLayer& EmplaceLayer(unsigned i){
      //#warning throw if exists
      //        fLayers.emplace(i,std::make_shared<DetectorVLayer>(shared_from_this()));
      //      }


    protected:
      std::shared_ptr<DetectorVLayer> fParentLayer;
      std::map<unsigned,std::shared_ptr<DetectorVLayer>> fLayers;
      std::map<unsigned,std::shared_ptr<DetectorVLayer>> fDetectors;
  };

  class VSubSystem: public VPadmeElement{
      public:
      virtual DetectorVLayer& GetTopLayer()=0;
    protected:
      VSubSystem(){}
      virtual ~VSubSystem(){}
    protected:
      std::deque<std::shared_ptr<VDetector>> fDetectors;
      det::eSubSystem fSubSystem;
  };
  class VDetectorLayer:public VPadmeElement{
  };

  class EVetoLayer     : public DetectorVLayer{
    public:
      EVetoLayer(std::shared_ptr<DetectorVLayer> parent):DetectorVLayer(parent){}
#warning throw if exists 
      virtual EVetoLayer& EmplaceLayer(unsigned i){fLayers.emplace(i,std::make_shared<EVetoLayer>(*this)); return *(std::dynamic_pointer_cast<EVetoLayer,DetectorVLayer>(fLayers.at(i))); }
      const EVetoLayer& GetLayer(unsigned i){
        return *std::dynamic_pointer_cast<EVetoLayer,DetectorVLayer>(fLayers.at(i));
      }
//      virtual std::shared_ptr<EVetoLayer> GetLayerPtr(unsigned i){
//#warning try cast
//        return std::dynamic_pointer_cast<EVetoLayer,DetectorVLayer>(fLayers.at(i));
//      }
  };






  class PVetoLayer     : public DetectorVLayer{
    public:
      PVetoLayer(std::shared_ptr<PVetoLayer> parent): DetectorVLayer(parent){}
      virtual PVetoLayer&      EmplaceLayer(unsigned i){fLayers.emplace(i,std::make_shared<PVetoLayer>                (*this));return *(std::dynamic_pointer_cast<PVetoLayer,DetectorVLayer>(fLayers.at(i))); }
  };
  class HEPVetoLayer   : public DetectorVLayer{
    public:
      HEPVetoLayer(std::shared_ptr<HEPVetoLayer> parent): DetectorVLayer(parent){}
    virtual HEPVetoLayer    &EmplaceLayer(unsigned i){fLayers.emplace(i,std::make_shared<HEPVetoLayer>              (*this)); return *(std::dynamic_pointer_cast<HEPVetoLayer,DetectorVLayer>(fLayers.at(i))); }
  };
  class SACLayer       : public DetectorVLayer{
    public:
      SACLayer(std::shared_ptr<SACLayer> parent): DetectorVLayer(parent){}
    virtual SACLayer        &EmplaceLayer(unsigned i){fLayers.emplace(i,std::make_shared<SACLayer>                  (*this)); return *(std::dynamic_pointer_cast<SACLayer,DetectorVLayer>(fLayers.at(i))); }
  };
  class ECALLayer      : public DetectorVLayer{
    public:
      ECALLayer(std::shared_ptr<ECALLayer> parent): DetectorVLayer(parent){}
    virtual ECALLayer   &EmplaceLayer(unsigned i){fLayers.emplace(i,std::make_shared<ECALLayer>                     (*this)); return *(std::dynamic_pointer_cast<ECALLayer,DetectorVLayer>(fLayers.at(i))); }
  };
  class TargetLayer    : public DetectorVLayer{
    public:
      TargetLayer(std::shared_ptr<TargetLayer> parent): DetectorVLayer(parent){}
    virtual TargetLayer &EmplaceLayer(unsigned i){fLayers.emplace(i,std::make_shared<TargetLayer>                   (*this)); return *(std::dynamic_pointer_cast<TargetLayer,DetectorVLayer>(fLayers.at(i))); }
  };


  class DetectorVChargedVeto {};
  class ChargedVetoVInMagnet : public DetectorVChargedVeto{};
  class ChargedVetoVOutMagnet: public DetectorVChargedVeto{};

  class EVetoSubSystem  :public VSubSystem,public ChargedVetoVInMagnet{
    public:
      EVetoSubSystem  ():fTopLayer(nullptr){fSubSystem=eSubSystem::eEVeto  ;};
      EVetoLayer& GetTopLayer(){return fTopLayer;}
      EVetoLayer fTopLayer;

  };
  class PVetoSubSystem  :public VSubSystem,public ChargedVetoVInMagnet {
    public:
      PVetoSubSystem  ():fTopLayer(nullptr){fSubSystem=eSubSystem::ePVeto  ;};
      virtual PVetoLayer& GetTopLayer(){return fTopLayer;}
      PVetoLayer fTopLayer;
  };
  class HEPVetoSubSystem:public VSubSystem,public ChargedVetoVOutMagnet{
    public:
      HEPVetoSubSystem():fTopLayer(nullptr){fSubSystem=eSubSystem::eHEPVeto;};
      virtual HEPVetoLayer& GetTopLayer(){return fTopLayer;}
      HEPVetoLayer fTopLayer;
  };
  class SACSubSystem    :public VSubSystem{ public:
    public:
      SACSubSystem    ():fTopLayer(nullptr){fSubSystem=eSubSystem::eSAC    ;};
      virtual SACLayer& GetTopLayer(){return fTopLayer;}
      SACLayer fTopLayer;
  };
  class ECALSubSystem   :public VSubSystem{ public:
    public:
      ECALSubSystem   ():fTopLayer(nullptr){fSubSystem=eSubSystem::eECAL   ;};
      virtual ECALLayer& GetTopLayer(){return fTopLayer;}
      ECALLayer fTopLayer;
  };
  class TargetSubSystem :public VSubSystem{ public:
    public:
      TargetSubSystem ():fTopLayer(nullptr){fSubSystem=eSubSystem::eTarget ;};
      virtual TargetLayer& GetTopLayer(){return fTopLayer;}
      TargetLayer fTopLayer;
  };


}








namespace det{
}


#endif
