#ifndef NPoTAnalysis_h
#define NPoTAnalysis_h

#include "PadmeVAnalyser.hh"


class NPoTAnalysis: public  PadmeVAnalyser {
public:
  NPoTAnalysis(){;};
  NPoTAnalysis(const char *s,int valid, int verb);
  NPoTAnalysis(const char *s);
  ~NPoTAnalysis(){;};

  virtual Bool_t InitHistos();
  virtual Bool_t Process();  
  virtual Bool_t Finalize(){return true;}


private:
  
  
};


#endif
