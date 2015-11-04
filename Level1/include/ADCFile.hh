#ifndef ADCFile_H
#define ADCFile_H

#include <string>

#include "Rtypes.h"

class ADCFile
{

 public:

  ADCFile();
  ADCFile(std::string);
  ADCFile(char*);

  ~ADCFile();

  void    SetPath(std::string v) { fPath = v; }
  std::string GetPath()          { return fPath; }

  void    SetVersion(Int_t v)    { fVersion = v; }
  Int_t   GetVersion()           { return fVersion; }

  void    SetIndex(Int_t v)      { fIndex = v; }
  Int_t   GetIndex()             { return fIndex; }

  void    SetRunNumber(Int_t v)  { fRunNumber = v; }
  Int_t   GetRunNumber()         { return fRunNumber; }

  void    SetStartTime(UInt_t v) { fStartTime = v; }
  UInt_t  GetStartTime()         { return fStartTime; }

  void    SetEndTime(UInt_t v)   { fEndTime = v; }
  UInt_t  GetEndTime()           { return fEndTime; }

  void    SetNEvents(Int_t v)    { fNEvents = v; }
  Int_t   GetNEvents()           { return fNEvents; }

  void    SetSize(ULong_t v)     { fSize = v; }
  ULong_t GetSize()              { return fSize; }

 private:

  std::string fPath;
  Int_t   fVersion;
  Int_t   fIndex;
  Int_t   fRunNumber;
  UInt_t  fStartTime;
  UInt_t  fEndTime;
  Int_t   fNEvents;
  ULong_t fSize;

};
#endif
