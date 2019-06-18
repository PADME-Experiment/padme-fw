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
#include "EVetoGeometry.hh"
 
EVetoGeometry::EVetoGeometry()
  : PadmeVGeometry()
{
  std::cout<<"EVetoGeometry being created ............"<<std::endl ;
  
}
/*

void EVetoGeometry::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr)
{
  PadmeVGeometry::Init(cfg, chIdMgr);
}

TVector3  EVetoGeometry::LocalPosition(Int_t chId)
{

  double x = ((fChIdx0-fChIdx0))*fStep1ChLocalX + fChIdx0Offset;
  double y = ((fChIdx0-fChIdy0))*fStep1ChLocalY + fChIdy0Offset;
  double z = ((fChIdy0-fChIdz0))*fStep1ChLocalZ + fChIdz0Offset;


  return TVector3(x,y,z);
}
*/
