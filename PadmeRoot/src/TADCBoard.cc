#include "TADCBoard.hh"

ClassImp(TADCBoard)

TADCBoard::TADCBoard()
{

  fBoardId             = 0;
  fBoardSN             = 0;
  fLVDSPattern         = 0;
  fBoardStatus         = 0;
  fGroupMask           = 0;
  fEventCounter        = 0;
  fEventTimeTag        = 0;
  f0SuppAlgrtm         = 0;
  fActiveChannelMask   = 0;
  fAcceptedChannelMask = 0;

  fADCChannels = new TClonesArray("TADCChannel",TADCBOARD_NCHANNELS);
  fNADCChannels = 0;

  fADCTriggers = new TClonesArray("TADCTrigger",TADCBOARD_NTRIGGERS);
  fNADCTriggers = 0;

}

TADCBoard::~TADCBoard()
{

  Clear();

}

void TADCBoard::Clear(Option_t* option)
{

  fBoardId             = 0;
  fBoardSN             = 0;
  fLVDSPattern         = 0;
  fBoardStatus         = 0;
  fGroupMask           = 0;
  fEventCounter        = 0;
  fEventTimeTag        = 0;
  f0SuppAlgrtm         = 0;
  fActiveChannelMask   = 0;
  fAcceptedChannelMask = 0;

  fNADCChannels = 0;
  fADCChannels->Clear(option);

  fNADCTriggers = 0;
  fADCTriggers->Clear(option);

}

void TADCBoard::SetGroupError(UChar_t g)
{
  // Set error bit (high 4 bits of fGroupMask) for group g
  if (g<4) {
    fGroupMask |= (1 << (g+4));
  } else {
    printf("ERROR - TADCBoard::SetGroupError - Invalid group: %u\n",g);
  }
}

void TADCBoard::ResetGroupError(UChar_t g)
{
  // Reset error bit (high 4 bits of fGroupMask) for group g
  if (g<4) {
    fGroupMask &= ~(1 << (g+4));
  } else {
    printf("ERROR - TADCBoard::ResetGroupError - Invalid group: %u\n",g);
  }
}

Bool_t TADCBoard::GetGroupError(UChar_t g)
{
  // Return boolean with error bit (high 4 bits of fGroupMask) for group g
  if (g<4) {
    if ( fGroupMask & (1 << (g+4)) ) {
	return true;
      } else {
	return false;
      }
  } else {
    printf("ERROR - TADCBoard::GetGroupError - Invalid group: %u\n",g);
    return false;
  }
}

TADCChannel* TADCBoard::AddADCChannel()
{
  if (fNADCChannels<TADCBOARD_NCHANNELS) {
    return (TADCChannel*)(fADCChannels->ConstructedAt(fNADCChannels++));
  } else {
    printf("ERROR - TADCBoard::AddADCChannel - Too many channels\n");
    return 0;
  }
}

TADCTrigger* TADCBoard::AddADCTrigger()
{
  if (fNADCTriggers<TADCBOARD_NTRIGGERS) {
    return (TADCTrigger*)(fADCTriggers->ConstructedAt(fNADCTriggers++));
  } else {
    printf("ERROR - TADCBoard::AddADCTrigger - Too many triggers\n");
    return 0;
  }
}

TADCChannel* TADCBoard::ADCChannel(Int_t i)
{
  return (TADCChannel*)(fADCChannels->At(i));
}

TADCTrigger* TADCBoard::ADCTrigger(Int_t i)
{
  return (TADCTrigger*)(fADCTriggers->At(i));
}
