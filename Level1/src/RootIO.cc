#include <stdio.h>

#include "RootIO.hh"

RootIO::RootIO()
{

  // Create TFile handle
  fTFileHandle = new TFile();

  // Create TRawEvent object
  fTRawEvent = new TRawEvent();
}

RootIO::~RootIO()
{
  delete fTFileHandle;
  delete fTRawEvent;
}

int RootIO::Init(std::string outfiletemplate, int nevtsperfile)
{

  fNMaxEvtsPerOutFile = nevtsperfile;
  fOutFileTemplate = outfiletemplate;

  fOutFileIndex = 0;
  fOutEventsCounter = 0;
  fOutEventsTotal = 0;

  // Set initial output filename
  if (fNMaxEvtsPerOutFile == 0) {
    fOutFile.Form("%s.root",fOutFileTemplate.Data());
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

  printf("Level1 process ended\n");
  printf("Total events written: %ld on %d files\n",fOutEventsTotal,fOutFileIndex+1);

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

  return ROOTIO_OK;
}

Int_t RootIO::CloseOutFile()
{
  printf("RootIO::CloseOutFile - Closing output file %s\n",fOutFile.Data());

  // Save TTree content
  fTTreeMain->Write();

  // Close output file
  fTFileHandle->Close();

  // Delete TTree used for this file
  delete fTTreeMain;

  return ROOTIO_OK;
}

Int_t RootIO::SetOutFile()
{
  fOutFile.Form("%s_%03d.root",fOutFileTemplate.Data(),fOutFileIndex);
  return ROOTIO_OK;
}

int RootIO::FillRawEvent(int runnr, int evtnr, std::vector<ADCBoard*>& boards)
{

  // Emtpy event structure
  fTRawEvent->Clear("C");
  //printf("TRawEvent cleared\n");

  // Set run and event number
  fTRawEvent->SetRunNumber(runnr);
  fTRawEvent->SetEventNumber(evtnr);
  //printf("TRawEvent run/event set\n");

  // Loop over all ADC boards
  for(unsigned int b=0; b<boards.size(); b++) {

    //printf("TRawEvent creating board %d\n",b);
    TADCBoard* tBoard = fTRawEvent->AddADCBoard();
    //printf("TRawEvent board %d created with address %ld\n",b,(long)tBoard);

    // Save general board information for this event
    tBoard->SetBoardId            (boards[b]->Event()->GetBoardId());
    tBoard->SetLVDSPattern        (boards[b]->Event()->GetLVDSPattern());
    tBoard->SetStatus             (boards[b]->Event()->GetStatus());
    tBoard->SetGroupMask          (boards[b]->Event()->GetGroupMask());
    tBoard->SetEventCounter       (boards[b]->Event()->GetEventCounter());
    tBoard->SetEventTimeTag       (boards[b]->Event()->GetEventTimeTag());
    tBoard->SetActiveChannelMask  (boards[b]->Event()->GetActiveChannelMask());
    tBoard->SetAcceptedChannelMask(boards[b]->Event()->GetAcceptedChannelMask());
    //printf("TRawEvent board info saved\n");

    // Save triggers information for this board
    for(unsigned int t=0; t<ADCEVENT_NTRIGGERS; t++) {
      if ( boards[b]->Event()->GetGroupMask() & (0x1 << t) ) {
	TADCTrigger* tTrig = tBoard->AddADCTrigger();
	//printf("TRawEvent board %d trigger %d created with address %ld\n",b,t,(long)tTrig);
	tTrig->SetGroupNumber   (t);
	tTrig->SetStartIndexCell(boards[b]->Event()->GetTriggerStartIndexCell(t));
	tTrig->SetFrequency     (boards[b]->Event()->GetTriggerFrequency(t));
	tTrig->SetTriggerSignal (boards[b]->Event()->GetTriggerHasSignal(t));
	tTrig->SetTriggerTimeTag(boards[b]->Event()->GetTriggerTimeTag(t));
	if ( boards[b]->Event()->GetTriggerHasSignal(t) ) {
	  for(unsigned int s=0; s<ADCEVENT_NSAMPLES; s++)
	    tTrig->SetSample(s,boards[b]->Event()->GetADCTriggerSample(t,s));
	}
      }
    }

    // Save channels information for this board
    for(unsigned int c=0; c<ADCEVENT_NCHANNELS; c++) {
      if ( boards[b]->Event()->GetAcceptedChannelMask() & (0x1 << c) ) {
	TADCChannel* tChan = tBoard->AddADCChannel();
	//printf("TRawEvent board %d channel %d created with address %ld\n",b,c,(long)tChan);
	tChan->SetChannelNumber(c);
	for(unsigned int s=0; s<ADCEVENT_NSAMPLES; s++)
	  tChan->SetSample(s,boards[b]->Event()->GetADCChannelSample(c,s));
      }
    }

  }

  // Write current event to file
  fTTreeMain->Fill();

  // Count event and see if we have to change file
  fOutEventsTotal++;
  fOutEventsCounter++;
  if (fNMaxEvtsPerOutFile && (fOutEventsCounter>=fNMaxEvtsPerOutFile)) {
    if (ChangeOutFile() == ROOTIO_ERROR) return ROOTIO_ERROR;
    fOutEventsCounter = 0;
  }

  return ROOTIO_OK;

}
