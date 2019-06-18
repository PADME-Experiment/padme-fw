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
#include "PVetoGeometry.hh"
 
PVetoGeometry::PVetoGeometry()
  : PadmeVGeometry()
{
  std::cout<<"PVetoGeometry being created ............"<<std::endl ;
  
}


void PVetoGeometry::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr)
{
  PadmeVGeometry::Init(cfg, chIdMgr);
}

TVector3  PVetoGeometry::LocalPosition(Int_t chId)
{

  double x = ((chId-fChIdx0))*fStep1ChLocalX + fChIdx0Offset;
  double y = ((chId-fChIdy0))*fStep1ChLocalY + fChIdy0Offset;
  double z = ((chId-fChIdz0))*fStep1ChLocalZ + fChIdz0Offset;


  return TVector3(x,y,z);
}

