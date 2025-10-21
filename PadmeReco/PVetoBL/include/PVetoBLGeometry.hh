// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#ifndef PVetoBLGeometry_H
#define PVetoBLGeometry_H

#include "PadmeVGeometry.hh"



class PVetoBLGeometry : public PadmeVGeometry
{

public:
  
  PVetoBLGeometry();
  //~PVetoBLGeometry();
  void Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr );
  TVector3  LocalPosition(Int_t chId);
  

private:

  double fFingerSizeX;
  double fFingerSizeY;
  double fFingerSizeZ;
  double fPVetoBLInnerFacePosX;
  double fPVetoBLInnerFacePosZ;
 
};
#endif

