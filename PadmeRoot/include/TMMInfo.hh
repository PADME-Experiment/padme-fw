#ifndef TMMInfo_H
#define TMMInfo_H

#include "TObject.h"

class TMMInfo : public TObject
{

 public:

  TMMInfo();
  ~TMMInfo();

 public:

  void SetDaqTimeSec(Int_t v) { fDaqTimeSec = v; }
  Int_t GetDaqTimeSec() { return fDaqTimeSec; }

  void SetDaqTimeMicroSec(Int_t v) { fDaqTimeMicroSec = v; }
  Int_t GetDaqTimeMicroSec() { return fDaqTimeMicroSec; }

  void SetSrsTimeStamp(Int_t v) { fSrsTimeStamp = v; }
  Int_t GetSrsTimeStamp() { return fSrsTimeStamp; }

  void SetSrsTrigger(UInt_t v) { fSrsTrigger = v; }
  UInt_t GetSrsTrigger() { return fSrsTrigger; }

  void SetSrsRunTime(ULong64_t t) { fSrsRunTime = t; }
  ULong64_t GetSrsRunTime() { return fSrsRunTime; }

  void SetRunTimeDiff(Double_t t) { fRunTimeDiff = t; }
  Double_t GetRunTimeDiff() { return fRunTimeDiff; }

private:

  Int_t  fDaqTimeSec;
  Int_t  fDaqTimeMicroSec;
  Int_t  fSrsTimeStamp;
  UInt_t fSrsTrigger;
  ULong64_t fSrsRunTime; // roll over corrected
  Double_t fRunTimeDiff; // roll over corrected

  ClassDef(TMMInfo,1);
};
#endif
