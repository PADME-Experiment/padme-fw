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

  void SetStreamListFile(std::string f) { fStreamListFile = f; }
  std::string StreamListFile() { return fStreamListFile; }

  void SetOutputFileHeader(std::string h) { fOutputFileHeader = h; }
  std::string OutputFileHeader() { return fOutputFileHeader; }

  void SetNEventsPerFile(unsigned int n) { fNEventsPerFile = n; }
  unsigned int NEventsPerFile() { return fNEventsPerFile; }

  void SetMergerId(int i) { fMergerId = i; }
  int MergerId() { return fMergerId; }

  void SetVerbose(unsigned int v) { fVerbose = v; }
  unsigned int Verbose() { return fVerbose; }

private:

  int fRunNumber;

  int fMergerId;

  std::string fStreamListFile;

  std::string fOutputFileHeader;

  unsigned int fNEventsPerFile;

  unsigned int fVerbose;

};
#endif
