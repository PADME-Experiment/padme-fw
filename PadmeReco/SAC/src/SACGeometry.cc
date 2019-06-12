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
  fCrystalGap      = (double)cfg->GetParOrDefault("GEOMETRY","CrystalGap",0.);
  fCrystalCoating  = (double)cfg->GetParOrDefault("GEOMETRY","CrystalCoating",0.);
  fTedlarThickness = (double)cfg->GetParOrDefault("GEOMETRY","TedlarThickness",0.);
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
 
  /*double x = ((chId-ix)/10)*fStep1ChLocalX + fChIdx0Offset;
  double y = ((chId-iy)%10)*fStep1ChLocalY + fChIdy0Offset;
  double z = (chId-iz)*fStep1ChLocalZ + fChIdz0Offset;*/


  return TVector3(x,y,z);
}

