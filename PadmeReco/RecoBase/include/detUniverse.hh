#ifndef _RecoBase_detUniverse_HH_
#define _RecoBase_detUniverse_HH_ 1

#include "det.hh"
#include "fwk.hh"
#include "utl.hh"
#include "detVDigitizer.hh"
#include "detVDigitizerChannel.hh"
#include "detVDetector.hh"
#include "detVDetectorSystem.hh"

namespace det{
  class Universe{
    protected:
    public:
      Universe(){};
      ~Universe(){};
    public:
      static Universe&GetInstance(){
        static Universe a=Universe();
        return a;
      }

      std::map<std::string,std::shared_ptr<VDetector>>        ::iterator DetectorsBegin()        {return fAllDetectors         .begin();}
      std::map<std::string,std::shared_ptr<VRODevice>>        ::iterator RODevicesBegin()        {return fAllRODevices         .begin();}
      std::map<std::string,std::shared_ptr<VDigitizerChannel>>::iterator DigitizerChannelsBegin(){return fAllDigitizerChannels .begin();}
      std::map<eSubSystem,std::shared_ptr<VSubSystem>>        ::iterator SubSystemsBegin()       {return fAllSubSystems        .begin();}
      std::map<std::string,std::shared_ptr<VDetector>>        ::iterator DetectorsEnd()          {return fAllDetectors         .end();}
      std::map<std::string,std::shared_ptr<VRODevice>>        ::iterator RODevicesEnd()          {return fAllRODevices         .end();}
      std::map<std::string,std::shared_ptr<VDigitizerChannel>>::iterator DigitizerChannelsEnd()  {return fAllDigitizerChannels .end();}
      std::map<eSubSystem,std::shared_ptr<VSubSystem>>        ::iterator SubSystemsEnd()         {return fAllSubSystems        .end();}

    private:
      std::map<std::string,std::shared_ptr<VDetector>>        fAllDetectors;
      std::map<std::string,std::shared_ptr<VRODevice>>        fAllRODevices;
      std::map<std::string,std::shared_ptr<VDigitizerChannel>>fAllDigitizerChannels;
      std::map<eSubSystem,std::shared_ptr<VSubSystem>>        fAllSubSystems;
    public:
      EVetoSubSystem  & GetEVetoSubSystem   (){
        std::shared_ptr<EVetoSubSystem>   a=
          std::dynamic_pointer_cast<EVetoSubSystem  ,VSubSystem>(
              fAllSubSystems.at(eSubSystem::eEVeto  ));
        if(!bool(a))throw true;//cannot cast
        return *a;
      }
      PVetoSubSystem  & GetPVetoSubSystem   (){
        std::shared_ptr<PVetoSubSystem >  a=
          std::dynamic_pointer_cast<PVetoSubSystem  ,VSubSystem>(
              fAllSubSystems.at(eSubSystem::ePVeto  ));
        if(!bool(a))throw true;//cannot cast
        return *a;
      }
      HEPVetoSubSystem& GetHEPVetoSubSystem (){
        std::shared_ptr<HEPVetoSubSystem> a=
          std::dynamic_pointer_cast<HEPVetoSubSystem,VSubSystem>(
              fAllSubSystems.at(eSubSystem::eHEPVeto));
        if(!bool(a))throw true;//cannot cast
        return *a;
      }
      SACSubSystem    & GetSACSubSystem     (){
        std::shared_ptr<SACSubSystem  >   a=
          std::dynamic_pointer_cast<SACSubSystem    ,VSubSystem>(
              fAllSubSystems.at(eSubSystem::eSAC    ));
        if(!bool(a))throw true;//cannot cast
        return *a;
      }
      ECALSubSystem   & GetECALSubSystem    (){
        std::shared_ptr<ECALSubSystem  >  a=
          std::dynamic_pointer_cast<ECALSubSystem   ,VSubSystem>(
              fAllSubSystems.at(eSubSystem::eECAL   ));
        if(!bool(a))throw true;//cannot cast
        return *a;
      }
      TargetSubSystem & GetTargetSubSystem  (){
        std::shared_ptr<TargetSubSystem>  a=
          std::dynamic_pointer_cast<TargetSubSystem ,VSubSystem>(
              fAllSubSystems.at(eSubSystem::eTarget ));
        if(!bool(a))throw true;//cannot cast
        return *a;
      }
      VSubSystem& GetSubSystem(eSubSystem s)    {
        if(!fAllSubSystems.count(s))
          throw true; // does not exist
        return *fAllSubSystems.at(s);
      }
      VSubSystem& EmplaceSubSystem(eSubSystem s){
        if(fAllSubSystems.count(s))
          throw true; // already exists
        switch(s) {
          case eSubSystem::eEVeto  : (fAllSubSystems.emplace(s,std::make_shared<EVetoSubSystem  >()));break;
          case eSubSystem::ePVeto  : (fAllSubSystems.emplace(s,std::make_shared<PVetoSubSystem  >()));break;
          case eSubSystem::eHEPVeto: (fAllSubSystems.emplace(s,std::make_shared<HEPVetoSubSystem>()));break;
          case eSubSystem::eSAC    : (fAllSubSystems.emplace(s,std::make_shared<SACSubSystem    >()));break;
          case eSubSystem::eECAL   : (fAllSubSystems.emplace(s,std::make_shared<ECALSubSystem   >()));break;
          case eSubSystem::eTarget : (fAllSubSystems.emplace(s,std::make_shared<TargetSubSystem >()));break;
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
          case eRODeviceTypes::eHamamatsuH9500  : fAllRODevices.emplace(name,std::make_shared<HamamatsuH9500  >());break;
          case eRODeviceTypes::ePhotonis        : fAllRODevices.emplace(name,std::make_shared<Photonis        >());break;
          default: ERROR("throw");
        }
        return *fAllRODevices.at(name);
      }
      std::shared_ptr<VDetector> EmplaceDetector(const std::string& name,eDetectorTypes s){
        if(fAllDetectors.count(name))ERROR("throw");
        switch(s) {
          case eDetectorTypes::eEVetoScintillatorBar  : fAllDetectors.emplace(name,std::make_shared<  EVetoScintillatorBar>());break;
          case eDetectorTypes::ePVetoScintillatorBar  : fAllDetectors.emplace(name,std::make_shared<  PVetoScintillatorBar>());break;
          case eDetectorTypes::eHEPVetoScintillatorBar: fAllDetectors.emplace(name,std::make_shared<HEPVetoScintillatorBar>());break;
          case eDetectorTypes::eECALScintillator      : fAllDetectors.emplace(name,std::make_shared<ECALScintillator   >());break;
          default: ERROR("throw");
        }
        fAllDetectors.at(name)->SetName(name);
        return fAllDetectors.at(name);
      }
      VDigitizerChannel& EmplaceDigitizerChannel(const std::string& name,eDigitizerTypes s){
        if(fAllDigitizerChannels.count(name))throw true;
        switch(s) {
          case eDigitizerTypes::eCAENv1742 : fAllDigitizerChannels.emplace(name,std::make_shared<CAENv1742Channel>());break;
          default: true; //throw 
        }
        return *fAllDigitizerChannels.at(name);
      }
  };
}

#endif
