#include <stdio.h>
#include <sys/stat.h>

#include "TChain.h"
#include "TString.h"

#include "Configuration.hh"
#include "TRawEvent.hh"

#include "InputHandler.hh"

InputHandler::InputHandler()
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

}

InputHandler::~InputHandler()
{
  for(UChar_t i=0; i<fConfig->NumberOfStreams(); i++) {
    delete fTChain[i];
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

    printf("Stream %d\n",i);

    fTRawEvent[i] = new TRawEvent();
    fTChain[i] = new TChain("RawEvents");
    fTChain[i]->SetBranchAddress("RawEvent",&fTRawEvent[i]);

    fCurrentFileInChain[i] = 0;
    fCurrentEventInChain[i] = 0;

    TString streamFilename = FormatFilename(i,fCurrentFileInChain[i]);
    //streamFilename.Form("%s/%s_lvl1_%2.2d_%3.3d.root",fConfig->DataDirectory().Data(),fConfig->FilenameTemplate().Data(),i,0);
    if (fConfig->Verbose()) printf("InputHandler - Filename %s\n",streamFilename.Data());
    if (! FileExists(streamFilename)) {
      printf("ERROR - InputHandler - File %s does not exist\n",streamFilename.Data());
      return 1;
    }
    fTChain[i]->AddFile(streamFilename.Data());
    fTotalEventsInChain[i] = fTChain[i]->GetEntries();
    if (fConfig->Verbose()) printf("InputHandler - Total events %d\n",fTotalEventsInChain[i]);

  }

  // Always start with stream 0
  fCurrentStream = fConfig->NumberOfStreams()-1;

  return 0;

}

TRawEvent* InputHandler::NextEvent()
{

  fCurrentStream++;
  if (fCurrentStream == fConfig->NumberOfStreams()) fCurrentStream = 0;

  if (fCurrentEventInChain[fCurrentStream] == fTotalEventsInChain[fCurrentStream]) {
    // We reached end of chain for current stream: see if the next file can be added to the chain
    fCurrentFileInChain[fCurrentStream]++;
    TString streamFilename = FormatFilename(fCurrentStream,fCurrentFileInChain[fCurrentStream]);
    if (fConfig->Verbose()) printf("InputHandler - Filename %s\n",streamFilename.Data());
    if (! FileExists(streamFilename)) {
      if (fConfig->Verbose()) printf("InputHandler - Reached end of stream %d\n",fCurrentStream);
      return 0;
    }
    fTChain[fCurrentStream]->AddFile(streamFilename.Data());
    fTotalEventsInChain[fCurrentStream] = fTChain[fCurrentStream]->GetEntries();
    if (fConfig->Verbose()) printf("InputHandler - Total events %d\n",fTotalEventsInChain[fCurrentStream]);
  }

  fTChain[fCurrentStream]->GetEntry(fCurrentEventInChain[fCurrentStream]);
  fCurrentEventInChain[fCurrentStream]++;
  fEventsRead++;
  return fTRawEvent[fCurrentStream];

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
  if ( stat(Form(fileName.Data()),&filestat) == 0 ) return 0;
  return 1;
}
