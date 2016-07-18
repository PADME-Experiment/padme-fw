#ifndef PadmeAnaEvent_H
#define PadmeAnaEvent_H

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


  //private:
  vector<PadmeAnaTrack *> negTrk;
  vector<PadmeAnaTrack *> posTrk;
  vector<PadmeAnaCluster *> calo;
  vector<PadmeAnaCluster *> sac;

  PadmeAnaBeam *beam;
};

#endif
