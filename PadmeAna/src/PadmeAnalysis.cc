#include "PadmeAnalysis.hh"
#include "iostream"

using namespace std;


PadmeAnalysis::PadmeAnalysis()
{
  fAnalysers.clear();
  Event= NULL;
  InEvent = NULL;
}

PadmeAnalysis::~PadmeAnalysis()
{;}

void PadmeAnalysis::ProcessEvent(){
  for(unsigned int iAna=0;iAna<fAnalysers.size();iAna++) {
    fAnalysers[iAna]->ProcessEvent(Event);
  }
}


int PadmeAnalysis::NextEvent() {
  if(InEvent) {
    return(InEvent->NextEvent());
    return 0;
  }
  return 0;
}

void PadmeAnalysis::GetEvent() {
  ;
}


void PadmeAnalysis::ProcessInputFile(string InFile){
  //  std::cout << "Processing file " << InFile <<  std::endl;
  
  fInput = new  PadmeInputHandler(InFile);
  

  
  // while(   ){
    
    
  // }
  
  delete fInput;
  fInput = 0;
}


void PadmeAnalysis::ProcessInputList(std::vector<string> FileList){
  for(size_t ifile = 0; ifile<FileList.size(); ++ifile) {
    ProcessInputFile(FileList[ifile]);
  }
}

// void PadmeAnalysis::ProcessSoBEvent(){
//   ;
// }
// void PadmeAnalysis::ProcessSoREvent(){
//   ;
// }
// void PadmeAnalysis::ProcessEoBEvent(){
//   ;
// }
// void PadmeAnalysis::ProcessEoREvent(){
//   ;
// }

// void PadmeAnalysis::ProcessDataEvent(){

  
//   ;
// }

void PadmeAnalysis::Init(string ConfigFileName){
  ;
}
void PadmeAnalysis::EndProcessing(){
  ;
}
