#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "Configuration.hh"
#include "TRawEvent.hh"

#include "EventCopier.hh"

EventCopier::EventCopier()
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

}

EventCopier::~EventCopier()
{;}

Int_t EventCopier::Initialize()
{
  if (fConfig->Verbose()) printf("EventCopier::Initialize - Initializing.\n");
  return 0;
}

Int_t EventCopier::Finalize()
{
  if (fConfig->Verbose()) printf("EventCopier::Finalize - Finalizing.\n");
  return 0;
}

Int_t EventCopier::CopyEvent(TRawEvent* outEv, TRawEvent* inEv)
{

  // Empty output event structure
  outEv->Clear("C");

  // Set run and event number
  outEv->SetRunNumber(inEv->GetRunNumber());
  outEv->SetEventNumber(inEv->GetEventNumber());

  // Save system time when event was merged
  outEv->SetEventAbsTime(inEv->GetEventAbsTime());

  // Set event time since start of run
  outEv->SetEventRunTime(inEv->GetEventRunTime());

  // Set trigger mask
  outEv->SetEventTrigMask(inEv->GetEventTrigMask());

  // Set event status
  outEv->SetEventStatus(inEv->GetEventStatus());

  // Set missing boards bit mask
  outEv->SetMissingADCBoards(inEv->GetMissingADCBoards());

  // Save low level trigger info to TTriggerInfo structure
  TTriggerInfo* inTrigInfo = inEv->TriggerInfo();
  TTriggerInfo* outTrigInfo = outEv->TriggerInfo();
  outTrigInfo->SetTriggerCounter(inTrigInfo->GetTriggerCounter());
  outTrigInfo->SetTriggerTime(inTrigInfo->GetTriggerTime());
  outTrigInfo->SetTriggerPattern(inTrigInfo->GetTriggerPattern());

  // Loop over all ADC boards
  for(Int_t b=0; b<(Int_t)inEv->GetNADCBoards(); b++) {

    TADCBoard* inBoard = inEv->ADCBoard(b);
    TADCBoard* outBoard = outEv->AddADCBoard();

    outBoard->SetBoardId            (inBoard->GetBoardId());
    outBoard->SetBoardSN            (inBoard->GetBoardSN());
    outBoard->SetLVDSPattern        (inBoard->GetLVDSPattern());
    outBoard->SetBoardStatus        (inBoard->GetBoardStatus());
    outBoard->SetGroupMask          (inBoard->GetGroupMask());
    outBoard->SetEventCounter       (inBoard->GetEventCounter());
    outBoard->SetEventTimeTag       (inBoard->GetEventTimeTag());
    outBoard->Set0SuppAlgrtm        (inBoard->Get0SuppAlgrtm());
    outBoard->SetActiveChannelMask  (inBoard->GetActiveChannelMask());
    outBoard->SetAcceptedChannelMask(inBoard->GetAcceptedChannelMask());

    // Save triggers information for this board
    for(Int_t t=0; t<(Int_t)inBoard->GetNADCTriggers(); t++) {

      TADCTrigger* inTrig = inBoard->ADCTrigger(t);
      TADCTrigger* outTrig = outBoard->AddADCTrigger();

      outTrig->SetGroupNumber   (inTrig->GetGroupNumber());
      outTrig->SetStartIndexCell(inTrig->GetStartIndexCell());
      outTrig->SetFrequency     (inTrig->GetFrequency());
      outTrig->SetTriggerSignal (inTrig->GetTriggerSignal());
      outTrig->SetTriggerTimeTag(inTrig->GetTriggerTimeTag());
      if (inTrig->GetTriggerSignal()) {
	for(Int_t s=0; s<(Int_t)inTrig->GetNSamples(); s++) outTrig->SetSample(s,inTrig->GetSample(s));
      }

    }

    // Save channels information for this board
    for(Int_t c=0; c<(Int_t)inBoard->GetNADCChannels(); c++) {

      TADCChannel* inChan = inBoard->ADCChannel(c);
      TADCChannel* outChan = outBoard->AddADCChannel();

      outChan->SetChannelNumber(inChan->GetChannelNumber());
      for(Int_t s=0; s<(Int_t)inChan->GetNSamples(); s++) outChan->SetSample(s,inChan->GetSample(s));

    }

  }

  return 0;

}
