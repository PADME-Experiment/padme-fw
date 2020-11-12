#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"

#include "Configuration.hh"
#include "TRawEvent.hh"

#include "InputHandler.hh"

InputHandler::InputHandler()
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Number of seconds to sleep between checks for new events
  fSleepPeriod = 1;

  // Time interval after which WaitForFileToGrow returns
  fWaitTimeout = 60.;

}

InputHandler::~InputHandler()
{
  for(UChar_t i=0; i<fConfig->NumberOfStreams(); i++) {
    delete fTRawEvent[i];
  }
}

Int_t InputHandler::Initialize()
{

  printf("Initializing\n");

  // Initialize global event counter
  fEventsRead = 0;

  // Initialize streams
  for(UChar_t i=0; i<fConfig->NumberOfStreams(); i++) {

    if (fConfig->Verbose()) printf("InputHandler - Initialize stream %d\n",i);

    fTRawEvent[i] = new TRawEvent();

    fCurrentFileInStream[i] = 0;
    fCurrentEventInFile[i] = 0;

    // Open first file of stream and check if it still open
    TString streamFilename = FormatFilename(i,fCurrentFileInStream[i]);
    if (fConfig->Verbose()) printf("InputHandler - Filename %s\n",streamFilename.Data());
    if (! FileExists(streamFilename)) {
      printf("ERROR - InputHandler - File %s does not exist\n",streamFilename.Data());
      return 1;
    }
    fTFile[i] = new TFile(streamFilename,"READ");
    // If in FOLLOW mode, check if file is currently been written to
    if (fConfig->FollowMode()) {
      if (fTFile[i]->TestBit(TFile::kRecovered)) {
	fCurrentFileIsOpen[i] = true;
      } else {
	fCurrentFileIsOpen[i] = false;
      }
    }

    gDirectory->GetObject("RawEvents",fTTree[i]);
    fTTree[i]->SetBranchAddress("RawEvent",&fTRawEvent[i]);

    fTotalEventsInFile[i] = fTTree[i]->GetEntries();
    if (fConfig->Verbose()) printf("InputHandler - Total events %d\n",fTotalEventsInFile[i]);

  }

  // Always start with stream 0
  fCurrentStream = fConfig->NumberOfStreams()-1;

  return 0;

}

TRawEvent* InputHandler::NextEvent()
{

  fCurrentStream++;
  if (fCurrentStream == fConfig->NumberOfStreams()) fCurrentStream = 0;

  if (fCurrentEventInFile[fCurrentStream] == fTotalEventsInFile[fCurrentStream]) {

    if (fConfig->FollowMode()) {

      // First handle files which are being written to
      if (fCurrentFileIsOpen[fCurrentStream]) {
	UInt_t rc = WaitForFileToGrow();
	if (rc == 1) return ReadNextEvent(); // New events have appeared: read the next
	if (rc == 2) {
	  // Open file did not grow and was not finalized for a long time: problem!
	  printf("InputHandler - WARNING - No new events for a long time: exiting\n");
	  return 0;
	}
      }

      // File has been finalized and we read all events: close it
      delete fTFile[fCurrentStream];

      // Wait for next file to appear while checking if run has ended
      UInt_t nextFileInStream = fCurrentFileInStream[fCurrentStream] + 1;
      TString streamFilename = FormatFilename(fCurrentStream,nextFileInStream);
      time_t start_time = time(0);
      while (! FileExists(streamFilename)) {
	if (FileExists(fConfig->StopFile())) {
	  // Run has ended: we can gracefully exit
	  if (fConfig->Verbose()) printf("InputHandler - Stop file %s found\n",fConfig->StopFile().Data());
	  return 0;
	}
	if (difftime(time(0),start_time) > fWaitTimeout) {
	  // No new file appeared for a long time: problem!
	  printf("InputHandler - WARNING - New file '%s' did not appear for a long time: exiting.\n",streamFilename.Data());
	  return 0;
	}
	sleep(fSleepPeriod);
      }

      // Next file is there: open it and start reading
      fCurrentFileInStream[fCurrentStream] = nextFileInStream;
      fTFile[fCurrentStream] = new TFile(streamFilename,"READ");
      if (fTFile[fCurrentStream]->TestBit(TFile::kRecovered)) {
	fCurrentFileIsOpen[fCurrentStream] = true;
      } else {
	fCurrentFileIsOpen[fCurrentStream] = false;
      }
      gDirectory->GetObject("RawEvents",fTTree[fCurrentStream]);
      fTTree[fCurrentStream]->SetBranchAddress("RawEvent",&fTRawEvent[fCurrentStream]);
      if (fConfig->Verbose()) printf("InputHandler - Filename %s\n",streamFilename.Data());
      fTotalEventsInFile[fCurrentStream] = fTTree[fCurrentStream]->GetEntries();
      fCurrentEventInFile[fCurrentStream] = 0;

      // Check if file is still empty and wait for new events
      if (fTotalEventsInFile[fCurrentStream] == 0) {
	UInt_t rc = WaitForFileToGrow();
	if (rc == 0) {
	  // File was finalized with 0 events: weird.
	  printf("InputHandler - WARNING - File opened and then close with 0 events: exiting\n");
	  return 0;
	}
	if (rc == 2) {
	  // Open file did not grow and was not finalized for a long time: problem!
	  printf("InputHandler - WARNING - No new events for a long time: exiting\n");
	  return 0;
	}
      }

    } else {

      // Not in FOLLOW mode: close old file (delete) then look for next file and exit if none is found
      delete fTFile[fCurrentStream];
      fCurrentFileInStream[fCurrentStream]++;
      TString streamFilename = FormatFilename(fCurrentStream,fCurrentFileInStream[fCurrentStream]);
      if (! FileExists(streamFilename)) {
	if (fConfig->Verbose()) printf("InputHandler - Reached end of stream %d\n",fCurrentStream);
	return 0;
      }
      fTFile[fCurrentStream] = new TFile(streamFilename,"READ");
      gDirectory->GetObject("RawEvents",fTTree[fCurrentStream]);
      fTTree[fCurrentStream]->SetBranchAddress("RawEvent",&fTRawEvent[fCurrentStream]);
      if (fConfig->Verbose()) printf("InputHandler - Filename %s\n",streamFilename.Data());
      fTotalEventsInFile[fCurrentStream] = fTTree[fCurrentStream]->GetEntries();
      if (fConfig->Verbose()) printf("InputHandler - Total events %d\n",fTotalEventsInFile[fCurrentStream]);
      fCurrentEventInFile[fCurrentStream] = 0;

    }

  }
  
  return ReadNextEvent();

}

TRawEvent* InputHandler::ReadNextEvent() 
{
  fTTree[fCurrentStream]->GetEntry(fCurrentEventInFile[fCurrentStream]);
  fCurrentEventInFile[fCurrentStream]++;
  fEventsRead++;
  return fTRawEvent[fCurrentStream];
}

UInt_t InputHandler::WaitForFileToGrow()
{
  time_t start_time = time(0);
  while(true) {
    // Close and reopen file
    delete fTFile[fCurrentStream];
    TString streamFilename = FormatFilename(fCurrentStream,fCurrentFileInStream[fCurrentStream]);
    fTFile[fCurrentStream] = new TFile(streamFilename,"READ");
    if (fTFile[fCurrentStream]->TestBit(TFile::kRecovered)) {
      fCurrentFileIsOpen[fCurrentStream] = true;
    } else {
      fCurrentFileIsOpen[fCurrentStream] = false;
    }
    gDirectory->GetObject("RawEvents",fTTree[fCurrentStream]);
    fTTree[fCurrentStream]->SetBranchAddress("RawEvent",&fTRawEvent[fCurrentStream]);
    UInt_t totEvents = fTTree[fCurrentStream]->GetEntries();
    if (totEvents > fTotalEventsInFile[fCurrentStream]) return 1; // 1: file has grown
    if (! fCurrentFileIsOpen[fCurrentStream]) return 0; // 0: file did not grow but was finalized
    if (difftime(time(0),start_time) > fWaitTimeout) return 2; // 2: file did not grow for a long time
    sleep(fSleepPeriod); // File has not grown and was not finalized: sleep and retry
  }
}

TString InputHandler::FormatFilename(UChar_t stream,UInt_t filenr)
{
  return TString::Format("%s/%s/%s_lvl1_%2.2d_%3.3d.root",
			 fConfig->DataDirectory().Data(),
			 fConfig->RunName().Data(),
			 fConfig->RunName().Data(),
			 stream,filenr);
}

Int_t InputHandler::FileExists(TString fileName)
{
  struct stat filestat;
  if ( stat(Form(fileName.Data()),&filestat) == 0 ) return 1;
  return 0;
}
