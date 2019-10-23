// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#ifndef PVetoCalibration_H
#define PVetoCalibration_H

#include "PadmeVCalibration.hh"
#include <vector>


class PVetoCalibration : public PadmeVCalibration
{

public:
  
  PVetoCalibration();
  ~PVetoCalibration();
  
  void ReadCalibrationConstants();
  void Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr );


private:

  Double_t fCalibCh   [96];
  int fCalibrationFile;


};
#endif

