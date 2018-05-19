#include "RecoVChannelID.hh"


RecoVChannelID::RecoVChannelID(){
  ;
}


int RecoVChannelID::GetChannelID(){
  return 0;
}

int RecoVChannelID::GetChannelID(int bID,int chID){
  return 32*bID + chID;
}

void RecoVChannelID::InitChannelID(PadmeVRecoConfig *cfg){
  ;
}
