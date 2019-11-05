#ifndef GlobalTimeAnalysis_h
#define GlobalTimeAnalysis_h 1

#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"

class PadmeAnalysisEvent;
class TString;

class GlobalTimeAnalysis {

public:
  GlobalTimeAnalysis();
  GlobalTimeAnalysis(int valid, int verb){fMode = valid; fVerbosity = verb;};
  ~GlobalTimeAnalysis();
  Bool_t InitHistos();
  Bool_t Init(PadmeAnalysisEvent *event);
  Bool_t Process();

  void ApplyTimeCalibPVeto  ();
  void ApplyTimeCalibEVeto  ();
  void ApplyTimeCalibHEPVeto();
  void ApplyTimeCalibSAC    ();
  void ApplyTimeCalibECal   ();
  Bool_t ApplyTimeCalibrationForData  ();

  
  Bool_t Finalize(){return true;}

private:
  TString fName;
  int fMode;
  int fVerbosity;
  PadmeAnalysisEvent *evt;
  
  Bool_t fMC;
  double ftimeOffsetPVeto[96];
  double ftimeOffsetEVeto[96];
  double ftimeOffsetHEPVeto[32];
  double ftimeOffsetECal[3000];
  double ftimeOffsetSAC[50];
  
};


#endif
