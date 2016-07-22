#include "PadmeInputHandler.hh"

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
