#include"detRODeviceMAPMTH9500.hh"
#include"detROChannelMAPMTH9500.hh"
#include"detVRODevice.hh"
#include<string>

det::RODeviceMAPMTH9500::RODeviceMAPMTH9500(const std::string&n)
  : VRODevice(n)
{
  fRODeviceType=eRODeviceTypes::eMAPMTH9500;
  //fROChannels.reserve(fkNChannels);
  for(unsigned i=0;i<fkNChannels;++i){
    fROChannels.emplace(i,new ROChannelMAPMTH9500(
          GetName()+"."+std::to_string(i),this));
    fROChannels.at(i)->SetROChannelNumber(i);
  }
}
det::RODeviceMAPMTH9500::~RODeviceMAPMTH9500(){
  for(auto it=fROChannels.begin();it!=fROChannels.end();++it){
    delete it->second;
  }
}
