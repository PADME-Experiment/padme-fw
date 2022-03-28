#ifndef FirstDataLook_h
#define FirstDataLook_h

#include "PadmeVAnalyser.hh"


class FirstDataLook: public  PadmeVAnalyser {
public:
  FirstDataLook(){;};
  FirstDataLook(const char *s,int valid, int verb);
  FirstDataLook(const char *s);
  ~FirstDataLook(){;};

  virtual Bool_t InitHistos();
  virtual Bool_t Process();  
  virtual Bool_t Finalize(){return true;}


private:
  
  
};


#endif
