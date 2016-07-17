#ifndef PadmeVAnalyser_H
#define PadmeVAnalyser_H

class PadmeVAnalyser{
  
  void ProcessSoBEvent();
  void ProcessSoREvent();
  void ProcessDataEvent();
  void ProcessEoBEvent();
  void ProcessEoREvent();

  void EndProcessing();

  void Init(string ConfigFileName);

  
};

#endif
