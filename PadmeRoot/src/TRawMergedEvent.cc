#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "TRawMergedEvent.hh"

ClassImp(TRawMergedEvent)

TRawMergedEvent::TRawMergedEvent()
{
  // PADME event
  fTRawEvent = new TRawEvent();

  // MM chamber event
  fMMInfo = new TMMInfo();
  fMMBoards = new TClonesArray("TMMBoard",TRAWEVENT_MAXMMBOARDS);
  fNMMBoards = 0;

  std::cout << " Allocate TRawMergedEvent " << fTRawEvent << " " << fMMInfo << " " << fNMMBoards << " " << fMMBoards << std::endl;
}

TRawMergedEvent::~TRawMergedEvent()
{
  std::cout << " Delete TRawMergedEvent " << fTRawEvent << " " << fMMInfo << " " << fNMMBoards << " " << fMMBoards << std::endl;

  Clear();

}

void TRawMergedEvent::Clear(Option_t* option)
{

  fTRawEvent->Clear(option);

  fMMInfo->Clear(option);
  fNMMBoards = 0;
  fMMBoards->Clear(option);


}


TMMBoard* TRawMergedEvent::AddMMBoard()
{
  if (fNMMBoards<TRAWEVENT_MAXMMBOARDS) {
    return (TMMBoard*)(fMMBoards->ConstructedAt(fNMMBoards++));
  } else {
    printf("ERROR - TRawMergedEvent::AddMMBoard - Too many MMBoards\n");
    return 0;
  }
}

TMMBoard* TRawMergedEvent::MMBoard(Int_t i)
{
  return (TMMBoard*)(fMMBoards->At(i));
}
