#ifndef TargetMonitor_H
#define TargetMonitor_H

#include "TFile.h"

class Configuration;

class TargetMonitor
{

public:

  TargetMonitor();
  ~TargetMonitor();

  void Initialize();
  void Finalize();
  void StartOfEvent();
  void EndOfEvent();
  void Analyze(UChar_t,UChar_t,Short_t*);

private:

  Configuration* fConfig;

};
#endif
