#ifndef PadmeAnalysis_H
#define PadmeAnalysis_H

#include "PadmeVAnalyser.hh"
#include "PadmeVAnaEvent.hh"
#include "PadmeVAnaInEvent.hh"
#include "PadmeInputHandler.hh"


//#include <stl>
#include <string>
#include <vector>

//class PadmeVAnaInEvent;

using namespace std;


class PadmeAnalysis:public PadmeVAnalyser {
public:
  PadmeAnalysis();
  ~PadmeAnalysis();
  
  void Init(string ConfigFileName);
  // void ProcessSoBEvent();
  // void ProcessSoREvent();
  // void ProcessDataEvent();
  // void ProcessEoBEvent();
  // void ProcessEoREvent();
  
  void EndProcessing();
  void ProcessInputFile(string InFile);
  void ProcessInputList(std::vector<string> FileList);
  void AddAnalyser(PadmeVAnalyser *ann){fAnalysers.push_back(ann);};
  void DelAnalyser();
  
  
  int NextEvent();
  void GetEvent();
  void ProcessEvent();
  
  
private:
  PadmeInputHandler *fInput;
  PadmeVAnaEvent *Event;
  PadmeVAnaInEvent *InEvent;
  std::vector<PadmeVAnalyser *> fAnalysers;
  

  
};


#endif
