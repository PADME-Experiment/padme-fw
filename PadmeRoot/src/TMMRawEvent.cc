#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "TMMRawEvent.hh"

ClassImp(TMMRawEvent)

TMMRawEvent::TMMRawEvent()
{
  // MM chamber event
  fMMInfo = new TMMInfo();
  fMMBoards = new TClonesArray("TMMBoard",TRAWEVENT_MAXMMBOARDS);
  fNMMBoards = 0;

  std::cout << " Allocate TMMRawEvent " << this << " " << fMMInfo << " " << fNMMBoards << " " << fMMBoards << std::endl;
}

TMMRawEvent::~TMMRawEvent()
{
  std::cout << " Delete TMMRawEvent " << this << " " << fMMInfo << " " << fNMMBoards << " " << fMMBoards << std::endl;

  Clear();

}

void TMMRawEvent::Clear(Option_t* option)
{
  fMMInfo->Clear(option);
  fNMMBoards = 0;
  fMMBoards->Clear(option);
}


TMMBoard* TMMRawEvent::AddMMBoard()
{
  if (fNMMBoards<TRAWEVENT_MAXMMBOARDS) {
    return (TMMBoard*)(fMMBoards->ConstructedAt(fNMMBoards++));
  } else {
    printf("ERROR - TRawMergedEvent::AddMMBoard - Too many MMBoards\n");
    return 0;
  }
}

TMMBoard* TMMRawEvent::MMBoard(Int_t i)
{
  return (TMMBoard*)(fMMBoards->At(i));
}
