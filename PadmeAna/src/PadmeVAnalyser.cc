#include "PadmeVAnalyser.hh"
#include "iostream"

using namespace std;

void PadmeVAnalyser::ProcessEvent(PadmeVAnaEvent *Event){
  std::cout << "Event type: " << Event->EventType() << std::endl;
}


void PadmeVAnalyser::Init(string ConfigFileName){
  cout << "Using config file " << ConfigFileName << endl;
}


void PadmeVAnalyser::EndProcessing(){
  ;
}
