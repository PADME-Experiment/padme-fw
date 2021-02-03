#ifndef Configuration_H
#define Configuration_H

#include <string>

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

  void SetRunNumber(int r) { fRunNumber = r; }
  int RunNumber() { return fRunNumber; }

  void SetInputStream(std::string f) { fInputStream = f; }
  std::string InputStream() { return fInputStream; }

  void SetInputStreamList(std::string f) { fInputStreamList = f; }
  std::string InputStreamList() { return fInputStreamList; }

  void SetOutputStreamList(std::string f) { fOutputStreamList = f; }
  std::string OutputStreamList() { return fOutputStreamList; }

  void SetRawFileHeader(std::string h) { fRawFileHeader = h; }
  std::string RawFileHeader() { return fRawFileHeader; }

  void SetNEventsPerFile(unsigned int n) { fNEventsPerFile = n; }
  unsigned int NEventsPerFile() { return fNEventsPerFile; }

  void SetProcessId(int i) { fProcessId = i; }
  int ProcessId() { return fProcessId; }

  void SetVerbose(unsigned int v) { fVerbose = v; }
  unsigned int Verbose() { return fVerbose; }

  void SetDebugScale(unsigned int v) { fDebugScale = v; }
  unsigned int DebugScale() { return fDebugScale; }

  char* FormatTime(const time_t);

private:

  int fRunNumber;

  int fProcessId;

  std::string fInputStream;

  std::string fInputStreamList;

  std::string fOutputStreamList;

  std::string fRawFileHeader;

  unsigned int fNEventsPerFile;

  unsigned int fVerbose;

  unsigned int fDebugScale;

};
#endif
