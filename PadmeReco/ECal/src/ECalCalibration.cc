// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
// 
// --------------------------------------------------------------
#include "Riostream.h"

#include "ECalCalibration.hh"
#include "TRecoVHit.hh"
 
ECalCalibration::ECalCalibration()
  : PadmeVCalibration()
{
  std::cout<<"ECalCalibration being created ............"<<std::endl ;
 
}

ECalCalibration::~ECalCalibration()
{;}


void ECalCalibration::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr ){
  fUseCalib    = (int)cfg->GetParOrDefault("EnergyCalibration","UseCalibration",1);
  fGlobEnScale = (double)cfg->GetParOrDefault("EnergyCalibration","AveragepCMeV",15.);
  ECalib.open("config/Calibration/ECalCalibConst.txt");
  if(fUseCalib==1 && !ECalib.is_open()){ 
    std::cout<<"ERROR: Cannot find ECal calibration file "<<"**************"<<std::endl;
    exit(1);
  }
  if(fUseCalib==1) ReadCalibConstant();
}

void ECalCalibration::ReadCalibConstant()
{
  double MIPCharge;
  int NBD,CID;
  int row,col;

  fMuonDepositedEnergy=17;
  fGlobEnScale=15;
  if(ECalib.is_open()){
    double temp;
    for(int i=0;i<616;i++){
      ECalib >> row >> col >> NBD >> CID >> MIPCharge;   //reads Piperno informations need cross-check
      fCalibMap[std::make_pair(NBD,CID)] = MIPCharge/(fMuonDepositedEnergy*fGlobEnScale);
      //      fCalibMap[std::make_pair(row,col)] = MIPCharge/(fMuonDepositedEnergy*fGlobEnScale);
      std::cout<<i<<" channel ID "<<CID<<" NBD "<<NBD<<" "<<fCalibMap[std::make_pair(NBD,CID)]<<std::endl;
    }
    ECalib.close();
  } else{ 
      std::cout << "WARNING:: No calibration file available "<<std::endl;
  } 
}
 
void ECalCalibration::PerformCalibration(std::vector<TRecoVHit *> &Hits)
{
  if (fUseCalib == 1){
    for(unsigned int iHit = 0;iHit < Hits.size();iHit++){
      int ich = Hits[iHit]->GetChannelId(); //need to convert into BDID e CHID
      unsigned int BD   = Hits[iHit]->getBDid(); 
      unsigned int ChID = Hits[iHit]->getCHid(); 
      // Correcting for different crystals response
      fHitE   = Hits[iHit]->GetEnergy();
      if(fCalibMap[std::make_pair(BD,ChID)]!=0){ 
	fHitECalibrated= fHitE/fCalibMap[std::make_pair(BD,ChID)];
	Hits[iHit]->SetEnergy(fHitECalibrated);
	//	std::cout<<"channel ID "<<ChID<<" BD "<<BD<<" ich "<<ich<<" HitE "<<fHitE<<" "<<fHitECalibrated<<" "<<fCalibMap[std::make_pair(BD,ChID)]<<std::endl;
      }else{
	std::cout<<"Missing calibration for channel ID "<<ChID<<" BD "<<BD<<" ich "<<ich<<" HitE "<<fHitE<<std::endl;
      }
      // Correcting for time offestets in between channels
      fHitT          = Hits[iHit]->GetTime();
      fHitTCorrected = fHitT-fT0Map[std::make_pair(fBID,fChID)];
      //      std::cout<<"channel ID "<<ich<<" HitT "<<fHitT<<" "<<fHitTCorrected<<std::endl;
      Hits[iHit]->SetTime(fHitTCorrected);
    }
  } 

}

