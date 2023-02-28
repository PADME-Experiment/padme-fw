#include "Riostream.h"

#include "TRawEvent.hh"

#include "PadmeVRecoConfig.hh"

#include "ETagHit.hh"

#include "ETagDigitizer.hh"

ETagDigitizer::ETagDigitizer(PadmeVRecoConfig* cfg)
  : fETagConfig(cfg)
{

  // Define verbose level
  fVerbose = fETagConfig->GetParOrDefault("Output", "Verbose", 0);

  if (fVerbose) printf("ETagDigitizer::ETagDigitizer - ETag digitization system created\n");
}

ETagDigitizer::~ETagDigitizer()
{
  if (fVerbose) printf("ETagDigitizer::~ETagDigitize - Deleting ETag digitization system\n");
}

void ETagDigitizer::Init()
{
  if (fVerbose) printf("ETagDigitizer::Init - Initilizing ETag digitizer\n");

  CreateChannelMap();

}

Bool_t ETagDigitizer::BuildHits(TRawEvent* rawEv, vector<ETagHit*>& hits)
{
  if (fVerbose>1) printf("ETagDigitizer::BuildHits - Building reconstructed hits\n");

  TADCBoard* board;
  TADCChannel* channel;

  for(UChar_t iBoard = 0; iBoard < rawEv->GetNADCBoards(); iBoard++) {

    board = rawEv->ADCBoard(iBoard);
    UChar_t boardId = board->GetBoardId();
    if (fETagConfig->BoardIsMine(boardId)) {

      for(UChar_t ich = 0; ich < board->GetNADCChannels();ich++) {

        channel = board->ADCChannel(ich);
	UChar_t channelId = channel->GetChannelNumber();

	Int_t etagChannel = fChannelMap[boardId][channelId];

	if (fVerbose>3) printf("ETagDigitizer::BuildHits - Processing board %2u channel %2u ETag channel %4.4d\n",boardId,channelId,etagChannel);
      }
    }
  }

  /*
  ETagHit* hit = new ETagHit();
  hit->SetADCBoard(23);
  hit->SetADCChannel(11);
  hit->SetChannelId(345);
  hit->SetStatus(0x0000000000000003);
  hit->SetPosition(TVector3(1.,1.,2.));
  hit->SetEnergy(324.3);
  hit->SetTime(23456.456);
  hits.push_back(hit);
  if (fVerbose>3) hit->Print();
  */

  return true;
}

Bool_t ETagDigitizer::CreateChannelMap()
{
  if (fVerbose) printf("ETagDigitizer::CreateChannelMap - Create ETag channel map\n");

  if(fETagConfig->GetReadOutType()  == "ADC") {

    std::vector<Short_t>* Boards = fETagConfig->GetBoards();
    for (UChar_t ib = 0;ib < Boards->size();ib++){
      int bID = (*Boards)[ib];
      std::string parName = "ADC" +  std::to_string(bID);
      if (fVerbose>1) std::cout << "ETagDigitizer::CreateChannelMap - Processing the map for board " << bID << std::endl;
      if(fETagConfig->GetConfigParser()->HasConfig("ADC",parName )){
	UChar_t ic = 0;
        std::vector<std::string> bMap = fETagConfig->GetConfigParser()->GetConfig("ADC",parName);
        for(auto it = bMap.begin(); it != bMap.end(); ++it) {
	  //std::cout << (*it) << " " << std::stoi(*it) << std::endl;
	  fChannelMap[bID][ic++] = std::stoi(*it);
	}
      }
    }

  }

  if (fVerbose>1) {
    printf("ETagDigitizer::CreateChannelMap - ETag channel map\n");
    for(UChar_t ib = 0; ib<32; ib++) {
      if (fETagConfig->BoardIsMine(ib)) {
	printf("Board %2d Chns",ib);
	for(UChar_t ic = 0; ic<32; ic++) {
	  printf(" %5.4d",fChannelMap[ib][ic]);
	}
	printf("\n");
      }
    }
  }

  return true;

}
