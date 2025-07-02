// --------------------------------------------------------------
// History:
//
// --------------------------------------------------------------
#include "Riostream.h"
#include "RecoVChannelID.hh"
#include "TRecoVCluster.hh"
#include "TRecoVHit.hh"
#include "MMGeometry.hh"
 
MMGeometry::MMGeometry()
  : PadmeVGeometry()
{
  std::cout<<"MMGeometry being created ............"<<std::endl ;
  
}


void MMGeometry::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr)
{
  PadmeVGeometry::Init(cfg, chIdMgr);
  
  fStripPitch      = cfg->GetParOrDefault("GEOMETRY","StripPitch",1.2); // mm
  fOffsetPlaneX[0] = cfg->GetParOrDefault("GEOMETRY","OffsetPlane1X",0); // mm 
  fOffsetPlaneY[0] = cfg->GetParOrDefault("GEOMETRY","OffsetPlane1Y",0); // mm
  fPosPlaneZ[0]	   = cfg->GetParOrDefault("GEOMETRY","PosPlane1Z",2357.32);// mm
  fOffsetPlaneX[1] = cfg->GetParOrDefault("GEOMETRY","OffsetPlane2X",0); // mm
  fOffsetPlaneY[1] = cfg->GetParOrDefault("GEOMETRY","OffsetPlane2Y",0);  // mm
  fPosPlaneZ[1]    = cfg->GetParOrDefault("GEOMETRY","PosPlane2Z",2458.32); // mm
  fOffsetCenterX   = cfg->GetParOrDefault("GEOMETRY","OffsetCenterX",9.995);  // mm
  fOffsetCenterY   = cfg->GetParOrDefault("GEOMETRY","OffsetCenterY",20.1975);  // mm

}

TVector3  MMGeometry::LocalPosition(Int_t chId)
// layer 0,1 -> y layer/2 = 0 
// layer 2,3 -> x layer/2 = 1
// layer 4,5 -> y layer/2 = 2
// layer 6,7 -> x layer/2 = 3
// layer 0,1 large hole
// layer 2,3 little hole
// layer 4,5 little hole
// layer 6,7 large hole
{
  int bdid = (chId & 0xF00 ) >> 8; // board id 0-15
  int layer = bdid/2;  // layer 0-7
  int plane = layer/4;  // plane 0-1
  int side = bdid%2; // left/right (X view), bottom/top (Y view)
  int strip = (chId & 0x0FF); // strip 0-255
  int view = (layer/2)%2; // 0 means Y view, 1 means X view
  int hole[8] = {6,6,1,1,1,1,6,6};
  int offs[2] = {0,256};
  
  double x = fOffsetPlaneX[plane];
  double y = fOffsetPlaneY[plane];
  double z = fPosPlaneZ[plane];

  double armo = (offs[side] + hole[layer]*side + strip + 0.5)*fStripPitch; // displacement with respect to strip 0

  double armwidth = (offs[1] + hole[layer] + 255 - offs[0])*fStripPitch;   // distance from strip 255, side1 to strip0 , side0

  if (view == 0) {
    y += armo - armwidth*0.5;
  } else {
    x += armo - armwidth*0.5;
  }

  //  0      255  0        255
  //  . . . . . . . . . . . .   Ntot = 513 
  //  0      255            0        255
  //  . . . . . . . . . . . . . . . . .   Ntot = 518
  // 
  
  return TVector3(x,y,z);
}

TVector3  MMGeometry::GlobalPosition(Int_t chId){
  TVector3 local = this->LocalPosition(chId);
  TVector3 global(local.X()+fOffsetCenterX, local.Y()+fOffsetCenterY,local.Z());
  return global; 
}
