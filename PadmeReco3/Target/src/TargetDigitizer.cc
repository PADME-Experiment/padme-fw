#include "Riostream.h"

#include <TH1D.h>

#include "TRawEvent.hh"

#include "PadmeVRecoConfig.hh"

#include "HistoSvc.hh"
#include "RunConditionSvc.hh"
#include "RunConfigurationSvc.hh"

//#include "TargetHit.hh"
//#include "TargetChannelDigitizer.hh"

#include "TargetDigitizer.hh"

TargetDigitizer::TargetDigitizer(PadmeVRecoConfig* cfg)
  : fTargetConfig(cfg)
{

  // Define verbose level
  fVerbose = fTargetConfig->GetParOrDefault("Output", "Verbose", 0);

  // Create Target channel digitizer
  //fTargetChannelDigitizer = new TargetChannelDigitizer(fTargetConfig);

  if (fVerbose) printf("TargetDigitizer::TargetDigitizer - Target digitization system created\n");
}

TargetDigitizer::~TargetDigitizer()
{
  if (fVerbose) printf("TargetDigitizer::~TargetDigitizer - Deleting Target digitization system\n");
}

Bool_t TargetDigitizer::Init()
{
  if (fVerbose) printf("TargetDigitizer::Init - Initilizing Target digitizer\n");

  //Create maps for ADC board/channel to/from Target channel conversions
  if (! CreateChannelMap()) {
    printf("TargetDigitizer::Init - ERROR - Problem during initialization. Please check.");
    return false;
  }

  // Initialize channel digitizer
  //fTargetChannelDigitizer->Init();

  return true;
}

void TargetDigitizer::ComputeChargePerStrip(TRawEvent* rawEv, vector<TargetStripCharge*>& charge)
{
  if (fVerbose>1) printf("TargetDigitizer::ComputeChargePerStrip - Computing charge collected per strip\n");

  // Update channel digitizer with current run/event number
  Int_t RunNumber = rawEv->GetRunNumber();
  Int_t EventNumber = rawEv->GetEventNumber();

  TADCBoard* board;
  TADCChannel* channel;

  for(UChar_t iBoard = 0; iBoard < rawEv->GetNADCBoards(); iBoard++) {

    board = rawEv->ADCBoard(iBoard);
    UChar_t boardId = board->GetBoardId();
    if (fTargetConfig->BoardIsMine(boardId)) {

      for(UChar_t ich = 0; ich < board->GetNADCChannels();ich++) {

        channel = board->ADCChannel(ich);
	UChar_t channelId = channel->GetChannelNumber();

	Int_t TargetChannel = fChannelMap[boardId][channelId];
	//	if (fVerbose>3) printf("TargetDigitizer::BuildHits - Processing run %2u event %2u board %2u channel %2u Target channel %4.4d\n",RunNumber,EventNumber,boardId,channelId,TargetChannel);
      }
    }
  }

  return;

}

/* //commented by Beth 21/3/24: The concept of "hits" in the target doesn't really have any meaning. It would be better to compute the charge per strip
Bool_t TargetDigitizer::BuildHits(TRawEvent* rawEv, vector<TargetHit*>& hits)
{
  if (fVerbose>1) printf("TargetDigitizer::BuildHits - Building reconstructed hits\n");

  // Update channel digitizer with current run/event number
  //fTargetChannelDigitizer->SetRunNumber(rawEv->GetRunNumber());
  //fTargetChannelDigitizer->SetEventNumber(rawEv->GetEventNumber());

  TADCBoard* board;
  TADCChannel* channel;

  for(UChar_t iBoard = 0; iBoard < rawEv->GetNADCBoards(); iBoard++) {

    board = rawEv->ADCBoard(iBoard);
    UChar_t boardId = board->GetBoardId();
    if (fTargetConfig->BoardIsMine(boardId)) {

      for(UChar_t ich = 0; ich < board->GetNADCChannels();ich++) {

        channel = board->ADCChannel(ich);
	UChar_t channelId = channel->GetChannelNumber();

	//Beth 19/3/24: What's this doing here? Probably nothing??
	Int_t etagChannel = fChannelMap[boardId][channelId];

	if (fVerbose>3) printf("TargetDigitizer::BuildHits - Processing board %2u channel %2u Target channel %4.4d\n",boardId,channelId,etagChannel);
	//fTargetChannelDigitizer->DigitizeChannel(boardId,channelId,etagChannel,channel->GetSamplesArray(),hits);
      }
    }
  }

  return true;
}
*/

Bool_t TargetDigitizer::CreateChannelMap()
{

  if (fVerbose) printf("TargetDigitizer::CreateChannelMap - Create Target channel maps\n");

  if (! (fTargetConfig->GetReadOutType()  == "ADC") ) {
    printf("TargetDigitizer::CreateChannelMap - ERROR - Readout type is not ADC. Check your configuration file.");
    return false;
  }

  std::vector<Short_t>* Boards = fTargetConfig->GetBoards();
  for (UChar_t ib = 0;ib < Boards->size();ib++){

    int bID = (*Boards)[ib];
    if (fVerbose>1) std::cout << "TargetDigitizer::CreateChannelMap - Processing the map for board " << bID << std::endl;

    std::string parName = "ADC" +  std::to_string(bID);
    if (! fTargetConfig->GetConfigParser()->HasConfig("ADC",parName) ){
      printf("TargetDigitizer::CreateChannelMap - ERROR - Cannot find map for board %d. Check your configuration file.",bID);
      return false;
    }

    UChar_t ic = 0;
    std::vector<std::string> bMap = fTargetConfig->GetConfigParser()->GetConfig("ADC",parName);
    for(auto it = bMap.begin(); it != bMap.end(); ++it) {
      
      Int_t chId = std::stoi(*it);
      
      // Fill map and reverse map with individual strips
      fChannelMap[bID][ic] = chId;
      if (chId != -1) fMapChannel[chId] = std::make_pair(bID,ic);
      
      ic++;
      
    }
  }

  if (fVerbose>1) {

    printf("TargetDigitizer::CreateChannelMap - Target channel map\n");
    for(UChar_t ib = 0; ib<32; ib++) {
      if (fTargetConfig->BoardIsMine(ib)) {
	printf("Board %2d Chns",ib);
	for(UChar_t ic = 0; ic<32; ic++) {
	  printf(" %5.4d",fChannelMap[ib][ic]);
	}
	printf("\n");
      }
    }

    printf("TargetDigitizer::CreateChannelMap - Target reverse channel map\n");
    std::map<Int_t,std::pair<UChar_t,UChar_t>>::iterator it;
    for (it = fMapChannel.begin(); it != fMapChannel.end(); it++)
      printf("%4.4d %2d/%2d\n",it->first,(it->second).first,(it->second).second);

  }

  return true;

}
