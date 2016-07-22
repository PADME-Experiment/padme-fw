#include "PadmeAnalysis.hh"
#include "PadmeAnaEvent.hh"


#include "iostream"

using namespace std;


PadmeAnalysis::PadmeAnalysis()
{
  fAnalysers.clear();
  Event = new PadmeAnaEvent();
  InEvent = NULL;
}

PadmeAnalysis::~PadmeAnalysis()
{
  if (InEvent) {
    std::cout << "[PadmeAnalysis] Input stream has to be cleared by the input handler..." << std::endl;
  }
  if(Event) delete Event;
  
}

void PadmeAnalysis::ProcessEvent(){
  for(unsigned int iAna=0;iAna<fAnalysers.size();iAna++) {
    fAnalysers[iAna]->ProcessEvent(Event);
  }
}


int PadmeAnalysis::NextEvent() {
  if(InEvent) {
    return(InEvent->NextEvent());
  }
  return 0;
}

void PadmeAnalysis::GetEvent() {
  ;
}


int PadmeAnalysis::ProcessInputFile(std::string InFile){
  //  std::cout << "Processing file " << InFile <<  std::endl;
  long int nev = 0;
  
  fInput  = new PadmeInputHandler(InFile);
  InEvent = fInput->GetEventHandler();
  
  if(InEvent != NULL) {    
    while(NextEvent() ){
      Event->FillEvent(InEvent);
      ProcessEvent();      
      nev++;
    }
  }
  
  delete fInput;
  fInput = NULL;
  InEvent = NULL;
  std::cout << "Processed " << nev << " events in file " << InFile << std::endl;
  return nev;
}


int PadmeAnalysis::ProcessInputList(std::vector<std::string> FileList){
  int nev = 0;
  for(size_t ifile = 0; ifile<FileList.size(); ++ifile) {
    nev+=ProcessInputFile(FileList[ifile]);
  }
  std::cout << "Processed total number of events: " << nev << std::endl;
  return nev;
}


void PadmeAnalysis::Init(string ConfigFileName){
  ;
}
void PadmeAnalysis::EndProcessing(){
  ;
}
