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

  // Initialize streams
  for(UChar_t stream=0; stream<fConfig->NumberOfStreams(); stream++) {

    if (fConfig->Verbose()) printf("InputHandler::Initialize - Initialize stream %d\n",stream);

    // Start with first file for each stream
    fCurrentFileInStream[stream] = 0;
    
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

    // We got to last event in current file: look for next file and exit if none is found
    delete fTFile[fCurrentStream];
    //fTFile[fCurrentStream]->Close();
    UInt_t nextFileInStream = fCurrentFileInStream[fCurrentStream] + 1;
    Int_t rc = OpenFileInStream(fCurrentStream,nextFileInStream);
    if (rc == -1) {
      if (fConfig->Verbose()) printf("InputHandler::NextEvent - reached end of stream %d: exiting.\n",fCurrentStream);
      return 0;
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

Int_t InputHandler::OpenFileInStream(UChar_t stream, UInt_t filenr)
{

  // Get full filename
  TString streamFilename = FormatFilename(stream,filenr);

  // Check if file exists
  if (! FileExists(streamFilename)) return -1; // File does not exist: nothing to open
  if (fConfig->Verbose()) printf("InputHandler::OpenFileInStream - Filename %s\n",streamFilename.Data());

  //fTFile[stream] = new TFile(streamFilename,"READ");
  //fTFile[stream]->Open(streamFilename,"READ");
  fTFile[stream] = TFile::Open(streamFilename.Data(),"READ");
  if (! fTFile[stream]) {
    printf("InputHandler::OpenFileInStream - WARNING - New file '%s' exists but could not be opened as a TFile.\n",streamFilename.Data());
  }

  // File is ready to be read
  fCurrentFileInStream[stream] = filenr;
 
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
