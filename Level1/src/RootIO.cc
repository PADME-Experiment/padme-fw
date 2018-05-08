#include <stdio.h>

#include "DBService.hh"
#include "Configuration.hh"
#include "RootIO.hh"

RootIO::RootIO()
{

  // Create TFile handle
  fTFileHandle = new TFile();

  // Create TRawEvent object
  fTRawEvent = new TRawEvent();

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Connect to DB service
  fDB = 0;
  if (fConfig->RunNumber()) fDB = DBService::GetInstance();

}

RootIO::~RootIO()
{
  delete fTFileHandle;
  delete fTRawEvent;
}

//int RootIO::Init(std::string outfiletemplate, int nevtsperfile)
int RootIO::Init()
{

  //fNMaxEvtsPerOutFile = nevtsperfile;
  //fOutFileTemplate = outfiletemplate;

  fOutFileDBId = 0;
  fOutFileIndex = 0;
  fOutFileEvents = 0;
  fOutEventsTotal = 0;
  fOutSizeTotal = 0;

  // Set initial output filename
  //if (fNMaxEvtsPerOutFile == 0) {
  if (fConfig->NEventsPerFile() == 0) {
    //fOutFile.Form("%s.root",fOutFileTemplate.Data());
    fOutFile.Form("%s.root",fConfig->OutputFileHeader().c_str());
  } else {
    SetOutFile();
  }

  // Open output file
  //printf("RootIO::Init - Creating %s output file.\n",fOutFile.Data());
  return (int)OpenOutFile();

}

int RootIO::Exit()
{
  printf("RootIO::Exit - Finalizing output.\n");
  CloseOutFile();

  printf("RootIO::Exit - Total files created:  %u\n",fOutFileIndex+1);
  printf("RootIO::Exit - Total events written: %u\n",fOutEventsTotal);
  printf("RootIO::Exit - Total output size:    %lu\n",fOutSizeTotal);

  return ROOTIO_OK;
}

Int_t RootIO::ChangeOutFile()
{

  CloseOutFile();

  // Update file index and create new filename
  fOutFileIndex++;
  SetOutFile();

  return OpenOutFile();

}

Int_t RootIO::OpenOutFile()
{

  if (file_exists(fOutFile.Data())) {
    printf("RootIO::OpenOutFile - ERROR: file %s already exists\n",fOutFile.Data());
    return ROOTIO_ERROR;
  }

  printf("RootIO::OpenOutFile - Opening output file %s\n",fOutFile.Data());
  fTFileHandle->Open(fOutFile,"NEW","PADME Merged Raw Events");

  // Create TTree to hold raw events
  fTTreeMain = new TTree("RawEvents","PADME Raw Events Tree");

  // Attach branch to TRawEvent
  fTTreeMain->Branch("RawEvent",&fTRawEvent);

  // Reset event counter for this file
  fOutFileEvents = 0;

  // Register file in DB
  if (fDB) {
    int rc = fDB->OpenRawFile(fOutFileDBId,fConfig->MergerId(),fOutFile.Data(),fOutFileIndex);
    if (rc != DBSERVICE_OK) {
      printf("RootIO::OpenOutFile - ERROR while updating DB\n");
      return ROOTIO_ERROR;
    }
  }

  return ROOTIO_OK;

}

Int_t RootIO::CloseOutFile()
{

  printf("RootIO::CloseOutFile - Closing output file %s\n",fOutFile.Data());

  // Save TTree content
  fTTreeMain->Write();

  // Close output file
  fTFileHandle->Close();

  // Add size of this file to total (does not work)
  //Long64_t size = fTFileHandle->GetSize();
  //Long64_t bytes = fTFileHandle->GetBytesWritten();
  //printf("File has size %lld and bytes %lld\n",size,bytes);
  //fOutFileSize = fTFileHandle->GetSize();
  //fOutSizeTotal += fOutFileSize;
  fOutFileSize = 0;

  // Delete TTree used for this file
  delete fTTreeMain;

  // Update DB with file close information
  if (fDB) {
    int rc = fDB->CloseRawFile(fOutFileDBId,fOutFileEvents,fOutFileSize);
    if (rc != DBSERVICE_OK) {
      printf("RootIO::CloseOutFile - ERROR while updating DB\n");
      return ROOTIO_ERROR;
    }
  }

  return ROOTIO_OK;
}

Int_t RootIO::SetOutFile()
{
  //fOutFile.Form("%s_%03d.root",fOutFileTemplate.Data(),fOutFileIndex);
  fOutFile.Form("%s_%03d.root",fConfig->OutputFileHeader().c_str(),fOutFileIndex);
  return ROOTIO_OK;
}

//int RootIO::FillRawEvent(int runnr, int evtnr, std::vector<ADCBoard*>& boards)
int RootIO::FillRawEvent(int runnr, int evtnr, unsigned long int runtime, unsigned int trgmsk, unsigned int evtstatus, std::vector<ADCBoard*>& boards)
{

  // Emtpy event structure
  fTRawEvent->Clear("C");
  //printf("TRawEvent cleared\n");

  // Set run and event number
  fTRawEvent->SetRunNumber((Int_t)runnr);
  fTRawEvent->SetEventNumber((Int_t)evtnr);

  // Set event time since start of run
  fTRawEvent->SetEventRunTime((ULong64_t)runtime);

  // Set trigger mask
  fTRawEvent->SetEventTrigMask((UInt_t)trgmsk);

  // Set event status
  fTRawEvent->SetEventStatus((UInt_t)evtstatus);

  // Set system time at event creation
  TTimeStamp systime = TTimeStamp();
  fTRawEvent->SetEventAbsTime(systime);

  // Print event info when in verbose mode
  if (fVerbose >= 1) printf("RootIO - Run %d Event %d Clock %lu Time %s Trig 0x%08x Status 0x%08x\n",runnr,evtnr,runtime,systime.AsString(),trgmsk,evtstatus);

  // Loop over all ADC boards
  for(unsigned int b=0; b<boards.size(); b++) {

    //printf("TRawEvent creating board %d\n",b);
    TADCBoard* tBoard = fTRawEvent->AddADCBoard();
    //printf("TRawEvent board %d created with address %ld\n",b,(long)tBoard);

    // Save general board information for this event
    tBoard->SetBoardId            (boards[b]->Event()->GetBoardId());
    tBoard->SetBoardSN            (boards[b]->Event()->GetBoardSN());
    tBoard->SetLVDSPattern        (boards[b]->Event()->GetLVDSPattern());
    tBoard->SetBoardStatus        (boards[b]->Event()->GetBoardStatus());
    tBoard->SetGroupMask          (boards[b]->Event()->GetGroupMask());
    tBoard->SetEventCounter       (boards[b]->Event()->GetEventCounter());
    tBoard->SetEventTimeTag       (boards[b]->Event()->GetEventTimeTag());
    tBoard->Set0SuppAlgrtm        (boards[b]->Event()->Get0SuppAlgrtm());
    tBoard->SetActiveChannelMask  (boards[b]->Event()->GetActiveChannelMask());
    tBoard->SetAcceptedChannelMask(boards[b]->Event()->GetAcceptedChannelMask());
    //printf("TRawEvent board info saved\n");

    if (fVerbose >= 2)
      printf("RootIO - Board %d S/N %d LVDS 0x%04x Status 0x%03x GMask 0x%1x Event %u Time %u 0SupAlg %u ActiveCh 0x%08x AcceptCh 0x%08x\n",
	     tBoard->GetBoardId(),tBoard->GetBoardSN(),tBoard->GetLVDSPattern(),tBoard->GetBoardStatus(),tBoard->GetGroupMask(),
	     tBoard->GetEventCounter(),tBoard->GetEventTimeTag(),tBoard->Get0SuppAlgrtm(),tBoard->GetActiveChannelMask(),tBoard->GetAcceptedChannelMask());

    // Get mask of channel to save taking into account the zero suppression functioning mode
    unsigned int channelmask;
    if ( tBoard->GetBoardStatus() & (0x1 << 2) ) {
      channelmask = tBoard->GetActiveChannelMask(); // Zero suppression is in flagging mode
    } else {
      channelmask = tBoard->GetAcceptedChannelMask(); // Zero suppression is in rejection mode
    }

    // If a tigger group has no channels, do not save its trigger information
    unsigned char groupmask = 0;
    for(unsigned int t=0; t<ADCEVENT_NTRIGGERS; t++) {
      if ( channelmask & ( 0xff << (t*8) ) ) groupmask |= (0x1 << t);
    }

    // Save triggers information for this board
    for(unsigned int t=0; t<ADCEVENT_NTRIGGERS; t++) {
      //if ( boards[b]->Event()->GetGroupMask() & (0x1 << t) ) {
      //if ( tBoard->GetGroupMask() & (0x1 << t) ) {
      if ( groupmask & (0x1 << t) ) {
	TADCTrigger* tTrig = tBoard->AddADCTrigger();
	//printf("TRawEvent board %d trigger %d created with address %ld\n",b,t,(long)tTrig);
	tTrig->SetGroupNumber   (t);
	tTrig->SetStartIndexCell(boards[b]->Event()->GetTriggerStartIndexCell(t));
	tTrig->SetFrequency     (boards[b]->Event()->GetTriggerFrequency(t));
	tTrig->SetTriggerSignal (boards[b]->Event()->GetTriggerHasSignal(t));
	tTrig->SetTriggerTimeTag(boards[b]->Event()->GetTriggerTimeTag(t));
	if (fVerbose >= 3)
	  printf("RootIO - Board %u Group %u SIC %u Freq 0x%1x HasData 0x%1x TTT 0x%08x\n",
	  tBoard->GetBoardId(),tTrig->GetGroupNumber(),tTrig->GetStartIndexCell(),tTrig->GetFrequency(),tTrig->GetTriggerSignal(),tTrig->GetTriggerTimeTag());
	if ( boards[b]->Event()->GetTriggerHasSignal(t) ) {
	  for(unsigned int s=0; s<ADCEVENT_NSAMPLES; s++)
	    tTrig->SetSample(s,boards[b]->Event()->GetADCTriggerSample(t,s));
	  if (fVerbose >= 4) {
	    for(unsigned int s=0; s<ADCEVENT_NSAMPLES; s++) {
	      if (s%16 == 0) printf("\t");
	      printf("%4x ",tTrig->GetSample(s));
	      if (s%16 == 15) printf("\n");
	    }
	  }
	}
      }
    }

    // Save channels information for this board
    for(unsigned int c=0; c<ADCEVENT_NCHANNELS; c++) {
      if ( channelmask & (0x1 << c) ) {
	TADCChannel* tChan = tBoard->AddADCChannel();
	tChan->SetChannelNumber(c);
	for(unsigned int s=0; s<ADCEVENT_NSAMPLES; s++)
	  tChan->SetSample(s,boards[b]->Event()->GetADCChannelSample(c,s));
	if (fVerbose >= 4) {
	  printf("RootIO - Board %u Channel %u\n",tBoard->GetBoardId(),tChan->GetChannelNumber());
	  for(unsigned int s=0; s<ADCEVENT_NSAMPLES; s++) {
	    if (s%16 == 0) printf("\t");
	    printf("%4x ",tChan->GetSample(s));
	    if (s%16 == 15) printf("\n");
	  }
	}
      }
    }

  }

  // Write current event to file
  fTTreeMain->Fill();

  // Count event and see if we have to change file
  fOutEventsTotal++;
  fOutFileEvents++;
  if (fNMaxEvtsPerOutFile && (fOutFileEvents>=fNMaxEvtsPerOutFile)) {
    if (ChangeOutFile() == ROOTIO_ERROR) return ROOTIO_ERROR;
  }

  return ROOTIO_OK;

}
