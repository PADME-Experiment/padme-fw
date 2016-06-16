// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------
#ifndef TMCVEvent_H
#define TMCVEvent_H 1

#include "TClass.h"
#include "TObject.h"
#include "TClonesArray.h"

#include "TMCVHit.hh"

class TMCVEvent : public TObject
{

public:

  TMCVEvent();
  TMCVEvent(TClass*);

  virtual ~TMCVEvent();

public:

  TMCVHit* AddHit();         // Append a new (empty) hit to hits array
  TMCVHit* AddHit(TMCVHit*); // Append given hit to hits array

  TMCVHit* Hit(Int_t iHit); // Return pointer to hit at given index in hits array

  TMCVHit* LastHit(); // Return pointer to last hit in hits array

  void RemoveHit(Int_t iHit); // Remove hit at given index and compress hits array

  void Clear(Option_t* = ""); // Clear hits array (all hits are removed)

public:

  Int_t GetRunNumber()            { return fRunNumber;  };
  void  SetRunNumber(Int_t value) { fRunNumber = value; };

  Int_t GetEventNumber()            { return fEventNumber;  };
  void  SetEventNumber(Int_t value) { fEventNumber = value; };

  Int_t GetNHits()              { return fNHits;  };
  //void  SetNHits(Int_t value)   { fNHits = value; };

  //TClonesArray* GetHits()                     { return fHits;   };
  //void          SetHits(TClonesArray * value) { fHits = value;  };

protected:

  Int_t fRunNumber;
  Int_t fEventNumber;

  Int_t fNHits;
  TClonesArray* fHits;

  ClassDef(TMCVEvent,1);
};
#endif
