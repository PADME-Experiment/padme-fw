#ifndef TADCChannel_H
#define TADCChannel_H

#include "TObject.h"

#define TADCCHANNEL_NSAMPLES 1024

class TADCChannel : public TObject
{

 public:

  TADCChannel();
  ~TADCChannel();

 public:

  void     SetChannelNumber(UChar_t v)  { fChannelNumber = v; }
  UChar_t  GetChannelNumber()           { return fChannelNumber; }

  void     SetSample(Int_t i,Short_t v) { fSamples[i] = v; }
  Short_t  GetSample(Int_t i)           { return fSamples[i]; }
  Short_t* GetSamplesArray()            { return fSamples; }

  UShort_t GetNSamples()                { return TADCCHANNEL_NSAMPLES; }

 private:

  UChar_t fChannelNumber;
  Short_t fSamples[TADCCHANNEL_NSAMPLES];

  ClassDef(TADCChannel,2);
};
#endif
