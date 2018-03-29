#ifndef TADCBoard_H
#define TADCBoard_H

#include "TObject.h"
#include "TClonesArray.h"

#include "TADCChannel.hh"
#include "TADCTrigger.hh"

#define TADCBOARD_NCHANNELS 32
#define TADCBOARD_NTRIGGERS 4

class TADCBoard : public TObject
{

 public:

  TADCBoard();
  ~TADCBoard();

  void Clear(Option_t* = "");

 public:

  void     SetBoardId(UChar_t v)            { fBoardId = v; }
  UChar_t  GetBoardId()                     { return fBoardId; }

  void     SetBoardSN(UInt_t v)             { fBoardSN = v; }
  UInt_t   GetBoardSN()                     { return fBoardSN; }

  void     SetLVDSPattern(UShort_t v)       { fLVDSPattern = v; }
  UShort_t GetLVDSPattern()                 { return fLVDSPattern; }

  // Support obsolete Status byte (8 bits)
  void     SetStatus(UChar_t v)             { fBoardStatus = (UInt_t)v; }
  UChar_t  GetStatus()                      { return (UChar_t)(fBoardStatus & 0xff); }

  void     SetBoardStatus(UChar_t v)        { fBoardStatus = v; }
  UInt_t   GetBoardStatus()                 { return fBoardStatus; }

  void     SetGroupMask(UChar_t v)          { fGroupMask = v; }
  UChar_t  GetGroupMask()                   { return fGroupMask; }

  void     SetEventCounter(UInt_t v)        { fEventCounter = v; }
  UInt_t   GetEventCounter()                { return fEventCounter; }

  void     SetEventTimeTag(UInt_t v)        { fEventTimeTag = v; }
  UInt_t   GetEventTimeTag()                { return fEventTimeTag; }

  void     Set0SuppAlgrtm(UChar_t v)        { f0SuppAlgrtm = v; }
  UChar_t  Get0SuppAlgrtm()                 { return f0SuppAlgrtm; }

  void     SetActiveChannelMask(UInt_t v)   { fActiveChannelMask = v; }
  UInt_t   GetActiveChannelMask()           { return fActiveChannelMask; }

  void     SetAcceptedChannelMask(UInt_t v) { fAcceptedChannelMask = v; }
  UInt_t   GetAcceptedChannelMask()         { return fAcceptedChannelMask; }

  UChar_t GetNADCChannels() { return fNADCChannels; }
  TADCChannel* AddADCChannel();
  TADCChannel* ADCChannel(Int_t);

  UChar_t GetNADCTriggers() { return fNADCTriggers; }
  TADCTrigger* AddADCTrigger();
  TADCTrigger* ADCTrigger(Int_t);

 private:

  UChar_t  fBoardId;
  UInt_t   fBoardSN;
  UShort_t fLVDSPattern;
  UInt_t   fBoardStatus;
  UChar_t  fGroupMask;
  UInt_t   fEventCounter;
  UInt_t   fEventTimeTag;
  UChar_t  f0SuppAlgrtm;
  UInt_t   fActiveChannelMask;
  UInt_t   fAcceptedChannelMask;

  UChar_t      fNADCChannels;
  TClonesArray* fADCChannels;

  UChar_t      fNADCTriggers;
  TClonesArray* fADCTriggers;

  ClassDef(TADCBoard,2);
};
#endif
