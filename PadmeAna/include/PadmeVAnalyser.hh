#ifndef PadmeVAnalyser_H
#define PadmeVAnalyser_H

#include "PadmeVAnaEvent.hh"


class PadmeVAnalyser{
  PadmeVAnalyser(){;};
  ~PadmeVAnalyser(){;};

  
  // void ProcessSoBEvent();
  // void ProcessSoREvent();
  // void ProcessDataEvent();
  // void ProcessEoBEvent();
  // void ProcessEoREvent();
 
  void ProcessEvent(PadmeVAnaEvent *);
  


  void EndProcessing();

  void Init(string ConfigFileName);

  
};

#endif
