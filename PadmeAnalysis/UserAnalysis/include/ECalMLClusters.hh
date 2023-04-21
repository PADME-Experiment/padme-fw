#ifndef ECalMLClusters_h
#define ECalMLClusters_h

#include "PadmeVAnalyser.hh"

class ECalMLClusters: public  PadmeVAnalyser {
public:
  ECalMLClusters(){;};
  ECalMLClusters(const char *s,int valid, int verb);
  ECalMLClusters(const char *s);
  ~ECalMLClusters(){;};

  virtual Bool_t InitHistos();
  virtual Bool_t Process();  
  virtual Bool_t Finalize(){return true;}


private:
  
  
};


#endif
