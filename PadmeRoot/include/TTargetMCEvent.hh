// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------
#ifndef TTargetMCEvent_H
#define TTargetMCEvent_H

#include "TMCVEvent.hh"

class TTargetMCEvent : public TMCVEvent {

public:

  TTargetMCEvent();
  ~TTargetMCEvent();

private:

  ClassDef(TTargetMCEvent,1);
};
#endif
