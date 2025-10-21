// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#ifndef PVetoBLCalibration_H
#define PVetoBLCalibration_H

#include "PadmeVCalibration.hh"
#include <vector>


class PVetoBLCalibration : public PadmeVCalibration
{

public:
  
  PVetoBLCalibration();
  ~PVetoBLCalibration();
  
  void ReadCalibrationConstants();
  void Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr );

private:

  Double_t fCalibCh   [96];
  int fCalibrationFile;
  
};
#endif

