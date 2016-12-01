#ifndef _RecoBase_detUniverse_HH_
#define _RecoBase_detUniverse_HH_ 1

#include"det.hh"
#include"fwk.hh"
#include"utl.hh"
#include"detVDigitizer.hh"
#include"detVDigitizerChannel.hh"
#include"detVDetector.hh"
#include"detVSubSystem.hh"
#include"detVRODevice.hh"
#include"detRODeviceMAPMTH9500.hh"


namespace det{
  class Universe{
    private:
      Universe(){};
      ~Universe(){
        DestroySubSystems();
        DestoryRODevices ();
        DestroyDigitizers();
      };
    public:
      static Universe&GetInstance(){
        static Universe a=Universe();
        return a;
      }
    public:

      std::map<std::string,VDigitizer*>::iterator DigitizersBegin(){return fAllDigitizers.begin();}
      std::map<std::string,VDigitizer*>::iterator DigitizersEnd  (){return fAllDigitizers.end()  ;}
      std::map<std::string,VRODevice *>::iterator RODevicesBegin (){return fAllRODevices .begin();}
      std::map<std::string,VRODevice *>::iterator RODevicesEnd   (){return fAllRODevices .end()  ;}
      std::map<eSubSystem ,VSubSystem*>::iterator SubSystemsBegin(){return fAllSubSystems.begin();}
      std::map<eSubSystem ,VSubSystem*>::iterator SubSystemsEnd  (){return fAllSubSystems.end()  ;}

      std::map<std::string,VDigitizerChannel*>::iterator DigitizerChannelsBegin(){return fAllDigitizerChannels .begin();}
      std::map<std::string,VDigitizerChannel*>::iterator DigitizerChannelsEnd()  {return fAllDigitizerChannels .end();}
      std::map<std::string,VDetector *>::iterator DetectorsBegin (){return fAllDetectors .begin();}
      std::map<std::string,VDetector *>::iterator DetectorsEnd   (){return fAllDetectors .end()  ;}

    private:
      std::map<std::string,VDigitizer*>fAllDigitizers; //owned
      std::map<std::string,VRODevice *>fAllRODevices;  //owned
      std::map<eSubSystem ,VSubSystem*>fAllSubSystems; //owned


      std::map<std::string,VDetectorLayer*>fAllDetectorLayers; ///<owned by the parent layer. The top layer is owned by the subsystem
      std::map<std::string,VDetector *>fAllDetectors; ///< VDetector is owned by DetectorLayer.
      std::map<std::string,VDigitizerChannel*>fAllDigitizerChannels; ///< VDigitizerChannel is owned by VDigitizer.
    public:
      void RegisterLayer(VDetectorLayer*lay){
        if(fAllDetectorLayers.count(lay->GetName())){
          ERROR(lay->GetName()+" already exists");
#warning check
        }
        fAllDetectorLayers[lay->GetName()]=lay;
      }
      VDetectorLayer* GetLayer(const std::string& n){
        if(!fAllDetectorLayers.count(n)){
          ERROR(n+" does not exist");
#warning check
        }
        return fAllDetectorLayers.at(n);
      }

      void RegisterDetector(VDetector*det){
        if(fAllDetectors.count(det->GetName())){
          ERROR(det->GetName()+" already exists");
#warning check
        }
        fAllDetectors[det->GetName()]=det;
      }
      VDetector* GetDetector(const std::string& n){
        if(!fAllDetectors.count(n)){
          ERROR(n+" does not exist");
#warning check
        }
        return fAllDetectors.at(n);
      }



      EVetoSubSystem  & GetEVetoSubSystem   (){
        EVetoSubSystem*   a=dynamic_cast<EVetoSubSystem*>(fAllSubSystems.at(eSubSystem::eEVeto  ));
        if(!bool(a))throw true;//cannot cast
        return *a;
      }
      PVetoSubSystem  & GetPVetoSubSystem   (){
        PVetoSubSystem*  a=dynamic_cast<PVetoSubSystem*>(fAllSubSystems.at(eSubSystem::ePVeto  ));
        if(!bool(a))throw true;//cannot cast
        return *a;
      }
      HEPVetoSubSystem& GetHEPVetoSubSystem (){
        HEPVetoSubSystem* a=dynamic_cast<HEPVetoSubSystem*>(fAllSubSystems.at(eSubSystem::eHEPVeto));
        if(!bool(a))throw true;//cannot cast
        return *a;
      }
      SACSubSystem    & GetSACSubSystem     (){
        SACSubSystem    * a=dynamic_cast<SACSubSystem    *>(fAllSubSystems.at(eSubSystem::eSAC    ));
        if(!bool(a))throw true;//cannot cast
        return *a;
      }
      ECALSubSystem   & GetECALSubSystem    (){
        ECALSubSystem   * a=dynamic_cast<ECALSubSystem   *>(fAllSubSystems.at(eSubSystem::eECAL   ));
        if(!bool(a))throw true;//cannot cast
        return *a;
      }
      TargetSubSystem & GetTargetSubSystem  (){
        TargetSubSystem * a=dynamic_cast<TargetSubSystem *>(fAllSubSystems.at(eSubSystem::eTarget ));
        if(!bool(a))throw true;//cannot cast
        return *a;
      }

      VSubSystem& GetSubSystem(eSubSystem s)    {
        if(!fAllSubSystems.count(s))
          throw true; // does not exist
        return *fAllSubSystems.at(s);
      }

      VSubSystem& EmplaceSubSystem(const std::string& name,eSubSystem s){
        if(fAllSubSystems.count(s))
          throw true; // already exists
        switch(s) {
          case eSubSystem::eEVeto  : (fAllSubSystems.emplace(s,new EVetoSubSystem  (name)));break;
          case eSubSystem::ePVeto  : (fAllSubSystems.emplace(s,new PVetoSubSystem  (name)));break;
          case eSubSystem::eHEPVeto: (fAllSubSystems.emplace(s,new HEPVetoSubSystem(name)));break;
          case eSubSystem::eSAC    : (fAllSubSystems.emplace(s,new SACSubSystem    (name)));break;
          case eSubSystem::eECAL   : (fAllSubSystems.emplace(s,new ECALSubSystem   (name)));break;
          case eSubSystem::eTarget : (fAllSubSystems.emplace(s,new TargetSubSystem (name)));break;
          default:
                                     throw true;// unknow detector type
        }
        return *fAllSubSystems.at(s);
      }

      VRODevice& EmplaceRODevice(const std::string& name,eRODeviceTypes s){
        if(fAllRODevices.count(name)) ERROR("throw");
        switch(s) {
          case eRODeviceTypes::eMAPMTH9500  : fAllRODevices.emplace(name,new RODeviceMAPMTH9500  (name));break;
          case eRODeviceTypes::ePhotonis    : fAllRODevices.emplace(name,new RODevicePhotonis    (name));break;
          default: ERROR("throw");
#warning throw
        }
        return *(fAllRODevices.at(name));
      }

      VDigitizerChannel& EmplaceDigitizer(const std::string& name,eDigitizerTypes s){
        if(fAllDigitizerChannels.count(name))throw true;
        switch(s) {
          case eDigitizerTypes::eCAENv1742 : fAllDigitizerChannels.emplace(name,new CAENv1742Channel());break;
          default: ERROR("unknown type");
#warning throw
        }
        return *fAllDigitizerChannels.at(name);
      }
      void DestroySubSystems(){for(auto it=fAllSubSystems.begin();it!=fAllSubSystems.end();++it)delete it->second;}
      void DestoryRODevices (){for(auto it=fAllRODevices .begin();it!=fAllRODevices .end();++it)delete it->second;}
      void DestroyDigitizers(){for(auto it=fAllDigitizers.begin();it!=fAllDigitizers.end();++it)delete it->second;}
  };
}

#endif
