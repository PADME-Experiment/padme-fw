// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#ifndef ECalCalibration_H
#define ECalCalibration_H

#include "PadmeVCalibration.hh"
#include <vector>


class ECalCalibration : public PadmeVCalibration
{

public:
  
  ECalCalibration();
  ~ECalCalibration();
  void Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr );
  void ReadCalibConstant();
  void PerformCalibration(std::vector<TRecoVHit *> &hitArray);

private:
  Int_t     fUseCalib;
  Double_t  fGlobEnScale;
  Double_t  fMuonDepositedEnergy;


  double fHitE;
  double fHitECalibrated;

  double fHitT;
  double fHitTCorrected;

  double fCalibConst;
  double fBID;
  double fChID;

  std::map < std::pair<int,int>,double> fCalibMap;
  std::map < std::pair<int,int>,double> fT0Map;
};
#endif

