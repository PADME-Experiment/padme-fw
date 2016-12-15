#include"fwkUniverseBuilder.hh"

#include"utlMessageBus.hh"

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
    if       ( det=="eEVeto"  ){
      det::VSubSystem& s=uni.EmplaceSubSystem(subs_it->first,det::eSubSystem::eEVeto  );
      //s.SetName(subs_it->first);s.GetTopLayer().SetName(subs_it->first+".topLayer");
    } else if( det=="ePVeto"  ){
      det::VSubSystem& s=uni.EmplaceSubSystem(subs_it->first,det::eSubSystem::ePVeto  );
      //s.SetName(subs_it->first);s.GetTopLayer().SetName(subs_it->first+".topLayer");
    } else if( det=="eHEPVeto"){
      det::VSubSystem& s=uni.EmplaceSubSystem(subs_it->first,det::eSubSystem::eHEPVeto);
      //s.SetName(subs_it->first);s.GetTopLayer().SetName(subs_it->first+".topLayer");
    } else if( det=="eSAC"    ){
      det::VSubSystem& s=uni.EmplaceSubSystem(subs_it->first,det::eSubSystem::eSAC    );
      //s.SetName(subs_it->first);s.GetTopLayer().SetName(subs_it->first+".topLayer");
    } else if( det=="eECAL"   ){
      det::VSubSystem& s=uni.EmplaceSubSystem(subs_it->first,det::eSubSystem::eECAL   );
      //s.SetName(subs_it->first);s.GetTopLayer().SetName(subs_it->first+".topLayer");
    } else if( det=="eTarget" ){
      det::VSubSystem& s=uni.EmplaceSubSystem(subs_it->first,det::eSubSystem::eTarget );
      //s.SetName(subs_it->first);s.GetTopLayer().SetName(subs_it->first+".topLayer");
    }
    else   WARNING(subs_it->first+" skipped");
  }
}
  void
fwk::UniverseBuilder::Layers()
{
  auto & uni= det::Universe::GetInstance();
  for(auto det_it=uni.SubSystemsBegin();det_it!=uni.SubSystemsEnd();++det_it){
    auto subSystemID=det_it->first;
    det::VSubSystem* subSystem=det_it->second;
    const std::string& ssName=subSystem->GetName();

    const std::map<std::string,std::vector<std::string>>&
      layers = fConfig.GetGroup(ssName+".Layers");
    for(auto line_it=layers.begin();line_it!=layers.end();++line_it){
      const std::string& parLayerName=line_it->first;
      auto const& subLayers=line_it->second;
      for(auto num_it=subLayers.begin();num_it!=subLayers.end();++num_it){
        subSystem->GetLayer(parLayerName)->EmplaceLayer(*num_it);
        SUCCESS(*num_it+" as a sublayer of "+parLayerName);
      }
    }
  }
}
  void
fwk::UniverseBuilder::Detectors()
{
  auto & uni= det::Universe::GetInstance();
  for(auto det_it=uni.SubSystemsBegin();det_it!=uni.SubSystemsEnd();++det_it){
    auto subSystemID=det_it->first;
    det::VSubSystem* subSystem=det_it->second;
    const std::string& ssName=subSystem->GetName();

    const std::map<std::string,std::vector<std::string>>&
      detectors = fConfig.GetGroup(ssName+".Detectors");
    for(auto line_it=detectors.begin();line_it!=detectors.end();++line_it){
      const std::string& detName=line_it->first;
      const auto& vect=line_it->second;
      if(vect.size()!=3)ERROR("throw");
#warning throw
      auto const& detTypeStr=vect[0];
      det::eDetectorTypes detType;
      if     (detTypeStr=="eEVetoScintillatorBar"  )detType=det::eDetectorTypes::eEVetoScintillatorBar   ;
      else if(detTypeStr=="ePVetoScintillatorBar"  )detType=det::eDetectorTypes::ePVetoScintillatorBar   ;
      else if(detTypeStr=="eHEPVetoScintillatorBar")detType=det::eDetectorTypes::eHEPVetoScintillatorBar ;
      else if(detTypeStr=="eECALScintillator"      )detType=det::eDetectorTypes::eECALScintillator       ;
#warning throw
      else detType=det::eDetectorTypes::eUnknown;
      const std::string& layName=vect[1];
#warning atoi needs to be checked
      const int detInd=std::atoi(vect[2].c_str());
      uni.GetLayer(layName)->EmplaceDetector(detName,detType,detInd);
    }
  }
}
