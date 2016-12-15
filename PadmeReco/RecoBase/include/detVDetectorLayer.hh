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
  class VSubSystem;
  class VDetectorLayer : public VPadmeElement{
    public:
      virtual ~VDetectorLayer();
      VDetectorLayer(const std::string&n,VDetectorLayer* parent):VPadmeElement(n),fParentLayer(parent){}
      VDetectorLayer* GetThisLayerPtr(){ return this; }
      const VDetectorLayer& GetParentLayer(){return *fParentLayer;}
      virtual VSubSystem* GetParentSubSystem(){return fParentSubSystem;}
      void SetParentSubSystem(VSubSystem* ss){fParentSubSystem=ss;}
      //const VDetectorLayer& GetLayer(unsigned i){
      //#warning throw
      //  return *(fLayers[i]); }
      void AddDetector(unsigned i,VDetector* d){
#warning if exists throw
      }
      //      virtual VDetectorLayer* GetLayerPtr(unsigned i){
      //#warning try cast
      //        return fLayers.at(i);
      //      }
      virtual VDetectorLayer& EmplaceLayer(const std::string&n)=0;
      virtual void EmplaceDetector(const std::string&n,eDetectorTypes t,int ind)=0;
      //      VDetectorLayer& EmplaceLayer(unsigned i){
      //#warning throw if exists
      //        fLayers.emplace(i,std::make_shared<VDetectorLayer>(shared_from_this()));
      //      }


    protected:
      VDetectorLayer* fParentLayer;
      VSubSystem    * fParentSubSystem;
      std::vector<VDetectorLayer*> fLayers;
      std::map<unsigned,VDetector*> fDetectors;
  };



  class EVetoLayer     : public VDetectorLayer{
    public:
      EVetoLayer(const std::string&n,VDetectorLayer* parent):VDetectorLayer(n,parent){}
#warning throw if exists
      EVetoLayer& EmplaceLayer(const std::string&n);
      void EmplaceDetector(const std::string&n,eDetectorTypes t,int ind);

      //      const EVetoLayer& GetLayer(unsigned i){
      //#warning try throw
      //        return *(dynamic_cast<EVetoLayer*>(fLayers.at(i)));
      //      }
  };

  class PVetoLayer     : public VDetectorLayer{
    public:
      PVetoLayer(const std::string&n,PVetoLayer* parent): VDetectorLayer(n,parent){}
      PVetoLayer&      EmplaceLayer(const std::string&n);
      void EmplaceDetector(const std::string&n,eDetectorTypes t,int ind);
  };

  class HEPVetoLayer   : public VDetectorLayer{
    public:
      HEPVetoLayer(const std::string&n,HEPVetoLayer* parent): VDetectorLayer(n,parent){}
      HEPVetoLayer    &EmplaceLayer(const std::string&n);
      void EmplaceDetector(const std::string&n,eDetectorTypes t,int ind);
  };

  class SACLayer       : public VDetectorLayer{
    public:
      SACLayer(const std::string&n,SACLayer* parent): VDetectorLayer(n,parent){}
      SACLayer        &EmplaceLayer(const std::string&n);
      void EmplaceDetector(const std::string&n,eDetectorTypes t,int ind);
  };

  class ECALLayer      : public VDetectorLayer{
    public:
      ECALLayer(const std::string&n,ECALLayer* parent): VDetectorLayer(n,parent){}
      ECALLayer   &EmplaceLayer(const std::string&n);
      void EmplaceDetector(const std::string&n,eDetectorTypes t,int ind);
  };

  class TargetLayer    : public VDetectorLayer{
    public:
      TargetLayer(const std::string&n,TargetLayer* parent): VDetectorLayer(n,parent){}
      TargetLayer &EmplaceLayer(const std::string&n);
      void EmplaceDetector(const std::string&n,eDetectorTypes t,int ind);
  };
}
#endif
