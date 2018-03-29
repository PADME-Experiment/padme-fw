#include "TRawEvent.hh"

ClassImp(TRawEvent)

TRawEvent::TRawEvent()
{

  fTriggerInfo = new TTriggerInfo();

  //printf("Creating TADCBoards clonearray\n");
  fADCBoards = new TClonesArray("TADCBoard",TRAWEVENT_MAXADCBOARDS);
  fNADCBoards = 0;

}

TRawEvent::~TRawEvent()
{

  delete fTriggerInfo;
  Clear();

}

void TRawEvent::Clear(Option_t* option)
{

  //printf("Clearing TRawEvent\n");
  fNADCBoards = 0;
  fADCBoards->Clear(option);

}

TADCBoard* TRawEvent::AddADCBoard()
{
  if (fNADCBoards<TRAWEVENT_MAXADCBOARDS) {
    //printf("Adding TADCBoard %d\n",fNADCBoards);
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
