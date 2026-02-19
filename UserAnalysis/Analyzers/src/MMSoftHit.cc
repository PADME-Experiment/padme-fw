#include "MMSoftHit.hh"
#include "GeneralInfo.hh"


MMSoftHit::MMSoftHit() {
  fChannelId = -9999;
  fPosition.SetXYZ(-9999,-9999,-9999);
  fEnergy = -9999;
  fTime = -9999;
  fIsolLevel = -9999;
  for(Int_t ipmode=0; ipmode<IPMODES; ipmode++) {
    for(Int_t clumode=0; clumode<CLUSTERMODES; clumode++) {
      fCluPtr[ipmode][clumode] = -9999;
    }
  }
}

MMSoftHit::~MMSoftHit() {};

void MMSoftHit::CopyHit(TRecoVHit *Hit) {
  fChannelId = Hit->GetChannelId(); //TODO forse getter bdid, view, quad, plane 
  fPosition = Hit->GetPosition();
  fEnergy = Hit->GetEnergy();
  fTime = Hit->GetTime();

  fmmi = GeneralInfo::GetInstance()->DecodeMMChannel(fChannelId);
}

void MMSoftHit::Print() const {
  std::cout<<"\t hit registred on plane "<<fmmi.plane<<" , quad: "<<fmmi.quad<<" , view: "<<fmmi.view<<std::endl;
  std::cout<<"\t hit channel: "<<fmmi.strip<<" [board id "<<fmmi.bdid<<" ]"<<std::endl;
  std::cout<<"\t hit coord (x,y,z): ( "<<fPosition.X()<<" ; "<<fPosition.Y()<<" ; "<<fPosition.Z()<<" )"<<std::endl;
  std::cout<<"\t hit time: "<<fTime<<std::endl;
  std::cout<<"\t hit energy: "<<fEnergy<<std::endl;
  std::cout<<"\t hit belonging to clusters:";
  for(Int_t ipmode=0; ipmode<IPMODES; ipmode++) { 
    for(Int_t clumode=0; clumode<CLUSTERMODES; clumode++) {
      if(fCluPtr[ipmode][clumode]!=-9999) std::cout<<"\t\t [IPmode:"<<ipmode<<" , CLUmoode"<<clumode<< " ] "<<fCluPtr[ipmode][clumode]<<std::endl;
    }
  }
  std::cout<<std::endl;
  std::cout<<std::endl;
}



























































































































