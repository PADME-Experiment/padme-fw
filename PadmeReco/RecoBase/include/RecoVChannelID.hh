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


private:
int fChannelID;


};

#endif
