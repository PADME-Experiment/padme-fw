#ifndef RECO_VCHANNEL_ID_H
#define RECO_VCHANNEL_ID_H

#include <vector>
#include <string>

#include "PadmeVRecoConfig.hh"



class RecoVChannelID {

public:
RecoVChannelID();
~RecoVChannelID(){;};

virtual int GetChannelID();
virtual int GetChannelID(int bID, int chID);
virtual void InitChannelID(PadmeVRecoConfig *cfg);
  //void  ConfigADCReadout(); 
  //  Int_t GetNBoards(){return fNBoards;};

private:
  
  PadmeVRecoConfig *fRecoConfig;
  std::map <int, std::vector<int>> fChannelIDs;

  //  int fChannelID;
  
  //  std::map <int, std::vector<int>> fChannelIDs;//fID;
  //  typedef std::vector<Int_t> ADCMap;
  //  std::vector<ADCMap> fChannelIDs;

};

#endif
