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
  
  fCrystalSizeX      = (double)cfg->GetParOrDefault("GEOMETRY","CrystalSizeX", 21. );
  fCrystalSizeY      = (double)cfg->GetParOrDefault("GEOMETRY","CrystalSizeY", 21. );
  fCrystalSizeZ      = (double)cfg->GetParOrDefault("GEOMETRY","CrystalSizeZ",230. );
  fCrystalGap        = (double)cfg->GetParOrDefault("GEOMETRY","CrystalGap", 0.    );
  fCrystalCoating    = (double)cfg->GetParOrDefault("GEOMETRY","CrystalCoating",0. );
  fTedlarThickness   = (double)cfg->GetParOrDefault("GEOMETRY","TedlarThickness",0.);
  fECalFrontFacePosZ = (double)cfg->GetParOrDefault("GEOMETRY","ECalFrontFacePosZ",3000.);

  float PitchX        = fCrystalSizeX+fCrystalGap+2*fCrystalCoating+2*fTedlarThickness;
  float PitchY        = fCrystalSizeY+fCrystalGap+2*fCrystalCoating+2*fTedlarThickness;
  float ECalZdistance = fECalFrontFacePosZ+0.5*fCrystalSizeZ;

 
  if (fStep1ChLocalX!=PitchX)
 {
  std::cout<<" WARNING!!! The ECal pitch X may be wrong! "<<std::endl ;
 }

 if (fStep1ChLocalY!=PitchY)
 {
  std::cout<<" WARNING!!! The ECal pitch Y may be wrong! "<<std::endl ;
 }
 
 if (fLocOzinPadmeFrame!=ECalZdistance)
 {
  std::cout<<" WARNING!!! The ECal Z distance from 0 may be wrong! "<<std::endl ;
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

