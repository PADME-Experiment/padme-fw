// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#ifndef EVetoBLGeometry_H
#define EVetoBLGeometry_H

#include "PadmeVGeometry.hh"



class EVetoBLGeometry : public PadmeVGeometry
{

public:
  
  EVetoBLGeometry();
  //~EVetoBLGeometry();
  void Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr );
  TVector3  LocalPosition(Int_t chId);
  

private:

  double fFingerSizeX;
  double fFingerSizeY;
  double fFingerSizeZ;
  double fEVetoBLInnerFacePosX;
  double fEVetoBLInnerFacePosZ;
 
};
#endif

