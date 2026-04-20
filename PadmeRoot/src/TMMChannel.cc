#include "TMMChannel.hh"

ClassImp(TMMChannel)

TMMChannel::TMMChannel()
{
  fChannelFail = kFALSE;
  fChannelNumber = 0;
  for(Int_t i = 0; i<TMMCHANNEL_NSAMPLES; i++) fSamples[i] = 0;
  
}

TMMChannel::~TMMChannel()
{
  Clear();
}

void TMMChannel::Clear(Option_t* option)
{
  fChannelFail = kFALSE;
  fChannelNumber = 0;
  for(Int_t i = 0; i<TMMCHANNEL_NSAMPLES; i++) fSamples[i] = 0;
}
