// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2017-03-15
//
// --------------------------------------------------------------
#ifndef TTPixMCEvent_H
#define TTPixMCEvent_H

#include "TMCVEvent.hh"

class TTPixMCEvent : public TMCVEvent {

public:

  TTPixMCEvent();
  ~TTPixMCEvent();

private:

  ClassDef(TTPixMCEvent,1);
};
#endif
