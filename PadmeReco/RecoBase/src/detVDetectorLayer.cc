#include"detVDetectorLayer.hh"
#include"detVDetector.hh"
#include"detVSubSystem.hh"

det::VDetectorLayer::~VDetectorLayer(){
  for(auto it=fLayers.begin();it!=fLayers.end();++it){delete *it;}
  for(auto it=fDetectors.begin();it!=fDetectors.end();++it){delete it->second;}
}







  det::EVetoLayer&
det::EVetoLayer::EmplaceLayer(const std::string&n)
{
#warning throw if exists
  fLayers.emplace_back(new EVetoLayer(n,this));
  VDetectorLayer* newLayer=*fLayers.rbegin();
  newLayer->SetParentSubSystem(GetParentSubSystem());
  GetParentSubSystem()->RegisterLayer(newLayer);
  return *(dynamic_cast<EVetoLayer*>(newLayer));
}


  void
det::EVetoLayer::EmplaceDetector(const std::string&n,eDetectorTypes t,int ind)
{
  switch (t){
    case eDetectorTypes::eEVetoScintillatorBar:
      fDetectors.emplace(ind,new EVetoScintillatorBar(n));
      break;
    default:
#warning throw
      ERROR("throw");
  }
  VDetector* lastAdded=fDetectors.rbegin()->second;
  GetParentSubSystem()->RegisterDetector(lastAdded);
}
  void
det::PVetoLayer::EmplaceDetector(const std::string&n,eDetectorTypes t,int ind)
{
  ERROR("not yet implemented");
}
  void
det::HEPVetoLayer::EmplaceDetector(const std::string&n,eDetectorTypes t,int ind)
{
  ERROR("not yet implemented");
}
  void
det::SACLayer::EmplaceDetector(const std::string&n,eDetectorTypes t,int ind)
{
  ERROR("not yet implemented");
}
  void
det::ECALLayer::EmplaceDetector(const std::string&n,eDetectorTypes t,int ind)
{
  ERROR("not yet implemented");
}
  void
det::TargetLayer::EmplaceDetector(const std::string&n,eDetectorTypes t,int ind)
{
  ERROR("not yet implemented");
}



  det::PVetoLayer&
det::PVetoLayer::EmplaceLayer(const std::string&n)
{
  fLayers.emplace_back(new PVetoLayer(n,this));

  VDetectorLayer* newLayer=*fLayers.rbegin();
  newLayer->SetParentSubSystem(GetParentSubSystem());
  GetParentSubSystem()->RegisterLayer(newLayer);
  return *(dynamic_cast<PVetoLayer*>(newLayer));
}




  det::HEPVetoLayer&
det::HEPVetoLayer::EmplaceLayer(const std::string&n)
{
  fLayers.emplace_back(new HEPVetoLayer(n,this));
  VDetectorLayer* newLayer=*fLayers.rbegin();
  newLayer->SetParentSubSystem(GetParentSubSystem());
  GetParentSubSystem()->RegisterLayer(newLayer);
  return *(dynamic_cast<HEPVetoLayer*>(newLayer));
}





  det::SACLayer&
det::SACLayer::EmplaceLayer(const std::string&n)
{
  fLayers.emplace_back(new SACLayer(n,this));
  VDetectorLayer* newLayer=*fLayers.rbegin();
  newLayer->SetParentSubSystem(GetParentSubSystem());
  GetParentSubSystem()->RegisterLayer(newLayer);
  return *(dynamic_cast<SACLayer*>(newLayer));
}




  det::ECALLayer&
det::ECALLayer::EmplaceLayer(const std::string&n)
{
  fLayers.emplace_back(new ECALLayer(n,this));
  VDetectorLayer* newLayer=*fLayers.rbegin();
  newLayer->SetParentSubSystem(GetParentSubSystem());
  GetParentSubSystem()->RegisterLayer(newLayer);
  return *(dynamic_cast<ECALLayer*>(newLayer));
}




  det::TargetLayer&
det::TargetLayer::EmplaceLayer(const std::string&n)
{
  fLayers.emplace_back(new TargetLayer(n,this));
  VDetectorLayer* newLayer=*fLayers.rbegin();
  newLayer->SetParentSubSystem(GetParentSubSystem());
  GetParentSubSystem()->RegisterLayer(newLayer);
  return *(dynamic_cast<TargetLayer*>(newLayer));
}
