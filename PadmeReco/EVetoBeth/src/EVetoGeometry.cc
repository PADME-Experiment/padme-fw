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


void EVetoGeometry::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr)
{
  PadmeVGeometry::Init(cfg, chIdMgr);
  
  fFingerSizeX         = (double)cfg->GetParOrDefault("GEOMETRY","FingerSizeX",10. );
  fFingerSizeY         = (double)cfg->GetParOrDefault("GEOMETRY","FingerSizeY",178.);
  fFingerSizeZ         = (double)cfg->GetParOrDefault("GEOMETRY","FingerSizeZ",10. );
  fEVetoInnerFacePosX  = (double)cfg->GetParOrDefault("GEOMETRY","EVetoInnerFacePosX",3000.);
  fEVetoInnerFacePosZ  = (double)cfg->GetParOrDefault("GEOMETRY","EVetoInnerFacePosZ",3000.);


  if (fLocOxinPadmeFrame!=fEVetoInnerFacePosX+0.5*fFingerSizeX)
 {
  std::cout<<" WARNING!!! The EVeto X position in PADME frame  may be wrong! "<<std::endl ;
 }

  if (fLocOzinPadmeFrame!=fEVetoInnerFacePosZ)
 {
  std::cout<<" WARNING!!! The EVeto Z position in PADME frame  may be wrong! "<<std::endl ;
 }

  
}

TVector3  EVetoGeometry::LocalPosition(Int_t chId)
{

  double x = ((chId-fChIdx0))*fStep1ChLocalX + fChIdx0Offset;
  double y = ((chId-fChIdy0))*fStep1ChLocalY + fChIdy0Offset;
  double z = ((chId-fChIdz0))*fStep1ChLocalZ + fChIdz0Offset;

  return TVector3(x,y,z);
}

