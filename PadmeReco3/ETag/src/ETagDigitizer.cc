#include "Riostream.h"

#include "TRawEvent.hh"

#include "PadmeVRecoConfig.hh"

#include "ETagHit.hh"
#include "ETagChannelDigitizer.hh"

#include "ETagDigitizer.hh"

ETagDigitizer::ETagDigitizer(PadmeVRecoConfig* cfg)
  : fETagConfig(cfg)
{

  // Define verbose level
  fVerbose = fETagConfig->GetParOrDefault("Output", "Verbose", 0);

  // Create ETag channel digitizer
  fETagChannelDigitizer = new ETagChannelDigitizer(fETagConfig);

  if (fVerbose) printf("ETagDigitizer::ETagDigitizer - ETag digitization system created\n");
}

ETagDigitizer::~ETagDigitizer()
{
  if (fVerbose) printf("ETagDigitizer::~ETagDigitizer - Deleting ETag digitization system\n");
}

Bool_t ETagDigitizer::Init()
{
  if (fVerbose) printf("ETagDigitizer::Init - Initilizing ETag digitizer\n");

  //Create maps for ADC board/channel to/from ETag channel conversions
  if (! CreateChannelMap()) {
    printf("ETagDigitizer::Init - ERROR - Problem during initialization. Please check.");
    return false;
  }

  // Initialize channel digitizer
  fETagChannelDigitizer->Init();

  return true;
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
	fETagChannelDigitizer->DigitizeChannel(boardId,channelId,etagChannel,channel->GetSamplesArray(),hits);
      }
    }
  }

  return true;
}

Bool_t ETagDigitizer::CreateChannelMap()
{
  if (fVerbose) printf("ETagDigitizer::CreateChannelMap - Create ETag channel maps\n");

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
	  Int_t chId = std::stoi(*it);

	  // Fill map with individual SiPMs
	  fChannelMap[bID][ic] = chId;
	  if (chId != -1) fMapChannel[chId] = std::make_pair(bID,ic);

	  // Fill map with SiPM boards
	  Int_t card = ic/4;
	  Int_t cardId = chId/10;
	  if (chId == -1) {
	    cardId = -1;
	  } else {
	    UChar_t sipmId = chId%10;
	    fMapCard[cardId][sipmId] = std::make_pair(bID,ic);
	  }
	  if (ic%4 == 0) fCardMap[bID][card] = cardId;
	  //printf("ETag board %d SiPM board %d id %d\n",bID,card,cardId);

	  ic++;

	}

      } else {

	printf("ETagDigitizer::CreateChannelMap - ERROR - Cannot find map for board %d. Check your configuration file.",bID);
	return false;

      }
    }

  } else {

    printf("ETagDigitizer::CreateChannelMap - ERROR - Readout type is not ADC. Check your configuration file.");
    return false;

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

    printf("ETagDigitizer::CreateChannelMap - ETag card map\n");
    for(UChar_t ib = 0; ib<32; ib++) {
      if (fETagConfig->BoardIsMine(ib)) {
	printf("Board %2d Cards",ib);
	for(UChar_t ic = 0; ic<8; ic++) {
	  printf(" %4.3d",fCardMap[ib][ic]);
	}
	printf("\n");
      }
    }

    printf("ETagDigitizer::CreateChannelMap - ETag reverse channel map\n");
    std::map<Int_t,std::pair<UChar_t,UChar_t>>::iterator it;
    for (it = fMapChannel.begin(); it != fMapChannel.end(); it++)
      printf("%4.4d %2d/%2d\n",it->first,(it->second).first,(it->second).second);

    printf("ETagDigitizer::CreateChannelMap - ETag reverse card map\n");
    std::map<Int_t,std::array<std::pair<UChar_t,UChar_t>,4>>::iterator ir;
    for (ir = fMapCard.begin(); ir != fMapCard.end(); ir++) {
      printf("%3.3d",ir->first);
      for (UChar_t is = 0; is<4; is++) printf(" %1d:%2d/%2d",is,(ir->second)[is].first,(ir->second)[is].second);
      printf("\n");
    }

  }

  return true;

}
