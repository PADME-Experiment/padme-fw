#ifndef TRawEvent_H
#define TRawEvent_H

#include "TClonesArray.h"
#include "TTimeStamp.h"

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

  void       SetEventAbsTime(TTimeStamp t) { fEventAbsTime = t; }
  TTimeStamp GetEventAbsTime()             { return fEventAbsTime; }

  void   SetEventRunTime(UInt_t t) { fEventRunTime = t; }
  UInt_t GetEventRunTime()         { return fEventRunTime; }

  void   SetEventTrigMask(UInt_t t) { fEventTrigMask = t; }
  UInt_t GetEventTrigMask() { return fEventTrigMask; }
  void   EventTrigMaskSetBit(UChar_t b) { fEventTrigMask |= (1U << b); }
  void   EventTrigMaskUnsetBit(UChar_t b) { fEventTrigMask &= ~(1U << b); }
  Bool_t EventTrigMaskGetBit(UChar_t b) { return ((fEventTrigMask >> b) & 1U) ? true : false;}

  void   SetEventStatus(UInt_t s) { fEventStatus = s; }
  UInt_t GetEventStatus() { return fEventStatus; }
  void   EventStatusSetBit(UChar_t b) { fEventStatus |= (1U << b); }
  void   EventStatusUnsetBit(UChar_t b) { fEventStatus &= ~(1U << b); }
  Bool_t EventStatusGetBit(UChar_t b) { return ((fEventStatus >> b) & 1U) ? true : false;}

  UChar_t GetNADCBoards() { return fNADCBoards; }
  TADCBoard* AddADCBoard();
  TADCBoard* ADCBoard(int);

 private:

  UInt_t   fRunNumber;
  UInt_t   fEventNumber;

  TTimeStamp fEventAbsTime;
  UInt_t fEventRunTime;

  UInt_t fEventTrigMask;
  UInt_t fEventStatus;

  UChar_t       fNADCBoards;
  TClonesArray* fADCBoards;

  ClassDef(TRawEvent,2);
};
#endif
