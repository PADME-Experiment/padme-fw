// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-15
//
// --------------------------------------------------------------
#ifndef TPadmeRun_h
#define TPadmeRun_h 1

#include "TObject.h"
#include "TString.h"

#include "TDetectorInfo.hh"

class TPadmeRun: public TObject
{

public:

  TPadmeRun();
  virtual ~TPadmeRun() { Clear(); };
  void Clear(Option_t* option ="");
  void Print(Option_t* option="") const;

  TDetectorInfo* GetDetectorInfo()                    { return &fDetInfo; }
  void           SetDetectorInfo(TDetectorInfo value) { fDetInfo = value; }

  Int_t GetRunNumber()            { return fRunNumber;  }
  void  SetRunNumber(Int_t value) { fRunNumber = value; }

  Int_t GetRunType()            { return fRunType;  }
  void  SetRunType(Int_t value) { fRunType = value; }

  Int_t GetTimeStart()            { return fTimeStart;  }
  void  SetTimeStart(Int_t value) { fTimeStart = value; }

  Int_t GetTimeStop()            { return fTimeStop;  }
  void  SetTimeStop(Int_t value) { fTimeStop = value; }

  Int_t GetNEvents()            { return fNEvents;  }
  void  SetNEvents(Int_t value) { fNEvents = value; }
  void  IncNEvents()            { fNEvents++; }

private:

  TDetectorInfo fDetInfo;

  Int_t fRunNumber;
  Int_t fRunType;
  Int_t fTimeStart;
  Int_t fTimeStop;
  Int_t fNEvents;

  ClassDef(TPadmeRun,1)
};
#endif
