#include <stdio.h>
#include <iostream>
#include "TRawEvent.hh"

ClassImp(TRawEvent)

TRawEvent::TRawEvent()
{

  fRunNumber     = 0;
  fEventNumber   = 0;

  fEventAbsTime  = 0;
  fEventRunTime  = 0;

  fEventTrigMask = 0;
  fEventStatus   = 0;
  fMissingADCBoards = 0;

  fTriggerInfo = new TTriggerInfo();

  fADCBoards = new TClonesArray("TADCBoard",TRAWEVENT_MAXADCBOARDS);
  fNADCBoards = 0;

  //  std::cout << " Allocate TRawEvent " << this << std::endl;
}

TRawEvent::~TRawEvent()
{
  //  std::cout << " Delete TRawEvent " << this << std::endl;

  Clear("C");
  delete fTriggerInfo;
  delete fADCBoards;
}

void TRawEvent::Clear(Option_t* option)
{

  fRunNumber     = 0;
  fEventNumber   = 0;

  fEventAbsTime  = 0;
  fEventRunTime  = 0;

  fEventTrigMask = 0;
  fEventStatus   = 0;
  fMissingADCBoards = 0;

  //  fTriggerInfo->Clear(option);

  fNADCBoards = 0;
  fADCBoards->Clear(option);

}

TADCBoard* TRawEvent::AddADCBoard()
{
  if (fNADCBoards<TRAWEVENT_MAXADCBOARDS) {
    return (TADCBoard*)(fADCBoards->ConstructedAt(fNADCBoards++));
  } else {
    printf("ERROR - TRawEvent::AddADCBoard - Too many ADCBoards\n");
    return 0;
  }
}

TADCBoard* TRawEvent::ADCBoard(Int_t i)
{
  return (TADCBoard*)(fADCBoards->At(i));
}
