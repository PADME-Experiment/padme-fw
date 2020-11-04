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

  void SetVerbose(unsigned int v) { fVerbose = v; }
  unsigned int Verbose() { return fVerbose; }

  void SetDebugScale(unsigned int v) { fDebugScale = v; }
  unsigned int DebugScale() { return fDebugScale; }

  char* FormatTime(const time_t);

private:

  unsigned int fVerbose;

  unsigned int fDebugScale;

};
#endif
