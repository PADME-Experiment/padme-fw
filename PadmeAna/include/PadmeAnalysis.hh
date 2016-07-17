#ifndef PadmeAnalysis_H
#define PadmeAnalysis_H

#include "PadmeVAnalyser.hh"
#include "PadmeAnaEvent.hh"

#include <stl>
#include <string>


class PadmeAnalysis:public PadmeVAnalyser {
public:
  PadmeAnalysis();
  ~PadmeAnalysis();

  void Init(string ConfigFileName);
  void ProcessSoBEvent();
  void ProcessSoREvent();
  void ProcessDataEvent();
  void ProcessEoBEvent();
  void ProcessEoREvent();

  void EndProcessing();
  void ProcessInputFile(string InFile);
  void ProcessInputList(std::vector<string> FileList);
  void AddAnalyser(PadmeVAnalyser *ann){fAnalysers.push_back(ann)};
  void DelAnalyser();
  
  
private:
  PadmeAnaEvent Event;
  std::vector<PadmeVAnalyser *> fAnalysers;
  
  
  
};


#endif
