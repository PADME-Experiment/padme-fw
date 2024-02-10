#ifndef ECalMLMuons_h
#define ECalMLMuons_h

#include "PadmeVAnalyser.hh"


class ECalMLMuons: public  PadmeVAnalyser {
public:
  ECalMLMuons(){;};
  ECalMLMuons(const char *s,int valid, int verb);
  ECalMLMuons(const char *s);
  ~ECalMLMuons(){;};

  virtual Bool_t InitHistos();
  virtual Bool_t Process();  
  virtual Bool_t Finalize();


private:
  
  
};


#endif
