// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#include "Riostream.h"
#include "RecoVChannelID.hh"
#include "TTargetRecoBeam.hh"
#include "TRecoVHit.hh"
#include "TargetGeometry.hh"
 
TargetGeometry::TargetGeometry()
  : PadmeVGeometry()
{
  std::cout<<"TargetGeometry being created ............"<<std::endl ;
  fRuler=0;
}

void TargetGeometry::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr)
{
  PadmeVGeometry::Init(cfg, chIdMgr);
  fRuler  = (double)cfg->GetParOrDefault("GEOMETRY",    "Ruler" ,   52);
  std::cout<<"*********Ruler Position    =    "<<fRuler<<std::endl ;
  fLocOxinPadmeFrame= fLocOxinPadmeFrame+(fRuler-52);
  PadmeVGeometry::UpdateTransform();
}

TVector3  TargetGeometry::LocalPosition(Int_t chId)
{
  int ix = fChIdx0;
  int iy = fChIdy0;
  int iz = fChIdz0;
 
  if(chId<17) {
    iy=chId;
   }
    else {
   ix=chId;
   }
 
  double x = (chId-ix)*fStep1ChLocalX + fChIdx0Offset;
  double y = (chId-iy)*fStep1ChLocalY + fChIdy0Offset;
  double z = (chId-iz)*fStep1ChLocalZ + fChIdz0Offset;
 
  
  return TVector3(x,y,z);
}

