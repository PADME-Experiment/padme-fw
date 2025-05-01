// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
// Modified by Gabriele Piperno (gabriele.piperno@roma1.infn.it) 2019-10-10
// --------------------------------------------------------------
#ifndef ECalMLCalibration_H
#define ECalMLCalibration_H

#include "PadmeVCalibration.hh"
#include "ECalMLCalibHandler.hh"
class TRawEvent;

#include <vector>
#include <string>


class ECalMLCalibration : public PadmeVCalibration
{

public:
  
  ECalMLCalibration();
  ~ECalMLCalibration();
  void Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr );
  void ReadCalibConstant();
  void PerformCalibration(std::vector<TRecoVHit *> &hitArray,TRawEvent* rawEv);
  int SearchCalibFile();
  void SetEventTime(TTimeStamp ET){fEvTime = ET;}
  TTimeStamp GetEventTime(){return fEvTime;}


private:

  ECalMLCalibHandler* fCalibHandler;

  TTimeStamp fEvTime;

  std::string fCalibList;

  Int_t     fUseCalibE;
  Int_t     fUseCalibT;

  Double_t  fGlobEnScale;
  Double_t  fMuonDepositedEnergy;
  Bool_t fCalibFileFound;

  double fHitE;
  double fHitECalMLibrated;

  double fHitT;
  double fHitTCorrected;

  double fCalibConst;
  std::string fCalibVersion;
  double fBID;
  double fChID;

  std::ifstream ECalMLib; 
  std::ifstream TCalib; 

  std::map < std::pair<int,int>,double> fCalibMap;
  std::map < std::pair<int,int>,double> fT0Map;
};
#endif

