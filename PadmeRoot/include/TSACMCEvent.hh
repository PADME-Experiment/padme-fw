// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-17
//
// --------------------------------------------------------------
#ifndef TSACMCEvent_H
#define TSACMCEvent_H 1

#include "TMCVEvent.hh"

class TSACMCHit;

class TSACMCEvent : public TMCVEvent {

public:

  TSACMCEvent();
  ~TSACMCEvent();

  TSACMCHit* GetOrCreateHit(Int_t);

private:

  ClassDef(TSACMCEvent,1);
};
#endif
