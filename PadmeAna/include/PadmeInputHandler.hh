#ifndef PadmeInputHandler_H
#define PadmeInputHandler_H


//Define all possible inputs
#include "PadmeAnaInMCHitsEvent.hh"


#include <string>
#include <iostream>


class PadmeInputHandler{


public:
  PadmeInputHandler(){;};
  PadmeInputHandler(std::string fname);
  ~PadmeInputHandler(){;};

  PadmeVAnaInEvent *GetEventHandler(){return fInEvent};

  int isROOTFile(std::string fname);

protected:
  PadmeVAnaInEvent *fInEvent;

};
#endif
