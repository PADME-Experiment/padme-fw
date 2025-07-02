// --------------------------------------------------------------
// History:
//
// --------------------------------------------------------------
#ifndef MMGeometry_H
#define MMGeometry_H

#include "PadmeVGeometry.hh"



class MMGeometry : public PadmeVGeometry
{

public:
  
  MMGeometry();
  //~EVetoGeometry();
  void Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr );
  TVector3  LocalPosition(Int_t chId);
  TVector3  GlobalPosition(Int_t chId);
  TVector3  GetPosition(Int_t chId){return this->GlobalPosition(chId);};

private:

  double fStripPitch     ;
  double fOffsetPlaneX[2];
  double fOffsetPlaneY[2];
  double fPosPlaneZ[2]	 ;
  double fOffsetCenterX  ;
  double fOffsetCenterY  ;
  
};
#endif

