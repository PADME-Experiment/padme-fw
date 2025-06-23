#include "TMMBoard.hh"

ClassImp(TMMBoard)

TMMBoard::TMMBoard()
{

  fBoardId             = 0;
  fBoardSN             = 0;
  fNumberOfProblematicChannels = 0;
  fMMChannels = new TClonesArray("TMMChannel",TMMBOARD_NCHANNELS);
  fNMMChannels = 0;

}

TMMBoard::~TMMBoard()
{

  Clear();

}

void TMMBoard::Clear(Option_t* option)
{

  fBoardId             = 0;
  fBoardSN             = 0;
  fNumberOfProblematicChannels = 0;

  fNMMChannels = 0;
  fMMChannels->Clear(option);

}

TMMChannel* TMMBoard::AddMMChannel()
{
  if (fNMMChannels<TMMBOARD_NCHANNELS) {
    return (TMMChannel*)(fMMChannels->ConstructedAt(fNMMChannels++));
  } else {
    printf("ERROR - TMMBoard::AddMMChannel - Too many channels\n");
    return 0;
  }
}

TMMChannel* TMMBoard::MMChannel(Int_t i)
{
  return (TMMChannel*)(fMMChannels->At(i));
}


