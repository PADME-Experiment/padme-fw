// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (emanuele.leonardi@roma1.infn.it) 2024-06-04
//
// --------------------------------------------------------------
#ifndef TMMegaMCEvent_H
#define TMMegaMCEvent_H

#include "TMCVEvent.hh"

class TMMegaMCEvent : public TMCVEvent {

public:

  TMMegaMCEvent();
  ~TMMegaMCEvent();

private:

  ClassDef(TMMegaMCEvent,1);
};
#endif