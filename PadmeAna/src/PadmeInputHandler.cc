#include "PadmeInputHandler.hh"
#include "PadmeAnaInMCHitsEvent.hh"
#include "PadmeAnaInMCHistoEvent.hh"
#include "PadmeAnaInMCDigiEvent.hh"

#include "TList.h"
#include "TFile.h"
#include "TTree.h"


#include <fstream>


int PadmeInputHandler::isROOTFile(std::string fname){
  char buf[5];
  buf[4] = '\0';
  std::ifstream infile;
  infile.open(fname.data(),std::ios::binary); 
  infile.read(buf,4);
  std::cout << "File identifier: " << buf << std::endl;
  infile.close();
  if(buf[0]=='r' && buf[1]=='o' && buf[2]=='o' && buf[3]=='t') {
    return 1;
  }
  return 0;
}


void PadmeInputHandler::PadmeROOTInputHandler(std::string fname){
  //Get the ROOT file structure:
  //Perform some checks what is inside  
  
  TFile *fin = new TFile(fname.data(),"read");
  
  TTree *MCTree = (TTree *) fin->Get("MC");
  TTree *MCHistoTree = (TTree *) fin->Get("U101");
  if( MCTree == NULL ) {
    std::cout << "Root file doesn't contain an MC tree "<< std::endl;
  } else {
    std::cout << "An MC tree with " << MCTree->GetEntries() << " entries located .... processing"<< std::endl;
    fInEvent = new PadmeAnaInMCHitsEvent(MCTree);
  }
  
  if( MCHistoTree == NULL ) {
    std::cout << "Root file doesn't contain an MC Histo tree "<< std::endl;
  } else {
    std::cout << "An MC Histo tree with " << MCHistoTree->GetEntries() << " entries located .... processing"<< std::endl;
    fInEvent = new PadmeAnaInMCHistoEvent(MCHistoTree );
  }
  
  
  
  
  // delete fin;

}

void PadmeInputHandler::InputDigiEvent(){
  
  ;
}

void PadmeInputHandler::InputMCHitsEvent(){
  fInEvent = new PadmeAnaInMCHitsEvent();
  
  
  
}

void PadmeInputHandler::InputRecoEvent(){
  ;
}


void PadmeInputHandler::PadmeGeneralInputHandler(std::string fname){
  
}




PadmeInputHandler::PadmeInputHandler(){
  fInEvent=NULL;
}


PadmeInputHandler::PadmeInputHandler(std::string fname){
  fInEvent=NULL;
  int isRoot=isROOTFile(fname);
  if(isRoot) {
    std::cout << "Working on a ROOT input file: " << fname << std::endl;
    PadmeROOTInputHandler(fname);
  } else {
    std::cout << "Working on a binary input file: " << fname << std::endl;
    PadmeGeneralInputHandler(fname);
  }

}

PadmeInputHandler::~PadmeInputHandler(){
  if(fInEvent) delete fInEvent; fInEvent=NULL;
}
