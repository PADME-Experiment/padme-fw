#ifndef PadmeAnaEvent_H
#define PadmeAnaEvent_H

//Input processors
#include "PadmeVAnaInEvent.hh"
#include "PadmeAnaInMCHitsEvent.hh"
#include "PadmeAnaInMCHistoEvent.hh"
#include "PadmeAnaInMCDigiEvent.hh"


#include "PadmeVAnaEvent.hh"
#include "PadmeAnaTrack.hh"
#include "PadmeAnaCluster.hh"
#include "PadmeAnaBeam.hh"


#include <vector>
#include <string>

using namespace std;

class PadmeAnaEvent:public PadmeVAnaEvent{
public:
  PadmeAnaEvent();
  ~PadmeAnaEvent();
  void FillEvent( PadmeVAnaEvent         * );
  void FillEvent( PadmeAnaInMCHitsEvent  * );
  void FillEvent( PadmeAnaInMCHistoEvent * );
  void FillEvent( PadmeAnaInMCDigiEvent * );

  //private:
  vector<PadmeAnaTrack *>   negTrk;
  vector<PadmeAnaTrack *>   posTrk;
  vector<PadmeAnaCluster *> calo;
  vector<PadmeAnaCluster *> sac;
  
  PadmeAnaBeam *beam;
};

#endif
