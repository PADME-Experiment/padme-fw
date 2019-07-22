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
#include "SACGeometry.hh"
 
SACGeometry::SACGeometry()
  : PadmeVGeometry()
{
  std::cout<<"SACGeometry being created ............"<<std::endl ;
  
  
}


void SACGeometry::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr)
{
  PadmeVGeometry::Init(cfg, chIdMgr);
  fCrystalSizeX      = (double)cfg->GetParOrDefault("GEOMETRY","CrystalSizeX",30.   );
  fCrystalSizeY      = (double)cfg->GetParOrDefault("GEOMETRY","CrystalSizeY",30.   );
  fCrystalSizeZ      = (double)cfg->GetParOrDefault("GEOMETRY","CrystalSizeZ",140.  );
  fCrystalGap        = (double)cfg->GetParOrDefault("GEOMETRY","CrystalGap",0.      );
  fCrystalCoating    = (double)cfg->GetParOrDefault("GEOMETRY","CrystalCoating",0.  );
  fSACFrontFacePosZ  = (double)cfg->GetParOrDefault("GEOMETRY","SACFrontFacePosZ",3000.);
  
  float PitchX      = fCrystalSizeX+fCrystalGap+2*fCrystalCoating;
  float PitchY      = fCrystalSizeX+fCrystalGap+2*fCrystalCoating;
  float SACZdistance= fSACFrontFacePosZ+0.5*fCrystalSizeZ;

  if (fStep1ChLocalX!=PitchX)
 {
  std::cout<<" WARNING!!! The SAC pitch X may be wrong! "<<std::endl ;
 }

 if (fStep1ChLocalY!=PitchY)
 {
  std::cout<<" WARNING!!! The SAC pitch Y may be wrong! "<<std::endl ;
 }

 if (fLocOzinPadmeFrame!=SACZdistance)
 {
  std::cout<<" WARNING!!! The SAC Z distance from 0 may be wrong! "<<std::endl ;
 }
}

TVector3  SACGeometry::LocalPosition(Int_t chId)
{

  
  int ix0 = fChIdx0/10;
  int iy0 = fChIdy0%10;
  int iz0 = fChIdz0;

  int ix = chId/10;
  int iy = chId%10;
  int iz = iz0;

  double x = (ix - ix0)*fStep1ChLocalX + fChIdx0Offset;
  double y = (iy - iy0)*fStep1ChLocalY + fChIdy0Offset;
  double z = (iz - iz0)*fStep1ChLocalZ + fChIdz0Offset;
 


  return TVector3(x,y,z);
}

