#ifndef TMMBoard_H
#define TMMBoard_H

#include "TObject.h"
#include "TClonesArray.h"

#include "TMMChannel.hh"

#define TMMBOARD_NCHANNELS 256

class TMMBoard : public TObject
{

 public:

  TMMBoard();
  ~TMMBoard();

  void Clear(Option_t* = "");

 public:

  void     SetBoardId(UChar_t v)            { fBoardId = v; }
  UChar_t  GetBoardId()                     { return fBoardId; }

  void     SetBoardSN(UInt_t v)             { fBoardSN = v; }
  UInt_t   GetBoardSN()                     { return fBoardSN; }


  UChar_t GetNMMChannels() { return fNMMChannels; }
  TMMChannel* AddMMChannel();
  TMMChannel* MMChannel(Int_t);

  UInt_t GetNumberOfProblematicChannels(){ return fNumberOfProblematicChannels;}
  void   AddProblematicChannel(){ fNumberOfProblematicChannels++; }
private:

  UChar_t  fBoardId; // only 4 LS bits used: bits 0-2 layer, bit 3: position
  UInt_t   fBoardSN; //  0 --> 15
  UInt_t   fNumberOfProblematicChannels;

  UChar_t      fNMMChannels;
  TClonesArray* fMMChannels;

  ClassDef(TMMBoard,1);
};
#endif
