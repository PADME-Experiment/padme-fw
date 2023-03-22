#include "Riostream.h"

#include "TRawEvent.hh"

#include "PadmeVRecoConfig.hh"
#include "HistoSvc.hh"

#include "ECalHit.hh"
#include "ECalChannelDigitizer.hh"

#include "ECalDigitizer.hh"

ECalDigitizer::ECalDigitizer(PadmeVRecoConfig* cfg)
  : fECalConfig(cfg)
{

  // Define verbose level
  fVerbose = fECalConfig->GetParOrDefault("Output", "Verbose", 0);

  // Create ECal channel digitizer
  fECalChannelDigitizer = new ECalChannelDigitizer(fECalConfig);

  // Attach to Histogrammin service
  fHistoSvc = HistoSvc::GetInstance();

  if (fVerbose) printf("ECalDigitizer::ECalDigitizer - ECal digitization system created\n");
}

ECalDigitizer::~ECalDigitizer()
{
  if (fVerbose) printf("ECalDigitizer::~ECalDigitizer - Deleting ECal digitization system\n");
}

Bool_t ECalDigitizer::Init()
{
  if (fVerbose) printf("ECalDigitizer::Init - Initilizing ECal digitizer\n");

  //Create maps for ADC board/channel to/from ECal channel conversions
  if (! CreateChannelMap()) {
    printf("ECalDigitizer::Init - ERROR - Problem during initialization. Please check.");
    return false;
  }

  // Initialize channel digitizer
  fECalChannelDigitizer->Init();

  return true;
}

Bool_t ECalDigitizer::BuildHits(TRawEvent* rawEv, vector<ECalHit*>& hits)
{
  if (fVerbose>1) printf("ECalDigitizer::BuildHits - Building reconstructed hits\n");

  // Update channel digitizer with current run/event number
  fECalChannelDigitizer->SetRunNumber(rawEv->GetRunNumber());
  fECalChannelDigitizer->SetEventNumber(rawEv->GetEventNumber());

  TADCBoard* board;
  TADCChannel* channel;
  TADCTrigger* group;

  for(UChar_t iBoard = 0; iBoard < rawEv->GetNADCBoards(); iBoard++) {

    board = rawEv->ADCBoard(iBoard);
    UChar_t boardId = board->GetBoardId();
    if (fECalConfig->BoardIsMine(boardId)) {

      // Get SIC for the 4 groups of the board
      UShort_t groupSIC[4] = {0,0,0,0};
      for(UChar_t igr = 0; igr < board->GetNADCTriggers(); igr++) {
	group = board->ADCTrigger(igr);
	groupSIC[group->GetGroupNumber()] = group->GetStartIndexCell();
      }
      if (fVerbose>3) printf("ECalDigitizer::BuildHits - Board %2u group SICs %d %d %d %d\n",boardId,groupSIC[0],groupSIC[1],groupSIC[2],groupSIC[3]);

      for(UChar_t ich = 0; ich < board->GetNADCChannels();ich++) {

        channel = board->ADCChannel(ich);
	UChar_t channelId = channel->GetChannelNumber();

	// Some channels in the ECal boards are used to record the plastic scintillator pads of the cosmics trigger: skip them
	Int_t ecalChannel = fChannelMap[boardId][channelId];
	if (ecalChannel == -1) continue;

	UChar_t groupId = channelId/8;
	UShort_t groupStartIndexCell = groupSIC[groupId];
	//printf("channelId %d groupId %d sic %d\n",channelId,groupId,groupStartIndexCell);

	if (fVerbose>3) printf("ECalDigitizer::BuildHits - Processing board %2u channel %2u ECal channel %4.4d SIC %d\n",boardId,channelId,ecalChannel,groupStartIndexCell);
	fECalChannelDigitizer->DigitizeChannel(boardId,channelId,ecalChannel,groupStartIndexCell,channel->GetSamplesArray(),hits);
      }
    }
  }

  return true;
}

Bool_t ECalDigitizer::CreateChannelMap()
{
  if (fVerbose) printf("ECalDigitizer::CreateChannelMap - Create ECal channel maps\n");

  if(! (fECalConfig->GetReadOutType()  == "ADC") ) {
    printf("ECalDigitizer::CreateChannelMap - ERROR - Readout type is not ADC. Check your configuration file.");
    return false;
  }

  std::vector<Short_t>* Boards = fECalConfig->GetBoards();
  for (UChar_t ib = 0;ib < Boards->size();ib++){

    int bID = (*Boards)[ib];

    std::string parName = "ADC" +  std::to_string(bID);
    if (! fECalConfig->GetConfigParser()->HasConfig("ADC",parName )) {
      printf("ECalDigitizer::CreateChannelMap - ERROR - Cannot find map for board %d. Check your configuration file.",bID);
      return false;
    }

    // Fill map with individual ECal channels
    if (fVerbose>1) std::cout << "ECalDigitizer::CreateChannelMap - Processing the map for board " << bID << std::endl;
    UChar_t ic = 0;
    std::vector<std::string> bMap = fECalConfig->GetConfigParser()->GetConfig("ADC",parName);
    for(auto it = bMap.begin(); it != bMap.end(); ++it) {
      Int_t chId = std::stoi(*it);
      fChannelMap[bID][ic] = chId;
      if (chId != -1) fMapChannel[chId] = std::make_pair(bID,ic);
      ic++;
    }

  }

  if (fVerbose>1) {

    printf("ECalDigitizer::CreateChannelMap - ECal channel map\n");
    for(UChar_t ib = 0; ib<32; ib++) {
      if (fECalConfig->BoardIsMine(ib)) {
	printf("Board %2d Chns",ib);
	for(UChar_t ic = 0; ic<32; ic++) {
	  printf(" %5.4d",fChannelMap[ib][ic]);
	}
	printf("\n");
      }
    }

    printf("ECalDigitizer::CreateChannelMap - ECal reverse channel map\n");
    std::map<Int_t,std::pair<UChar_t,UChar_t>>::iterator it;
    for (it = fMapChannel.begin(); it != fMapChannel.end(); it++)
      printf("%4.4d %2d/%2d\n",it->first,(it->second).first,(it->second).second);

  }

  return true;

}
