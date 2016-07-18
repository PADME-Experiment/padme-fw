#ifndef PadmeVAnalyser_H
#define PadmeVAnalyser_H

#include "PadmeVAnaEvent.hh"

#include <string>
using namespace std;


class PadmeVAnalyser{
public:
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
