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

  // MM chamber information
  fMMInfo = new TMMInfo();
  fMMBoards = new TClonesArray("TMMBoard",TRAWEVENT_MAXMMBOARDS);
  fNMMBoards = 0;

}

TRawEvent::~TRawEvent()
{

  Clear();

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

  fTriggerInfo->Clear(option);

  fNADCBoards = 0;
  fADCBoards->Clear(option);

  fMMInfo->Clear(option);

  fNMMBoards = 0;
  fMMBoards->Clear(option);


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

TMMBoard* TRawEvent::AddMMBoard()
{
  if (fNMMBoards<TRAWEVENT_MAXMMBOARDS) {
    return (TMMBoard*)(fMMBoards->ConstructedAt(fNMMBoards++));
  } else {
    printf("ERROR - TRawEvent::AddMMBoard - Too many MMBoards\n");
    return 0;
  }
}

TMMBoard* TRawEvent::MMBoard(Int_t i)
{
  return (TMMBoard*)(fMMBoards->At(i));
}
