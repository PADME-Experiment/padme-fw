#ifndef ChamberInputHandler_H
#define ChamberInputHandler_H

#include "ChamberEvent.hh"

class Configuration;

class TFile;
class TTree;
class TRawEvent;

class ChamberInputHandler
{

public:

  ChamberInputHandler();
  ~ChamberInputHandler();

public:

  Int_t Initialize();
  Int_t Finalize();

  ChamberEvent* NextEvent();

  UInt_t EventsRead() { return fEventsRead; }
  UInt_t EventNumber() { return fEventsRead-1; } // Event number starts at 0

private:

  // Connection to configuration class
  Configuration* fConfig;

  Int_t Resume();
  ChamberEvent* ReadNextEvent();
  Int_t WaitForFileToGrow();
  Int_t OpenFileInStream(UInt_t);
  TString FormatFilename(UInt_t);
  Bool_t FileExists(TString);
  Long64_t GetLocalFileSize(TString);

  ChamberEvent* fChamberEvent;

  TFile* fTFile;
  TTree* fTTree;
  UInt_t fTotalEventsInFile;
  UInt_t fCurrentEventInFile;
  UInt_t fCurrentFileInStream;
  Bool_t fCurrentFileIsOpen;

  // Number of current event
  UInt_t fEventsRead;

  // Number of seconds to sleep between checks for new events
  UInt_t fSleepPeriod;

  // Time interval after which WaitForFileToGrow returns
  Double_t fWaitTimeout;

};
#endif
