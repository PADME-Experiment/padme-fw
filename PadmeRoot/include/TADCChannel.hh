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

  void      SetChannelNumber(UChar_t v) { fChannelNumber = v; }
  UChar_t   GetChannelNumber()          { return fChannelNumber; }

  void      SetSample(int i,UShort_t v) { fSamples[i] = v; }
  UShort_t  GetSample(int i)            { return fSamples[i]; }
  UShort_t* GetSamplesArray()           { return fSamples; }

  UShort_t  GetNSamples()               { return TADCCHANNEL_NSAMPLES; }

 private:

  UChar_t  fChannelNumber;
  UShort_t fSamples[TADCCHANNEL_NSAMPLES];

  ClassDef(TADCChannel,1);
};
#endif
