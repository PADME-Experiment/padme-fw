// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (Emanuele.Leonardi@cern.ch) 2016-03-15
//
// --------------------------------------------------------------
#ifndef TPadmeEvent_h
#define TPadmeEvent_h 1

#include "TObject.h"
#include "TClonesArray.h"

//#include "KinePart.hh"
//#include "TRandom3.h"
#include "Riostream.h"

class TPadmeEvent: public TObject
{

public:

  TPadmeEvent();
  virtual ~TPadmeEvent() { Clear(); };

  Int_t GetRunNumber() { return fRunNumber; }
  void  SetRunNumber(Int_t val) { fRunNumber = val; }

  Int_t GetEventNumber()          { return fEventNumber; }
  void  SetEventNumber(Int_t val) { fEventNumber = val; }

  Double_t GetTime()             { return fTime; }
  void     SetTime(Double_t val) { fTime = val; }

  void Clear(Option_t* option ="");
  void Reset(Option_t* option ="");
  void Print(Option_t* option="") const;
  //void PrintAll() const;

  //Int_t GetNPurgedKineParts() { return fNPurgedKineParts; };

  //Int_t         GetNKineParts() const { return fNKineParts; }
  //TClonesArray* GetKineParts()  const { return fKineParts; }
  //KinePart*     AddKinePart();
  //void          EraseKineParts();

  //Int_t         GetNGeneParts() const { return fNGeneParts; }
  //TClonesArray* GetGeneParts()  const { return fGeneParts; }
  //GenePart*     AddGenePart();
  //void          EraseGeneParts();

  //void StoreRandomState(TRandom3* RandomDecayState, long *RanecuState);
  //TRandom3* GetRandomDecayState () {return fRandomDecayState;}
  //long* GetRanecuState() {return fRanecuState;}

private:

  Int_t fRunNumber;
  Int_t fEventNumber;
  Double_t fTime;

  //Int_t fNGeneParts;        // Number of generated particles
  //Int_t fNKineParts;        // Number of saved particles
  //Int_t fNPurgedKineParts;  // Number of removed KineParts

  //TClonesArray* fKineParts; // Array of saved particles
  //TClonesArray* fGeneParts; // Array of generated particles in K rest frame

  //static TRandom3* fgRandomDecayState;
  //TRandom3* fRandomDecayState;
  //Long_t fRanecuState[2];

  ClassDef(TPadmeEvent,1)
};
#endif
