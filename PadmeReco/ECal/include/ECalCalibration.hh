// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
// Modified by Gabriele Piperno (gabriele.piperno@roma1.infn.it) 2019-10-10
// --------------------------------------------------------------
#ifndef ECalCalibration_H
#define ECalCalibration_H

#include "PadmeVCalibration.hh"
#include "ECalCalibHandler.hh"
class TRawEvent;

#include <vector>
#include <string>


class ECalCalibration : public PadmeVCalibration
{

public:
  
  ECalCalibration();
  ~ECalCalibration();
  void Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr );
  void ReadCalibConstant();
  void PerformMCCalibration(std::vector<TRecoVHit *> &hitArray);
  void PerformCalibration(std::vector<TRecoVHit *> &hitArray,TRawEvent* rawEv);
  int SearchCalibFile();
  void SetEventTime(TTimeStamp ET){fEvTime = ET;}
  TTimeStamp GetEventTime(){return fEvTime;}


private:

  ECalCalibHandler* fCalibHandler;

  TTimeStamp fEvTime;

  std::string fCalibList;

  Int_t     fUseCalibE;
  Int_t     fUseCalibT;

  Double_t  fGlobHitEnScaleMC;
  Double_t  fGlobHitEnScaleData;

  Double_t  fGlobEnScale;
  Double_t  fMuonDepositedEnergy;
  Bool_t fCalibFileFound;

  double fHitE;
  double fHitECalibrated;

  double fHitT;
  double fHitTCorrected;

  double fCalibConst;
  std::string fCalibVersion;
  double fBID;
  double fChID;

  std::ifstream ECalib; 
  std::ifstream TCalib; 

  std::map < std::pair<int,int>,double> fCalibMap;
  std::map < std::pair<int,int>,double> fT0Map;
};
#endif

