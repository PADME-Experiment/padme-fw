// --------------------------------------------------------------
// History:
//
// Created by Elisa Di Meco (elisa.dimeco@lnf.infn.it) 2026-05-27
//
// --------------------------------------------------------------
#ifndef TTMMMCEvent_H
#define TTMMMCEvent_H

#include "TMCVEvent.hh"

class TTMMMCEvent : public TMCVEvent {

public:

  TTMMMCEvent();
  ~TTMMMCEvent();

private:

  ClassDef(TTMMMCEvent,1);
};
#endif
