#ifndef TRecoEvent_h
#define TRecoEvent_h 1

#include "TObject.h"
#include "TClonesArray.h"
#include "TTimeStamp.h"

#include "Riostream.h"

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

  void Clear(Option_t* option ="");
  void Reset(Option_t* option ="");
  void Print(Option_t* option="") const;

  UInt_t GetEventStatus() { return fEventStatus; }
  void SetEventStatus(UInt_t val) { fEventStatus = val; }

  UInt_t GetTriggerMask() { return fTriggerMask; }
  void SetTriggerMask(UInt_t val) { fTriggerMask = val; }

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
