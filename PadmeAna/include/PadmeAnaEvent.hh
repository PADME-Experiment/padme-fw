#ifndef PadmeAnaEvent_H
#define PadmeAnaEvent_H

#include "PadmeAnaTrack.hh"
#include "PadmeAnaCluster.hh"



class PadmeAnaEvent{
  PadmeAnaEvent();
  ~PadmeAnaEvent();

  //  void FillEvent();

private:
  vector<PadmeAnaTrack> negTrk;
  vector<PadmeAnaTrack> posTrk;
  vector<PadmeAnaCluster> calo;
  vector<PadmeAnaCluster> sac;
  PadmeAnaBeam beam;
};

#endif
