#ifndef UserAnalysis_h
#define UserAnalysis_h 1

#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"

#include "PadmeAnalysisEvent.hh"

class UserAnalysis {

public:
  UserAnalysis();
  UserAnalysis(int valid, int verb){fMode = valid; fVerbosity = verb;};
  ~UserAnalysis();
  Bool_t InitHistos();
  Bool_t Init(PadmeAnalysisEvent *event);
  Bool_t Process();

  
  Bool_t Finalize(){return true;}

private:
  int fMode;
  int fVerbosity;
  PadmeAnalysisEvent *evt;
  
  
};


#endif
