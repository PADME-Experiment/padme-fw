#ifndef TMMInfo_H
#define TMMInfo_H

#include "TObject.h"

class TMMInfo : public TObject
{

 public:

  TMMInfo();
  ~TMMInfo();
  void Clear(Option_t* = "");
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

  void            AddProblematicChannel(int isrs, int chipid){ fNumberOfProblematicChannels[isrs][chipid]++; }
  UInt_t  GetNumberOfProblematicChannels(int isrs, int ichipid){ return fNumberOfProblematicChannels[isrs][ichipid]; }
  void            AddFiredChannel(int isrs, int chipid){          fNumberOfFiredChannels[isrs][chipid]++; }
  UInt_t  GetNumberOfFiredChannels(int isrs, int ichipid){ return fNumberOfFiredChannels[isrs][ichipid]; }
private:

  Int_t  fDaqTimeSec;
  Int_t  fDaqTimeMicroSec;
  Int_t  fSrsTimeStamp;
  UInt_t fSrsTrigger;
  ULong64_t fSrsRunTime; // roll over corrected
  Double_t fRunTimeDiff; // roll over corrected
  UInt_t fNumberOfProblematicChannels[2][16]; // total number of problematic channels in the event
  UInt_t fNumberOfFiredChannels[2][16]; // total number of fired channels in the event
  
  ClassDef(TMMInfo,1);
};
#endif
