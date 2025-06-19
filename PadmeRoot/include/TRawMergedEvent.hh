#ifndef TRawMergedEvent_H
#define TRawMergedEvent_H

#include "TRawEvent.hh"
#include "TMMBoard.hh"
#include "TMMInfo.hh"

#define TRAWEVENT_MAXMMBOARDS 16

class TRawMergedEvent : public TObject
{

 public:

  TRawMergedEvent();
  ~TRawMergedEvent();

  void Clear(Option_t* = "");

 public:
  void SetTRawEvent(TRawEvent* v){fTRawEvent = v;}  
  TRawEvent* GetTRawEvent(){return fTRawEvent;}
    
  // MM Information
  TMMInfo* MMInfo(){ return fMMInfo; } // header information
  UChar_t GetNMMBoards() { return fNMMBoards; }

  TMMBoard* AddMMBoard(); 
  TMMBoard* MMBoard(Int_t);
  
 private:
  TRawEvent* fTRawEvent;
  
  TMMInfo* fMMInfo;
  UChar_t fNMMBoards;
  TClonesArray* fMMBoards;
  
  
  ClassDef(TRawMergedEvent,1);
};
#endif
