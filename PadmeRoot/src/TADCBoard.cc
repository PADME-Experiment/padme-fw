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
