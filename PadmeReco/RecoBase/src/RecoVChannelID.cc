#include "RecoVChannelID.hh"
#include <iostream>

RecoVChannelID::RecoVChannelID(){
  ;
}


int RecoVChannelID::GetChannelID(){
  return 0;
}

int RecoVChannelID::GetChannelID(int bID,int chID){

  return fChannelIDs.count( bID )  >= 1 ?   fChannelIDs[bID][chID]: -1; 
  //return 32*bID + chID;
}





void RecoVChannelID::InitChannelID(PadmeVRecoConfig *cfg){
  fRecoConfig = cfg;
  utl::ConfigParser *cfgParser = cfg->GetConfigParser();
  std::cout << "InitChannelID called for : " << cfg->GetName() << std::endl;
  std::string ADCName = "ADC";
  std::string ParName;

  if(fRecoConfig->GetReadOutType()  == "ADC") {
    std::cout << cfg->GetName() << ": readout is " << fRecoConfig->GetReadOutType() << std::endl;
    
    std::vector<Short_t> *Boards = fRecoConfig->GetBoards();
    //    for(auto it = Boards->begin(); it != Boards->end(); ++it) {
    for (int ib = 0;ib < Boards->size();ib++){
      int bID = (*Boards)[ib];
      ParName = ADCName +  std::to_string(bID);
      std::cout << "Processing the map for board:  " <<  bID <<"   " <<  ParName << std::endl;
      
      //Get the corresponding mapping
      if(cfgParser->HasConfig("ADC",ParName )){
	std::vector<std::string> bMap = cfgParser->GetConfig("ADC",ParName);
	for(auto it = bMap.begin(); it != bMap.end(); ++it) {
	  fChannelIDs[bID].push_back(std::stoi(*it));
	  //	  fBoards.push_back(std::stoi(*it));
	}
      }
      
      



    }

  }
  
  

}
