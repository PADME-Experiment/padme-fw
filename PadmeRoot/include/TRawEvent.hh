#ifndef TRawEvent_H
#define TRawEvent_H

#include "TClonesArray.h"
#include "TTimeStamp.h"

#include "TADCBoard.hh"
#include "TTriggerInfo.hh"

#define TRAWEVENT_MAXADCBOARDS 50

#define TRAWEVENT_STATUSBIT_COMPLETE   0
#define TRAWEVENT_STATUSBIT_AUTOPASS   1

class TRawEvent : public TObject
{

 public:

  TRawEvent();
  ~TRawEvent();

  void Clear(Option_t* = "");

 public:

  void  SetRunNumber(Int_t v) { fRunNumber = v; }
  Int_t GetRunNumber() { return fRunNumber; }

  void SetEventNumber(UInt_t v) { fEventNumber = v; }
  UInt_t GetEventNumber() { return fEventNumber; }

  void SetEventAbsTime(TTimeStamp t) { fEventAbsTime = t; }
  TTimeStamp GetEventAbsTime() { return fEventAbsTime; }

  void SetEventRunTime(ULong64_t t) { fEventRunTime = t; }
  ULong64_t GetEventRunTime() { return fEventRunTime; }

  void SetEventTrigMask(UInt_t t) { fEventTrigMask = t; }
  UInt_t GetEventTrigMask() { return fEventTrigMask; }
  void EventTrigMaskSetBit(UChar_t b) { fEventTrigMask |= (1U << b); }
  void EventTrigMaskUnsetBit(UChar_t b) { fEventTrigMask &= ~(1U << b); }
  Bool_t EventTrigMaskGetBit(UChar_t b) { return ((fEventTrigMask >> b) & 1U) ? true : false;}

  void SetEventStatus(UInt_t s) { fEventStatus = s; }
  UInt_t GetEventStatus() { return fEventStatus; }
  void EventStatusSetBit(UChar_t b) { fEventStatus |= (1U << b); }
  void EventStatusUnsetBit(UChar_t b) { fEventStatus &= ~(1U << b); }
  Bool_t EventStatusGetBit(UChar_t b) { return ((fEventStatus >> b) & 1U) ? true : false;}

  void SetMissingADCBoards(UInt_t m) { fMissingADCBoards = m; }
  UInt_t GetMissingADCBoards() { return fMissingADCBoards; }
  void MissingADCBoardsSetBit(UChar_t b) { fMissingADCBoards |= (1U << b); }
  void MissingADCBoardsUnsetBit(UChar_t b) { fMissingADCBoards &= ~(1U << b); }
  Bool_t MissingADCBoardsGetBit(UChar_t b) { return ((fMissingADCBoards >> b) & 1U) ? true : false;}

  TTriggerInfo* TriggerInfo() { return fTriggerInfo; }

  UChar_t GetNADCBoards() { return fNADCBoards; }
  TADCBoard* AddADCBoard();
  TADCBoard* ADCBoard(Int_t);

 private:

  Int_t fRunNumber;
  UInt_t fEventNumber;

  TTimeStamp fEventAbsTime;
  //UInt_t fEventRunTime;
  ULong64_t fEventRunTime;

  UInt_t fEventTrigMask;
  UInt_t fEventStatus;
  UInt_t fMissingADCBoards;

  TTriggerInfo* fTriggerInfo;

  UChar_t fNADCBoards;
  TClonesArray* fADCBoards;

  ClassDef(TRawEvent,3);
};
#endif
