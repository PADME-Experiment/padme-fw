#ifndef Trigger_H
#define Trigger_H

#include<vector>
#include<string>
#include<iostream>
#include<fstream>

#include "Rtypes.h"

#include "TrigFile.hh"

class Configuration;

class Trigger
{

 public:

  Trigger();
  ~Trigger();

  void Reset();
  void Init();

  TrigFile* File(int i);
  TrigFile* AddFile(std::string);
  int GetNFiles() { return fFiles.size(); }
  std::string GetFileName(int);

  int NextEvent();
  int UnpackEvent();
  void* Buffer() { return fBuffer; }

  UInt_t GetVersion() { return fVersion; }
  void SetVersion(UInt_t v) { fVersion = v; }

  UInt_t GetEventSize() { return fEventSize; }

  UChar_t GetTriggerMask() { return fTriggerMask; }
  UChar_t GetTriggerFifo() { return fTriggerFifo; }
  UChar_t GetTriggerAuto() { return fTriggerAuto; }
  UInt_t GetTriggerCounter() { return fTriggerCounter; }
  ULong64_t GetClockCounter() { return fClockCounter; }
  ULong64_t GetTotalClockCounter() { return fTotalClockCounter; }

 private:

  int OpenFile();
  int ReadFileHead();
  int ReadNextEvent();
  int UnpackEvent_v03();

 private:

  Configuration* fCfg;

  std::vector<TrigFile> fFiles;

  UInt_t fNewFile;
  UInt_t fCurrentFile;
  std::ifstream fFileHandle;
  UInt_t fFinished;
  void* fBuffer;

  UInt_t fEventSize;
  UInt_t fVersion;

  UInt_t fFirstEvent;
  UChar_t fTriggerMask;
  UInt_t fTriggerCounter;
  ULong64_t fClockCounter;
  ULong64_t fTotalClockCounter;
  UChar_t fTriggerFifo;
  UChar_t fTriggerAuto;

};
#endif
