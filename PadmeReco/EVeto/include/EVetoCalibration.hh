// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#ifndef EVetoCalibration_H
#define EVetoCalibration_H

#include "PadmeVCalibration.hh"
#include <vector>


class EVetoCalibration : public PadmeVCalibration
{

public:
  
  EVetoCalibration();
  ~EVetoCalibration();
  

  void ReadCalibrationConstants();
  void Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr );


private:

  Double_t fCalibCh   [96];
  int fCalibrationFile;

};
#endif

