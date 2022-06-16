// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2022-06-16
//
// --------------------------------------------------------------
#include "Riostream.h"
#include "RecoVChannelID.hh"
#include "TRecoVCluster.hh"
#include "TRecoVHit.hh"
#include "ETagGeometry.hh"
 
ETagGeometry::ETagGeometry()
  : PadmeVGeometry()
{
  std::cout<<"ETagGeometry being created ............"<<std::endl ;
}

void ETagGeometry::Init(PadmeVRecoConfig* cfg, RecoVChannelID* chIdMgr)
{
  PadmeVGeometry::Init(cfg, chIdMgr);
}

TVector3  ETagGeometry::LocalPosition(Int_t chId)
{
  // Return position of scintillator slab with given channel id
  double x = 0.;
  double y = 0.;
  double z = 0.;
  return TVector3(x,y,z);
}
