// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------
#ifndef TEVetoMCEvent_H
#define TEVetoMCEvent_H

#include "TMCVEvent.hh"

class TEVetoMCEvent : public TMCVEvent {

public:

  TEVetoMCEvent();
  ~TEVetoMCEvent();

private:

  ClassDef(TEVetoMCEvent,1);
};
#endif
