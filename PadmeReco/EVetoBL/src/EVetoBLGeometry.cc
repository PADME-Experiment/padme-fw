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
#include "EVetoBLGeometry.hh"
 
EVetoBLGeometry::EVetoBLGeometry()
  : PadmeVGeometry()
{
  std::cout<<"EVetoBLGeometry being created ............"<<std::endl ;
  
}


void EVetoBLGeometry::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr)
{
  PadmeVGeometry::Init(cfg, chIdMgr);
  
  fFingerSizeX         = (double)cfg->GetParOrDefault("GEOMETRY","FingerSizeX",10. );
  fFingerSizeY         = (double)cfg->GetParOrDefault("GEOMETRY","FingerSizeY",178.);
  fFingerSizeZ         = (double)cfg->GetParOrDefault("GEOMETRY","FingerSizeZ",10. );
  fEVetoBLInnerFacePosX  = (double)cfg->GetParOrDefault("GEOMETRY","EVetoBLInnerFacePosX",3000.);
  fEVetoBLInnerFacePosZ  = (double)cfg->GetParOrDefault("GEOMETRY","EVetoBLInnerFacePosZ",3000.);


  if (fLocOxinPadmeFrame!=fEVetoBLInnerFacePosX+0.5*fFingerSizeX)
 {
  std::cout<<" WARNING!!! The EVetoBL X position in PADME frame  may be wrong! "<<std::endl ;
 }

  if (fLocOzinPadmeFrame!=fEVetoBLInnerFacePosZ)
 {
  std::cout<<" WARNING!!! The EVetoBL Z position in PADME frame  may be wrong! "<<std::endl ;
 }

  
}

TVector3  EVetoBLGeometry::LocalPosition(Int_t chId)
{

  double x = ((chId-fChIdx0))*fStep1ChLocalX + fChIdx0Offset;
  double y = ((chId-fChIdy0))*fStep1ChLocalY + fChIdy0Offset;
  double z = ((chId-fChIdz0))*fStep1ChLocalZ + fChIdz0Offset;

  return TVector3(x,y,z);
}

