#ifndef InputHandler_H
#define InputHandler_H

//#include "TChain.h"

#include "Configuration.hh"

class TChain;
class TRawEvent;

class InputHandler
{

public:

  InputHandler();
  ~InputHandler();

public:

  Int_t Initialize(); // Returns <>0 if an error occours during initialization

  TRawEvent* NextEvent();

  UInt_t EventsRead() { return fEventsRead; }
  UInt_t EventNumber() { return fEventsRead-1; } // Event number starts at 0

private:

  // Connection to configuration class
  Configuration* fConfig;

  TString FormatFilename(UChar_t,UInt_t);
  Int_t FileExists(TString);

  // Each stream uses a different TChain and a different TRawEvent
  TChain* fTChain[CONFIGURATION_N_STREAMS_MAX];
  UInt_t fCurrentFileInChain[CONFIGURATION_N_STREAMS_MAX];
  UInt_t fCurrentEventInChain[CONFIGURATION_N_STREAMS_MAX];
  UInt_t fTotalEventsInChain[CONFIGURATION_N_STREAMS_MAX];

  TRawEvent* fTRawEvent[CONFIGURATION_N_STREAMS_MAX];

  // Number of current event
  UInt_t fEventsRead;

  // Number of current stream
  UChar_t fCurrentStream;

  // Number of seconds to sleep between checks for new events
  UInt_t fSleepPeriod;

};
#endif
