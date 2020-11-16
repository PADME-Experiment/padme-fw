#ifndef TargetMonitor_H
#define TargetMonitor_H

#include "TFile.h"
#include "TString.h"

#include "utlConfigParser.hh"

class Configuration;

class TargetMonitor
{

public:

  TargetMonitor(TString);
  ~TargetMonitor();

  void Initialize();
  void Finalize();
  void StartOfEvent();
  void EndOfEvent();
  void Analyze(UChar_t,UChar_t,Short_t*);

private:

  Configuration* fConfig;

  utl::ConfigParser* fConfigParser;

};
#endif
