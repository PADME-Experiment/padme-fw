// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#ifndef HEPVetoCalibration_H
#define HEPVetoCalibration_H

#include "PadmeVCalibration.hh"
#include <vector>


class HEPVetoCalibration : public PadmeVCalibration
{

public:
  
  HEPVetoCalibration();
  ~HEPVetoCalibration();
  

  void ReadCalibrationConstants();
  void Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr );


private:

  Double_t fCalibCh   [32];
  int fCalibrationFile;
 

};
#endif

