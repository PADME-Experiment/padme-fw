#ifndef ECalMLCalibration_h
#define ECalMLCalibration_h

#include "PadmeVAnalyser.hh"


class ECalMLCalibration: public  PadmeVAnalyser {
public:
  ECalMLCalibration(){;};
  ECalMLCalibration(const char *s,int valid, int verb);
  ECalMLCalibration(const char *s);
  ~ECalMLCalibration(){;};

  virtual Bool_t InitHistos();
  virtual Bool_t Process();  
  virtual Bool_t Finalize(){return true;}


private:
  
  
};


#endif
