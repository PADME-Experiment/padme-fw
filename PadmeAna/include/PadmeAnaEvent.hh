#ifndef PadmeAnaEvent_H
#define PadmeAnaEvent_H

#include "PadmeVAnaEvent.hh"
#include "PadmeAnaTrack.hh"
#include "PadmeAnaCluster.hh"



class PadmeAnaEvent:public PadmeVAnaEvent{
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
