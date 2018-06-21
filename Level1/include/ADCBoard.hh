#ifndef ADCBoard_H
#define ADCBoard_H

#include<vector>
#include<string>
#include<iostream>
#include<fstream>

#include "Rtypes.h"

#include "ADCEvent.hh"
#include "ADCFile.hh"

class Configuration;

class ADCBoard
{

 public:

  ADCBoard(int);
  ~ADCBoard();

  void Reset();
  void Init();

  int GetBoardId() { return fBoardId; }

  ADCFile* File(int i);
  ADCFile* AddFile(std::string);
  int GetNFiles() { return fFiles.size(); }
  std::string GetFileName(int);

  int NextEvent();
  int UnpackEvent();
  ADCEvent* Event() { return fADCEvent; }
  void* Buffer() { return fBuffer; }

  UInt_t GetEventSize();
  UInt_t GetSerialNumber();
  UInt_t GetEventCounter();
  unsigned char GetGroupMask();
  int GetTriggerTimeTags(UInt_t*);

 private:

  int OpenFile();
  int ReadFileHead();
  int ReadNextEvent();
  int UnpackEvent_v01();
  int UnpackEvent_v02();
  int UnpackEvent_v03();
  int GetTriggerTimeTags_v01(UInt_t*);
  int GetTriggerTimeTags_v02(UInt_t*);
  int GetTriggerTimeTags_v03(UInt_t*);

 private:

  int fBoardId;

  Configuration* fCfg;

  std::vector<ADCFile> fFiles;

  UInt_t fNewFile;
  UInt_t fCurrentFile;
  std::ifstream fFileHandle;
  void* fBuffer;
  UInt_t fEventSize;

  ADCEvent* fADCEvent;

};
#endif
