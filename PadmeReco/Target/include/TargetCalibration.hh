// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#ifndef TargetCalibration_H
#define TargetCalibration_H

#include "PadmeVCalibration.hh"
#include <vector>

class TTargetRecoBeam;

class TargetCalibration : public PadmeVCalibration
{

public:
  
  TargetCalibration();
  ~TargetCalibration();
  void InitCommonEnergyScale(PadmeVRecoConfig *cfg);
  void PerformCalibration(std::vector<TRecoVHit *> &hitArray);
  void PerformBeamCalibration(TTargetRecoBeam* b);

private:

  double fAverageGain;
  double fHVsign;
  double fCCD;
};
#endif

