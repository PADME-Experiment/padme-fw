#include "TADCBoard.hh"

ClassImp(TADCBoard)

TADCBoard::TADCBoard()
{

  //printf("Creating TADCChannels clonearray\n");
  fADCChannels = new TClonesArray("TADCChannel",TADCBOARD_NCHANNELS);
  fNADCChannels = 0;

  //printf("Creating TADCTriggers clonearray\n");
  fADCTriggers = new TClonesArray("TADCTrigger",TADCBOARD_NTRIGGERS);
  fNADCTriggers = 0;

}

TADCBoard::~TADCBoard()
{

  Clear();

}

void TADCBoard::Clear(Option_t* option)
{

  //printf("Clearing TADCBoard\n");

  fNADCChannels = 0;
  fADCChannels->Clear(option);

  fNADCTriggers = 0;
  fADCTriggers->Clear(option);

}

TADCChannel* TADCBoard::AddADCChannel()
{
  if (fNADCChannels<TADCBOARD_NCHANNELS) {
    //printf("Adding TADCChannel %d\n",fNADCChannels);
    return (TADCChannel*)(fADCChannels->ConstructedAt(fNADCChannels++));
  } else {
    printf("ERROR - TADCBoard::AddADCChannel - Too many channels\n");
    return 0;
  }
}

TADCTrigger* TADCBoard::AddADCTrigger()
{
  if (fNADCTriggers<TADCBOARD_NTRIGGERS) {
    //printf("Adding TADCTrigger %d\n",fNADCTriggers);
    return (TADCTrigger*)(fADCTriggers->ConstructedAt(fNADCTriggers++));
  } else {
    printf("ERROR - TADCBoard::AddADCTrigger - Too many triggers\n");
    return 0;
  }
}

TADCChannel* TADCBoard::ADCChannel(int i)
{
  return (TADCChannel*)(fADCChannels->At(i));
}

TADCTrigger* TADCBoard::ADCTrigger(int i)
{
  return (TADCTrigger*)(fADCTriggers->At(i));
}
