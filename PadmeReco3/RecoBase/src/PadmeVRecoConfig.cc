#include "PadmeVRecoConfig.hh"
#include <iostream>
#include <algorithm>
	  
PadmeVRecoConfig::PadmeVRecoConfig(utl::ConfigParser *cfgParser,TString Name) 
  :PadmeVNamedModule(Name)
{

  fConfigParser = cfgParser;

  fReadOutType = GetParOrDefault("DETECTOR","READOUT","NO_READOUT");
  
  std::cout << "Readout type: " << fReadOutType << std::endl;

  if(fReadOutType == "ADC") ConfigADCReadout();

}

PadmeVRecoConfig::~PadmeVRecoConfig() {;}

void PadmeVRecoConfig::ConfigADCReadout(){
  
  if(fConfigParser->HasConfig("ADC","NADC")){
    fNBoards = std::stoi(fConfigParser->GetSingleArg("ADC","NADC"));
  } else {
    return;
  }

  if(fConfigParser->HasConfig("ADC","ID")){
    std::vector<std::string> bIDs = fConfigParser->GetConfig("ADC","ID");
    for(auto it = bIDs.begin(); it != bIDs.end(); ++it) {
      fBoards.push_back(std::stoi(*it));
    }
  }

  std::cout << fBoards.size() << " boards belonging to detector " << this->GetName() << ", IDs   ";
  for(auto it = fBoards.begin(); it != fBoards.end(); ++it) std::cout << (*it) << "   ";
  std::cout << std::endl;

  // Create channel map (board/channel -> detector channel)
  for (UChar_t ib = 0;ib < fBoards.size();ib++){

    Short_t bID = fBoards[ib];

    std::string parName = "ADC" + std::to_string(bID);
    if (! fConfigParser->HasConfig("ADC",parName)) {
      printf("PadmeVRecoConfig::ConfigADCReadout - ERROR - Cannot find map for board %d. Check your configuration file.",bID);
      return;
    }

    // Fill map with individual channels
    UChar_t ic = 0;
    std::vector<std::string> bMap = fConfigParser->GetConfig("ADC",parName);
    for(auto it = bMap.begin(); it != bMap.end(); ++it) {
      Int_t chID = std::stoi(*it);
      fChannelMap[bID][ic] = chID;
      ic++;
    }

  }


}

Bool_t PadmeVRecoConfig::BoardIsMine(Int_t iBoard) {
  if (std::find(fBoards.begin(),fBoards.end(),iBoard) != fBoards.end()) return true;

  //for (auto it = fBoards.begin(); it != fBoards.end(); ++it) {
  //  if(*it == iBoard) return true;
  //}

  return false;
}

int PadmeVRecoConfig::GetParOrDefault(std::string group,std::string name,int val){
  return fConfigParser->HasConfig(group,name)?std::stoi(fConfigParser->GetSingleArg(group,name)):val;
}

double PadmeVRecoConfig::GetParOrDefault(std::string group,std::string name,double val){
  return fConfigParser->HasConfig(group,name)?std::stof(fConfigParser->GetSingleArg(group,name)):val;
}

std::string PadmeVRecoConfig::GetParOrDefault(std::string group,std::string name,std::string val){
  return fConfigParser->HasConfig(group,name)?fConfigParser->GetSingleArg(group,name):val;
}
