#ifndef TMMInfo_H
#define TMMInfo_H

#include "TObject.h"

class TMMInfo : public TObject
{

 public:

  TMMInfo();
  ~TMMInfo();

 public:

  void SetdaqTimeSec(Int_t v) { fdaqTimeSec = v; }
  Int_t GetdaqTimeSec() { return fdaqTimeSec; }

  void SetdaqTimeMicroSec(Int_t v) { fdaqTimeMicroSec = v; }
  Int_t GetdaqTimeMicroSec() { return fdaqTimeMicroSec; }

  void SetsrsTimeStamp(Int_t v) { fsrsTimeStamp = v; }
  Int_t GetsrsTimeStamp() { return fsrsTimeStamp; }

  void SetsrsTrigger(UInt_t v) { fsrsTrigger = v; }
  UInt_t GetsrsTrigger() { return fsrsTrigger; }

 private:

  Int_t  fdaqTimeSec;
  Int_t  fdaqTimeMicroSec;
  Int_t  fsrsTimeStamp;
  UInt_t fsrsTrigger;


  ClassDef(TMMInfo,1);
};
#endif
