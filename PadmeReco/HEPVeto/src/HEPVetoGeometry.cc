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
#include "HEPVetoGeometry.hh"
 
HEPVetoGeometry::HEPVetoGeometry()
  : PadmeVGeometry()
{
  std::cout<<"HEPVetoGeometry being created ............"<<std::endl ;
  
}


void HEPVetoGeometry::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr)
{
  PadmeVGeometry::Init(cfg, chIdMgr);
  fFingerSizeX           = (double)cfg->GetParOrDefault("GEOMETRY","FingerSizeX",10. );
  fFingerSizeY           = (double)cfg->GetParOrDefault("GEOMETRY","FingerSizeY",178.);
  fFingerSizeZ           = (double)cfg->GetParOrDefault("GEOMETRY","FingerSizeZ",10. );
  fHEPVetoInnerFacePosX  = (double)cfg->GetParOrDefault("GEOMETRY","HEPVetoInnerFacePosX",3000.);
  fHEPVetoInnerFacePosZ  = (double)cfg->GetParOrDefault("GEOMETRY","HEPVetoInnerFacePosZ",3000.);


  if (fLocOxinPadmeFrame!=fHEPVetoInnerFacePosX+0.5*fFingerSizeX)
 {
  std::cout<<" WARNING!!! The HEPVeto X position in PADME frame  may be wrong! "<<std::endl ;
  std::cout<<" fLocOxinPadmeFrame  "<<fLocOxinPadmeFrame<<"  fHEPVetoInnerFacePosX+0.5*fFingerSizeX "<<fHEPVetoInnerFacePosX+0.5*fFingerSizeX<<std::endl ;
  
 }

  if (fLocOzinPadmeFrame!=fHEPVetoInnerFacePosZ)
 {
  std::cout<<" WARNING!!! The HEPVeto Z position in PADME frame  may be wrong! "<<std::endl ;
  std::cout<<" fLocOzinPadmeFrame  "<<fLocOzinPadmeFrame<<"  fHEPVetoInnerFacePosZ "<<fHEPVetoInnerFacePosZ<<std::endl ;
  
 }
}

TVector3  HEPVetoGeometry::LocalPosition(Int_t chIdInput)
{

  Int_t chId = chIdInput;
  if (chId > 15)  chId = chId - 16;
  double x = ((chId-fChIdx0))*fStep1ChLocalX + fChIdx0Offset;
  double y = ((chId-fChIdy0))*fStep1ChLocalY + fChIdy0Offset;
  double z = ((-chId+fChIdz0))*fStep1ChLocalZ + fChIdz0Offset;


  return TVector3(x,y,z);
}

