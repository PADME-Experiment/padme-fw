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

  void SetVerbose(UInt_t v) { fVerbose = v; }
  UInt_t Verbose() { return fVerbose; }

  void SetDebugScale(Int_t v) { fDebugScale = v; }
  Int_t DebugScale() { return fDebugScale; }

  void SetOutputFile(TString f) { fOutputFile = f; }
  TString OutputFile() { return fOutputFile; }

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

  void SetNumberOfStreams(UInt_t n) { fNumberOfStreams = n; }
  UChar_t NumberOfStreams() { return fNumberOfStreams; }
  UChar_t NumberOfStreamsMax() { return CONFIGURATION_N_STREAMS_MAX; }

  void SetDataDirectory(TString f) { fDataDirectory = f; }
  TString DataDirectory() { return fDataDirectory; }

  void SetRunName(TString f) { fRunName = f; }
  TString RunName() { return fRunName; }

  void EnableFollowMode() { fFollowMode = true; }
  void DisableFollowMode() { fFollowMode = false; }
  Bool_t FollowMode() { return fFollowMode; }

  void EnableResumeMode() { fResumeMode = true; }
  void DisableResumeMode() { fResumeMode = false; }
  Bool_t ResumeMode() { return fResumeMode; }

  void SetStopFile(TString f) { fStopFile = f; }
  TString StopFile() { return fStopFile; }

  char* FormatTime(const time_t);

private:

  // Verbose level
  UInt_t fVerbose;

  // Events interval between debug printouts
  Int_t fDebugScale;

  // Name (including possible path) of output file in PadmeMOnitor format
  TString fOutputFile;

  // Event information
  Int_t fRunNumber;
  UInt_t fEventNumber;
  TTimeStamp fEventAbsTime;
  ULong64_t fEventRunTime;
  UInt_t fEventTrigMask;
  UInt_t fEventStatus;

  // Number of streams to use
  UChar_t fNumberOfStreams;

  // Directory where all rawdata files are stored
  TString fDataDirectory;

  // Run name, e.g. run_0030571_20201028_194712
  TString fRunName;

  // Follow mode enabled/disabled
  Bool_t fFollowMode;

  // Resume mode enabled/disabled
  Bool_t fResumeMode;

  // Name of file to check if program should stop when in follow mode
  TString fStopFile;

};
#endif
