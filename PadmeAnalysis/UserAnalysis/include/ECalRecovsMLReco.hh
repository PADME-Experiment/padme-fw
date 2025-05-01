#ifndef ECalRecovsMLReco_h
#define ECalRecovsMLReco_h

#include "PadmeVAnalyser.hh"


class ECalRecovsMLReco: public  PadmeVAnalyser {
public:
  ECalRecovsMLReco(){;};
  ECalRecovsMLReco(const char *s,int valid, int verb);
  ECalRecovsMLReco(const char *s);
  ~ECalRecovsMLReco(){;};

  virtual Bool_t InitHistos();
  virtual Bool_t Process();  
  virtual Bool_t Finalize(){return true;}


private:
  
  
};


#endif
