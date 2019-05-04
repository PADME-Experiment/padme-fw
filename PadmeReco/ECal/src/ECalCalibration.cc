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
  if(fUseCalib==1) ReadCalibConstant();
}

void ECalCalibration::ReadCalibConstant()
{
  std::ifstream ECalib; 
  std::ifstream TCalib; 
//  sprintf(fname,"config/BDPed/ECalCalibConst.txt",NBD);
//  sprintf(fname,"config/BDPed/ECalT0.txt",NBD);
  ECalib.open("config/Calibration/ECalCalibConst.txt");

  double MIPCharge;
  int NBD,CID;
  int row,col;

  fMuonDepositedEnergy=18;
  fGlobEnScale=15;
  if(ECalib.is_open()){
    double temp;
    for(int i=0;i<616;i++){
      ECalib >> row >> col >> NBD >> CID >> MIPCharge;   //reads Piperno informations need cross-check
      fCalibMap[std::make_pair(NBD,CID)] = MIPCharge/(fMuonDepositedEnergy*fGlobEnScale);
      //      fCalibMap[std::make_pair(row,col)] = MIPCharge/(fMuonDepositedEnergy*fGlobEnScale);

      std::cout<<i<<" channel ID "<<CID<<" NBD "<<NBD<<" "<<fCalibMap[std::make_pair(row,col)]<<std::endl;
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
      fHitECalibrated= fHitE/fCalibMap[std::make_pair(BD,ChID)];
      std::cout<<"channel ID "<<ChID<<" BD "<<BD<<" ich "<<ich<<" HitE "<<fHitE<<" "<<fHitECalibrated<<" "<<fCalibMap[std::make_pair(BD,ChID)]<<std::endl;
      Hits[iHit]->SetEnergy(fHitECalibrated);

      // Correcting for time offestets in between channels
      fHitT          = Hits[iHit]->GetTime();
      fHitTCorrected = fHitT-fT0Map[std::make_pair(fBID,fChID)];
      //      std::cout<<"channel ID "<<ich<<" HitT "<<fHitT<<" "<<fHitTCorrected<<std::endl;
      Hits[iHit]->SetTime(fHitTCorrected);
    }
  } 

}

