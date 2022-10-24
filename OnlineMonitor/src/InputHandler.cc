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
#include "TRawEvent.hh"

#include "InputHandler.hh"

#define SIZE_THRESHOLD 100000

InputHandler::InputHandler()
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Number of seconds to sleep between checks for new events
  fSleepPeriod = 5;

  // Time interval after which WaitForFileToGrow returns
  fWaitTimeout = 120.;

  // Disable WARNING reporting from ROOT
  // Can be set to kPrint, kInfo, kWarning, kError, kBreak, kSysError, kFatal
  gErrorIgnoreLevel = kError; // Will only report kError, kBreak, kSysError, kFatal

}

InputHandler::~InputHandler()
{
  for(UChar_t i=0; i<fConfig->NumberOfStreams(); i++) {
    delete fTRawEvent[i];
  }
}

Int_t InputHandler::Initialize()
{

  if (fConfig->Verbose()) printf("InputHandler::Initialize - Start initialization\n");

  // Initialize global event counter
  fEventsRead = 0;

  // Check if we are resuming monitor
  if (fConfig->FollowMode() && fConfig->ResumeMode()) {
    Int_t rc = Resume();
    if (rc == -1) {
      printf("InputHandler::Initialize - ERROR - Cannot resume monitoring\n");
      return -1;
    }
  } else {
    for(UChar_t stream=0; stream<fConfig->NumberOfStreams(); stream++) {
      fCurrentFileInStream[stream] = 0;
    }
  }

  // Initialize streams
  for(UChar_t stream=0; stream<fConfig->NumberOfStreams(); stream++) {

    if (fConfig->Verbose()) printf("InputHandler::Initialize - Initialize stream %d\n",stream);

    // Create TFile structure for this stream
    //fTFile[stream] = new TFile();

    // Create TRawEvent structure for this stream
    fTRawEvent[stream] = new TRawEvent();

    // Open first file in stream
    Int_t rc = OpenFileInStream(stream,fCurrentFileInStream[stream]);
    if (rc == -1) {
      printf("InputHandler::Initialize - ERROR - Cannot initialize stream %d\n",stream);
      return -1;
    }
    if (rc == 1) {
      printf("InputHandler::Initialize - WARNING - Run ended while initializing stream %d\n",stream);
      return -1;
    }

  }

  // Always start to read from stream 0
  fCurrentStream = fConfig->NumberOfStreams()-1;

  return 0;

}

Int_t InputHandler::Finalize()
{
  if (fConfig->Verbose()) printf("InputHandler::Finalize - Finalizing.\n");
  return 0;
}

TRawEvent* InputHandler::NextEvent()
{

  fCurrentStream++;
  if (fCurrentStream == fConfig->NumberOfStreams()) fCurrentStream = 0;

  if (fCurrentEventInFile[fCurrentStream] == fTotalEventsInFile[fCurrentStream]) {

    // Time to check if stop file has appeared
    if (FileExists(fConfig->StopFile())) {
      if (fConfig->Verbose()) printf("InputHandler::NextEvent - Stop file '%s' found: exiting\n",fConfig->StopFile().Data());
      if (std::remove(fConfig->StopFile().Data()) != 0) perror( "Error deleting stop file" );
      return 0;
    }

    if (fConfig->FollowMode()) {

      // First handle files which are being written to
      if (fCurrentFileIsOpen[fCurrentStream]) {
	UInt_t rc = WaitForFileToGrow();
	if (rc == 1) {
	  if (fConfig->Verbose()) printf("InputHandler::NextEvent - Events in file now: %d\n",fTotalEventsInFile[fCurrentStream]);
	  return ReadNextEvent(); // New events have appeared: read the next
	}
	if (rc == 2) {
	  // Open file did not grow and was not finalized for a long time: problem!
	  printf("InputHandler::NextEvent - WARNING - No new events for a long time: exiting\n");
	  return 0;
	}
      }

      // File has been finalized and we read all events: close it
      delete fTFile[fCurrentStream];
      //fTFile[fCurrentStream]->Close();

      // Wait for next file to appear
      UInt_t nextFileInStream = fCurrentFileInStream[fCurrentStream] + 1;
      Int_t rc = OpenFileInStream(fCurrentStream,nextFileInStream);
      if (rc == -1) {
	printf("InputHandler::NextEvent - WARNING - Cannot open next file for stream %d: exiting.\n",fCurrentStream);
	return 0;
      }
      if (rc == 1) {
	if (fConfig->Verbose()) printf("InputHandler::NextEvent - End of run while reading stream %d: exiting.\n",fCurrentStream);
	return 0;
      }

      // If new file is still empty, wait for events
      if (fTotalEventsInFile[fCurrentStream] == 0) {
	UInt_t rc = WaitForFileToGrow();
	if (rc == 0) {
	  // File was finalized with 0 events: weird.
	  printf("InputHandler::NextEvent - WARNING - File opened and then closed with 0 events: exiting\n");
	  return 0;
	}
	if (rc == 2) {
	  // Open file did not grow and was not finalized for a long time: problem!
	  printf("InputHandler::NextEvent - WARNING - No new events for a long time: exiting\n");
	  return 0;
	}
      }

    } else {

      // Not in FOLLOW mode: close old file then look for next file and exit if none is found
      delete fTFile[fCurrentStream];
      //fTFile[fCurrentStream]->Close();
      UInt_t nextFileInStream = fCurrentFileInStream[fCurrentStream] + 1;
      Int_t rc = OpenFileInStream(fCurrentStream,nextFileInStream);
      if (rc == -1) {
	if (fConfig->Verbose()) printf("InputHandler::NextEvent - reached end of stream %d: exiting.\n",fCurrentStream);
	return 0;
      }

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

Int_t InputHandler::WaitForFileToGrow()
{
  time_t start_time = time(0);
  while(true) {
    // Close and reopen file
    if (fTFile[fCurrentStream]) { delete fTFile[fCurrentStream]; fTFile[fCurrentStream] = 0; }
    //fTFile[fCurrentStream]->Close();
    TString streamFilename = FormatFilename(fCurrentStream,fCurrentFileInStream[fCurrentStream]);
    fTFile[fCurrentStream] = new TFile(streamFilename,"READ");
    //fTFile[fCurrentStream]->Open(streamFilename,"READ");
    //fTFile[fCurrentStream] = TFile::Open(streamFilename.Data(),"READ");
    if (fTFile[fCurrentStream]) {
      if (! fTFile[fCurrentStream]->IsZombie()) {
	if (fTFile[fCurrentStream]->TestBit(TFile::kRecovered)) {
	  fCurrentFileIsOpen[fCurrentStream] = true;
	} else {
	  fCurrentFileIsOpen[fCurrentStream] = false;
	}
	gDirectory->GetObject("RawEvents",fTTree[fCurrentStream]);
	fTTree[fCurrentStream]->SetBranchAddress("RawEvent",&fTRawEvent[fCurrentStream]);
	UInt_t totEvents = fTTree[fCurrentStream]->GetEntries();
	if (totEvents > fTotalEventsInFile[fCurrentStream]) {
	  fTotalEventsInFile[fCurrentStream] = totEvents;
	  return 1; // 1: file has grown
	}
	if (! fCurrentFileIsOpen[fCurrentStream]) return 0; // 0: file did not grow but was finalized
      }
    } else {
      printf("InputHandler::WaitForFileToGrow - WARNING - File '%s' could not be reopened.\n",streamFilename.Data());
    }
    // File is either a zombie or did not grow and was not finalized: sleep and retry
    if (difftime(time(0),start_time) > fWaitTimeout) return 2; // 2: file did not grow for a long time
    sleep(fSleepPeriod);
  }
}

Int_t InputHandler::OpenFileInStream(UChar_t stream, UInt_t filenr)
{

  time_t start_time;

  // Get full filename
  TString streamFilename = FormatFilename(stream,filenr);

  // Wait for file to appear
  start_time = time(0);
  while(true) {
    if (FileExists(streamFilename)) break;
    if (! fConfig->FollowMode()) return -1; // In NORMAL mode this means we are at end of stream
    if (FileExists(fConfig->StopFile())) {
      // Run has ended: we can gracefully exit
      if (fConfig->Verbose()) printf("InputHandler::OpenFileInStream - Stop file %s found\n",fConfig->StopFile().Data());
      if (std::remove(fConfig->StopFile().Data()) != 0) perror( "Error deleting stop file" );
      return 1;
    }
    if (difftime(time(0),start_time) > fWaitTimeout) {
      // New file did not appear after a long time: problem!
      printf("InputHandler::OpenFileInStream - WARNING - New file '%s' not created for a long time: exiting.\n",streamFilename.Data());
      return -1;
    }
    sleep(fSleepPeriod); // File is still empty: sleep and retry
  }
  if (fConfig->Verbose()) printf("InputHandler::OpenFileInStream - Filename %s\n",streamFilename.Data());

  // Wait for file to be written to
  start_time = time(0);
  while(true) {
    // If file is local check that at least some data were written
    if ( streamFilename.BeginsWith("root:") || (GetLocalFileSize(streamFilename) > SIZE_THRESHOLD) ) {
      fTFile[stream] = new TFile(streamFilename,"READ");
      //fTFile[stream]->Open(streamFilename,"READ");
      //fTFile[stream] = TFile::Open(streamFilename.Data(),"READ");
      if (fTFile[stream]) {
	if (! fTFile[stream]->IsZombie()) break;
	// File is still a zombie: close it and wait
	delete fTFile[stream]; fTFile[stream] = 0;
	//fTFile[stream]->Close();
	if (! fConfig->FollowMode()) return -1; // Zombie file in NORMAL mode?
      } else {
	printf("InputHandler::OpenFileInStream - WARNING - New file '%s' was created but could not be opened as a TFile.\n",streamFilename.Data());
      }
    }
    if (FileExists(fConfig->StopFile())) {
      // Run has ended: we can gracefully exit
      if (fConfig->Verbose()) printf("InputHandler::OpenFileInStream - Stop file %s found\n",fConfig->StopFile().Data());
      if (std::remove(fConfig->StopFile().Data()) != 0) perror( "Error deleting stop file" );
      return 1;
    }
    if (difftime(time(0),start_time) > fWaitTimeout) {
      // New file stays empty for a long time: problem!
      printf("InputHandler::OpenFileInStream - WARNING - New file '%s' created but not filled for a long time: exiting.\n",streamFilename.Data());
      return -1;
    }
    sleep(fSleepPeriod); // File is still empty: sleep and retry
  }

  // File is ready to be read
  fCurrentFileInStream[stream] = filenr;

  // Check if file is currently been written to
  if (fTFile[stream]->TestBit(TFile::kRecovered)) {
    fCurrentFileIsOpen[stream] = true;
  } else {
    fCurrentFileIsOpen[stream] = false;
  }
  
  // Define Tree to read events
  gDirectory->GetObject("RawEvents",fTTree[stream]);
  fTTree[stream]->SetBranchAddress("RawEvent",&fTRawEvent[stream]);

  // Get file information
  fTotalEventsInFile[stream] = fTTree[stream]->GetEntries();
  if (fConfig->Verbose()) printf("InputHandler::OpenFileInStream - Events in file: %d\n",fTotalEventsInFile[stream]);

  // Initialize event counter for this file
  fCurrentEventInFile[stream] = 0;

  return 0;

}

Int_t InputHandler::Resume()
{
  // Set all streams to last currently available file
  UInt_t filenr = 0;
  while (true) {
    for(UChar_t stream=0; stream<fConfig->NumberOfStreams(); stream++) {
      TString streamFilename = FormatFilename(stream,filenr);
      if (! FileExists(streamFilename)) return 0; // Stop as soon as one stream file is not found
      fCurrentFileInStream[stream] = filenr;
    }
    filenr++;
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

Bool_t InputHandler::FileExists(TString fileName)
{

  if (fConfig->Verbose() > 2) printf("InputHandler::FileExists - Testing file %s\n",fileName.Data());

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
      printf("InputHandler::FileExists - File %s exists\n",fileName.Data());
    } else {
      printf("InputHandler::FileExists - File %s does not exist\n",fileName.Data());
    }
  }

  return exists;

}

Int_t InputHandler::GetLocalFileSize(TString fileName)
{

  if (fConfig->Verbose() > 2) printf("InputHandler::GetLocalFileSize - Testing size of file %s\n",fileName.Data());

  Int_t fileSize = 0;

  // Get size of local file if it exists
  struct stat filestat;
  if ( stat(Form(fileName.Data()),&filestat) == 0 ) fileSize = filestat.st_size;

  if (fConfig->Verbose() > 2) printf("InputHandler::GetLocalFileSize - File %s has size %d\n",fileName.Data(),fileSize);

  return fileSize;

}
