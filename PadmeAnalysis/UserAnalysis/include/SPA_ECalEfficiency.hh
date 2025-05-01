#ifndef SPA_ECalEfficiency_h
#define SPA_ECalEfficiency_h

#include "PadmeVAnalyser.hh"


class SPA_ECalEfficiency: public  PadmeVAnalyser {
public:
  SPA_ECalEfficiency(){;};
  SPA_ECalEfficiency(const char *s,int valid, int verb);
  SPA_ECalEfficiency(const char *s);
  ~SPA_ECalEfficiency(){;};

  virtual Bool_t InitHistos();
  virtual Bool_t Process();  
  virtual Bool_t Finalize(){return true;}


private:
  
  
};


#endif
