#include "PadmeVAnalyser.hh"
#include "HistoSvc.hh"
#include <iostream>


PadmeVAnalyser::PadmeVAnalyser(){
  fUsed = false;
}

PadmeVAnalyser::PadmeVAnalyser(const char *s,int valid, int verb)
  :PadmeVAnalyser(s)
{
  fMode = valid;
  fVerbosity = verb;
}


PadmeVAnalyser::PadmeVAnalyser(const char *s)
  : PadmeVAnalyser(){
  std::cout << "====== New analyser appeared:  " << s << std::endl; 
  fName = s;
  fAnalysers[fName] = this;  
}



PadmeVAnalyser::~PadmeVAnalyser(){
  ;
}
Bool_t PadmeVAnalyser::Init(PadmeAnalysisEvent *event){
  evt = event;
  if (fMode < 2) InitHistos();//active on standard analysis and validation mode 
  std::cout << "Analyser " << GetName() << " Initialized" << std::endl;
  return true;
}
Bool_t PadmeVAnalyser::Init(PadmeAnalysisEvent *event, int valid, int verb){
  fMode = valid;
  fVerbosity = verb;
  return Init(event);
}


Bool_t PadmeVAnalyser::InitHistos(){
  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  return true;
}


Bool_t PadmeVAnalyser::Process(){

  return true;
}
