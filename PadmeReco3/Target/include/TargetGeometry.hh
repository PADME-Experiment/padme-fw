// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#ifndef TargetGeometry_H
#define TargetGeometry_H

#include "PadmeVGeometry.hh"

class TTargetRecoBeam;

class TargetGeometry : public PadmeVGeometry
{

public:
  
  TargetGeometry();
  //~TargetGeometry();
  void Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr );
  TVector3  LocalPosition(Int_t chId);

private:
  
  double fRuler;
};
#endif

