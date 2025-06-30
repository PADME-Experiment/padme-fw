#ifndef TRawMergedEvent_H
#define TRawMergedEvent_H

#include "TRawEvent.hh"
#include "TMMRawEvent.hh"


class TRawMergedEvent : public TObject
{

 public:

  TRawMergedEvent();
  ~TRawMergedEvent();

  void Clear(Option_t* = "");

 public:
  void SetTRawEvent(TRawEvent* v){fTRawEvent = v;}  
  TRawEvent* GetTRawEvent(){return fTRawEvent;}
    
  void SetTMMRawEvent(TMMRawEvent* v){fTMMRawEvent = v;}  
  TMMRawEvent* GetTMMRawEvent(){return fTMMRawEvent;}
    
  
 private:
  TRawEvent* fTRawEvent;
  TMMRawEvent* fTMMRawEvent;
  
  
  
  ClassDef(TRawMergedEvent,1);
};
#endif
