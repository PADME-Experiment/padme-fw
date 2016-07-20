// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------
#ifndef TPVetoMCEvent_H
#define TPVetoMCEvent_H

#include "TMCVEvent.hh"

class TPVetoMCEvent : public TMCVEvent {

public:

  TPVetoMCEvent();
  ~TPVetoMCEvent();

private:

  ClassDef(TPVetoMCEvent,1);
};
#endif
