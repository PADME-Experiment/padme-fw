// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "HEPVetoCalibration.hh"
#include "RecoVChannelID.hh"
#include "TRecoVHit.hh"
 
HEPVetoCalibration::HEPVetoCalibration()
  : PadmeVCalibration()
{
  std::cout<<"HEPVetoCalibration being created ............"<<std::endl ;
 
}

HEPVetoCalibration::~HEPVetoCalibration()
{;}


void HEPVetoCalibration::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr ){

  PadmeVCalibration::Init(cfg, chIdMgr );
  
  fCalibrationFile  = (int)cfg->GetParOrDefault("EnergyCalibration", "CalibrationFile", 0); 
  std::cout <<"HEP Veto Calibration File Chosen "<<fCalibrationFile<<std::endl; 

  ReadCalibrationConstants();

  std::vector<int> chIds = chIdMgr->GetChannelIDVector();

  for (unsigned int j=0; j<chIds.size();++j)
	{
   
          int chId = chIds[j];
	  double channelGain = fCalibCh[chId];
          
	  //std::cout<<" init calib consts .... j "<<j<<" ID="<<chIds[j]<<" .... READ FROM FILE ... "<<channelGain<<" "<<std::endl;
          std::cout<<" HEPVetoCalibration::Init calib consts .... j "<<j<<" ID="<<chIds[j]<<" .... READ FROM FILE ... "<<channelGain<<" "<<std::endl;
	  

	  if (fEnergyCalibMap->find(chIds[j])!=fEnergyCalibMap->end()) 
	    {
	      fEnergyCalibMap->find(chIds[j])->second = (fEnergyCalibMap->find(chIds[j])->second)*channelGain;
	    }
	  else std::cout<<"HEPVetoCalibration::Init - WARNING - no entry found in energy calib map for id="<<chIds[j]<<std::endl;  
	}

}
void HEPVetoCalibration::ReadCalibrationConstants(){


  std::ifstream Calib;
  char fname[50];
  //Int_t Calibration=0;

  sprintf(fname,"config/Calibration/HEPVeto_EnergyCalibration_%d.txt", fCalibrationFile);
  Calib.open(fname);
  if (Calib.is_open()){
  	double temp;
  	for (int i=0;i<32;i++){
          Calib >> temp >> fCalibCh[i];
          std::cout <<"FileRow  "<< i<<" Calibration Constant "<<fCalibCh[i]<<std::endl; 
	}
  Calib.close();
  }
  else{ 
  	std::cout<<"No previous data available, resorting to default calibration constant (1)"<<std::endl;
  }

  return;

}

