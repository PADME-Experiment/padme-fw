#ifndef TRecoEvent_h
#define TRecoEvent_h 1

#include "TObject.h"
#include "TClonesArray.h"

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

  Double_t GetTime()             { return fTime; }
  void     SetTime(Double_t val) { fTime = val; }

  void Clear(Option_t* option ="");
  void Reset(Option_t* option ="");
  void Print(Option_t* option="") const;

private:

  Int_t fRunNumber;
  Int_t fEventNumber;
  Double_t fTime;

  //static TRandom3* fgRandomDecayState;
  //TRandom3* fRandomDecayState;
  //Long_t fRanecuState[2];

  ClassDef(TRecoEvent,1)
};
#endif
