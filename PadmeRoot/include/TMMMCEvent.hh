// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------
#ifndef TMMMCEvent_H
#define TMMMCEvent_H

#include "TMCVEvent.hh"

class TMMMCEvent : public TMCVEvent {

public:

  TMMMCEvent();
  ~TMMMCEvent();

private:

  ClassDef(TMMMCEvent,1);
};
#endif
