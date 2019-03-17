#include "RecoVChannelID.hh"
#include <iostream>

RecoVChannelID::RecoVChannelID(){
  fnChannelID=0;
}


int RecoVChannelID::GetChannelID(){
  return 0;
}

int RecoVChannelID::GetChannelID(int bID,int chID){

  return fChannelIDs.count( bID )  >= 1 ?   fChannelIDs[bID][chID]: -1; 
  //return 32*bID + chID;
}


std::vector<int> RecoVChannelID::GetChannelIDVector()
{
  std::vector<int> IDs;
  IDs.clear();
  if (fnChannelID==0) return IDs;
  for ( const auto &p : fChannelIDs )
    {
      //    cout << p.first << " :";
      for ( const auto &s : p.second )
	{
	  //cout << ' ' << s;
	  IDs.push_back((int)s);
	  //	  if (index==i) return (int)s;
	  //	  ++i;
	}
      //    cout << endl;
    }
  return IDs;  
}
int RecoVChannelID::scanChannelID(int index)
{
  if (fnChannelID==0) return -1;
  if (index<0) return -1;
  if (index>=fnChannelID) return -1;

  int i=0;
  for ( const auto &p : fChannelIDs )
    {
      //    cout << p.first << " :";
      for ( const auto &s : p.second )
	{
	  //cout << ' ' << s;
	  if (index==i) return (int)s;
	  ++i;
	}
      //    cout << endl;
    }

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
	  ++fnChannelID;
	  //	  fBoards.push_back(std::stoi(*it));
	}
      }
      
      



    }

  }
  std::cout << "Total n. of channelID for this detector is " << fnChannelID << std::endl;
 

}
