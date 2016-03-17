// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-17
//
// --------------------------------------------------------------
#ifndef TSACMCEvent_H
#define TSACMCEvent_H

#include "TMCVEvent.hh"

class TSACMCEvent : public TMCVEvent {

public:

  TSACMCEvent();
  ~TSACMCEvent();

private:

  ClassDef(TSACMCEvent,1);
};
#endif
