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

  // Attach global services
  fHistoSvc = HistoSvc::GetInstance();
  fRunConditionSvc = RunConditionSvc::GetInstance();
  fRunConfigurationSvc = RunConfigurationSvc::GetInstance();

  // Create LeadGlass histogram directory
  fHistoSvc->CreateDir("Target");

  // Define verbose level
  fVerbose = fTargetConfig->GetParOrDefault("Output", "Verbose", 0);

  // Create Target channel digitizer
  //fTargetChannelDigitizer = new TargetChannelDigitizer(fTargetConfig);

  fTimeBin           = fTargetConfig->GetParOrDefault( "ADC","TimeBin"                ,      1.);
  fVoltageBin        = fTargetConfig->GetParOrDefault( "ADC","VoltageBin"             ,0.000244);
  fImpedance         = fTargetConfig->GetParOrDefault( "ADC","InputImpedance"         ,     50.);
  fSignalWidth       = fTargetConfig->GetParOrDefault("RECO","SignalWindow"           ,    1024);
  fPedOffset         = fTargetConfig->GetParOrDefault("RECO","PedestalOffset"         ,     0); 
  fPreSamples        = fTargetConfig->GetParOrDefault("RECO","SignalPreSamples"       ,    1024);
  fPostSamples       = fTargetConfig->GetParOrDefault("RECO","SignalPostSamples"      ,    1024);
  fPedMaxNSamples    = fTargetConfig->GetParOrDefault("RECO","NumberOfSamplesPedestal",     100);
  fNEventsToPrint    = fTargetConfig->GetParOrDefault("RECO","NumberOfEventsToPrint"  ,       3);

  if (fNEventsToPrint>100)
    {
      printf("TargetDigitizer::TargetDigitizer - Can't print more than 100 events of full waveforms. Setting fNEventsToPrint to 10\n");
      fNEventsToPrint=100;
    }

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
  
  if (fRunConfigurationSvc->IsMonitorMode()) {
    hTargetDigitizer = fHistoSvc->BookHisto("Target","TargetDigitizer","TargetChannel",32,0,31);
    
    char iName[100];
    for(int iCh=0; iCh!=32 ; iCh++){
      for(int NEv = 0;NEv<fNEventsToPrint;NEv++)
	{
	  sprintf(iName,"TargetEvent%dCh%d",NEv,iCh);
	  hTargetSignals[iCh] = new TH1D(iName, iName,  1024,  0, 1024);
	}
    }
  }

  fEventCounter =0;

  return true;
}

void TargetDigitizer::ComputeChargePerStrip(TRawEvent* rawEv, vector<TargetStripCharge*>& charge)
{
  if (fVerbose>1) printf("TargetDigitizer::ComputeChargePerStrip - Computing charge collected per strip\n");

  fEventCounter++;

  // Update channel digitizer with current run/event number
  Int_t RunNumber = rawEv->GetRunNumber();
  Int_t EventNumber = rawEv->GetEventNumber();

  TADCBoard* board;
  TADCChannel* channel;

  //Beth 21/3/24: limits of signal to be integrated.
  Short_t begin = fPedMaxNSamples;
  Short_t end   =    fPostSamples;

  for(UChar_t iBoard = 0; iBoard < rawEv->GetNADCBoards(); iBoard++) {

    board = rawEv->ADCBoard(iBoard);
    UChar_t boardId = board->GetBoardId();
    if (fTargetConfig->BoardIsMine(boardId)) {
      for(UChar_t iChannel = 0; iChannel < board->GetNADCChannels(); iChannel++){

        channel = board->ADCChannel(iChannel);
	UShort_t channelId = channel->GetChannelNumber();
	//	std::cout<<channelId<<std::endl;

	/*	if (fRunConfigurationSvc->IsMonitorMode())
	  {
	    char iName[100];
	    //		sprintf(iName,"TargetEvent%dCh%d",fEventCounter,channelId);
	    sprintf(iName,"TargetCh%d",channelId);
	    std::cout<<iName<<std::endl;
	    hTargetSignals[iChannel]->SetNameTitle(iName,iName);//.c_str(),iName.c_str());
	    std::cout<<"name "<<hTargetSignals[iChannel]->GetName()<<std::endl;
	    }*/

	if (fRunConfigurationSvc->IsMonitorMode())  hTargetDigitizer->Fill(channelId);

	Int_t TargetChannel = fChannelMap[boardId][channelId];
	//	if (fVerbose>3) printf("TargetDigitizer::BuildHits - Processing run %2u event %2u board %2u channel %2u Target channel %4.4d\n",RunNumber,EventNumber,boardId,channelId,TargetChannel);

	//read in analog signal from digitiser board & channel
	fSamples = rawEv->ADCBoard(iBoard)->ADCChannel(iChannel)->GetSamplesArray();

	Double_t ped = CalcPedestal();    
	//make signals positive instead of negative
    //	SetAbsSignals();

	//Beth 21/3/24: following lines of hCharge calculation taken from old PadmeReco written by FedeO
	Double_t hCharge=0.;
	
	//	for(Short_t ii = begin;ii<end;++ii)
	for(Short_t ii = 0;ii<1024;++ii)
	  {
	    fSamples[ii] = fSamples[ii] - fPed;
	    hCharge+=1.* fSamples[ii];
	    if (fRunConfigurationSvc->IsMonitorMode())
	      hTargetSignals[iChannel]->SetBinContent(ii+1,fSamples[ii]);
	  }
	hCharge = hCharge- ((1.*end-1.*begin) * fPed);
	// hCharge *= (fVoltageBin*fTimeBin/fImpedance/fAverageGain);//fTimeBin in ns than charge in nC   
	hCharge *= (fVoltageBin*fTimeBin/fImpedance);             //fTimeBin in ns than charge in nC (charge in output of the amplifier)   
	hCharge *= 1000;                                          //charge in pC
	// fCharge *= (1/1.60217662e-7/fCCD/36);                     //electron charge and 36 e-h/um // going to calib step 
	// if( fHVsign<0 && fCh>15 ) hCharge = - hCharge;            // going to calib step 
	// if( fHVsign>0 && fCh<16 ) hCharge = - hCharge;            // going to calib step
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

void TargetDigitizer::SetAbsSignals(){
  for(UShort_t ii = 0;ii<1024;ii++){
    if (fSamples[ii] < 2048) {
      fSamples[ii] = 4096 - fSamples[ii];
    }
  }
}

Double_t TargetDigitizer::CalcPedestal(){
  fPed = 0.;
  fNPedSamples = 0;
  
  for(Short_t ii = fPedOffset ; ii  !=   fPedMaxNSamples; ii++) {
       fNPedSamples ++;
       fPed+= fSamples[ii];
  }
  
  fPed /= fNPedSamples;

  return fPed;
}
