// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------
#ifndef TETagMCEvent_H
#define TETagMCEvent_H

#include "TMCVEvent.hh"

class TETagMCEvent : public TMCVEvent {

public:

  TETagMCEvent();
  ~TETagMCEvent();

private:

  ClassDef(TETagMCEvent,1);
};
#endif
