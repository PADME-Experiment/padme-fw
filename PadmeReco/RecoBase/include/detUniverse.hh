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
    protected:
    public:
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

      std::map<std::string,VDetector        *>::iterator DetectorsBegin()        {return fAllDetectors         .begin();}
      std::map<std::string,VDetector        *>::iterator DetectorsEnd()          {return fAllDetectors         .end();}
      std::map<std::string,VRODevice        *>::iterator RODevicesBegin()        {return fAllRODevices         .begin();}
      std::map<std::string,VRODevice        *>::iterator RODevicesEnd()          {return fAllRODevices         .end();}
      std::map<eSubSystem ,VSubSystem       *>::iterator SubSystemsBegin()       {return fAllSubSystems        .begin();}
      std::map<eSubSystem ,VSubSystem       *>::iterator SubSystemsEnd()         {return fAllSubSystems        .end();}

      //std::map<std::string,VDigitizerChannel*>::iterator DigitizerChannelsBegin(){return fAllDigitizerChannels .begin();}
      //std::map<std::string,VDigitizerChannel*>::iterator DigitizerChannelsEnd()  {return fAllDigitizerChannels .end();}

    private:
      std::map<std::string,VDetector        *>fAllDetectors;
      std::map<std::string,VRODevice        *>fAllRODevices;
      std::map<eSubSystem ,VSubSystem       *>fAllSubSystems;

      std::map<std::string,VDigitizerChannel*>fAllDigitizerChannels;
    public:
      // EVetoSubSystem  & GetEVetoSubSystem   (){
      //   std::shared_ptr<EVetoSubSystem>   a=
      //     std::dynamic_pointer_cast<EVetoSubSystem  ,VSubSystem>(
      //         fAllSubSystems.at(eSubSystem::eEVeto  ));
      //   if(!bool(a))throw true;//cannot cast
      //   return *a;
      // }
      // PVetoSubSystem  & GetPVetoSubSystem   (){
      //   std::shared_ptr<PVetoSubSystem >  a=
      //     std::dynamic_pointer_cast<PVetoSubSystem  ,VSubSystem>(
      //         fAllSubSystems.at(eSubSystem::ePVeto  ));
      //   if(!bool(a))throw true;//cannot cast
      //   return *a;
      // }
      // HEPVetoSubSystem& GetHEPVetoSubSystem (){
      //   std::shared_ptr<HEPVetoSubSystem> a=
      //     std::dynamic_pointer_cast<HEPVetoSubSystem,VSubSystem>(
      //         fAllSubSystems.at(eSubSystem::eHEPVeto));
      //   if(!bool(a))throw true;//cannot cast
      //   return *a;
      // }
      // SACSubSystem    & GetSACSubSystem     (){
      //   std::shared_ptr<SACSubSystem  >   a=
      //     std::dynamic_pointer_cast<SACSubSystem    ,VSubSystem>(
      //         fAllSubSystems.at(eSubSystem::eSAC    ));
      //   if(!bool(a))throw true;//cannot cast
      //   return *a;
      // }
      // ECALSubSystem   & GetECALSubSystem    (){
      //   std::shared_ptr<ECALSubSystem  >  a=
      //     std::dynamic_pointer_cast<ECALSubSystem   ,VSubSystem>(
      //         fAllSubSystems.at(eSubSystem::eECAL   ));
      //   if(!bool(a))throw true;//cannot cast
      //   return *a;
      // }
      // TargetSubSystem & GetTargetSubSystem  (){
      //   std::shared_ptr<TargetSubSystem>  a=
      //     std::dynamic_pointer_cast<TargetSubSystem ,VSubSystem>(
      //         fAllSubSystems.at(eSubSystem::eTarget ));
      //   if(!bool(a))throw true;//cannot cast
      //   return *a;
      // }
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


      //  enum class eSubSystem { eUnknown=0, eEVeto, ePVeto, eHEPVeto, eSAC, eECAL, eTarget };
      //  enum class eDigitizerTypes { eUnknown=0, eCAENv1751, eCAENv1742, eTEL62 } ;
      //  enum class eRODeviceTypes  { eUnknown=0, eHamamatsuH9500, ePhotonis, eChineesePMT, eSiPM };
      //enum class eDetectorTypes  { eUnknown=0, eVetoScintillatorBar, eECALScintillator};

      VRODevice& EmplaceRODevice(const std::string& name,eRODeviceTypes s){
        if(fAllRODevices.count(name)) ERROR("throw");
        switch(s) {
          case eRODeviceTypes::eMAPMTH9500  : fAllRODevices.emplace(name,new RODeviceMAPMTH9500  (name));break;
          case eRODeviceTypes::ePhotonis    : fAllRODevices.emplace(name,new RODevicePhotonis    (name));break;
          default: ERROR("throw");
        }
        //fAllRODevices.at(name)->SetName(name);
        return *(fAllRODevices.at(name));
      }
      VDetector* EmplaceDetector(const std::string& name,eDetectorTypes s){
        if(fAllDetectors.count(name))ERROR("throw");
        switch(s) {
          case eDetectorTypes::eEVetoScintillatorBar   : fAllDetectors.emplace(name,new   EVetoScintillatorBar(name));break;
          case eDetectorTypes::ePVetoScintillatorBar   : fAllDetectors.emplace(name,new   PVetoScintillatorBar(name));break;
          case eDetectorTypes::eHEPVetoScintillatorBar : fAllDetectors.emplace(name,new HEPVetoScintillatorBar(name));break;
          case eDetectorTypes::eECALScintillator       : fAllDetectors.emplace(name,new ECALScintillator      (name));break;
          default: ERROR("throw");
        }
        //fAllDetectors.at(name)->SetName(name);
        return fAllDetectors.at(name);
      }
      //VDigitizerChannel& EmplaceDigitizerChannel(const std::string& name,eDigitizerTypes s){
      //  if(fAllDigitizerChannels.count(name))throw true;
      //  switch(s) {
      //    case eDigitizerTypes::eCAENv1742 : fAllDigitizerChannels.emplace(name,new CAENv1742Channel());break;
      //    default: true; //throw 
      //  }
      //  return *fAllDigitizerChannels.at(name);
      //}
      void DestroySubSystems(){for(auto it=fAllSubSystems.begin();it!=fAllSubSystems.end();++it)delete it->second;}
      void DestoryRODevices (){for(auto it=fAllRODevices .begin();it!=fAllRODevices .end();++it)delete it->second;}
      void DestroyDigitizers(){for(auto it=fAllDetectors .begin();it!=fAllDetectors .end();++it)delete it->second;}
  };
}

#endif
