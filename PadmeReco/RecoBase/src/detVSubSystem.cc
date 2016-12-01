#include"detVSubSystem.hh"
#include"detVDetectorLayer.hh"
#include"detUniverse.hh"

  void
det::VSubSystem::RegisterLayer(VDetectorLayer* lay)
{
  if(fLayers.count(lay->GetName())){
    ERROR(lay->GetName()+" already exists");
#warning try throw
  }
  fLayers[lay->GetName()]=lay;
  det::Universe::GetInstance().RegisterLayer(lay);
}

void
det::VSubSystem::RegisterDetector(VDetector* det)
{
  if(fDetectors.count(det->GetName())){
    ERROR(det->GetName()+" already exists");
#warning try throw
  }
  fDetectors[det->GetName()]=det;
  det::Universe::GetInstance().RegisterDetector(det);
}



det::EVetoSubSystem::EVetoSubSystem  (const std::string&n):VSubSystem(n){
  fSubSystem=eSubSystem::eEVeto  ;
  fTopLayer=new EVetoLayer  (GetName()+".TopLayer",nullptr);
  fTopLayer->SetParentSubSystem(this);
  RegisterLayer(fTopLayer);
}
det::PVetoSubSystem::PVetoSubSystem  (const std::string&n):VSubSystem(n){
  fSubSystem=eSubSystem::ePVeto  ;
  fTopLayer=new PVetoLayer  (GetName()+".TopLayer",nullptr);
  fTopLayer->SetParentSubSystem(this);
  RegisterLayer(fTopLayer);
}
det::HEPVetoSubSystem::HEPVetoSubSystem(const std::string&n):VSubSystem(n){
  fSubSystem=eSubSystem::eHEPVeto;
  fTopLayer=new HEPVetoLayer(GetName()+".TopLayer",nullptr);
  fTopLayer->SetParentSubSystem(this);
  RegisterLayer(fTopLayer);
}
det::SACSubSystem::SACSubSystem    (const std::string&n):VSubSystem(n){
  fSubSystem=eSubSystem::eSAC    ;
  fTopLayer=new SACLayer    (GetName()+".TopLayer",nullptr);
  fTopLayer->SetParentSubSystem(this);
  RegisterLayer(fTopLayer);
}
det::ECALSubSystem::ECALSubSystem   (const std::string&n):VSubSystem(n){
  fSubSystem=eSubSystem::eECAL   ;
  fTopLayer=new ECALLayer   (GetName()+".TopLayer",nullptr);
  fTopLayer->SetParentSubSystem(this);
  RegisterLayer(fTopLayer);
}
det::TargetSubSystem::TargetSubSystem (const std::string&n):VSubSystem(n){
  fSubSystem=eSubSystem::eTarget ;
  fTopLayer=new TargetLayer (GetName()+".TopLayer",nullptr);
  fTopLayer->SetParentSubSystem(this);
  RegisterLayer(fTopLayer);
}
