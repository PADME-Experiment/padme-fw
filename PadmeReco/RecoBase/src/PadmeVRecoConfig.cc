#include "PadmeVRecoConfig.hh"
#include<iostream>

PadmeVRecoConfig::PadmeVRecoConfig(utl::ConfigParser *cfgParser,TString Name) 
  :PadmeVNamedModule(Name)
{
  if(cfgParser->HasConfig("ADC","NADC")){
    fNBoards = std::stoi(cfgParser->GetSingleArg("ADC","NADC"));
  }
  if(cfgParser->HasConfig("ADC","ID")){
    std::vector<std::string> bIDs = cfgParser->GetConfig("ADC","ID");
    for(auto it = bIDs.begin(); it != bIDs.end(); ++it) {
      fBoards.push_back(std::stoi(*it));
    }
  }

  std::cout << fBoards.size() << "  boards belonging to detector " << this->GetName() << ", IDs   ";
  for(auto it = fBoards.begin(); it != fBoards.end(); ++it) {
    std::cout << (*it) << "   ";
  }
  std::cout << std::endl;

}

PadmeVRecoConfig::~PadmeVRecoConfig() {;}

Bool_t PadmeVRecoConfig::BoardIsMine(Int_t iBoard){
  for(auto it = fBoards.begin(); it != fBoards.end(); ++it) {
    if(*it == iBoard) return true;
  }
  return false;
}

