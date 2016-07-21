// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
// Modified by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-21
//   - added MCDigi structure
//
// --------------------------------------------------------------
#ifndef TMCVEvent_H
#define TMCVEvent_H 1

#include "TClass.h"
#include "TObject.h"
#include "TClonesArray.h"

#include "TMCVHit.hh"
#include "TMCVDigi.hh"

class TMCVEvent : public TObject
{

public:

  TMCVEvent();
  TMCVEvent(TClass*,TClass*); // MCHit class, MCDigi class

  virtual ~TMCVEvent();

public:

  TMCVHit* AddHit();         // Append a new (empty) hit to hits array
  TMCVHit* AddHit(TMCVHit*); // Append given hit to hits array
  TMCVHit* Hit(Int_t);       // Return pointer to hit at given index in hits array
  TMCVHit* LastHit();        // Return pointer to last hit in hits array
  void RemoveHit(Int_t);     // Remove hit at given index and compress hits array

  TMCVDigi* AddDigi();          // Append a new (empty) digi to digi array
  TMCVDigi* AddDigi(TMCVDigi*); // Append given digi to digi array
  TMCVDigi* Digi(Int_t);        // Return pointer to digi at given index in digi array
  TMCVDigi* LastDigi();         // Return pointer to last digi in digi array
  void RemoveDigi(Int_t);       // Remove digi at given index and compress digi array

  void Clear(Option_t* = ""); // Clear hits/digi arrays (all hits/digi are removed)

public:

  Int_t GetRunNumber()            { return fRunNumber;  };
  void  SetRunNumber(Int_t value) { fRunNumber = value; };

  Int_t GetEventNumber()            { return fEventNumber;  };
  void  SetEventNumber(Int_t value) { fEventNumber = value; };

  Int_t GetNHits() { return fNHits; };
  Int_t GetNDigi() { return fNDigi; };

protected:

  Int_t fRunNumber;
  Int_t fEventNumber;

  Int_t fNHits;
  TClonesArray* fHits;

  Int_t fNDigi;
  TClonesArray* fDigi;

  ClassDef(TMCVEvent,2);
};
#endif
