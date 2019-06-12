#include "TADCTrigger.hh"

ClassImp(TADCTrigger)

TADCTrigger::TADCTrigger()
{
  fGroupNumber = 0;
  fStartIndexCell = 0;
  fFrequency = 0;
  fTriggerSignal = 0;
  fTriggerTimeTag = 0;
  for(Int_t i = 0; i<TADCTRIGGER_NSAMPLES; i++) fSamples[i] = 0;
}

TADCTrigger::~TADCTrigger()
{;}
