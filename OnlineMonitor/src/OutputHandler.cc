#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <memory>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TError.h"
#include "TSystem.h"

#include "Configuration.hh"

#include "OutputHandler.hh"

OutputHandler::OutputHandler()
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Disable WARNING reporting from ROOT
  // Can be set to kPrint, kInfo, kWarning, kError, kBreak, kSysError, kFatal
  gErrorIgnoreLevel = kError; // Will only report kError, kBreak, kSysError, kFatal

  // Prepare output ROOT structures
  fTFileHandle = 0;
  fTTreeMain = 0;
  fTRawMergedEvent = new TRawMergedEvent();

}

OutputHandler::~OutputHandler()
{
  delete fTRawMergedEvent;
}

Int_t OutputHandler::Initialize()
{

  if (fConfig->Verbose()) printf("OutputHandler::Initialize - Start initialization.\n");

  // Initialize global counters
  fOutEventsTotal = 0;
  fOutSizeTotal = 0;

  // Reset vector with list of files
  fOutFileList.clear();

  // Initialize file counter
  fOutFileIndex = 0;

  // Set initial output filename
  fOutFileName = FormatFilename(fOutFileIndex);

  // Open first file in stream
  return OpenOutFile();

}

Int_t OutputHandler::Finalize()
{
  if (fConfig->Verbose()) printf("OutputHandler::Finalize - Finalizing output.\n");
  if (CloseOutFile() != 0) return -1;

  if (fConfig->Verbose()) {
    printf("OutputHandler::Finalize - Total files created:  %u\n",fOutFileIndex+1);
    printf("OutputHandler::Finalize - Total events written: %u\n",fOutEventsTotal);
    printf("OutputHandler::Finalize - Total output size:    %llu\n",fOutSizeTotal);
  }

  return 0;
}

Int_t OutputHandler::ChangeOutFile()
{

  if (CloseOutFile() != 0) return -1;

  // Update file index and create new filename
  fOutFileIndex++;
  fOutFileName = FormatFilename(fOutFileIndex);

  return OpenOutFile();

}

Int_t OutputHandler::OpenOutFile()
{

  if (FileExists(fOutFileName)) {
    printf("OutputHandler::OpenOutFile - ERROR: file %s already exists\n",fOutFileName.Data());
    return -1;
  }

  if (fConfig->Verbose() > 0) printf("OutputHandler::OpenOutFile - Opening output file %s\n",fOutFileName.Data());
  fTFileHandle = TFile::Open(fOutFileName.Data(),"NEW","PADME Merged Raw Events");
  if ( (!fTFileHandle) || fTFileHandle->IsZombie() ) {
    printf("OutputHandler::OpenOutFile - ERROR: cannot open file %s\n",fOutFileName.Data());
    if (fTFileHandle) delete fTFileHandle;
    return -1;
  }

  // Add new output file to list
  fOutFileList.push_back(fOutFileName);

  // Create TTree to hold raw events
  fTTreeMain = new TTree("RawMergedEvents","PADME Raw Merged Events Tree");

  // Attach branch to TRawEvent
  fTTreeMain->Branch("RawMergedEvent",&fTRawMergedEvent);

  // Reset event counter for this file
  fOutFileEvents = 0;

  return 0;

}

Int_t OutputHandler::CloseOutFile()
{

  if (fConfig->Verbose() > 0) printf("OutputHandler::CloseOutFile - Closing output file %s\n",fOutFileName.Data());

  // Switch back to output file top directory
  fTFileHandle->cd();

  // Save TTree content
  fTTreeMain->Write();

  // Close output file
  fTFileHandle->Close();

  // Add size of this file to total
  fOutFileSize = fTFileHandle->GetSize();
  if (fConfig->Verbose() > 0) printf("OutputHandler::CloseOutFile - Final file size is %lld bytes\n",fOutFileSize);
  fOutSizeTotal += fOutFileSize;

  // Delete file handle for this file
  delete fTFileHandle; // This takes also care of deleting the TTree

  return 0;
}

Int_t OutputHandler::WriteEvent(TRawEvent* rawEv, Chamber* chEv, Double_t timediff)//ULong64_t srsRunTime)
//Int_t OutputHandler::WriteEvent()
{
  fTRawMergedEvent->SetTRawEvent(rawEv);
  fTRawMergedEvent->MMInfo()->SetDaqTimeSec     (chEv->daqTimeSec);
  fTRawMergedEvent->MMInfo()->SetDaqTimeMicroSec(chEv->daqTimeMicroSec);
  fTRawMergedEvent->MMInfo()->SetSrsTimeStamp   (chEv->srsTimeStamp);
  fTRawMergedEvent->MMInfo()->SetSrsTrigger     (chEv->srsTrigger);
  //  fTRawMergedEvent->MMInfo()->SetSrsRunTime     (srsRunTime);
  fTRawMergedEvent->MMInfo()->SetRunTimeDiff    (timediff);

  //
  int nFiredStrips = (chEv->mmLayer)->size();
  int pointerToChannel[16][256];
  int nchannelsPerBoard[16];  
  for (int i=0; i<16; i++){
    nchannelsPerBoard[i] = 0;  
    for (int j=0; j<256; j++) pointerToChannel[i][j] = -1;
  }

  for (uint i=0; i<nFiredStrips; i++){
    int ilayer = chEv->mmLayer->at(i); // 0--7
    int istrip = chEv->mmStrip->at(i); // 1--512

    int boardSN = 2*ilayer;    
    if (istrip > 256) {
      boardSN += 1;
    }

    nchannelsPerBoard[boardSN]++;

    
    int channelid = (istrip-1)%256;
    if (pointerToChannel[boardSN][channelid] != -1) { // double channel fired
      std::cerr << "Strange number of samples " << nsamples << " different than 27 " << " row " << i << " board = " << boardSN << " layer " << ilayer << " strip " << istrip << endl;
      exit 1;
    }
      
    pointerToChannel[boardSN][channelid] = i;
  }

  for (int i=0; i<16; i++){
    if (nchannelsPerBoard[i] == 0) continue;

    int ilayer = i/2;   
    int boardid = ilayer;
    if (i%2) boardid += 8;
    
    TMMBoard* board = fTRawMergedEvent->AddMMBoard();
    board->SetBoardId(boardid);
    board->SetBoardSN(i);
    
    for (int j=0; j<256; j++){
      if (pointerToChannel[i][j] == -1) continue;
      
      TMMChannel* mmchan = board->AddMMChannel(); // assume that the same strip is NEVER fired twice in the event
      mmchan->SetChannelNumber(j);

      int ptrToFiredStrip = pointerToChannel[i][j];
      
      int nsamples = chEv->raw_q->at(ptrToFiredStrip).size();
      if (nsamples != TMMCHANNEL_NSAMPLES) {
	std::cerr << "Strange number of samples " << nsamples << " different than 27 " << " row " << pointerToChannel[i][j] << " board = " << i << " layer " << ilayer << " strip " << j << endl;
	exit 1;
      }
      
      for (int k = 0; k < nsamples; k++) mmchan->SetSample(k,chEv->raw_q->at(ptrToFiredStrip).at(k));
    }
  }

  //// Empty output event structure
  //fTRawEvent->Clear("C");

  //// Copy input event to output event (may apply zero suppression)
  //CopyTRawEvent(fTRawEvent,rawEv);

  // Write current event to file
  fTTreeMain->Fill();

  // Count event and see if we have to change file
  fOutEventsTotal++;
  fOutFileEvents++;
  if (fConfig->EventsPerFile() && (fOutFileEvents>=fConfig->EventsPerFile())) return ChangeOutFile();

  return 0;

}

/*
void OutputHandler::CopyTRawEvent(TRawEvent* outEv, TRawEvent* inEv)
{

  // Set run and event number
  outEv->SetRunNumber(inEv->GetRunNumber());
  outEv->SetEventNumber(inEv->GetEventNumber());

  // Save system time when event was merged
  outEv->SetEventAbsTime(inEv->GetEventAbsTime());

  // Set event time since start of run
  outEv->SetEventRunTime(inEv->GetEventRunTime());

  // Set trigger mask
  outEv->SetEventTrigMask(inEv->GetEventTrigMask());

  // Set event status
  outEv->SetEventStatus(inEv->GetEventStatus());

  // Set missing boards bit mask
  outEv->SetMissingADCBoards(inEv->GetMissingADCBoards());

  // Save low level trigger info to TTriggerInfo structure
  TTriggerInfo* inTrigInfo = inEv->TriggerInfo();
  TTriggerInfo* outTrigInfo = outEv->TriggerInfo();
  outTrigInfo->SetTriggerCounter(inTrigInfo->GetTriggerCounter());
  outTrigInfo->SetTriggerTime(inTrigInfo->GetTriggerTime());
  outTrigInfo->SetTriggerPattern(inTrigInfo->GetTriggerPattern());

  // Loop over all ADC boards
  for(Int_t b=0; b<(Int_t)inEv->GetNADCBoards(); b++) {

    TADCBoard* inBoard = inEv->ADCBoard(b);
    TADCBoard* outBoard = outEv->AddADCBoard();

    outBoard->SetBoardId            (inBoard->GetBoardId());
    outBoard->SetBoardSN            (inBoard->GetBoardSN());
    outBoard->SetLVDSPattern        (inBoard->GetLVDSPattern());
    outBoard->SetBoardStatus        (inBoard->GetBoardStatus());
    outBoard->SetGroupMask          (inBoard->GetGroupMask());
    outBoard->SetEventCounter       (inBoard->GetEventCounter());
    outBoard->SetEventTimeTag       (inBoard->GetEventTimeTag());
    outBoard->Set0SuppAlgrtm        (inBoard->Get0SuppAlgrtm());
    outBoard->SetActiveChannelMask  (inBoard->GetActiveChannelMask());
    outBoard->SetAcceptedChannelMask(inBoard->GetAcceptedChannelMask());

    // Save triggers information for this board
    for(Int_t t=0; t<(Int_t)inBoard->GetNADCTriggers(); t++) {

      TADCTrigger* inTrig = inBoard->ADCTrigger(t);
      TADCTrigger* outTrig = outBoard->AddADCTrigger();

      outTrig->SetGroupNumber   (inTrig->GetGroupNumber());
      outTrig->SetStartIndexCell(inTrig->GetStartIndexCell());
      outTrig->SetFrequency     (inTrig->GetFrequency());
      outTrig->SetTriggerSignal (inTrig->GetTriggerSignal());
      outTrig->SetTriggerTimeTag(inTrig->GetTriggerTimeTag());
      if (inTrig->GetTriggerSignal()) {
	for(Int_t s=0; s<(Int_t)inTrig->GetNSamples(); s++) outTrig->SetSample(s,inTrig->GetSample(s));
      }

    }

    // Save channels information for this board
    for(Int_t c=0; c<(Int_t)inBoard->GetNADCChannels(); c++) {

      TADCChannel* inChan = inBoard->ADCChannel(c);
      TADCChannel* outChan = outBoard->AddADCChannel();

      outChan->SetChannelNumber(inChan->GetChannelNumber());
      for(Int_t s=0; s<(Int_t)inChan->GetNSamples(); s++) outChan->SetSample(s,inChan->GetSample(s));

    }

  }

}
*/

TString OutputHandler::FormatFilename(UInt_t filenr)
{
  return TString::Format("%s/%s/%s_raw_%4.4d.root",
			 fConfig->OutputDirectory().Data(),
			 fConfig->RunName().Data(),
			 fConfig->RunName().Data(),
			 filenr);
}

Bool_t OutputHandler::FileExists(TString fileName)
{

  if (fConfig->Verbose() > 2) printf("OutputHandler::FileExists - Testing file %s\n",fileName.Data());

  Bool_t exists = true;
  if (fileName.BeginsWith("root:")) {

    // Remote file: try to open it and check if this succeeds
    Int_t oldErrorIgnoreLevel = gErrorIgnoreLevel;
    gErrorIgnoreLevel = kBreak; // Do not report "open file" errors
    std::unique_ptr<TFile> testTF(TFile::Open(fileName.Data(),"READ"));
    if (testTF == 0) exists = false;
    gErrorIgnoreLevel = oldErrorIgnoreLevel; // Go back to previous error-reporting behaviour

  } else {

    // Local file: use standard 'stat' call
    struct stat filestat;
    if ( stat(Form(fileName.Data()),&filestat) != 0 ) exists = false;

  }

  if (fConfig->Verbose() > 2) {
    if (exists) {
      printf("OutputHandler::FileExists - File %s exists\n",fileName.Data());
    } else {
      printf("OutputHandler::FileExists - File %s does not exist\n",fileName.Data());
    }
  }

  return exists;

}
