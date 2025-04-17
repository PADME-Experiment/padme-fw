// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2023-09-21
//
// --------------------------------------------------------------
#ifndef TLeadGlassMCEvent_H
#define TLeadGlassMCEvent_H

#include "TMCVEvent.hh"

class TLeadGlassMCEvent : public TMCVEvent {

public:

  TLeadGlassMCEvent();
  ~TLeadGlassMCEvent();

private:

  ClassDef(TLeadGlassMCEvent,1);
};
#endif
