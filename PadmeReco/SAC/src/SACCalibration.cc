// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "SACCalibration.hh"
#include "RecoVChannelID.hh"
#include "TRecoVHit.hh"
 
SACCalibration::SACCalibration()
  : PadmeVCalibration()
{
  std::cout<<"SACCalibration being created ............"<<std::endl ;
  
}

SACCalibration::~SACCalibration()
{;}


void SACCalibration::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr ){

  PadmeVCalibration::Init(cfg, chIdMgr );
  
  fCalibrationFile  = cfg->GetParOrDefault("EnergyCalibration", "CalibrationFile", "config/Calibration/SAC_EnergyCalibration.txt");
  std::cout <<"Calibration File Chosen "<<fCalibrationFile<<std::endl; 

  ReadCalibrationConstants();

  std::vector<int> chIds = chIdMgr->GetChannelIDVector();

  for (unsigned int j=0; j<chIds.size();++j)
	{
	  double channelGain = fCalibCh[j];
	  std::cout<<" init calib consts .... j "<<j<<" ID="<<chIds[j]<<" .... READ FROM FILE ... "<<channelGain<<" "<<std::endl;
	  

	  if (fEnergyCalibMap->find(chIds[j])!=fEnergyCalibMap->end()) 
	    {
	      fEnergyCalibMap->find(chIds[j])->second = (fEnergyCalibMap->find(chIds[j])->second)*channelGain;
	    }
	  else std::cout<<"SACCalibration::Init - WARNING - no entry found in energy calib map for id="<<chIds[j]<<std::endl;  
	}

}
void SACCalibration::ReadCalibrationConstants(){


  std::ifstream Calib;
  char fname[50];
  //Int_t Calibration=0;

  //sprintf(fname,"config/Calibration/SAC_EnergyCalibration_%d.txt", fCalibrationFile);
  //Calib.open(fname);
  Calib.open(fCalibrationFile.Data());
  if (Calib.is_open()){
  	double temp;
  	for (int i=0;i<25;i++){
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

/*
void SACCalibration::PerformCalibration(std::vector<TRecoVHit *> &Hits){
 
  double Energy = 0.;
  int ich = 0;

  for(unsigned int iHit = 0;iHit < Hits.size();iHit++){
    ich     = Hits[iHit]->GetChannelId(); 
    Energy = Hits[iHit]->GetEnergy();
    std::cout <<"Hits.size  "<<Hits.size()<<"   Channel Id  "<< ich <<"   Energy before calibration    "<<Energy<<std::endl; 
  }
  
  
 
  PadmeVCalibration::PerformCalibration(Hits);
  
  for(unsigned int iHit = 0;iHit < Hits.size();iHit++){
    ich     = Hits[iHit]->GetChannelId(); 
    Energy = Hits[iHit]->GetEnergy();
    std::cout <<"Hits.size  "<<Hits.size()<<"   Channel Id  "<< ich <<"   Energy after calibration     "<<Energy<<std::endl;  
  }
  
 
 
  
  return;

}
*/





