#ifndef UserTemplateAnalyser_h
#define UserTemplateAnalyser_h

#include "PadmeVAnalyser.hh"


class UserTemplateAnalyser: public  PadmeVAnalyser {
public:
  UserTemplateAnalyser(){;};
  UserTemplateAnalyser(const char *s,int valid, int verb);
  UserTemplateAnalyser(const char *s);
  ~UserTemplateAnalyser(){;};

  virtual Bool_t InitHistos();
  virtual Bool_t Process();  
  virtual Bool_t Finalize(){return true;}


private:
  
  
};


#endif
