// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2022-06-16
//
// --------------------------------------------------------------
#ifndef ETagCalibration_H
#define ETagCalibration_H

#include "ETagGeometry.hh"
#include "PadmeVCalibration.hh"
#include <vector>

class ETagCalibration : public PadmeVCalibration
{

public:
  
  ETagCalibration();
  ~ETagCalibration();
  
  void ReadCalibrationConstants();
  void Init(PadmeVRecoConfig*, RecoVChannelID*);

private:

  Double_t fCalibCh[ETAG_NUMBER_OF_CHANNELS];
  int fCalibrationFile;
  
};
#endif
