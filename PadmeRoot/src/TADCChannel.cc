#include "TADCChannel.hh"

ClassImp(TADCChannel)

TADCChannel::TADCChannel()
{
  fChannelNumber = 0;
  for(Int_t i = 0; i<TADCCHANNEL_NSAMPLES; i++) fSamples[i] = 0;
}

TADCChannel::~TADCChannel()
{;}
