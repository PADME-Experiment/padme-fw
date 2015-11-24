#ifndef TADCTrigger_H
#define TADCTrigger_H

#include "TObject.h"

#define TADCTRIGGER_NSAMPLES 1024

class TADCTrigger : public TObject
{

 public:

  TADCTrigger();
  ~TADCTrigger();

 public:

  void      SetGroupNumber(UChar_t v)     { fGroupNumber = v; }
  UChar_t   GetGroupNumber()              { return fGroupNumber; }

  void      SetStartIndexCell(UShort_t v) { fStartIndexCell = v; }
  UShort_t  GetStartIndexCell()           { return fStartIndexCell; }

  void      SetFrequency(UChar_t v)       { fFrequency = v; }
  UChar_t   GetFrequency()                { return fFrequency; }

  void      SetTriggerSignal(Bool_t v)    { fTriggerSignal = v; }
  Bool_t    GetTriggerSignal()            { return fTriggerSignal; }

  void      SetTriggerTimeTag(UInt_t v)   { fTriggerTimeTag = v; }
  UInt_t    GetTriggerTimeTag()           { return fTriggerTimeTag; }

  void      SetSample(int i,Short_t v)   { fSamples[i] = v; }
  Short_t   GetSample(int i)              { return fSamples[i]; }
  Short_t*  GetSamplesArray()             { return fSamples; }

  UShort_t  GetNSamples()                 { return TADCTRIGGER_NSAMPLES; }

 private:

  UChar_t  fGroupNumber;
  UShort_t fStartIndexCell;
  UChar_t  fFrequency;
  Bool_t   fTriggerSignal;
  UInt_t   fTriggerTimeTag;
  Short_t  fSamples[TADCTRIGGER_NSAMPLES];

  ClassDef(TADCTrigger,1);
};
#endif
