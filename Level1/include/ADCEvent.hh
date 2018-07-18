#ifndef ADCEvent_H
#define ADCEvent_H

#include "Rtypes.h"

//#include "ADCEventTags.hh"
#include "EventTags.hh"

class ADCEvent
{

 public:

  ADCEvent();
  ~ADCEvent();

  void Reset();

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

  void     SetTriggerStartIndexCell(int i, UShort_t v) { fTriggerStartIndexCell[i] = v; }
  UShort_t GetTriggerStartIndexCell(int i)             { return fTriggerStartIndexCell[i]; }

  void    SetTriggerFrequency(int i, UChar_t v)        { fTriggerFrequency[i] = v; }
  UChar_t GetTriggerFrequency(int i)                   { return fTriggerFrequency[i]; }

  void   SetTriggerHasSignal(int i, Bool_t v)          { fTriggerHasSignal[i] = v; }
  Bool_t GetTriggerHasSignal(int i)                    { return fTriggerHasSignal[i]; }

  void   SetTriggerTimeTag(int i, UInt_t v)            { fTriggerTimeTag[i] = v; }
  UInt_t GetTriggerTimeTag(int i)                      { return fTriggerTimeTag[i]; }

  void    SetADCChannelSample(int i, int s, Short_t v) { fADCChannelSample[i][s] = v; }
  Short_t GetADCChannelSample(int i, int s)            { return fADCChannelSample[i][s]; }

  void    SetADCTriggerSample(int i, int s, Short_t v) { fADCTriggerSample[i][s] = v; }
  Short_t GetADCTriggerSample(int i, int s)            { return fADCTriggerSample[i][s]; }

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
  
  UShort_t fTriggerStartIndexCell[ADCEVENT_NTRIGGERS];
  UChar_t  fTriggerFrequency[ADCEVENT_NTRIGGERS];
  Bool_t   fTriggerHasSignal[ADCEVENT_NTRIGGERS];
  UInt_t   fTriggerTimeTag[ADCEVENT_NTRIGGERS];
  Short_t  fADCTriggerSample[ADCEVENT_NTRIGGERS][ADCEVENT_NSAMPLES];

  Short_t  fADCChannelSample[ADCEVENT_NCHANNELS][ADCEVENT_NSAMPLES];

};
#endif
