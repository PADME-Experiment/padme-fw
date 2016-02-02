// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------
#ifndef TECalMCEvent_H
#define TECalMCEvent_H

#include "TMCVEvent.hh"

class TECalMCEvent : public TMCVEvent {

public:

  TECalMCEvent();
  ~TECalMCEvent();

private:

  ClassDef(TECalMCEvent,1);
};
#endif
