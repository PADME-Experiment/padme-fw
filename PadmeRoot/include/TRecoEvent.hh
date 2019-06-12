#ifndef TRecoEvent_h
#define TRecoEvent_h 1

#include "TObject.h"
#include "TClonesArray.h"
#include "TTimeStamp.h"

#include "Riostream.h"

#define TRECOEVENT_STATUSBIT_COMPLETE   0
#define TRECOEVENT_STATUSBIT_AUTOPASS   1
#define TRECOEVENT_STATUSBIT_SIMULATED  2
#define TRECOEVENT_STATUSBIT_BEAMON     3

#define TRECOEVENT_TRIGMASKBIT_BEAM          0
#define TRECOEVENT_TRIGMASKBIT_COSMICS       1
#define TRECOEVENT_TRIGMASKBIT_DUALTIMER     3
#define TRECOEVENT_TRIGMASKBIT_UNCORRELATED  6
#define TRECOEVENT_TRIGMASKBIT_CORRELATED    7

class TRecoEvent: public TObject
{

public:

  TRecoEvent();
  virtual ~TRecoEvent() { Clear(); };

  Int_t GetRunNumber() { return fRunNumber; }
  void  SetRunNumber(Int_t val) { fRunNumber = val; }

  Int_t GetEventNumber()          { return fEventNumber; }
  void  SetEventNumber(Int_t val) { fEventNumber = val; }

  // Obsolete: will be removed
  Double_t GetTime()             { return fTime; }
  void     SetTime(Double_t val) { fTime = val; }

  TTimeStamp GetRecoTime() { return fRecoTime; }
  void SetRecoTime(TTimeStamp t) { fRecoTime = t; }

  TTimeStamp GetEventTime() { return fEventTime; }
  void SetEventTime(TTimeStamp t) { fEventTime = t; }

  ULong64_t GetRunClock() { return fRunClock; }
  void SetRunClock(ULong64_t t) { fRunClock = t; }

  UInt_t GetEventStatus() { return fEventStatus; }
  void SetEventStatus(UInt_t val) { fEventStatus = val; }
  Bool_t GetEventStatusBit(UChar_t b) { return ((fEventStatus >> b) & 1U) ? true : false; }
  void EventStatusSetBit(UChar_t b) { fEventStatus |= (1U << b); }
  void EventStatusUnsetBit(UChar_t b) { fEventStatus &= ~(1U << b); }

  UInt_t GetTriggerMask() { return fTriggerMask; }
  void SetTriggerMask(UInt_t val) { fTriggerMask = val; }
  Bool_t GetTriggerMaskBit(UChar_t b) { return ((fTriggerMask >> b) & 1U) ? true : false; }
  void TriggerMaskSetBit(UChar_t b) { fTriggerMask |= (1U << b); }
  void TriggerMaskUnsetBit(UChar_t b) { fTriggerMask &= ~(1U << b); }

  void Clear(Option_t* option ="");
  void Reset(Option_t* option ="");
  void Print(Option_t* option="") const;

private:

  Int_t fRunNumber;
  Int_t fEventNumber;
  Double_t fTime; // Obsolete: will be removed
  TTimeStamp fRecoTime;
  TTimeStamp fEventTime;
  ULong64_t fRunClock;
  UInt_t fEventStatus;
  UInt_t fTriggerMask;

  //static TRandom3* fgRandomDecayState;
  //TRandom3* fRandomDecayState;
  //Long_t fRanecuState[2];

  ClassDef(TRecoEvent,1)
};
#endif
