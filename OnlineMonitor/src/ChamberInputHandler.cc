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

#include "ChamberInputHandler.hh"

#define SIZE_THRESHOLD 100000

ChamberInputHandler::ChamberInputHandler()
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

  // Create structure to read in events
  fChamberEvent = new ChamberEvent();

}

ChamberInputHandler::~ChamberInputHandler()
{
  if (fChamberEvent) delete fChamberEvent;
}

Int_t ChamberInputHandler::Initialize()
{

  if (fConfig->Verbose()) printf("ChamberInputHandler::Initialize - Start initialization\n");

  // Initialize global event counter
  fEventsRead = 0;

  // Check if we are resuming monitor
  if (fConfig->FollowMode() && fConfig->ResumeMode()) {
    Int_t rc = Resume();
    if (rc == -1) {
      printf("ChamberInputHandler::Initialize - ERROR - Cannot resume monitoring\n");
      return -1;
    }
  } else {
    fCurrentFileInStream = 0;
  }

  // Initialize file

  if (fConfig->Verbose()) printf("ChamberInputHandler::Initialize - Initialize Chamber file\n");

  // Open first file in stream
  Int_t rc = OpenFileInStream(fCurrentFileInStream);
  if (rc == -1) {
    printf("ChamberInputHandler::Initialize - ERROR - Cannot initialize file\n");
    return -1;
  }
  if (rc == 1) {
    printf("ChamberInputHandler::Initialize - WARNING - Run ended while initializing file\n");
    return 1;
  }
  return 0;

}

Int_t ChamberInputHandler::Finalize()
{
  if (fConfig->Verbose()) printf("ChamberInputHandler::Finalize - Finalizing.\n");
  return 0;
}

ChamberEvent* ChamberInputHandler::NextEvent()
{

  if (fCurrentEventInFile == fTotalEventsInFile) {

    // Time to check if stop file has appeared
    if (FileExists(fConfig->StopFile())) {
      if (fConfig->Verbose()) printf("ChamberInputHandler::NextEvent - Stop file '%s' found: exiting\n",fConfig->StopFile().Data());
      if (std::remove(fConfig->StopFile().Data()) != 0) perror( "Error deleting stop file" );
      return 0;
    }

    if (fConfig->FollowMode()) {

      // First handle files which are being written to
      if (fCurrentFileIsOpen) {
	UInt_t rc = WaitForFileToGrow();
	if (rc == 1) {
	  if (fConfig->Verbose()>1) printf("ChamberInputHandler::NextEvent - Events in file now: %d\n",fTotalEventsInFile);
	  return ReadNextEvent(); // New events have appeared: read the next
	}
	if (rc == 2) {
	  // Open file did not grow and was not finalized for a long time: problem!
	  printf("ChamberInputHandler::NextEvent - WARNING - No new events for a long time: exiting\n");
	  return 0;
	}
      }

      // WaitForFileToGrow returned 0: file has been finalized and we read all events, time to close it
      delete fTFile;
      //fTFile->Close();

      // Wait for next file to appear
      UInt_t nextFileInStream = fCurrentFileInStream + 1;
      Int_t rc = OpenFileInStream(nextFileInStream);
      if (rc == -1 || rc == 1) {
	if (rc == -1) {
	  printf("ChamberInputHandler::NextEvent - WARNING - Cannot open next file: run has ended.\n");
	}
	if (rc == 1) {
	  if (fConfig->Verbose()) printf("ChamberInputHandler::NextEvent - End of run while reading file: exiting.\n");
	}
	return 0;
      }

      // If new file is still empty, wait for events
      if (fTotalEventsInFile == 0) {
	UInt_t rc = WaitForFileToGrow();
	if (rc == 0 || rc == 2) {
	  // File was finalized with 0 events: weird.
	  if (rc == 0) printf("ChamberInputHandler::NextEvent - WARNING - File opened and then closed with 0 events: exiting\n");
	  // Open file did not grow and was not finalized for a long time: problem!
	  if (rc == 2) printf("ChamberInputHandler::NextEvent - WARNING - No new events for a long time: exiting\n");
	  return 0;
	}
      }

    } else {

      // Not in FOLLOW mode: close old file then look for next file and exit if none is found
      delete fTFile;
      //fTFile->Close();
      UInt_t nextFileInStream = fCurrentFileInStream + 1;
      Int_t rc = OpenFileInStream(nextFileInStream);
      if (rc == -1) {
	if (fConfig->Verbose()) printf("ChamberInputHandler::NextEvent - reached end of stream: exiting.\n");
	return 0;
      }

    }

  }
  
  return ReadNextEvent();

}

ChamberEvent* ChamberInputHandler::ReadNextEvent() 
{
  fTTree->GetEntry(fCurrentEventInFile);
  fCurrentEventInFile++;
  fEventsRead++;
  return fChamberEvent;
}

Int_t ChamberInputHandler::WaitForFileToGrow()
{
  time_t start_time = time(0);
  while(true) {
    // Close and reopen file
    if (fTFile) { delete fTFile; fTFile = 0; }
    //fTFile->Close();
    TString streamFilename = FormatFilename(fCurrentFileInStream);
    if (fConfig->Verbose()>1) printf("ChamberInputHandler::WaitForFileToGrow - Opening file %s\n",streamFilename.Data());
    fTFile = new TFile(streamFilename,"READ");
    //fTFile->Open(streamFilename,"READ");
    //fTFile = TFile::Open(streamFilename.Data(),"READ");
    if (fTFile) {
      if (! fTFile->IsZombie()) {
	if (fTFile->TestBit(TFile::kRecovered)) {
	  fCurrentFileIsOpen = true;
	  if (fConfig->Verbose()>1) printf("ChamberInputHandler::WaitForFileToGrow - File %s is still being written\n",streamFilename.Data());
	} else {
	  fCurrentFileIsOpen = false;
	}
	gDirectory->GetObject("apv_raw",fTTree);
	UInt_t totEvents = fTTree->GetEntries();
	if (totEvents > fTotalEventsInFile) {
	  fTotalEventsInFile = totEvents;
	  if (fConfig->Verbose()>1) printf("ChamberInputHandler::WaitForFileToGrow - Events in file now: %d\n",fTotalEventsInFile);
	  fChamberEvent->Init(fTTree);
	  return 1; // 1: file has grown
	}
	if (! fCurrentFileIsOpen) return 0; // 0: file did not grow but was finalized
      } else {
	if (fConfig->Verbose()>1) printf("ChamberInputHandler::WaitForFileToGrow - File %s still a zombie: wait and retry\n",streamFilename.Data());
      }
    } else {
      printf("ChamberInputHandler::WaitForFileToGrow - WARNING - File '%s' could not be reopened.\n",streamFilename.Data());
    }
    // File is either a zombie or did not grow and was not finalized: sleep and retry
    if (difftime(time(0),start_time) > fWaitTimeout) return 2; // 2: file did not grow for a long time
    sleep(fSleepPeriod);
  }
}

Int_t ChamberInputHandler::OpenFileInStream(UInt_t filenr)
{

  time_t start_time;

  // Get full filename
  TString streamFilename = FormatFilename(filenr);

  // Wait for file to appear
  start_time = time(0);
  while(true) {
    if (FileExists(streamFilename)) break;
    if (! fConfig->FollowMode()) return -1; // In NORMAL mode this means we are at end of stream
    if (FileExists(fConfig->StopFile())) {
      // Run has ended: we can gracefully exit
      if (fConfig->Verbose()) printf("ChamberInputHandler::OpenFileInStream - Stop file %s found\n",fConfig->StopFile().Data());
      if (std::remove(fConfig->StopFile().Data()) != 0) perror( "Error deleting stop file" );
      return 1;
    }
    if (difftime(time(0),start_time) > fWaitTimeout) {
      // New file did not appear after a long time: problem!
      printf("ChamberInputHandler::OpenFileInStream - WARNING - New file '%s' not created for a long time: exiting.\n",streamFilename.Data());
      return -1;
    }
    sleep(fSleepPeriod); // File is still empty: sleep and retry
  }
  if (fConfig->Verbose()) printf("ChamberInputHandler::OpenFileInStream - Filename %s\n",streamFilename.Data());

  // Wait for file to be written to
  start_time = time(0);
  while(true) {
    // If file is local check that at least some data were written
    if ( streamFilename.BeginsWith("root:") || (GetLocalFileSize(streamFilename) > SIZE_THRESHOLD) ) {
      fTFile = new TFile(streamFilename,"READ");
      //fTFile->Open(streamFilename,"READ");
      //fTFile = TFile::Open(streamFilename.Data(),"READ");
      if (fTFile) {
	if (! fTFile->IsZombie()) break;
	// File is still a zombie: close it and wait
	if (fConfig->Verbose()>1) printf("ChamberInputHandler::OpenFileInStream - File %s still a zombie: close and wait\n",streamFilename.Data());
	delete fTFile; fTFile = 0;
	//fTFile->Close();
	if (! fConfig->FollowMode()) return -1; // Zombie file in NORMAL mode?
      } else {
	printf("ChamberInputHandler::OpenFileInStream - WARNING - New file '%s' was created but could not be opened as a TFile.\n",streamFilename.Data());
      }
    } else {
      if (fConfig->Verbose()>1) printf("ChamberInputHandler::OpenFileInStream - File %s size still small: %lld\n",streamFilename.Data(),GetLocalFileSize(streamFilename));
    }
    if (FileExists(fConfig->StopFile())) {
      // Run has ended: we can gracefully exit
      if (fConfig->Verbose()) printf("ChamberInputHandler::OpenFileInStream - Stop file %s found\n",fConfig->StopFile().Data());
      if (std::remove(fConfig->StopFile().Data()) != 0) perror( "Error deleting stop file" );
      return 1;
    }
    if (difftime(time(0),start_time) > fWaitTimeout) {
      // New file stays empty for a long time: problem!
      printf("ChamberInputHandler::OpenFileInStream - WARNING - New file '%s' created but not filled for a long time: exiting.\n",streamFilename.Data());
      return -1;
    }
    sleep(fSleepPeriod); // File is still empty: sleep and retry
  }

  // File is ready to be read
  fCurrentFileInStream = filenr;

  // Check if file is currently been written to
  if (fTFile->TestBit(TFile::kRecovered)) {
    fCurrentFileIsOpen = true;
    if (fConfig->Verbose()>1) printf("ChamberInputHandler::OpenFileInStream - File %s is still being written\n",streamFilename.Data());
  } else {
    fCurrentFileIsOpen = false;
  }
  
  // Define Tree to read events
  //TFile* f = new TFile("run30.root");
  //f->GetObject("apv_raw",tree);
  //Init(tree);
  gDirectory->GetObject("apv_raw",fTTree);
  fChamberEvent->Init(fTTree);

  // Get file information
  fTotalEventsInFile = fTTree->GetEntries();
  if (fConfig->Verbose()>1) printf("ChamberInputHandler::OpenFileInStream - Events in file: %d\n",fTotalEventsInFile);

  // Initialize event counter for this file
  fCurrentEventInFile = 0;

  return 0;

}

Int_t ChamberInputHandler::Resume()
{
  // Look for last currently available file
  UInt_t filenr = 0;
  while (true) {
    TString streamFilename = FormatFilename(filenr);
    if (! FileExists(streamFilename)) return 0; // Stop as soon as one stream file is not found
    fCurrentFileInStream = filenr;
    filenr++;
  }
}

TString ChamberInputHandler::FormatFilename(UInt_t filenr)
{
  if (filenr == 0) {
    return TString::Format("%s/%s.root",fConfig->DataDirectory().Data(),fConfig->RunName().Data());
  } else {
    return TString::Format("%s/%s_%d.root",fConfig->DataDirectory().Data(),fConfig->RunName().Data(),filenr);
  }
}

Bool_t ChamberInputHandler::FileExists(TString fileName)
{

  if (fConfig->Verbose() > 2) printf("ChamberInputHandler::FileExists - Testing file %s\n",fileName.Data());

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

  if (fConfig->Verbose() > 1) {
    if (exists) {
      printf("ChamberInputHandler::FileExists - File %s exists\n",fileName.Data());
    } else {
      printf("ChamberInputHandler::FileExists - File %s does not exist\n",fileName.Data());
    }
  }

  return exists;

}

Long64_t ChamberInputHandler::GetLocalFileSize(TString fileName)
{

  //if (fConfig->Verbose() > 1) printf("ChamberInputHandler::GetLocalFileSize - Testing size of file %s\n",fileName.Data());

  Long64_t fileSize = 0;

  // Get size of local file if it exists
  struct stat filestat;
  if ( stat(Form(fileName.Data()),&filestat) == 0 ) fileSize = filestat.st_size;

  if (fConfig->Verbose() > 1) printf("ChamberInputHandler::GetLocalFileSize - File %s has size %lld\n",fileName.Data(),fileSize);

  return fileSize;

}
