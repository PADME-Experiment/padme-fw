// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
// Modified by G. Piperno (gabriele.piperno@roma1.infn.it) 2019-10-10 
// --------------------------------------------------------------
#include "Riostream.h"
#include "ECalMLCalibration.hh"
#include "TRecoVHit.hh"
#include "TRawEvent.hh"

#include <string>

 
ECalMLCalibration::ECalMLCalibration()
  : PadmeVCalibration()
{
  std::cout<<"ECalMLCalibration being created ............"<<std::endl ;
 
}



ECalMLCalibration::~ECalMLCalibration()
{
  fT0Map.clear();
  fCalibMap.clear();
}



void ECalMLCalibration::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr ){

  PadmeVCalibration::Init(cfg, chIdMgr );
  
  fUseCalibE   = (int)cfg->GetParOrDefault("EnergyCalibration","UseCalibration",1);
  fGlobEnScale = (double)cfg->GetParOrDefault("EnergyCalibration","AveragepCMeV",15.);
  fCalibList = (std::string)cfg->GetParOrDefault("EnergyCalibration","EnergyCalibIntervalsList","ECalMLEnergyCalibTimeIntervals.txt");
  fCalibVersion = (std::string)cfg->GetParOrDefault("EnergyCalibration","CalibVersion","0");
  //std::cout<<" ma giarda un po' "<<fCalibVersion<<std::endl;
  fUseCalibT   = (int)cfg->GetParOrDefault("TimeAlignment","UseTimeAlignment",1);


  // Energy calibration //

  if(fUseCalibE){

    fMuonDepositedEnergy=17.5;
    fGlobEnScale=15;

    /*
  fUseCalibT   = (int)cfg->GetParOrDefault("TimeAlignment","UseTimeAlignment",0);
  fCalibVersion = (int)cfg->GetParOrDefault("EnergyCalibration","CalibVersion",3);
  // Energy calibration 
  
  if(fUseCalibE) {
    char fname[256];
    sprintf(fname,"config/Calibration/ECalMLEnergyCalibration_%d.dat",fCalibVersion);
    ECalMLib.open(fname);
  }
  // if(fUseCalibE==1) ECalMLib.open("config/Calibration/ECalMLCalibConst.txt");
  // if(fUseCalibE==2) ECalMLib.open("config/Calibration/equalization_constants2.dat");
  */
  
    if(fCalibVersion=="0") { // calib file from event time

      fCalibHandler = ECalMLCalibHandler::GetInstance();
      // set energy calibration list file
      char path[256];
      sprintf(path,"config/Calibration/%s",fCalibList.c_str());
      fCalibHandler->SetFileList(path);
      std::cout<<"List file in use: "<<fCalibHandler->GetFileList()<<std::endl;
      // calibration initialisation
      fCalibHandler->Initialise();
      
    } else if(fCalibVersion!="0") { // same calib file for all the events
      
      char fname[256];
      //      sprintf(fname,"config/Calibration/%s",fCalibVersion.c_str());
      sprintf(fname,"config/Calibration/ECalMLEnergyCalibration_%s.dat",fCalibVersion.c_str());
      ECalMLib.open(fname);
      std::cout<<"Using a USER SELECTED energy calibration file for ECalML: "
	       <<fname<<std::endl;
      
      if(!ECalMLib.is_open()){
	std::cout<<"ERROR: Cannot find ECalML calibration file "<<"**************"<<std::endl;
	exit(1);
      }
    }
  }

  // Time offsets calibration 
  if(fUseCalibT==1) TCalib.open("config/Calibration/ECalMLTimeOffSets.txt");
  if(fUseCalibT==1 && !TCalib.is_open()){ 
    std::cout<<"ERROR: Cannot find ECalML time offset file "<<"**************"<<std::endl;
  }


  if((fUseCalibE>0 && fCalibVersion!="0") || fUseCalibT==1) ReadCalibConstant();

}



void ECalMLCalibration::ReadCalibConstant()
{
  double MIPCharge,TimeOffSet;
  int NBD,CID;
  int row,col;


  //Read Energy calibration constants if a specific file has been selected
  if(fCalibVersion!="0"){
    if(ECalMLib.is_open()){
      for(int i=0;i<616;i++){
	ECalMLib >> row >> col >> NBD >> CID >> MIPCharge;   //reads Piperno informations need cross-check
	fCalibMap[std::make_pair(NBD,CID)] = MIPCharge/(fMuonDepositedEnergy*fGlobEnScale);
	//fCalibMap[std::make_pair(row,col)] = MIPCharge/(fMuonDepositedEnergy*fGlobEnScale);
	//std::cout<<i<<" channel ID "<<CID<<" NBD "<<NBD<<" "<<fCalibMap[std::make_pair(NBD,CID)]<<std::endl;
      }
      ECalMLib.close();
    } else{ 
      std::cout << "================ WARNING!!! No ECalML energy calibration file available "<<std::endl;
    } 
  }

  //Read Time calibration constants
  if(TCalib.is_open()){
    for(int i=0;i<616;i++){
      TCalib >> row >> col >> NBD >> CID>>TimeOffSet;
      fT0Map[std::make_pair(NBD,CID)] = TimeOffSet;
      //fCalibMap[std::make_pair(row,col)] = MIPCharge/(fMuonDepositedEnergy*fGlobEnScale);
      //std::cout<<i<<" channel ID "<<CID<<" NBD "<<NBD<<" "<<fT0Map[std::make_pair(NBD,CID)]<<std::endl;
    }
    TCalib.close();
  } else{ 
    std::cout << "================ WARNING!!! No ECalML time calibration file available "<<std::endl;
  } 
  
}


 
void ECalMLCalibration::PerformCalibration(std::vector<TRecoVHit *> &Hits, TRawEvent* rawEv)
{
  static int PRINTED = 0; 
  for(unsigned int iHit = 0;iHit < Hits.size();++iHit){

    // Energy calibration //
    if (fUseCalibE > 0){

      int ich = Hits[iHit]->GetChannelId(); //need to convert into BDID e CHID
      unsigned int BD   = Hits[iHit]->getBDid(); 
      unsigned int ChID = Hits[iHit]->getCHid();

      fHitE   = Hits[iHit]->GetEnergy();

      // Correcting for different crystals response
      if(fCalibVersion=="0"){
	TTimeStamp time = rawEv->GetEventAbsTime();
	/*
	std::cout<<"Hit "<<iHit<<", time: "<<time<<" (day "<<time.GetDate()
		 <<", hour "<<time.GetTime()<<")"<<std::endl;
	*/
	fCalibHandler->SetTimeInterval(time);
	fHitECalMLibrated = fHitE*(fCalibHandler->GetCalibVal(BD,ChID))*fMuonDepositedEnergy*fGlobEnScale;
	//Hits[iHit]->SetEnergy(fHitECalMLibrated);
	//std::cout<<"channel ID "<<ChID<<" BD "<<BD<<" ich "<<ich<<" HitE "<<fHitE<<" "<<fHitECalMLibrated<<" "<<(fCalibHandler->GetCalibVal(BD,ChID))*fMuonDepositedEnergy*fGlobEnScale<<std::endl;
	
      } else { // fCalibVersion != "0"

	if(fCalibMap[std::make_pair(BD,ChID)]!=0){ 
	  fHitECalMLibrated = fHitE/fCalibMap[std::make_pair(BD,ChID)];
	  //Hits[iHit]->SetEnergy(fHitECalMLibrated);
	  //std::cout<<"channel ID "<<ChID<<" BD "<<BD<<" ich "<<ich<<" HitE "<<fHitE<<" "<<fHitECalMLibrated<<" "<<1./fCalibMap[std::make_pair(BD,ChID)]<<std::endl;
	}else{
	  if(!PRINTED) {
	    std::cout<<"Missing ECalML energy calibration for channel ID "<<ChID<<" BD "<<BD<<" ich "<<ich<<" HitE "<<fHitE<<std::endl;
	    PRINTED++;
	  }
	}
      }      
    }


    // Time calibration //    
    if (fUseCalibT == 1){
      // Correcting for time offestets in between channels
      fHitT          = Hits[iHit]->GetTime();
      fHitTCorrected = fHitT-fT0Map[std::make_pair(fBID,fChID)];
      std::cout<<" HitT "<<fHitT<<" "<<fHitTCorrected<<std::endl;
      Hits[iHit]->SetTime(fHitTCorrected);
    }
    else{
    fHitT          = Hits[iHit]->GetTime();
    fHitTCorrected = fHitT-fCommonT0;
    //std::cout<<" HitT "<<fHitT<<" "<<fHitTCorrected<<std::endl;
    Hits[iHit]->SetTime(fHitTCorrected);
    }
  } 

}
