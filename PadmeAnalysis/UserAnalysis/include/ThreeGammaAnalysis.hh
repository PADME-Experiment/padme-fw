#ifndef ThreeGammaAnalysis_h
#define ThreeGammaAnalysis_h

#include "PadmeVAnalyser.hh"


class ThreeGammaAnalysis: public  PadmeVAnalyser {
public:
  ThreeGammaAnalysis(){;};
  ThreeGammaAnalysis(const char *s,int valid, int verb);
  ThreeGammaAnalysis(const char *s);
  ~ThreeGammaAnalysis(){;};

  virtual Bool_t InitHistos();
  virtual Bool_t Process();  
  virtual Bool_t Finalize(){return true;}


private:
  
  
};


#endif
