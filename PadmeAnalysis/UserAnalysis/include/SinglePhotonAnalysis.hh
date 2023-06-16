#ifndef SinglePhotonAnalysis_h
#define SinglePhotonAnalysis_h

#include "PadmeVAnalyser.hh"


class SinglePhotonAnalysis: public  PadmeVAnalyser {
public:
  SinglePhotonAnalysis(){;};
  SinglePhotonAnalysis(const char *s,int valid, int verb);
  SinglePhotonAnalysis(const char *s);
  ~SinglePhotonAnalysis(){;};

  virtual Bool_t InitHistos();
  virtual Bool_t Process();  
  virtual Bool_t Finalize(){return true;}


private:
  
  
};


#endif
