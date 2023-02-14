// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2022-06-16
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "ETagCalibration.hh"
#include "RecoVChannelID.hh"
#include "TRecoVHit.hh"
 
ETagCalibration::ETagCalibration()
  : PadmeVCalibration()
{
  std::cout<<"ETagCalibration being created ............"<<std::endl ;
}

ETagCalibration::~ETagCalibration()
{;}

void ETagCalibration::Init(PadmeVRecoConfig* cfg, RecoVChannelID* chIdMgr )
{

  PadmeVCalibration::Init(cfg, chIdMgr );
  
  fCalibrationFile  = (int)cfg->GetParOrDefault("EnergyCalibration", "CalibrationFile", 1); 
  std::cout <<"ETag Calibration File Chosen "<<fCalibrationFile<<std::endl; 

  ReadCalibrationConstants();

  std::vector<int> chIds = chIdMgr->GetChannelIDVector();

  for (unsigned int j=0; j<chIds.size();++j) {
    int chId = chIds[j];
    double channelGain = fCalibCh[chId];
    std::cout<<"ETagCalibration::Init calib consts .... j "<<j<<" ID="<<chIds[j]<<" .... READ FROM FILE ... "<<channelGain<<" "<<std::endl;
    if (fEnergyCalibMap->find(chIds[j])!=fEnergyCalibMap->end())
      fEnergyCalibMap->find(chIds[j])->second = (fEnergyCalibMap->find(chIds[j])->second)*channelGain;
    else
      std::cout<<"ETagCalibration::Init - WARNING - no entry found in energy calib map for id="<<chIds[j]<<std::endl;  
  }

}

void ETagCalibration::ReadCalibrationConstants()
{

  std::ifstream Calib;
  char fname[50];
  //Int_t Calibration=0;

  sprintf(fname,"config/Calibration/ETag_EnergyCalibration_%d.txt", fCalibrationFile);
  Calib.open(fname);
  if (Calib.is_open()) {
    double temp;
    for (int i=0; i<ETAG_NUMBER_OF_CHANNELS; i++) {
      Calib >> temp >> fCalibCh[i];
      //std::cout <<"FileRow  "<< i<<" ETag Calibration Constant "<<fCalibCh[i]<<std::endl; 
    }
    Calib.close();
  } else { 
    std::cout<<"No previous data available, resorting to default calibration constant (1)"<<std::endl;
  }

}
