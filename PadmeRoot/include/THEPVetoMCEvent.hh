// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------
#ifndef THEPVetoMCEvent_H
#define THEPVetoMCEvent_H

#include "TMCVEvent.hh"

class THEPVetoMCEvent : public TMCVEvent {

public:

  THEPVetoMCEvent();
  ~THEPVetoMCEvent();

private:

  ClassDef(THEPVetoMCEvent,1);
};
#endif
