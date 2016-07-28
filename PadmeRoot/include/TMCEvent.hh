// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (Emanuele.Leonardi@cern.ch) 2016-03-15
//
// --------------------------------------------------------------
#ifndef TMCEvent_h
#define TMCEvent_h 1

#include "TObject.h"
#include "TClonesArray.h"

#include "Riostream.h"

class TMCEvent: public TObject
{

public:

  TMCEvent();
  virtual ~TMCEvent() { Clear(); };

  Int_t GetRunNumber() { return fRunNumber; }
  void  SetRunNumber(Int_t val) { fRunNumber = val; }

  Int_t GetEventNumber()          { return fEventNumber; }
  void  SetEventNumber(Int_t val) { fEventNumber = val; }

  Double_t GetTime()             { return fTime; }
  void     SetTime(Double_t val) { fTime = val; }

  void Clear(Option_t* option ="");
  void Reset(Option_t* option ="");
  void Print(Option_t* option="") const;

  //void StoreRandomState(TRandom3* RandomDecayState, long *RanecuState);
  //TRandom3* GetRandomDecayState () {return fRandomDecayState;}
  //long* GetRanecuState() {return fRanecuState;}

private:

  Int_t fRunNumber;
  Int_t fEventNumber;
  Double_t fTime;

  //static TRandom3* fgRandomDecayState;
  //TRandom3* fRandomDecayState;
  //Long_t fRanecuState[2];

  ClassDef(TMCEvent,1)
};
#endif
