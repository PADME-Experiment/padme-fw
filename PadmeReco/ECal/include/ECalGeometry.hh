// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#ifndef ECalGeometry_H
#define ECalGeometry_H

#include "PadmeVGeometry.hh"



class ECalGeometry : public PadmeVGeometry
{

public:
  
  ECalGeometry();
  //~ECalGeometry();
  void Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr );
  TVector3  LocalPosition(Int_t chId);
  

private:

  double fCrystalWidth;
  double fCrystalGap;
  double fCrystalCoating;
  double fTedlarThickness;
  
 
};
#endif

