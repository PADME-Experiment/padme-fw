#ifndef InputHandler_H
#define InputHandler_H

class Configuration;

class TFile;
class TTree;
class TRawEvent;

class InputHandler
{

public:

  InputHandler();
  ~InputHandler();

public:

  Int_t Initialize();
  Int_t Finalize();

  TRawEvent* NextEvent();

  UInt_t EventsRead() { return fEventsRead; }
  UInt_t EventNumber() { return fEventsRead-1; } // Event number starts at 0

private:

  // Connection to configuration class
  Configuration* fConfig;

  Int_t Resume();
  TRawEvent* ReadNextEvent();
  Int_t WaitForFileToGrow();
  Int_t OpenFileInStream(UChar_t,UInt_t);
  TString FormatFilename(UChar_t,UInt_t);
  Bool_t FileExists(TString);
  Int_t GetLocalFileSize(TString);

  // Each stream uses a different TFile and a different TRawEvent
  TFile* fTFile[CONFIGURATION_N_STREAMS_MAX];
  TTree* fTTree[CONFIGURATION_N_STREAMS_MAX];
  UInt_t fTotalEventsInFile[CONFIGURATION_N_STREAMS_MAX];
  UInt_t fCurrentEventInFile[CONFIGURATION_N_STREAMS_MAX];
  UInt_t fCurrentFileInStream[CONFIGURATION_N_STREAMS_MAX];
  Bool_t fCurrentFileIsOpen[CONFIGURATION_N_STREAMS_MAX];

  TRawEvent* fTRawEvent[CONFIGURATION_N_STREAMS_MAX];

  // Number of current event
  UInt_t fEventsRead;

  // Number of current stream
  UChar_t fCurrentStream;

  // Number of seconds to sleep between checks for new events
  UInt_t fSleepPeriod;

  // Time interval after which WaitForFileToGrow returns
  Double_t fWaitTimeout;

};
#endif
