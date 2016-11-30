#include"fwkUniverseBuilder.hh"

#include"detVROChannel.hh"
#include"detVRODevice.hh"
#include"detVSubSystem.hh"
#include"detVPadmeElement.hh"
#include"detVDigitizerChannel.hh"
#include"detVDigitizer.hh"
#include"detVSubSystem.hh"
#include"detVDetector.hh"
#include"detUniverse.hh"

  void
fwk::UniverseBuilder::InitSeq()
{
  INFO("UniverseBuilder");
  INFO("SubSystems();");SubSystems();SUCCESS("SubSystems();");
  INFO("Layers();    ");Layers();    SUCCESS("Layers();    ");
  INFO("Detectors(); ");Detectors(); SUCCESS("Detectors(); ");
  INFO("RODevices(); ");RODevices(); SUCCESS("RODevices(); ");
  INFO("Digitizers();");Digitizers();SUCCESS("Digitizers();");
}

  void
fwk::UniverseBuilder::Digitizers()
{
}

  void
fwk::UniverseBuilder::RODevices()
{
  const std::map<std::string,std::vector<std::string> > & rodev=fConfig.GetGroup("RO_DEVICES");
  auto & uni= det::Universe::GetInstance();
  for(auto rodev_it=rodev.begin();rodev_it!=rodev.end();++rodev_it){
    auto const& n=rodev_it->first;
    if(rodev_it->second.size()!=1)throw true; //except
    const std::string& dev=rodev_it->second[0];//=cfg.GetSingleArg("RO_DEVICES",n);
    det::eRODeviceTypes rotype;
    if     ( dev=="eHamamatsuH9500"  )rotype=det::eRODeviceTypes::eMAPMTH9500;
    else if( dev=="ePhotonis"        )rotype=det::eRODeviceTypes::ePhotonis      ;
    else   WARNING(n+" skipped");
    INFO(n);
    uni.EmplaceRODevice(n,rotype).SetName(n);
    SUCCESS(n);
  }
}
  void
fwk::UniverseBuilder::SubSystems()
{
  const std::map<std::string,std::vector<std::string> > & subs = fConfig.GetGroup("SUBSYSTEMS");
  auto & uni= det::Universe::GetInstance();
  for(auto subs_it=subs.begin();subs_it!=subs.end();++subs_it){
    if(subs_it->second.size()!=1)throw true; //except
    const std::string&  det=subs_it->second[0];
    if     ( det=="eEVeto"  ){det::VSubSystem& s=uni.EmplaceSubSystem(subs_it->first,det::eSubSystem::eEVeto  );s.SetName(subs_it->first);s.GetTopLayer().SetName(subs_it->first+"_topLayer");}
    else if( det=="ePVeto"  ){det::VSubSystem& s=uni.EmplaceSubSystem(subs_it->first,det::eSubSystem::ePVeto  );s.SetName(subs_it->first);s.GetTopLayer().SetName(subs_it->first+"_topLayer");}
    else if( det=="eHEPVeto"){det::VSubSystem& s=uni.EmplaceSubSystem(subs_it->first,det::eSubSystem::eHEPVeto);s.SetName(subs_it->first);s.GetTopLayer().SetName(subs_it->first+"_topLayer");}
    else if( det=="eSAC"    ){det::VSubSystem& s=uni.EmplaceSubSystem(subs_it->first,det::eSubSystem::eSAC    );s.SetName(subs_it->first);s.GetTopLayer().SetName(subs_it->first+"_topLayer");}
    else if( det=="eECAL"   ){det::VSubSystem& s=uni.EmplaceSubSystem(subs_it->first,det::eSubSystem::eECAL   );s.SetName(subs_it->first);s.GetTopLayer().SetName(subs_it->first+"_topLayer");}
    else if( det=="eTarget" ){det::VSubSystem& s=uni.EmplaceSubSystem(subs_it->first,det::eSubSystem::eTarget );s.SetName(subs_it->first);s.GetTopLayer().SetName(subs_it->first+"_topLayer");}
    else   WARNING(subs_it->first+" skipped");
  }
}
  void
fwk::UniverseBuilder::Layers()
{
  auto & uni= det::Universe::GetInstance();
  for(auto det_it=uni.SubSystemsBegin();det_it!=uni.SubSystemsEnd();++det_it){
    const std::string& detName=uni.GetSubSystem(det_it->first).GetName();
    auto det=det_it->second;

    const std::map<std::string,std::vector<std::string>>&
      layers = fConfig.GetGroup(detName+".Layers");
    for(auto line_it=layers.begin();line_it!=layers.end();++line_it){
      const std::string& layerName=line_it->first;
      auto const& layerIDs=line_it->second;
      det::VDetectorLayer* layer=&det->GetTopLayer();
      static int kkk=0;
      INFO(std::to_string(kkk++));
      for(auto num_it=layerIDs.begin();num_it!=(--layerIDs.end());++num_it){
        int idInt=std::atoi(num_it->c_str());
        layer=layer->GetLayerPtr(idInt).get();
      }
      INFO(std::to_string(std::atoi(layerIDs.rbegin()->c_str())));
      (layer->EmplaceLayer(std::atoi(layerIDs.rbegin()->c_str()))).SetName(layerName);
    }
  }
}
  void
fwk::UniverseBuilder::Detectors()
{
  auto & uni= det::Universe::GetInstance();
  for(auto det_it=uni.SubSystemsBegin();det_it!=uni.SubSystemsEnd();++det_it){
    const std::string& detName=uni.GetSubSystem(det_it->first).GetName();
    auto det=det_it->second;

    const std::map<std::string,std::vector<std::string>>&
      detectors = fConfig.GetGroup(detName+".Detectors");
    for(auto line_it=detectors.begin();line_it!=detectors.end();++line_it){
      const std::string& detectorName=line_it->first;
      const auto& vect=line_it->second;
      if(vect.size()<2)ERROR("throw");
      det::VDetectorLayer* layer=&det->GetTopLayer();
      for(unsigned i=1;i<vect.size()-1;++i){
        INFO("i  "+std::to_string(i));
        int idInt=std::atoi(vect[i].c_str());
        INFO("idInt  "+std::to_string(idInt));
        layer=layer->GetLayerPtr(idInt).get();
      }
      auto const& detTypeStr=vect[0];
      det::eDetectorTypes detType;
      if     (detTypeStr=="eEVetoScintillatorBar"  )detType=det::eDetectorTypes::eEVetoScintillatorBar   ;
      else if(detTypeStr=="ePVetoScintillatorBar"  )detType=det::eDetectorTypes::ePVetoScintillatorBar   ;
      else if(detTypeStr=="eHEPVetoScintillatorBar")detType=det::eDetectorTypes::eHEPVetoScintillatorBar ;
      else if(detTypeStr=="eECALScintillator"      )detType=det::eDetectorTypes::eECALScintillator       ;
#warning throw
      else detType=det::eDetectorTypes::eUnknown;

      det::VDetector* det=uni.EmplaceDetector(detectorName,detType);
      layer->AddDetector(std::atoi(vect.rbegin()->c_str()),det);
    }
  }
}
