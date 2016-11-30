#ifndef _RecoBase_detVDetectorLayer_hh
#define _RecoBase_detVDetectorLayer_hh 1
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
#include"detVDetector.hh"

namespace det{
  class VDetectorLayer : public VPadmeElement,public std::enable_shared_from_this<VDetectorLayer>{
    public:
      virtual ~VDetectorLayer(){}
      VDetectorLayer(std::shared_ptr<VDetectorLayer> parent):fParentLayer(parent){}
      std::shared_ptr<VDetectorLayer> GetThisLayerPtr(){ return shared_from_this(); }
      const VDetectorLayer& GetParentLayer(){return *fParentLayer;}
      virtual VSubSystem& GetParentSubSystem(){return *fParentSubSystem;}
      const VDetectorLayer& GetLayer(unsigned i){
#warning throw
        return *fLayers.at(i); }
      void AddDetector(unsigned i,VDetector* d){
#warning if exists throw
      }
      virtual std::shared_ptr<VDetectorLayer> GetLayerPtr(unsigned i){
#warning try cast
        return fLayers.at(i);
      }
      virtual VDetectorLayer& EmplaceLayer(unsigned)=0;
      //      VDetectorLayer& EmplaceLayer(unsigned i){
      //#warning throw if exists
      //        fLayers.emplace(i,std::make_shared<VDetectorLayer>(shared_from_this()));
      //      }


    protected:
      std::shared_ptr<VDetectorLayer> fParentLayer;
      std::shared_ptr<VSubSystem> fParentSubSystem;
      std::map<unsigned,std::shared_ptr<VDetectorLayer>> fLayers;
      std::map<unsigned,std::shared_ptr<VDetectorLayer>> fDetectors;
  };



  class EVetoLayer     : public VDetectorLayer{
    public:
      EVetoLayer(std::shared_ptr<VDetectorLayer> parent):VDetectorLayer(parent){}
#warning throw if exists
      virtual EVetoLayer& EmplaceLayer(unsigned i){fLayers.emplace(i,std::make_shared<EVetoLayer>(*this)); return *(std::dynamic_pointer_cast<EVetoLayer,VDetectorLayer>(fLayers.at(i))); }
      const EVetoLayer& GetLayer(unsigned i){
        return *std::dynamic_pointer_cast<EVetoLayer,VDetectorLayer>(fLayers.at(i));
      }
      //      virtual std::shared_ptr<EVetoLayer> GetLayerPtr(unsigned i){
      //#warning try cast
      //        return std::dynamic_pointer_cast<EVetoLayer,VDetectorLayer>(fLayers.at(i));
      //      }
  };


  class PVetoLayer     : public VDetectorLayer{
    public:
      PVetoLayer(std::shared_ptr<PVetoLayer> parent): VDetectorLayer(parent){}
      virtual PVetoLayer&      EmplaceLayer(unsigned i){fLayers.emplace(i,std::make_shared<PVetoLayer>                (*this));return *(std::dynamic_pointer_cast<PVetoLayer,VDetectorLayer>(fLayers.at(i))); }
  };

  class HEPVetoLayer   : public VDetectorLayer{
    public:
      HEPVetoLayer(std::shared_ptr<HEPVetoLayer> parent): VDetectorLayer(parent){}
      virtual HEPVetoLayer    &EmplaceLayer(unsigned i){fLayers.emplace(i,std::make_shared<HEPVetoLayer>              (*this)); return *(std::dynamic_pointer_cast<HEPVetoLayer,VDetectorLayer>(fLayers.at(i))); }
  };

  class SACLayer       : public VDetectorLayer{
    public:
      SACLayer(std::shared_ptr<SACLayer> parent): VDetectorLayer(parent){}
      virtual SACLayer        &EmplaceLayer(unsigned i){fLayers.emplace(i,std::make_shared<SACLayer>                  (*this)); return *(std::dynamic_pointer_cast<SACLayer,VDetectorLayer>(fLayers.at(i))); }
  };

  class ECALLayer      : public VDetectorLayer{
    public:
      ECALLayer(std::shared_ptr<ECALLayer> parent): VDetectorLayer(parent){}
      virtual ECALLayer   &EmplaceLayer(unsigned i){fLayers.emplace(i,std::make_shared<ECALLayer>                     (*this)); return *(std::dynamic_pointer_cast<ECALLayer,VDetectorLayer>(fLayers.at(i))); }
  };

  class TargetLayer    : public VDetectorLayer{
    public:
      TargetLayer(std::shared_ptr<TargetLayer> parent): VDetectorLayer(parent){}
      virtual TargetLayer &EmplaceLayer(unsigned i){fLayers.emplace(i,std::make_shared<TargetLayer>                   (*this)); return *(std::dynamic_pointer_cast<TargetLayer,VDetectorLayer>(fLayers.at(i))); }
  };
}
#endif
