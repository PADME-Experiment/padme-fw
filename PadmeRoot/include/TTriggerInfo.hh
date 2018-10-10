#ifndef TTriggerInfo_H
#define TTriggerInfo_H

#include "TObject.h"

class TTriggerInfo : public TObject
{

 public:

  TTriggerInfo();
  ~TTriggerInfo();

 public:

  void SetTriggerCounter(UInt_t v) { fTriggerCounter = v; }
  UInt_t GetTriggerCounter() { return fTriggerCounter; }

  void SetTriggerTime(ULong64_t v) { fTriggerTime = v; }
  ULong64_t GetTriggerTime() { return fTriggerTime; }

  void SetTriggerPattern(UInt_t v) { fTriggerPattern = v; }
  UInt_t GetTriggerPattern() { return fTriggerPattern; }

 private:

  UInt_t    fTriggerCounter;
  ULong64_t fTriggerTime;
  UInt_t    fTriggerPattern;

  ClassDef(TTriggerInfo,1);
};
#endif
