// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#ifndef ECalMLGeometry_H
#define ECalMLGeometry_H

#include "PadmeVGeometry.hh"



class ECalMLGeometry : public PadmeVGeometry
{

public:
  
  ECalMLGeometry();
  //~ECalMLGeometry();
  void Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr );
  TVector3  LocalPosition(Int_t chId);
  

private:


  double fCrystalSizeX;
  double fCrystalSizeY;
  double fCrystalSizeZ;
  double fCrystalGap;
  double fCrystalCoating;
  double fTedlarThickness;
  double fECalMLFrontFacePosZ;

 
};
#endif

