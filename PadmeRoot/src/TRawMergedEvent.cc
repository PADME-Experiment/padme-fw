#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "TRawMergedEvent.hh"

ClassImp(TRawMergedEvent)

TRawMergedEvent::TRawMergedEvent()
{
  // PADME event
  fTRawEvent = new TRawEvent();
  fTMMRawEvent = new TMMRawEvent();

  std::cout << " Allocate TRawMergedEvent " << fTRawEvent << " " << fTMMRawEvent << std::endl;
}

TRawMergedEvent::~TRawMergedEvent()
{
  std::cout << " Delete TRawMergedEvent " << fTRawEvent << " " << fTMMRawEvent << std::endl;
  Clear();
}

void TRawMergedEvent::Clear(Option_t* option)
{
  fTRawEvent->Clear(option);
  fTMMRawEvent->Clear(option);
}
