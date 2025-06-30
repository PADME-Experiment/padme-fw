#ifndef TMMRawEvent_H
#define TMMRawEvent_H

#include "TClonesArray.h"
#include "TMMBoard.hh"
#include "TMMInfo.hh"

#define TRAWEVENT_MAXMMBOARDS 16

class TMMRawEvent : public TObject
{

 public:

  TMMRawEvent();
  ~TMMRawEvent();

  void Clear(Option_t* = "");

 public:

  // MM Information
  TMMInfo* MMInfo(){ return fMMInfo; } // header information
  UChar_t GetNMMBoards() { return fNMMBoards; }

  TMMBoard* AddMMBoard(); 
  TMMBoard* MMBoard(Int_t);

 private:

  TMMInfo* fMMInfo;
  UChar_t fNMMBoards;
  TClonesArray* fMMBoards;

  ClassDef(TMMRawEvent,1);
};
#endif
