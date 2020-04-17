#ifndef PadmeVAnalyser_h
#define PadmeVAnalyser_h 1

#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include <string>
#include <map>

#include "PadmeAnalysisEvent.hh"


class PadmeVAnalyser {

public:
  PadmeVAnalyser();
  PadmeVAnalyser(const char *s,int valid, int verb);
  PadmeVAnalyser(const char *s);

  ~PadmeVAnalyser();
  virtual Bool_t InitHistos();
  virtual Bool_t Init(PadmeAnalysisEvent *event);
  virtual Bool_t Init(PadmeAnalysisEvent *event, int valid, int verb);
  virtual Bool_t Process();  
  virtual Bool_t Finalize(){return true;}
  void SetName(const char *s) {fName = s;};
  std::string GetName(){return fName;};
  Bool_t GetResult(){return fResult;};
  Bool_t IsUsed(){return fUsed;};
  void SetUsed(){fUsed = true;};
  
  static std::map<std::string, PadmeVAnalyser *>&  GetAnalysersMap(){return fAnalysers;};  
  //The setting of the result should be done only inside the Analyser!  
  static std::map<std::string, PadmeVAnalyser *> fAnalysers;


private:
  int fMode;
  int fVerbosity;
  Bool_t fUsed;
  PadmeAnalysisEvent *evt;
  std::string fName;    

protected:
  Bool_t fResult;

};

#endif
