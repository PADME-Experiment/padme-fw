#ifndef Configuration_H
#define Configuration_H

#include "TFile.h"
#include "TTimeStamp.h"

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

  void SetDebugScale(UInt_t v) { fDebugScale = v; }
  UInt_t DebugScale() { return fDebugScale; }

  void SetOutputFile(TString f) { fOutputFile = f; }
  TString GetOutputFile() { return fOutputFile; }

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

  char* FormatTime(const time_t);

private:

  // Verbose level
  UInt_t fVerbose;

  // Events interval between debug printouts
  UInt_t fDebugScale;

  // Name (including possible path) of output file in PadmeMOnitor format
  TString fOutputFile;

  // Event information
  Int_t fRunNumber;
  UInt_t fEventNumber;
  TTimeStamp fEventAbsTime;
  ULong64_t fEventRunTime;
  UInt_t fEventTrigMask;
  UInt_t fEventStatus;

};
#endif
