// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2022-06-16
//
// --------------------------------------------------------------
#ifndef ETagGeometry_H
#define ETagGeometry_H

#include "PadmeVGeometry.hh"

#define ETAG_NUMBER_OF_CHANNELS 120

class ETagGeometry : public PadmeVGeometry
{

public:
  
  ETagGeometry();
  //~ETagGeometry();
  void Init(PadmeVRecoConfig*, RecoVChannelID*);
  TVector3  LocalPosition(Int_t);

private:

};
#endif
