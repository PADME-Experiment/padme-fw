// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
// Last modified by M. Raggi 13/05/2019 
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
{
  fT0Map.clear();
  fCalibMap.clear();
}


void ECalCalibration::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr ){
  fUseCalibE   = (int)cfg->GetParOrDefault("EnergyCalibration","UseCalibration",1);
  fGlobEnScale = (double)cfg->GetParOrDefault("EnergyCalibration","AveragepCMeV",15.);
  fUseCalibT   = (int)cfg->GetParOrDefault("TimeAlignment","UseTimeAlignment",1);

  // Energy calibration 
  if(fUseCalibE==1) ECalib.open("config/Calibration/ECalCalibConst.txt");
  if(fUseCalibE==1 && !ECalib.is_open()){ 
    std::cout<<"ERROR: Cannot find ECal calibration file "<<"**************"<<std::endl;
    exit(1);
  }
  //  if(fUseCalibE==1) ReadCalibConstant();

  // Time offsets calibration 
  if(fUseCalibT==1) TCalib.open("config/Calibration/ECalTimeOffSets.txt");
  if(fUseCalibT==1 && !TCalib.is_open()){ 
    std::cout<<"ERROR: Cannot find ECal time ofsset file "<<"**************"<<std::endl;
    exit(1);
  }
  //  if(fUseCalibE==1) ReadCalibConstant();

  if(fUseCalibE==1 || fUseCalibT==1) ReadCalibConstant();
}

void ECalCalibration::ReadCalibConstant()
{
  double MIPCharge,TimeOffSet;
  int NBD,CID;
  int row,col;

  fMuonDepositedEnergy=17.5;
  fGlobEnScale=15;

  //Read Energy calibration constants
  if(ECalib.is_open()){
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

  //Read Time calibration constants
  if(TCalib.is_open()){
    for(int i=0;i<616;i++){
      TCalib >> row >> col >> NBD >> CID>>TimeOffSet;   //reads Piperno informations need cross-check
      fT0Map[std::make_pair(NBD,CID)] = TimeOffSet;
      //      fCalibMap[std::make_pair(row,col)] = MIPCharge/(fMuonDepositedEnergy*fGlobEnScale);
      std::cout<<i<<" channel ID "<<CID<<" NBD "<<NBD<<" "<<fT0Map[std::make_pair(NBD,CID)]<<std::endl;
    }
    TCalib.close();
  } else{ 
    std::cout << "WARNING:: No calibration file available "<<std::endl;
  } 
  
}
 
void ECalCalibration::PerformCalibration(std::vector<TRecoVHit *> &Hits)
{
  for(unsigned int iHit = 0;iHit < Hits.size();iHit++){
    if (fUseCalibE == 1){
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
      
    }
    
    if (fUseCalibT == 1){
      // Correcting for time offestets in between channels
      fHitT          = Hits[iHit]->GetTime();
      fHitTCorrected = fHitT-fT0Map[std::make_pair(fBID,fChID)];
      //      std::cout<<"channel ID "<<ich<<" HitT "<<fHitT<<" "<<fHitTCorrected<<std::endl;
      Hits[iHit]->SetTime(fHitTCorrected);
    }
  } 

}

