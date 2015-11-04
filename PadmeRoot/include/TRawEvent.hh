#ifndef TRawEvent_H
#define TRawEvent_H

#include "TClonesArray.h"

#include "TADCBoard.hh"

#define TRAWEVENT_MAXADCBOARDS 50

class TRawEvent : public TObject
{

 public:

  TRawEvent();
  ~TRawEvent();

  void Clear(Option_t* = "");

 public:

  void     SetRunNumber(UInt_t v)   { fRunNumber = v; }
  UInt_t   GetRunNumber()           { return fRunNumber; }

  void     SetEventNumber(UInt_t v) { fEventNumber = v; }
  UInt_t   GetEventNumber()         { return fEventNumber; }

  UChar_t GetNADCBoards() { return fNADCBoards; }
  TADCBoard* AddADCBoard();
  TADCBoard* ADCBoard(int);

 private:

  UInt_t   fRunNumber;
  UInt_t   fEventNumber;

  UChar_t       fNADCBoards;
  TClonesArray* fADCBoards;

  ClassDef(TRawEvent,1);
};
#endif
