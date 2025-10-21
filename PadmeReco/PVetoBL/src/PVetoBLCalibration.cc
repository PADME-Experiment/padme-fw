// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "PVetoBLCalibration.hh"
#include "RecoVChannelID.hh"
#include "TRecoVHit.hh"
 
PVetoBLCalibration::PVetoBLCalibration()
  : PadmeVCalibration()
{
  std::cout<<"PVetoBLCalibration being created ............"<<std::endl ;
 
}

PVetoBLCalibration::~PVetoBLCalibration()
{;}




void PVetoBLCalibration::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr ){

  PadmeVCalibration::Init(cfg, chIdMgr );
  
  fCalibrationFile  = (int)cfg->GetParOrDefault("EnergyCalibration", "CalibrationFile", 1); 
  std::cout <<"PVetoBL Calibration File Chosen "<<fCalibrationFile<<std::endl; 

  ReadCalibrationConstants();

  std::vector<int> chIds = chIdMgr->GetChannelIDVector();

  for (unsigned int j=0; j<chIds.size();++j)
	{
          int chId = chIds[j];
	  double channelGain = fCalibCh[chId];
	  std::cout<<"PVetoBLCalibration::Init calib consts .... j "<<j<<" ID="<<chIds[j]<<" .... READ FROM FILE ... "<<channelGain<<" "<<std::endl;
	  

	  if (fEnergyCalibMap->find(chIds[j])!=fEnergyCalibMap->end()) 
	    {
	      fEnergyCalibMap->find(chIds[j])->second = (fEnergyCalibMap->find(chIds[j])->second)*channelGain;
	    }
	  else std::cout<<"PVetoBLCalibration::Init - WARNING - no entry found in energy calib map for id="<<chIds[j]<<std::endl;  
	}

}
void PVetoBLCalibration::ReadCalibrationConstants(){


  std::ifstream Calib;
  char fname[100];
  //Int_t Calibration=0;

  sprintf(fname,"config/Calibration/PVetoBL_EnergyCalibration_%d.txt", fCalibrationFile);
  Calib.open(fname);
  if (Calib.is_open()){
  	double temp;
  	for (int i=0;i<96;i++){
          Calib >> temp >> fCalibCh[i];
          //std::cout <<"FileRow  "<< i<<" PVetoBL Calibration Constant "<<fCalibCh[i]<<std::endl; 
	}
  Calib.close();
  }
  else{ 
  	std::cout<<"No previous data available, resorting to default calibration constant (1)"<<std::endl;
  }

  return;

}

