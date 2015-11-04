#include "ADCEvent.hh"

ADCEvent::ADCEvent()
{
  Reset();
}

ADCEvent::~ADCEvent()
{;}

void ADCEvent::Reset()
{

  fBoardId             = 0;
  fLVDSPattern         = 0;
  fStatus              = 0;
  fGroupMask           = 0;
  fEventCounter        = 0;
  fEventTimeTag        = 0;
  fActiveChannelMask   = 0;
  fAcceptedChannelMask = 0;

  for(int i=0;i<ADCEVENT_NTRIGGERS;i++) {
    fTriggerStartIndexCell[i] = 0;
    fTriggerFrequency[i]      = 0;
    fTriggerHasSignal[i]      = 0;
    fTriggerTimeTag[i]        = 0;
    for(int s=0;s<ADCEVENT_NSAMPLES;s++) fADCTriggerSample[i][s] = 0;
  }

  for(int i=0;i<ADCEVENT_NCHANNELS;i++) {
    for(int s=0;s<ADCEVENT_NSAMPLES;s++) fADCChannelSample[i][s] = 0;
  }

}
