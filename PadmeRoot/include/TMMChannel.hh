#ifndef TMMChannel_H
#define TMMChannel_H

#include "TObject.h"

#define TMMCHANNEL_NSAMPLES 27

class TMMChannel : public TObject
{

 public:

  TMMChannel();
  ~TMMChannel();

 public:

  void     SetChannelNumber(UChar_t v)  { fChannelNumber = v; }
  UChar_t  GetChannelNumber()           { return fChannelNumber; }

  void     SetSample(Int_t i,Short_t v) { fSamples[i] = v; }
  Short_t  GetSample(Int_t i)           { return fSamples[i]; }
  Short_t* GetSamplesArray()            { return fSamples; }

  UShort_t GetNSamples()                { return TMMCHANNEL_NSAMPLES; }

 private:

  UChar_t fChannelNumber; // 1-256
  Short_t fSamples[TMMCHANNEL_NSAMPLES];

  ClassDef(TMMChannel,1);
};
#endif
