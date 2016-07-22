#ifndef PadmeVAnaInEvent_H
#define PadmeVAnaInEvent_H


#include "PadmeVAnaEvent.hh"


class PadmeVAnaInEvent:public PadmeVAnaEvent {

public:
  PadmeVAnaInEvent(){;};
  ~PadmeVAnaInEvent(){;};

  virtual int NextEvent(){return 0;};
  
private:
  int mcType;
  int dataType;
  
  
};

#endif
