// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#ifndef EVetoGeometry_H
#define EVetoGeometry_H

#include "PadmeVGeometry.hh"



class EVetoGeometry : public PadmeVGeometry
{

public:
  
  EVetoGeometry();
  //~EVetoGeometry();
  void Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr );
  TVector3  LocalPosition(Int_t chId);
  

private:

  double fFingerSizeX;
  double fFingerSizeY;
  double fFingerSizeZ;
  double fEVetoInnerFacePosX;
  double fEVetoInnerFacePosZ;
 
};
#endif

