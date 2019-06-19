// --------------------------------------------------------------
// History:
//
// Created by Stefania Spagnolo (stefania.spagnolo@le.infn.it) 2019-03-14
//
// --------------------------------------------------------------
#include "Riostream.h"
#include "RecoVChannelID.hh"
#include "TRecoVCluster.hh"
#include "TRecoVHit.hh"
#include "ECalGeometry.hh"
 
ECalGeometry::ECalGeometry()
  : PadmeVGeometry()
{
  std::cout<<"ECalGeometry being created ............"<<std::endl ;
  
}


void ECalGeometry::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr)
{
  PadmeVGeometry::Init(cfg, chIdMgr);

  fCrystalWidth    = (double)cfg->GetParOrDefault("GEOMETRY","CrystalWidth",21.);
  fCrystalGap      = (double)cfg->GetParOrDefault("GEOMETRY","CrystalGap", 0.);
  fCrystalCoating  = (double)cfg->GetParOrDefault("GEOMETRY","CrystalCoating",0.);
  fTedlarThickness = (double)cfg->GetParOrDefault("GEOMETRY","TedlarThickness",0.);
 
  if (fStep1ChLocalX!=fCrystalWidth+fCrystalGap+2*fCrystalCoating+2*fTedlarThickness)
 {
  std::cout<<" WARNING!!! The ECal pitch value X may be wrong! "<<std::endl ;
 }

 if (fStep1ChLocalY!=fCrystalWidth+fCrystalGap+2*fCrystalCoating+2*fTedlarThickness)
 {
  std::cout<<" WARNING!!! The ECal pitch value Y may be wrong! "<<std::endl ;
 }

}

TVector3  ECalGeometry::LocalPosition(Int_t chId)
{

  int ix0 = fChIdx0/100;
  int iy0 = fChIdy0%100;
  int iz0 = fChIdz0;

  int ix = chId/100;
  int iy = chId%100;
  int iz = iz0;

  double x = (ix - ix0)* fStep1ChLocalX + fChIdx0Offset;
  double y = (iy - iy0)* fStep1ChLocalY + fChIdy0Offset;
  double z = (iz - iz0)* fStep1ChLocalZ + fChIdz0Offset;

  
  return TVector3(x,y,z);
}

