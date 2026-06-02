// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
// Modified by Gabriele Piperno (gabriele.piperno@roma1.infn.it) 2019-10-10
// --------------------------------------------------------------
#ifndef ECalCalibration_H
#define ECalCalibration_H

#include "PadmeVCalibration.hh"
#include "TGraph.h"
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
  std::string fCalibRunList;  //Input file with the number of runs when there's new broken channels in ecal

  Int_t     fUseCalibE;
  Int_t     fUseCalibT;

  Int_t fOverrideEScale; // if set to 1, FOR DATA will override energy scale from input file and use the values below
  Double_t  fGlobHitEnScaleOverrideData; //Used to override any scale from input file / the code itself
  Double_t  fGlobHitEnScaleData; //Value used
  Double_t  fGlobHitEnScaleMC;   //Value used for MC
  TGraph* fRunDependentScale; 

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
  std::vector<int> fRunList; // EDM keep track of what run is used for switching the calibration file --> files must have this name after the calib version f.i. Calib_8_80532
  std::ifstream ECalib; 
  std::ifstream ECalibList; 
  std::ifstream TCalib; 

  std::map < std::pair<int,int>,double> fCalibMap;
  std::map<int, std::map<std::pair<int,int>, double>> fCalibMapRun;// EDM map matching the different Run to the corresponding calibration maps
  std::map < std::pair<int,int>,double> fT0Map;
};
#endif

