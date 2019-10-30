// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "PVetoCalibration.hh"
#include "RecoVChannelID.hh"
#include "TRecoVHit.hh"
 
PVetoCalibration::PVetoCalibration()
  : PadmeVCalibration()
{
  std::cout<<"PVetoCalibration being created ............"<<std::endl ;
 
}

PVetoCalibration::~PVetoCalibration()
{;}




void PVetoCalibration::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr ){

  PadmeVCalibration::Init(cfg, chIdMgr );
  
  fCalibrationFile  = (int)cfg->GetParOrDefault("EnergyCalibration", "CalibrationFile", 1); 
  std::cout <<"PVeto Calibration File Chosen "<<fCalibrationFile<<std::endl; 

  ReadCalibrationConstants();

  std::vector<int> chIds = chIdMgr->GetChannelIDVector();

  for (unsigned int j=0; j<chIds.size();++j)
	{
          int chId = chIds[j];
	  double channelGain = fCalibCh[chId];
	  std::cout<<"PVetoCalibration::Init calib consts .... j "<<j<<" ID="<<chIds[j]<<" .... READ FROM FILE ... "<<channelGain<<" "<<std::endl;
	  

	  if (fEnergyCalibMap->find(chIds[j])!=fEnergyCalibMap->end()) 
	    {
	      fEnergyCalibMap->find(chIds[j])->second = (fEnergyCalibMap->find(chIds[j])->second)*channelGain;
	    }
	  else std::cout<<"PVetoCalibration::Init - WARNING - no entry found in energy calib map for id="<<chIds[j]<<std::endl;  
	}

}
void PVetoCalibration::ReadCalibrationConstants(){


  std::ifstream Calib;
  char fname[50];
  //Int_t Calibration=0;

  sprintf(fname,"config/Calibration/PVeto_EnergyCalibration_%d.txt", fCalibrationFile);
  Calib.open(fname);
  if (Calib.is_open()){
  	double temp;
  	for (int i=0;i<96;i++){
          Calib >> temp >> fCalibCh[i];
          //std::cout <<"FileRow  "<< i<<" PVeto Calibration Constant "<<fCalibCh[i]<<std::endl; 
	}
  Calib.close();
  }
  else{ 
  	std::cout<<"No previous data available, resorting to default calibration constant (1)"<<std::endl;
  }

  return;

}

