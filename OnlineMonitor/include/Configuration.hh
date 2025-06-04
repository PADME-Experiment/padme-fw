#ifndef Configuration_H
#define Configuration_H

#include "TFile.h"
#include "TTimeStamp.h"

#define CONFIGURATION_N_STREAMS_MAX 5

class Configuration
{

public:

  ~Configuration();
  static Configuration* GetInstance();

private:

  static Configuration* fInstance;

protected:

  Configuration();

public:

  void SetVerbose(Int_t v) { fVerbose = v; }
  Int_t Verbose() { return fVerbose; }

  void SetDebugScale(Int_t v) { fDebugScale = v; }
  Int_t DebugScale() { return fDebugScale; }

  void SetEventsPerFile(UInt_t n) { fEventsPerFile = n; }
  UInt_t EventsPerFile() { return fEventsPerFile; }

  void SetOutputDirectory(TString d) { fOutputDirectory = d; }
  TString OutputDirectory() { return fOutputDirectory; }

  void SetTrendDirectory(TString d) { fTrendDirectory = d; }
  TString TrendDirectory() { return fTrendDirectory; }

  TString TmpDirectory() { return fTmpDirectory; }

  void SetRunNumber(Int_t r) { fRunNumber = r; }
  Int_t GetRunNumber() { return fRunNumber; }

  void SetEventNumber(UInt_t e) { fEventNumber = e; }
  UInt_t GetEventNumber() { return fEventNumber; }

  void SetEventAbsTime(TTimeStamp t) { fEventAbsTime = t; }
  TTimeStamp GetEventAbsTime() { return fEventAbsTime; }

  void SetEventRunTime(ULong64_t t) { fEventRunTime = t; }
  ULong64_t GetEventRunTime() { return fEventRunTime; }

  void SetEventTrigMask(UInt_t m) { fEventTrigMask = m; }
  UInt_t GetEventTrigMask() { return fEventTrigMask; }

  void SetEventStatus(UInt_t s) { fEventStatus = s; }
  UInt_t GetEventStatus() { return fEventStatus; }

  void SetBoardActiveMask(UInt_t m) { fBoardActiveMask = m; }
  UInt_t GetBoardActiveMask() { return fBoardActiveMask; }

  void SetBoardAcceptMask(UInt_t m) { fBoardAcceptMask = m; }
  UInt_t GetBoardAcceptMask() { return fBoardAcceptMask; }

  void SetBoardTriggerTime(UChar_t n,Double_t t) { fBoardTriggerTime[n] = t; }
  Double_t GetBoardTriggerTime(UChar_t n) { return fBoardTriggerTime[n]; }

  void SetBoardGroupSIC(UChar_t n,UShort_t s) { fBoardGroupSIC[n] = s; }
  UShort_t GetBoardGroupSIC(UChar_t n) { return fBoardGroupSIC[n]; }

  void SetNumberOfStreams(UInt_t n) { fNumberOfStreams = n; }
  UChar_t NumberOfStreams() { return fNumberOfStreams; }
  UChar_t NumberOfStreamsMax() { return CONFIGURATION_N_STREAMS_MAX; }

  void SetDataDirectory(TString f) { fDataDirectory = f; }
  TString DataDirectory() { return fDataDirectory; }

  void SetChamberDataDirectory(TString f) { fChamberDataDirectory = f; }
  TString ChamberDataDirectory() { return fChamberDataDirectory; }

  void SetNumberOfEventsToSkip(Int_t n) { fNumberOfEventsToSkip = n; }
  Int_t NumberOfEventsToSkip() { return fNumberOfEventsToSkip; }

  void SetRunName(TString f) { fRunName = f; }
  TString RunName() { return fRunName; }

  void SetChamberRunName(TString f) { fChamberRunName = f; }
  TString ChamberRunName() { return fChamberRunName; }

  void EnableFollowMode() { fFollowMode = true; }
  void DisableFollowMode() { fFollowMode = false; }
  Bool_t FollowMode() { return fFollowMode; }

  void EnableIgnoreDisabledChannels() { fIgnoreDisabledChannels = true; }
  void DisableIgnoreDisabledChannels() { fIgnoreDisabledChannels = false; }
  Bool_t IgnoreDisabledChannels() { return fIgnoreDisabledChannels; }

  void EnableResumeMode() { fResumeMode = true; }
  void DisableResumeMode() { fResumeMode = false; }
  Bool_t ResumeMode() { return fResumeMode; }

  void SetStopFile(TString f) { fStopFile = f; }
  TString StopFile() { return fStopFile; }

  void SetEndRunFile(TString f) { fEndRunFile = f; }
  TString EndRunFile() { return fEndRunFile; }

  void SetConfigFile(TString f) { fConfigFile = f; }
  TString ConfigFile() { return fConfigFile; }

  char* FormatTime(const time_t);
  char* FormatCurrentTime();

private:

  // Verbose level
  Int_t fVerbose;

  // Events interval between debug printouts
  Int_t fDebugScale;

  // Maximum number of events to write to a RawFile
  UInt_t fEventsPerFile;

  // Directory where output files (either PadmeMonitor or RawFile) will be copied to
  TString fOutputDirectory;

  // Directory where support data files (e.g. trends) will be written to
  TString fTrendDirectory;

  // Temporary directory where PadmeMonitor output files are written before being moved to final output directory
  TString fTmpDirectory;

  // Name (including possible path) of configuration file for monitor analysis
  TString fConfigFile;

  // Event information
  Int_t fRunNumber;
  UInt_t fEventNumber;
  TTimeStamp fEventAbsTime;
  ULong64_t fEventRunTime;
  UInt_t fEventTrigMask;
  UInt_t fEventStatus;

  // Board information
  UInt_t fBoardActiveMask;
  UInt_t fBoardAcceptMask;
  Double_t fBoardTriggerTime[4];
  UShort_t fBoardGroupSIC[4];

  // Number of streams to use
  UChar_t fNumberOfStreams;

  // Directory where all PADME rawdata files are stored
  TString fDataDirectory;

  // PADME run name, e.g. run_0030571_20201028_194712
  TString fRunName;

  // Directory where all Chamber rawdata files are stored
  TString fChamberDataDirectory;

  // Chamber run name, e.g. run123
  TString fChamberRunName;

  // Number of events to skip at startup.  >0: skip PADME events. <0: skip Chamber events.
  Int_t fNumberOfEventsToSkip;

  // Follow mode enabled/disabled
  Bool_t fFollowMode;

  // Resume mode enabled/disabled
  Bool_t fResumeMode;

  // Ignore disabled channels
  Bool_t fIgnoreDisabledChannels;

  // Name of file to check if program should stop when in follow mode
  TString fStopFile;

  // Name of file to signal that the run has ended
  TString fEndRunFile;

};
#endif
