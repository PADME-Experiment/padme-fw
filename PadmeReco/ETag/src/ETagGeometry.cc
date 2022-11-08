// --------------------------------------------------------------
// History:
//
// Created by Mauro Raggi (mauro.raggi@roma1.infn.it) 2022-06-16
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
//  fBarSizeX         = (double)cfg->GetParOrDefault("GEOMETRY","BarSizeX",10. );
//  fBarSizeY         = (double)cfg->GetParOrDefault("GEOMETRY","BarSizeY",178.);
//  fBarSizeZ         = (double)cfg->GetParOrDefault("GEOMETRY","BarSizeZ",10. );
//  fETagInnerFacePosX  = (double)cfg->GetParOrDefault("GEOMETRY","ETagInnerFacePosX",3000.);
//  fETagInnerFacePosZ  = (double)cfg->GetParOrDefault("GEOMETRY","ETagInnerFacePosZ",3000.);


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
